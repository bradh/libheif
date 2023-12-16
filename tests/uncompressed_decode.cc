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

#define FILES "uncompressed_tile_ABGR_tiled.heif", "uncompressed_tile_RGB_tiled.heif", "uncompressed_tile_RGxB_tiled.heif"
    
#if 0
"uncompressed_comp_ABGR.heif", \
"uncompressed_comp_ABGR_tiled.heif", \
"uncompressed_comp_B16R16G16.heif", \
"uncompressed_comp_B16R16G16_tiled.heif", \
"uncompressed_comp_M.heif", \
"uncompressed_comp_M_tiled.heif", \
"uncompressed_comp_p.heif", \
"uncompressed_comp_p_tiled.heif", \
"uncompressed_comp_RGB.heif", \
"uncompressed_comp_RGB_tiled.heif", \
"uncompressed_comp_RGxB.heif", \
"uncompressed_comp_RGxB_tiled.heif", \
"uncompressed_pix_ABGR.heif", \
"uncompressed_pix_ABGR_tiled.heif", \
"uncompressed_pix_B16R16G16.heif", \
"uncompressed_pix_B16R16G16_tiled.heif", \
"uncompressed_pix_M.heif", \
"uncompressed_pix_M_tiled.heif", \
"uncompressed_pix_p.heif", \
"uncompressed_pix_p_tiled.heif", \
"uncompressed_pix_RGB.heif", \
"uncompressed_pix_RGB_tiled.heif", \
"uncompressed_pix_RGxB.heif", \
"uncompressed_pix_RGxB_tiled.heif", \
"uncompressed_row_ABGR.heif", \
"uncompressed_row_ABGR_tiled.heif", \
"uncompressed_row_B16R16G16.heif", \
"uncompressed_row_B16R16G16_tiled.heif", \
"uncompressed_row_M.heif", \
"uncompressed_row_M_tiled.heif", \
"uncompressed_row_p.heif", \
"uncompressed_row_p_tiled.heif", \
"uncompressed_row_RGB.heif", \
"uncompressed_row_RGB_tiled.heif", \
"uncompressed_row_RGxB.heif", \
"uncompressed_row_RGxB_tiled.heif", \
"uncompressed_tile_B16R16G16_tiled.heif", \
"uncompressed_tile_M_tiled.heif", \
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
  auto file = GENERATE(FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_handle_size(context);
  heif_context_free(context);
}

TEST_CASE("check image handle alpha channel") {
  auto file = GENERATE(FILES);
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
  auto file = GENERATE(FILES);
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
  auto file = GENERATE(FILES);
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
  auto file = GENERATE(FILES);
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
  auto file = GENERATE(FILES);
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
  for (int row = 4; row < 7; row++) {
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
  for (int row = 8; row < 11; row++) {
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
  for (int row = 12; row < 15; row++) {
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
  for (int row = 16; row < 19; row++) {
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
  for (int row = 0; row < 3; row++) {
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
  for (int row = 4; row < 7; row++) {
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
  for (int row = 8; row < 11; row++) {
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
  for (int row = 12; row < 15; row++) {
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
  for (int row = 16; row < 19; row++) {
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
  for (int row = 0; row < 3; row++) {
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
  for (int row = 4; row < 7; row++) {
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
  for (int row = 8; row < 11; row++) {
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
  for (int row = 12; row < 15; row++) {
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
  for (int row = 16; row < 19; row++) {
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
