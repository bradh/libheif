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

#include "catch.hpp"
#include "libheif/heif.h"
#include "libheif/api_structs.h"
#include <cstdint>
#include <stdio.h>
#include "test-config.h"
#include "test_utils.h"
#include <string.h>

#define FILES \
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
"uncompressed_comp_B16R16G16.heif", \
"uncompressed_comp_B16R16G16_tiled.heif", \
"uncompressed_comp_p.heif", \
"uncompressed_comp_p_tiled.heif", \
"uncompressed_pix_B16R16G16.heif", \
"uncompressed_pix_B16R16G16_tiled.heif", \
"uncompressed_pix_p.heif", \
"uncompressed_pix_p_tiled.heif", \
"uncompressed_row_B16R16G16.heif", \
"uncompressed_row_B16R16G16_tiled.heif", \
"uncompressed_row_p.heif", \
"uncompressed_row_p_tiled.heif", \
"uncompressed_tile_B16R16G16_tiled.heif", \
"uncompressed_tile_p_tiled.heif", \

#endif

void check_image_handle_size(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  int ispe_width = heif_image_handle_get_ispe_width(handle);
  REQUIRE(ispe_width == 30);
  int ispe_height = heif_image_handle_get_ispe_height(handle);
  REQUIRE(ispe_height == 20);
  int width = heif_image_handle_get_width(handle);
  REQUIRE(width == 30);
  int height = heif_image_handle_get_height(handle);
  REQUIRE(height == 20);

  heif_image_handle_release(handle);
}

TEST_CASE("check image handle size") {
  auto file = GENERATE(FILES, MONO_FILES, YUV_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_handle_size(context);
  heif_context_free(context);
}

void check_image_handle_size_subsampled(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  int ispe_width = heif_image_handle_get_ispe_width(handle);
  REQUIRE(ispe_width == 32);
  int ispe_height = heif_image_handle_get_ispe_height(handle);
  REQUIRE(ispe_height == 20);
  int width = heif_image_handle_get_width(handle);
  REQUIRE(width == 32);
  int height = heif_image_handle_get_height(handle);
  REQUIRE(height == 20);

  heif_image_handle_release(handle);
}


TEST_CASE("check image handle size subsampled") {
  auto file = GENERATE(YUV_422_FILES, YUV_420_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_handle_size_subsampled(context);
  heif_context_free(context);
}

TEST_CASE("check image handle alpha channel") {
  auto file = GENERATE(FILES, MONO_FILES, ALL_YUV_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  // int expect_alpha = (strchr(file, 'A') == NULL) ? 0 : 1;
  int expect_alpha = 0; // TODO: fix this
  heif_image_handle *handle = get_primary_image_handle(context);
  int has_alpha = heif_image_handle_has_alpha_channel(handle);
  REQUIRE(has_alpha == expect_alpha);

  heif_image_handle_release(handle);
  heif_context_free(context);
}

void check_image_handle_no_depth_images(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);

  int has_depth = heif_image_handle_has_depth_image(handle);
  REQUIRE(has_depth == 0);

  int numdepth = heif_image_handle_get_number_of_depth_images(handle);
  REQUIRE(numdepth == 0);

  heif_image_handle_release(handle);
}

TEST_CASE("check image handle no depth images") {
  auto file = GENERATE(FILES, MONO_FILES, ALL_YUV_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_handle_no_depth_images(context);
  heif_context_free(context);
}

void check_image_handle_no_thumbnails(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);

  int numthumbs = heif_image_handle_get_number_of_thumbnails(handle);
  REQUIRE(numthumbs == 0);

  heif_image_handle_release(handle);
}

TEST_CASE("check image handle no thumbnails") {
  auto file = GENERATE(FILES, MONO_FILES, ALL_YUV_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_handle_no_thumbnails(context);
  heif_context_free(context);
}

void check_image_handle_no_aux_images(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);

  int num_aux = heif_image_handle_get_number_of_auxiliary_images(handle, 0);
  REQUIRE(num_aux == 0);

  heif_image_handle_release(handle);
}

TEST_CASE("check image handle no auxiliary images") {
  auto file = GENERATE(FILES, MONO_FILES, ALL_YUV_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_handle_no_aux_images(context);
  heif_context_free(context);
}

void check_image_handle_no_metadata(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);

  int num_metadata_blocks =
      heif_image_handle_get_number_of_metadata_blocks(handle, NULL);
  REQUIRE(num_metadata_blocks == 0);

  heif_image_handle_release(handle);
}

TEST_CASE("check image handle no metadata blocks") {
  auto file = GENERATE(FILES, MONO_FILES, ALL_YUV_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_handle_no_metadata(context);
  heif_context_free(context);
}

void check_image_size(struct heif_context *&context, int expect_alpha) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image(handle);

  REQUIRE(heif_image_has_channel(img, heif_channel_Y) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cb) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cr) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_R) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_G) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_B) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_Alpha) == expect_alpha);
  REQUIRE(heif_image_has_channel(img, heif_channel_interleaved) == 0);
  int width = heif_image_get_primary_width(img);
  REQUIRE(width == 30);
  int height = heif_image_get_primary_height(img);
  REQUIRE(height == 20);
  width = heif_image_get_width(img, heif_channel_R);
  REQUIRE(width == 30);
  height = heif_image_get_height(img, heif_channel_R);
  REQUIRE(height == 20);
  width = heif_image_get_width(img, heif_channel_G);
  REQUIRE(width == 30);
  height = heif_image_get_height(img, heif_channel_G);
  REQUIRE(height == 20);
  width = heif_image_get_width(img, heif_channel_B);
  REQUIRE(width == 30);
  height = heif_image_get_height(img, heif_channel_B);
  REQUIRE(height == 20);
  if (expect_alpha == 1) {
    width = heif_image_get_width(img, heif_channel_Alpha);
    REQUIRE(width == 30);
    height = heif_image_get_height(img, heif_channel_Alpha);
    REQUIRE(height == 20);
  }

  int pixel_depth = heif_image_get_bits_per_pixel(img, heif_channel_R);
  REQUIRE(pixel_depth == 8);
  pixel_depth = heif_image_get_bits_per_pixel(img, heif_channel_G);
  REQUIRE(pixel_depth == 8);
  pixel_depth = heif_image_get_bits_per_pixel(img, heif_channel_B);
  REQUIRE(pixel_depth == 8);

  int pixel_range = heif_image_get_bits_per_pixel_range(img, heif_channel_R);
  REQUIRE(pixel_range == 8);
  pixel_range = heif_image_get_bits_per_pixel_range(img, heif_channel_G);
  REQUIRE(pixel_range == 8);
  pixel_range = heif_image_get_bits_per_pixel_range(img, heif_channel_B);
  REQUIRE(pixel_range == 8);

  heif_image_release(img);
  heif_image_handle_release(handle);
}

TEST_CASE("check image size") {
  auto file = GENERATE(FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  int expect_alpha = (strchr(file, 'A') == NULL) ? 0 : 1;
  check_image_size(context, expect_alpha);
  heif_context_free(context);
}

void check_image_size_mono(struct heif_context *&context, int expect_alpha) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_mono(handle);

  REQUIRE(heif_image_has_channel(img, heif_channel_Y) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cb) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cr) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_R) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_G) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_B) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_Alpha) == expect_alpha);
  REQUIRE(heif_image_has_channel(img, heif_channel_interleaved) == 0);
  int width = heif_image_get_primary_width(img);
  REQUIRE(width == 30);
  int height = heif_image_get_primary_height(img);
  REQUIRE(height == 20);
  width = heif_image_get_width(img, heif_channel_Y);
  REQUIRE(width == 30);
  height = heif_image_get_height(img, heif_channel_Y);
  REQUIRE(height == 20);
  if (expect_alpha == 1) {
    width = heif_image_get_width(img, heif_channel_Alpha);
    REQUIRE(width == 30);
    height = heif_image_get_height(img, heif_channel_Alpha);
    REQUIRE(height == 20);
  }

  int pixel_depth = heif_image_get_bits_per_pixel(img, heif_channel_Y);
  REQUIRE(pixel_depth == 8);

  int pixel_range = heif_image_get_bits_per_pixel_range(img, heif_channel_Y);
  REQUIRE(pixel_range == 8);

  heif_image_release(img);
  heif_image_handle_release(handle);
}

TEST_CASE("check image size mono") {
  auto file = GENERATE(MONO_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  int expect_alpha = (strchr(file, 'A') == NULL) ? 0 : 1;
  check_image_size_mono(context, expect_alpha);
  heif_context_free(context);
}


void check_image_size_ycbcr(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_ycbcr(handle, heif_chroma_444);

  REQUIRE(heif_image_has_channel(img, heif_channel_Y) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cb) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cr) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_R) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_G) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_B) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_Alpha) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_interleaved) == 0);
  int width = heif_image_get_primary_width(img);
  REQUIRE(width == 30);
  int height = heif_image_get_primary_height(img);
  REQUIRE(height == 20);
  width = heif_image_get_width(img, heif_channel_Y);
  REQUIRE(width == 30);
  height = heif_image_get_height(img, heif_channel_Y);
  REQUIRE(height == 20);
  width = heif_image_get_width(img, heif_channel_Cb);
  REQUIRE(width == 30);
  height = heif_image_get_height(img, heif_channel_Cr);
  REQUIRE(height == 20);
  width = heif_image_get_width(img, heif_channel_Cr);
  REQUIRE(width == 30);
  height = heif_image_get_height(img, heif_channel_Cr);
  REQUIRE(height == 20);

  int pixel_depth = heif_image_get_bits_per_pixel(img, heif_channel_Y);
  REQUIRE(pixel_depth == 8);
  pixel_depth = heif_image_get_bits_per_pixel(img, heif_channel_Cb);
  REQUIRE(pixel_depth == 8);
  pixel_depth = heif_image_get_bits_per_pixel(img, heif_channel_Cr);
  REQUIRE(pixel_depth == 8);
  int pixel_range = heif_image_get_bits_per_pixel_range(img, heif_channel_Y);
  REQUIRE(pixel_range == 8);
  pixel_range = heif_image_get_bits_per_pixel_range(img, heif_channel_Cb);
  REQUIRE(pixel_range == 8);
  pixel_range = heif_image_get_bits_per_pixel_range(img, heif_channel_Cr);
  REQUIRE(pixel_range == 8);

  heif_image_release(img);
  heif_image_handle_release(handle);
}

TEST_CASE("check image size YCbCr") {
  auto file = GENERATE(YUV_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_size_ycbcr(context);
  heif_context_free(context);
}

void check_image_size_ycbcr_422(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_ycbcr(handle, heif_chroma_422);

  REQUIRE(heif_image_has_channel(img, heif_channel_Y) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cb) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cr) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_R) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_G) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_B) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_Alpha) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_interleaved) == 0);
  int width = heif_image_get_primary_width(img);
  REQUIRE(width == 32);
  int height = heif_image_get_primary_height(img);
  REQUIRE(height == 20);
  width = heif_image_get_width(img, heif_channel_Y);
  REQUIRE(width == 32);
  height = heif_image_get_height(img, heif_channel_Y);
  REQUIRE(height == 20);
  width = heif_image_get_width(img, heif_channel_Cb);
  REQUIRE(width == 16);
  height = heif_image_get_height(img, heif_channel_Cr);
  REQUIRE(height == 20);
  width = heif_image_get_width(img, heif_channel_Cr);
  REQUIRE(width == 16);
  height = heif_image_get_height(img, heif_channel_Cr);
  REQUIRE(height == 20);

  int pixel_depth = heif_image_get_bits_per_pixel(img, heif_channel_Y);
  REQUIRE(pixel_depth == 8);
  pixel_depth = heif_image_get_bits_per_pixel(img, heif_channel_Cb);
  REQUIRE(pixel_depth == 8);
  pixel_depth = heif_image_get_bits_per_pixel(img, heif_channel_Cr);
  REQUIRE(pixel_depth == 8);
  int pixel_range = heif_image_get_bits_per_pixel_range(img, heif_channel_Y);
  REQUIRE(pixel_range == 8);
  pixel_range = heif_image_get_bits_per_pixel_range(img, heif_channel_Cb);
  REQUIRE(pixel_range == 8);
  pixel_range = heif_image_get_bits_per_pixel_range(img, heif_channel_Cr);
  REQUIRE(pixel_range == 8);

  heif_image_release(img);
  heif_image_handle_release(handle);
}

TEST_CASE("check image size YCbCr 4:2:2") {
  auto file = GENERATE(YUV_422_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_size_ycbcr_422(context);
  heif_context_free(context);
}

void check_image_content(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image(handle);

  int stride;
  const uint8_t *img_plane =
      heif_image_get_plane_readonly(img, heif_channel_R, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 128);
  }
  for (int row = 4; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 255);
  }
  for (int row = 8; row < 12; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 238);
  }
  for (int row = 12; row < 16; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 255);
  }
  for (int row = 16; row < 20; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 0);
  }
  img_plane = heif_image_get_plane_readonly(img, heif_channel_G, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 128);
  }
  for (int row = 4; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 165);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 165);
  }
  for (int row = 8; row < 12; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 165);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 165);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 130);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 130);
  }
  for (int row = 12; row < 16; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 165);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 165);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 130);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 130);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 0);
  }
  for (int row = 16; row < 20; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 165);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 165);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 130);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 130);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 128);
  }
  img_plane = heif_image_get_plane_readonly(img, heif_channel_B, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 128);
  }
  for (int row = 4; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 0);
  }
  for (int row = 8; row < 12; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 238);
  }
  for (int row = 12; row < 16; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 0);
  }
  for (int row = 16; row < 20; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 0);
  }

  heif_image_release(img);
  heif_image_handle_release(handle);
}

TEST_CASE("check image content") {
  auto file = GENERATE(FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_content(context);
  heif_context_free(context);
}

void check_image_content_mono(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_mono(handle);

  int stride;
  const uint8_t *img_plane =
      heif_image_get_plane_readonly(img, heif_channel_Y, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 128);
  }
  for (int row = 4; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 255);
  }
  for (int row = 8; row < 12; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 238);
  }
  for (int row = 12; row < 16; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 255);
  }
  for (int row = 16; row < 20; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 238);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 255);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 0);
  }
}

TEST_CASE("check image content mono") {
  auto file = GENERATE(MONO_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_content_mono(context);
  heif_context_free(context);
}


void check_image_content_ycbcr(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_ycbcr(handle, heif_chroma_444);

  int stride;
  const uint8_t *img_plane =
      heif_image_get_plane_readonly(img, heif_channel_Y, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 128);
  }
  for (int row = 4; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 173);
  }
  for (int row = 8; row < 12; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 174);
  }
  for (int row = 12; row < 16; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 76);
  }
  for (int row = 16; row < 20; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 75);
  }
  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cb, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 127);
  }
  for (int row = 4; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 29);
  }
  for (int row = 8; row < 12; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 163);
  }
  for (int row = 12; row < 16; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 84);
  }
  for (int row = 16; row < 20; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 85);
  }
  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cr, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 127);
  }
  for (int row = 4; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 185);
  }
  for (int row = 8; row < 12; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 172);
  }
  for (int row = 12; row < 16; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 254);
  }
  for (int row = 16; row < 20; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 73);
  }

  heif_image_release(img);
  heif_image_handle_release(handle);
}

TEST_CASE("check image content YCbCr") {
  auto file = GENERATE(YUV_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_content_ycbcr(context);
  heif_context_free(context);
}

void check_image_content_ycbcr422(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_ycbcr(handle, heif_chroma_422);

  int stride;
  const uint8_t *img_plane =
      heif_image_get_plane_readonly(img, heif_channel_Y, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 128);
  }
  for (int row = 4; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 173);
  }
  for (int row = 8; row < 12; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 174);
  }
  for (int row = 12; row < 16; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 76);
  }
  for (int row = 16; row < 20; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 75);
  }
  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cb, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 127);
  }
  for (int row = 4; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 29);
  }
  for (int row = 8; row < 12; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 163);
  }
  for (int row = 12; row < 16; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 84);
  }
  for (int row = 16; row < 20; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 85);
  }
  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cr, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 127);
  }
  for (int row = 4; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 185);
  }
  for (int row = 8; row < 12; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 172);
  }
  for (int row = 12; row < 16; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 254);
  }
  for (int row = 16; row < 20; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 73);
  }

  heif_image_release(img);
  heif_image_handle_release(handle);
}

TEST_CASE("check image content YCbCr 4:2:2") {
  auto file = GENERATE(YUV_422_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_content_ycbcr422(context);
  heif_context_free(context);
}

void check_image_content_ycbcr420(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_ycbcr(handle, heif_chroma_420);

  int stride;
  const uint8_t *img_plane =
      heif_image_get_plane_readonly(img, heif_channel_Y, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 128);
  }
  for (int row = 4; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 173);
  }
  for (int row = 8; row < 12; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 174);
  }
  for (int row = 12; row < 16; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 76);
  }
  for (int row = 16; row < 20; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 225);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 178);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 128);
    REQUIRE(((int)(img_plane[stride * row + 16])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 19])) == 173);
    REQUIRE(((int)(img_plane[stride * row + 20])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 23])) == 174);
    REQUIRE(((int)(img_plane[stride * row + 24])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 27])) == 76);
    REQUIRE(((int)(img_plane[stride * row + 28])) == 75);
    REQUIRE(((int)(img_plane[stride * row + 29])) == 75);
  }
  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cb, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 2; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 127);
  }
  for (int row = 2; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 29);
  }
  for (int row = 4; row < 6; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 163);
  }
  for (int row = 6; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 84);
  }
  for (int row = 8; row < 10; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 170);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 29);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 163);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 84);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 85);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 85);
  }
  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cr, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 2; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 127);
  }
  for (int row = 2; row < 4; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 185);
  }
  for (int row = 4; row < 6; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 106);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 172);
  }
  for (int row = 6; row < 8; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 254);
  }
  for (int row = 8; row < 10; row++) {
    INFO("row: " << row);
    REQUIRE(((int)(img_plane[stride * row + 0])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 1])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 2])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 3])) == 148);
    REQUIRE(((int)(img_plane[stride * row + 4])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 5])) == 0);
    REQUIRE(((int)(img_plane[stride * row + 6])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 7])) == 127);
    REQUIRE(((int)(img_plane[stride * row + 8])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 9])) == 185);
    REQUIRE(((int)(img_plane[stride * row + 10])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 11])) == 172);
    REQUIRE(((int)(img_plane[stride * row + 12])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 13])) == 254);
    REQUIRE(((int)(img_plane[stride * row + 14])) == 73);
    REQUIRE(((int)(img_plane[stride * row + 15])) == 73);
  }

  heif_image_release(img);
  heif_image_handle_release(handle);
}

TEST_CASE("check image content YCbCr 4:2:0") {
  auto file = GENERATE(YUV_420_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_content_ycbcr420(context);
  heif_context_free(context);
}

