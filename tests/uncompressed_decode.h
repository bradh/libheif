/*
  libheif integration tests for uncompressed decoder

  MIT License

  Copyright (c) 2023 Brad Hards <bradh@frogmouth.net>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#define FILES_RGB \
  "uncompressed_comp_ABGR.heif", "uncompressed_comp_ABGR.heif", \
  "uncompressed_comp_RGB.heif", "uncompressed_comp_RGB_tiled.heif", \
  "uncompressed_comp_RGB_tiled_row_tile_align.heif", \
  "uncompressed_comp_RGxB.heif", "uncompressed_comp_RGxB_tiled.heif", \
  "uncompressed_pix_ABGR.heif", "uncompressed_pix_ABGR_tiled.heif", \
  "uncompressed_pix_RGB.heif", "uncompressed_pix_RGB_tiled.heif", \
  "uncompressed_pix_RGxB.heif", "uncompressed_pix_RGxB_tiled.heif", \
  "uncompressed_row_ABGR.heif", "uncompressed_row_ABGR_tiled.heif", \
  "uncompressed_row_RGB.heif", "uncompressed_row_RGB_tiled.heif", \
  "uncompressed_row_RGB_tiled_row_tile_align.heif", \
  "uncompressed_row_RGxB.heif", "uncompressed_row_RGxB_tiled.heif", \
  "uncompressed_tile_ABGR_tiled.heif", \
  "uncompressed_tile_RGB_tiled.heif", \
  "uncompressed_tile_RGB_tiled_row_tile_align.heif", \
  "uncompressed_tile_RGxB_tiled.heif"

// TODO: these need to be turned back on (and tile_ moved to the bottom)
#define FILES_16BIT_RGB \
  "uncompressed_comp_B16R16G16.heif", "uncompressed_comp_B16R16G16_tiled.heif", \
  "uncompressed_row_B16R16G16.heif", "uncompressed_row_B16R16G16_tiled.heif", \
  "uncompressed_tile_B16R16G16_tiled.heif"
  // "uncompressed_pix_B16R16G16.heif", "uncompressed_pix_B16R16G16_tiled.heif",
  
  

#define FILES FILES_RGB, FILES_16BIT_RGB

#define MONO_FILES \
  "uncompressed_comp_M.heif", "uncompressed_comp_M_tiled.heif", \
  "uncompressed_pix_M.heif", "uncompressed_pix_M_tiled.heif", \
  "uncompressed_row_M.heif", "uncompressed_row_M_tiled.heif", \
  "uncompressed_tile_M_tiled.heif"

#define YUV_422_FILES \
  "uncompressed_comp_VUY_422.heif", "uncompressed_comp_YUV_422.heif", "uncompressed_comp_YVU_422.heif", \
  "uncompressed_mix_VUY_422.heif", "uncompressed_mix_YUV_422.heif", "uncompressed_mix_YVU_422.heif"

#define YUV_420_FILES \
  "uncompressed_comp_VUY_420.heif", "uncompressed_comp_YUV_420.heif", "uncompressed_comp_YVU_420.heif", \
  "uncompressed_mix_VUY_420.heif", "uncompressed_mix_YUV_420.heif", "uncompressed_mix_YVU_420.heif"

#define YUV_FILES \
  "uncompressed_comp_YUV_tiled.heif", \
  "uncompressed_pix_YUV_tiled.heif", \
  "uncompressed_row_YUV_tiled.heif", \
  "uncompressed_tile_YUV_tiled.heif"

#define ALL_YUV_FILES \
  YUV_422_FILES, YUV_420_FILES, YUV_FILES

#if 0
"uncompressed_pix_RGB_tiled_row_tile_align.heif", \
"uncompressed_comp_p.heif", \
"uncompressed_comp_p_tiled.heif", \
"uncompressed_pix_p.heif", \
"uncompressed_pix_p_tiled.heif", \
"uncompressed_row_p.heif", \
"uncompressed_row_p_tiled.heif", \
"uncompressed_tile_p_tiled.heif"

#endif
