/*
 * NVIDIA Decoder.
 * Copyright (c) 2023 Brad Hards <bradh@frogmouth.net>
 *
 * This file is part of libheif.
 *
 * libheif is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libheif is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libheif.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "libheif/heif.h"
#include "libheif/heif_plugin.h"
#include "libheif/security_limits.h"
#include "libheif/common_utils.h"
#include "decoder_nvdec.h"
#include <memory>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cuda.h>
#include <iostream>
#include "nvcuvid.h"
#include "cuviddec.h"
#include "NvDecoder.h"
#include "nalu.h"
#include <mutex>

static const int NVDEC_PLUGIN_PRIORITY = 120;

#define MAX_PLUGIN_NAME_LENGTH 80

static char plugin_name[MAX_PLUGIN_NAME_LENGTH];

static const char *nvdec_plugin_name()
{
    snprintf(plugin_name, MAX_PLUGIN_NAME_LENGTH, "NVIDIA Video Decoder SDK (Hardware)");

    // make sure that the string is null-terminated
    plugin_name[MAX_PLUGIN_NAME_LENGTH - 1] = 0;

    return plugin_name;
}

static void nvdec_init_plugin()
{
    cuInit(0);
}

static void nvdec_deinit_plugin()
{
}

static int nvdec_does_support_format(enum heif_compression_format format)
{
    // We have to check the hardware capabilities
    CUcontext cuContext = NULL;
    CUdevice cuDevice = 0;

    CUresult result;
    result = cuDeviceGet(&cuDevice, 0);
    if (result != CUDA_SUCCESS)
    {
        return 0;
    }
#if 0
  char szDeviceName[80];
  result = cuDeviceGetName(szDeviceName, sizeof(szDeviceName), cuDevice);
  if (result != CUDA_SUCCESS) {
	return 0;
  }
  std::cout << "GPU in use: " << szDeviceName << std::endl;
#endif
    result = cuCtxCreate(&cuContext, 0, cuDevice);
    if (result != CUDA_SUCCESS)
    {
        return 0;
    }

    CUVIDDECODECAPS decodeCaps = {};
    decodeCaps.eChromaFormat = cudaVideoChromaFormat_420;
    decodeCaps.nBitDepthMinus8 = 0;
    if (format == heif_compression_JPEG)
    {
        decodeCaps.eCodecType = cudaVideoCodec_JPEG;
    }
    else if (format == heif_compression_HEVC)
    {
        decodeCaps.eCodecType = cudaVideoCodec_HEVC;
    }
    else if (format == heif_compression_AVC)
    {
        decodeCaps.eCodecType = cudaVideoCodec_H264;
    }
    else if (format == heif_compression_AV1)
    {
        decodeCaps.eCodecType = cudaVideoCodec_AV1;
    }
    else
    {
        cuCtxDestroy(cuContext);
        return 0;
    }
    result = cuvidGetDecoderCaps(&decodeCaps);
    if (result != CUDA_SUCCESS)
    {
        cuCtxDestroy(cuContext);
        return 0;
    }
    cuCtxDestroy(cuContext);
    return decodeCaps.bIsSupported ? NVDEC_PLUGIN_PRIORITY : 0;
}

struct heif_error nvdec_new_decoder(void **decoder)
{
    struct nvdec_context *ctx = new nvdec_context();
    ctx->strict = false;
    *decoder = ctx;

    return heif_error_ok;
}

void nvdec_free_decoder(void *decoder)
{
    struct nvdec_context *ctx = (nvdec_context *)decoder;

    if (!ctx)
    {
        return;
    }

    delete ctx;
}

struct heif_error nvdec_push_data(void *decoder, const void *frame_data, size_t frame_size)
{
    struct nvdec_context *ctx = (struct nvdec_context *)decoder;

    const uint8_t *input_data = (const uint8_t *)frame_data;

    ctx->data.insert(ctx->data.end(), input_data, input_data + frame_size);

    return heif_error_ok;
}


struct heif_error nvdec_decode_image(void *decoder, struct heif_image **out_img)
{
    struct nvdec_context *ctx = (struct nvdec_context *)decoder;

    NalUnitMap nalus;
    heif_error err = nalus.parseNALU(ctx->data.data(), ctx->data.size());
    if (err.code != heif_error_Ok) {
        return err;
    }
    if ((!nalus.NUTs_are_valid()) || (!nalus.IDR_is_valid())) {
        struct heif_error err = {heif_error_Decoder_plugin_error,
                                 heif_suberror_End_of_data,
                                 "Unexpected end of data"};
        return err;
    }

    // TODO: we should remove this and just use the ctx instance directly
    CUcontext cuContext = NULL;
    CUdevice cuDevice = 0;

    CUresult result;
    result = cuDeviceGet(&cuDevice, 0);
    if (result != CUDA_SUCCESS)
    {
        struct heif_error err = {heif_error_Decoder_plugin_error,
                                 heif_suberror_Plugin_loading_error,
                                 "could not get CUDA device"};
        return err;
    }
    result = cuCtxCreate(&cuContext, 0, cuDevice);
    if (result != CUDA_SUCCESS)
    {
        struct heif_error err = {heif_error_Decoder_plugin_error,
                                 heif_suberror_Plugin_loading_error,
                                 "could not get CUDA context"};
        return err;
    }
    // TODO: we don't want to hard code this
    ctx->eCodec = cudaVideoCodec_HEVC;
    ctx->cuContext = cuContext;
    result = cuvidCtxLockCreate(&(ctx->ctxLock), ctx->cuContext);
    if (result != CUDA_SUCCESS) {
        cuCtxDestroy(cuContext);
        struct heif_error err = {heif_error_Decoder_plugin_error,
                                 heif_suberror_Plugin_loading_error,
                                 "could not create CUDA context lock"};
        return err;
    }
    result = cuStreamCreate(&(ctx->cuvidStream), CU_STREAM_DEFAULT);
    if (result != CUDA_SUCCESS) {
        const char *szErrName = NULL;
        cuGetErrorName(result, &szErrName);
        std::ostringstream errMsg;
        errMsg << "could not create CUDA stream " << szErrName;
        struct heif_error err = {heif_error_Decoder_plugin_error,
                                 heif_suberror_Plugin_loading_error,
                                 errMsg.str().c_str()};
        cuvidCtxLockDestroy(ctx->ctxLock);
        cuCtxDestroy(cuContext);
        return err;
    }

    NvDecoder dec(ctx);
    uint8_t *hevc_data;
    size_t hevc_data_size;
    nalus.buildWithStartCodes(&hevc_data, &hevc_data_size);
    int nFrameReturned = dec.Decode(hevc_data, hevc_data_size);
    if (nFrameReturned > 0) {
        uint8_t *pFrame = dec.GetFrame();

        struct heif_image *heif_img = nullptr;
        // dummy entry for chroma
        err = heif_image_create(dec.GetWidth(), dec.GetHeight(),
                                heif_colorspace_YCbCr,
                                heif_chroma_420,
                                &heif_img);
        if (err.code != heif_error_Ok)
        {
            assert(heif_img == nullptr);
            return err;
        }
        heif_image_add_plane(heif_img, heif_channel_Y, dec.GetWidth(), dec.GetHeight(), dec.GetBitDepth());
        heif_image_add_plane(heif_img, heif_channel_Cb, dec.GetWidth() / 2, dec.GetChromaHeight(), dec.GetBitDepth());
        heif_image_add_plane(heif_img, heif_channel_Cr, dec.GetWidth() / 2, dec.GetChromaHeight(), dec.GetBitDepth());
    
        int strideY;
        uint8_t *Y = heif_image_get_plane(heif_img, heif_channel_Y, &strideY);
        for (int r = 0; r < dec.GetHeight(); r++) {
            memcpy(Y + r * strideY, pFrame, dec.GetWidth() * dec.GetBPP());
            pFrame += dec.GetWidth() * dec.GetBPP();
        }
        int strideCb;
        uint8_t *Cb = heif_image_get_plane(heif_img, heif_channel_Cb, &strideCb);
        for (int r = 0; r < dec.GetChromaHeight(); r++) {
            memcpy(Cb + r * strideCb, pFrame, (dec.GetWidth() / 2) * dec.GetBPP());
            pFrame += (dec.GetWidth() / 2) * dec.GetBPP();
        }
        int strideCr;
        uint8_t *Cr = heif_image_get_plane(heif_img, heif_channel_Cr, &strideCr);
        for (int r = 0; r < dec.GetChromaHeight(); r++) {
            memcpy(Cr + r * strideCr, pFrame, (dec.GetWidth() / 2) * dec.GetBPP());
            pFrame += (dec.GetWidth() / 2) * dec.GetBPP();
        }
        *out_img = heif_img;
    }
    return heif_error_ok;
}

void nvdec_set_strict_decoding(void *decoder, int strict)
{
    struct nvdec_context *ctx = (struct nvdec_context *)decoder;
    ctx->strict = strict;
}

static const struct heif_decoder_plugin decoder_nvdec
{
    3,
        nvdec_plugin_name,
        nvdec_init_plugin,
        nvdec_deinit_plugin,
        nvdec_does_support_format,
        nvdec_new_decoder,
        nvdec_free_decoder,
        nvdec_push_data,
        nvdec_decode_image,
        nvdec_set_strict_decoding,
        "NVDEC"
};

const struct heif_decoder_plugin *get_decoder_plugin_nvdec()
{
    return &decoder_nvdec;
}

#if PLUGIN_NVDEC
heif_plugin_info plugin_info{
    1,
    heif_plugin_type_decoder,
    &decoder_nvdec};
#endif
