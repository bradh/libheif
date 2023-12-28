/*
 * This copyright notice applies to this header file only:
 *
 * Copyright (c) 2010-2023 NVIDIA Corporation
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the software, and to permit persons to whom the
 * software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>

#include "nvcuvid.h"
#include "NvDecoder.h"

#define CUDA_DRVAPI_CALL( call )                                                                                                 \
    do                                                                                                                           \
    {                                                                                                                            \
        CUresult err__ = call;                                                                                                   \
        if (err__ != CUDA_SUCCESS)                                                                                               \
        {                                                                                                                        \
            const char *szErrName = NULL;                                                                                        \
            cuGetErrorName(err__, &szErrName);                                                                                   \
            std::ostringstream errorLog;                                                                                         \
            errorLog << "CUDA driver API error " << szErrName ;                                                                  \
            throw NVDECException::makeNVDECException(errorLog.str(), err__, __FUNCTION__, __FILE__, __LINE__);                   \
        }                                                                                                                        \
    }                                                                                                                            \
    while (0)

static const char * GetVideoCodecString(cudaVideoCodec eCodec) {
    static struct {
        cudaVideoCodec eCodec;
        const char *name;
    } aCodecName [] = {
        { cudaVideoCodec_MPEG1,     "MPEG-1"       },
        { cudaVideoCodec_MPEG2,     "MPEG-2"       },
        { cudaVideoCodec_MPEG4,     "MPEG-4 (ASP)" },
        { cudaVideoCodec_VC1,       "VC-1/WMV"     },
        { cudaVideoCodec_H264,      "AVC/H.264"    },
        { cudaVideoCodec_JPEG,      "M-JPEG"       },
        { cudaVideoCodec_H264_SVC,  "H.264/SVC"    },
        { cudaVideoCodec_H264_MVC,  "H.264/MVC"    },
        { cudaVideoCodec_HEVC,      "H.265/HEVC"   },
        { cudaVideoCodec_VP8,       "VP8"          },
        { cudaVideoCodec_VP9,       "VP9"          },
        { cudaVideoCodec_AV1,       "AV1"          },
        { cudaVideoCodec_NumCodecs, "Invalid"      },
        { cudaVideoCodec_YUV420,    "YUV  4:2:0"   },
        { cudaVideoCodec_YV12,      "YV12 4:2:0"   },
        { cudaVideoCodec_NV12,      "NV12 4:2:0"   },
        { cudaVideoCodec_YUYV,      "YUYV 4:2:2"   },
        { cudaVideoCodec_UYVY,      "UYVY 4:2:2"   },
    };

    if (eCodec >= 0 && eCodec <= cudaVideoCodec_NumCodecs) {
        return aCodecName[eCodec].name;
    }
    for (size_t i = cudaVideoCodec_NumCodecs + 1; i < sizeof(aCodecName) / sizeof(aCodecName[0]); i++) {
        if (eCodec == aCodecName[i].eCodec) {
            return aCodecName[eCodec].name;
        }
    }
    return "Unknown";
}

static const char * GetVideoChromaFormatString(cudaVideoChromaFormat eChromaFormat) {
    static struct {
        cudaVideoChromaFormat eChromaFormat;
        const char *name;
    } aChromaFormatName[] = {
        { cudaVideoChromaFormat_Monochrome, "YUV 400 (Monochrome)" },
        { cudaVideoChromaFormat_420,        "YUV 420"              },
        { cudaVideoChromaFormat_422,        "YUV 422"              },
        { cudaVideoChromaFormat_444,        "YUV 444"              },
    };

    if (eChromaFormat >= 0 && eChromaFormat < sizeof(aChromaFormatName) / sizeof(aChromaFormatName[0])) {
        return aChromaFormatName[eChromaFormat].name;
    }
    return "Unknown";
}

static float GetChromaHeightFactor(cudaVideoSurfaceFormat eSurfaceFormat)
{
    float factor = 0.5;
    switch (eSurfaceFormat)
    {
    case cudaVideoSurfaceFormat_NV12:
    case cudaVideoSurfaceFormat_P016:
        factor = 0.5;
        break;
    case cudaVideoSurfaceFormat_YUV444:
    case cudaVideoSurfaceFormat_YUV444_16Bit:
        factor = 1.0;
        break;
    }

    return factor;
}

static int GetChromaPlaneCount(cudaVideoSurfaceFormat eSurfaceFormat)
{
    int numPlane = 1;
    switch (eSurfaceFormat)
    {
    case cudaVideoSurfaceFormat_NV12:
    case cudaVideoSurfaceFormat_P016:
        numPlane = 1;
        break;
    case cudaVideoSurfaceFormat_YUV444:
    case cudaVideoSurfaceFormat_YUV444_16Bit:
        numPlane = 2;
        break;
    }

    return numPlane;
}


/* Called when the parser encounters sequence header for AV1 SVC content
*  return value interpretation:
*      < 0 : fail, >=0: succeeded (bit 0-9: currOperatingPoint, bit 10-10: bDispAllLayer, bit 11-30: reserved, must be set 0)
*/
int NvDecoder::GetOperatingPoint(CUVIDOPERATINGPOINTINFO *pOPInfo)
{
    if (pOPInfo->codec == cudaVideoCodec_AV1)
    {
        if (pOPInfo->av1.operating_points_cnt > 1)
        {
            // clip has SVC enabled
            if (m_nOperatingPoint >= pOPInfo->av1.operating_points_cnt)
                m_nOperatingPoint = 0;

            printf("AV1 SVC clip: operating point count %d  ", pOPInfo->av1.operating_points_cnt);
            printf("Selected operating point: %d, IDC 0x%x bOutputAllLayers %d\n", m_nOperatingPoint, pOPInfo->av1.operating_points_idc[m_nOperatingPoint], m_bDispAllLayers);
            return (m_nOperatingPoint | (m_bDispAllLayers << 10));
        }
    }
    return -1;
}

/* Return value from HandleVideoSequence() are interpreted as   :
*  0: fail, 1: succeeded, > 1: override dpb size of parser (set by CUVIDPARSERPARAMS::ulMaxNumDecodeSurfaces while creating parser)
*/
int NvDecoder::HandleVideoSequence(CUVIDEOFORMAT *pVideoFormat)
{
    m_videoInfo.str("");
    m_videoInfo.clear();
    m_videoInfo << "Video Input Information" << std::endl
        << "\tCodec        : " << GetVideoCodecString(pVideoFormat->codec) << std::endl
        << "\tFrame rate   : " << pVideoFormat->frame_rate.numerator << "/" << pVideoFormat->frame_rate.denominator
            << " = " << 1.0 * pVideoFormat->frame_rate.numerator / pVideoFormat->frame_rate.denominator << " fps" << std::endl
        << "\tSequence     : " << (pVideoFormat->progressive_sequence ? "Progressive" : "Interlaced") << std::endl
        << "\tCoded size   : [" << pVideoFormat->coded_width << ", " << pVideoFormat->coded_height << "]" << std::endl
        << "\tDisplay area : [" << pVideoFormat->display_area.left << ", " << pVideoFormat->display_area.top << ", "
            << pVideoFormat->display_area.right << ", " << pVideoFormat->display_area.bottom << "]" << std::endl
        << "\tChroma       : " << GetVideoChromaFormatString(pVideoFormat->chroma_format) << std::endl
        << "\tBit depth    : " << pVideoFormat->bit_depth_luma_minus8 + 8
    ;
    m_videoInfo << std::endl;

    int nDecodeSurface = pVideoFormat->min_num_decode_surfaces;

    CUVIDDECODECAPS decodecaps;
    memset(&decodecaps, 0, sizeof(decodecaps));

    decodecaps.eCodecType = pVideoFormat->codec;
    decodecaps.eChromaFormat = pVideoFormat->chroma_format;
    decodecaps.nBitDepthMinus8 = pVideoFormat->bit_depth_luma_minus8;

    CUDA_DRVAPI_CALL(cuCtxPushCurrent(m_cuContext));
    NVDEC_API_CALL(cuvidGetDecoderCaps(&decodecaps));
    CUDA_DRVAPI_CALL(cuCtxPopCurrent(NULL));

    if(!decodecaps.bIsSupported){
        NVDEC_THROW_ERROR("Codec not supported on this GPU", CUDA_ERROR_NOT_SUPPORTED);
        return nDecodeSurface;
    }

    if ((pVideoFormat->coded_width > decodecaps.nMaxWidth) ||
        (pVideoFormat->coded_height > decodecaps.nMaxHeight)){

        std::ostringstream errorString;
        errorString << std::endl
                    << "Resolution          : " << pVideoFormat->coded_width << "x" << pVideoFormat->coded_height << std::endl
                    << "Max Supported (wxh) : " << decodecaps.nMaxWidth << "x" << decodecaps.nMaxHeight << std::endl
                    << "Resolution not supported on this GPU";

        const std::string cErr = errorString.str();
        NVDEC_THROW_ERROR(cErr, CUDA_ERROR_NOT_SUPPORTED);
        return nDecodeSurface;
    }

    if ((pVideoFormat->coded_width>>4)*(pVideoFormat->coded_height>>4) > decodecaps.nMaxMBCount){

        std::ostringstream errorString;
        errorString << std::endl
                    << "MBCount             : " << (pVideoFormat->coded_width >> 4)*(pVideoFormat->coded_height >> 4) << std::endl
                    << "Max Supported mbcnt : " << decodecaps.nMaxMBCount << std::endl
                    << "MBCount not supported on this GPU";

        const std::string cErr = errorString.str();
        NVDEC_THROW_ERROR(cErr, CUDA_ERROR_NOT_SUPPORTED);
        return nDecodeSurface;
    }

    // eCodec has been set in the constructor (for parser). Here it's set again for potential correction
    m_eCodec = pVideoFormat->codec;
    m_eChromaFormat = pVideoFormat->chroma_format;
    m_nBitDepthMinus8 = pVideoFormat->bit_depth_luma_minus8;
    m_nBPP = m_nBitDepthMinus8 > 0 ? 2 : 1;

    // Set the output surface format same as chroma format
    if (m_eChromaFormat == cudaVideoChromaFormat_420 || cudaVideoChromaFormat_Monochrome)
        m_eOutputFormat = pVideoFormat->bit_depth_luma_minus8 ? cudaVideoSurfaceFormat_P016 : cudaVideoSurfaceFormat_NV12;
    else if (m_eChromaFormat == cudaVideoChromaFormat_444)
        m_eOutputFormat = pVideoFormat->bit_depth_luma_minus8 ? cudaVideoSurfaceFormat_YUV444_16Bit : cudaVideoSurfaceFormat_YUV444;
    else if (m_eChromaFormat == cudaVideoChromaFormat_422)
        m_eOutputFormat = cudaVideoSurfaceFormat_NV12;  // no 4:2:2 output format supported yet so make 420 default

    // Check if output format supported. If not, check falback options
    if (!(decodecaps.nOutputFormatMask & (1 << m_eOutputFormat)))
    {
        if (decodecaps.nOutputFormatMask & (1 << cudaVideoSurfaceFormat_NV12))
            m_eOutputFormat = cudaVideoSurfaceFormat_NV12;
        else if (decodecaps.nOutputFormatMask & (1 << cudaVideoSurfaceFormat_P016))
            m_eOutputFormat = cudaVideoSurfaceFormat_P016;
        else if (decodecaps.nOutputFormatMask & (1 << cudaVideoSurfaceFormat_YUV444))
            m_eOutputFormat = cudaVideoSurfaceFormat_YUV444;
        else if (decodecaps.nOutputFormatMask & (1 << cudaVideoSurfaceFormat_YUV444_16Bit))
            m_eOutputFormat = cudaVideoSurfaceFormat_YUV444_16Bit;
        else 
            NVDEC_THROW_ERROR("No supported output format found", CUDA_ERROR_NOT_SUPPORTED);
    }
    m_videoFormat = *pVideoFormat;

    CUVIDDECODECREATEINFO videoDecodeCreateInfo = { 0 };
    videoDecodeCreateInfo.CodecType = pVideoFormat->codec;
    videoDecodeCreateInfo.ChromaFormat = pVideoFormat->chroma_format;
    videoDecodeCreateInfo.OutputFormat = m_eOutputFormat;
    videoDecodeCreateInfo.bitDepthMinus8 = pVideoFormat->bit_depth_luma_minus8;
    if (pVideoFormat->progressive_sequence)
        videoDecodeCreateInfo.DeinterlaceMode = cudaVideoDeinterlaceMode_Weave;
    else
        videoDecodeCreateInfo.DeinterlaceMode = cudaVideoDeinterlaceMode_Adaptive;
    videoDecodeCreateInfo.ulNumOutputSurfaces = 2;
    // With PreferCUVID, JPEG is still decoded by CUDA while video is decoded by NVDEC hardware
    videoDecodeCreateInfo.ulCreationFlags = cudaVideoCreate_PreferCUVID;
    videoDecodeCreateInfo.ulNumDecodeSurfaces = nDecodeSurface;
    videoDecodeCreateInfo.vidLock = m_ctxLock;
    videoDecodeCreateInfo.ulWidth = pVideoFormat->coded_width;
    videoDecodeCreateInfo.ulHeight = pVideoFormat->coded_height;

    unsigned int maxHeight = 0;
    unsigned int maxWidth = 0;
    // AV1 has max width/height of sequence in sequence header
    if (pVideoFormat->codec == cudaVideoCodec_AV1 && pVideoFormat->seqhdr_data_length > 0)
    {
        CUVIDEOFORMATEX *vidFormatEx = (CUVIDEOFORMATEX *)pVideoFormat;
        maxWidth = vidFormatEx->av1.max_width;
        maxHeight = vidFormatEx->av1.max_height;
    }
    if (maxWidth < pVideoFormat->coded_width) {
        maxWidth = pVideoFormat->coded_width;
    }
    if (maxHeight < pVideoFormat->coded_height) {
        maxHeight = pVideoFormat->coded_height;
    }
    videoDecodeCreateInfo.ulMaxWidth = maxWidth;
    videoDecodeCreateInfo.ulMaxHeight = maxHeight;

    m_nWidth = pVideoFormat->display_area.right - pVideoFormat->display_area.left;
    m_nLumaHeight = pVideoFormat->display_area.bottom - pVideoFormat->display_area.top;
    videoDecodeCreateInfo.ulTargetWidth = pVideoFormat->coded_width;
    videoDecodeCreateInfo.ulTargetHeight = pVideoFormat->coded_height;

    m_nChromaHeight = (int)(ceil((float)m_nLumaHeight * GetChromaHeightFactor(m_eOutputFormat)));
    m_nNumChromaPlanes = GetChromaPlaneCount(m_eOutputFormat);
    m_nSurfaceHeight = (int) videoDecodeCreateInfo.ulTargetHeight;
    m_nSurfaceWidth = (int) videoDecodeCreateInfo.ulTargetWidth;

    m_videoInfo << "Video Decoding Params:" << std::endl
        << "\tNum Surfaces : " << videoDecodeCreateInfo.ulNumDecodeSurfaces << std::endl
        << "\tCrop         : [" << videoDecodeCreateInfo.display_area.left << ", " << videoDecodeCreateInfo.display_area.top << ", "
        << videoDecodeCreateInfo.display_area.right << ", " << videoDecodeCreateInfo.display_area.bottom << "]" << std::endl
        << "\tResize       : " << videoDecodeCreateInfo.ulTargetWidth << "x" << videoDecodeCreateInfo.ulTargetHeight << std::endl
        << "\tDeinterlace  : " << std::vector<const char *>{"Weave", "Bob", "Adaptive"}[videoDecodeCreateInfo.DeinterlaceMode]
    ;
    m_videoInfo << std::endl;

    CUDA_DRVAPI_CALL(cuCtxPushCurrent(m_cuContext));
    NVDEC_API_CALL(cuvidCreateDecoder(&m_hDecoder, &videoDecodeCreateInfo));
    CUDA_DRVAPI_CALL(cuCtxPopCurrent(NULL));
    return nDecodeSurface;
}


/* Return value from HandlePictureDecode() are interpreted as:
*  0: fail, >=1: succeeded
*/
int NvDecoder::HandlePictureDecode(CUVIDPICPARAMS *pPicParams) {
    if (!m_hDecoder)
    {
        NVDEC_THROW_ERROR("Decoder not initialized.", CUDA_ERROR_NOT_INITIALIZED);
        return false;
    }
    CUDA_DRVAPI_CALL(cuCtxPushCurrent(m_cuContext));
    NVDEC_API_CALL(cuvidDecodePicture(m_hDecoder, pPicParams));
    if ((!pPicParams->field_pic_flag) || (pPicParams->second_field))
    {
        CUVIDPARSERDISPINFO dispInfo;
        memset(&dispInfo, 0, sizeof(dispInfo));
        dispInfo.picture_index = pPicParams->CurrPicIdx;
        dispInfo.progressive_frame = !pPicParams->field_pic_flag;
        dispInfo.top_field_first = pPicParams->bottom_field_flag ^ 1;
        HandlePictureDisplay(&dispInfo);
    }
    CUDA_DRVAPI_CALL(cuCtxPopCurrent(NULL));
    return 1;
}

/* Return value from HandlePictureDisplay() are interpreted as:
*  0: fail, >=1: succeeded
*/
int NvDecoder::HandlePictureDisplay(CUVIDPARSERDISPINFO *pDispInfo) {
    CUVIDPROCPARAMS videoProcessingParameters = {};
    videoProcessingParameters.progressive_frame = pDispInfo->progressive_frame;
    videoProcessingParameters.second_field = pDispInfo->repeat_first_field + 1;
    videoProcessingParameters.top_field_first = pDispInfo->top_field_first;
    videoProcessingParameters.unpaired_field = pDispInfo->repeat_first_field < 0;
    videoProcessingParameters.output_stream = m_cuvidStream;

    CUdeviceptr dpSrcFrame = 0;
    unsigned int nSrcPitch = 0;
    CUDA_DRVAPI_CALL(cuCtxPushCurrent(m_cuContext));
    NVDEC_API_CALL(cuvidMapVideoFrame(m_hDecoder, pDispInfo->picture_index, &dpSrcFrame,
        &nSrcPitch, &videoProcessingParameters));

    CUVIDGETDECODESTATUS DecodeStatus;
    memset(&DecodeStatus, 0, sizeof(DecodeStatus));
    CUresult result = cuvidGetDecodeStatus(m_hDecoder, pDispInfo->picture_index, &DecodeStatus);
    if (result == CUDA_SUCCESS && (DecodeStatus.decodeStatus == cuvidDecodeStatus_Error || DecodeStatus.decodeStatus == cuvidDecodeStatus_Error_Concealed))
    {
        printf("Decode Error occurred for picture.\n");
    }

    uint8_t *pDecodedFrame = nullptr;
    {
        if ((unsigned)++m_nDecodedFrame > m_vpFrame.size())
        {
            // Not enough frames in stock
            m_nFrameAlloc++;
            uint8_t *pFrame = NULL;
            pFrame = new uint8_t[GetFrameSize()];
            m_vpFrame.push_back(pFrame);
        }
        pDecodedFrame = m_vpFrame[m_nDecodedFrame - 1];
    }

    // Copy luma plane
    CUDA_MEMCPY2D m = { 0 };
    m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
    m.srcDevice = dpSrcFrame;
    m.srcPitch = nSrcPitch;
    m.dstMemoryType = CU_MEMORYTYPE_HOST;
    m.dstDevice = (CUdeviceptr)(m.dstHost = pDecodedFrame);
    m.dstPitch = GetWidth() * m_nBPP;
    m.WidthInBytes = GetWidth() * m_nBPP;
    m.Height = m_nLumaHeight;
    CUDA_DRVAPI_CALL(cuMemcpy2DAsync(&m, m_cuvidStream));

    // Copy chroma plane
    // NVDEC output has luma height aligned by 2. Adjust chroma offset by aligning height
    m.srcDevice = (CUdeviceptr)((uint8_t *)dpSrcFrame + m.srcPitch * ((m_nSurfaceHeight + 1) & ~1));
    m.dstDevice = (CUdeviceptr)(m.dstHost = pDecodedFrame + m.dstPitch * m_nLumaHeight);
    m.Height = m_nChromaHeight;
    CUDA_DRVAPI_CALL(cuMemcpy2DAsync(&m, m_cuvidStream));

    if (m_nNumChromaPlanes == 2)
    {
        m.srcDevice = (CUdeviceptr)((uint8_t *)dpSrcFrame + m.srcPitch * ((m_nSurfaceHeight + 1) & ~1) * 2);
        m.dstDevice = (CUdeviceptr)(m.dstHost = pDecodedFrame + m.dstPitch * m_nLumaHeight * 2);
        m.Height = m_nChromaHeight;
        CUDA_DRVAPI_CALL(cuMemcpy2DAsync(&m, m_cuvidStream));
    }
    CUDA_DRVAPI_CALL(cuStreamSynchronize(m_cuvidStream));
    CUDA_DRVAPI_CALL(cuCtxPopCurrent(NULL));

    NVDEC_API_CALL(cuvidUnmapVideoFrame(m_hDecoder, dpSrcFrame));
    return 1;
}

NvDecoder::NvDecoder(CUcontext cuContext, cudaVideoCodec eCodec) :
    m_cuContext(cuContext), m_eCodec(eCodec)
{
    NVDEC_API_CALL(cuvidCtxLockCreate(&m_ctxLock, cuContext));

    ck(cuStreamCreate(&m_cuvidStream, CU_STREAM_DEFAULT));

    CUVIDPARSERPARAMS videoParserParameters = {};
    videoParserParameters.CodecType = eCodec;
    videoParserParameters.ulMaxNumDecodeSurfaces = 1;
    videoParserParameters.ulClockRate = 1000;
    videoParserParameters.ulMaxDisplayDelay = 0;
    videoParserParameters.pUserData = this;
    videoParserParameters.pfnSequenceCallback = HandleVideoSequenceProc;
    videoParserParameters.pfnDecodePicture = HandlePictureDecodeProc;
    videoParserParameters.pfnDisplayPicture = NULL;
    videoParserParameters.pfnGetOperatingPoint = HandleOperatingPointProc;
    videoParserParameters.pfnGetSEIMsg = NULL;
    NVDEC_API_CALL(cuvidCreateVideoParser(&m_hParser, &videoParserParameters));
}

NvDecoder::~NvDecoder() {

    if (m_hParser) {
        cuvidDestroyVideoParser(m_hParser);
    }
    cuCtxPushCurrent(m_cuContext);
    if (m_hDecoder) {
        cuvidDestroyDecoder(m_hDecoder);
    }

    for (uint8_t *pFrame : m_vpFrame)
    {
        delete[] pFrame;
    }
    cuCtxPopCurrent(NULL);

    cuvidCtxLockDestroy(m_ctxLock);
}

int NvDecoder::Decode(const uint8_t *pData, size_t nSize)
{
    m_nDecodedFrame = 0;
    m_nDecodedFrameReturned = 0;
    CUVIDSOURCEDATAPACKET packet = { 0 };
    packet.payload = pData;
    packet.payload_size = nSize;
    packet.flags = CUVID_PKT_ENDOFSTREAM;
    packet.timestamp = 0;
    NVDEC_API_CALL(cuvidParseVideoData(m_hParser, &packet));

    return m_nDecodedFrame;
}

uint8_t* NvDecoder::GetFrame()
{
    if (m_nDecodedFrame > 0)
    {
        m_nDecodedFrame--;
        return m_vpFrame[m_nDecodedFrameReturned++];
    }

    return NULL;
}
