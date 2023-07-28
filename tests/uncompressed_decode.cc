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

#define RGB_FILES "uncompressed_rgb3.heif", "uncompressed_planar_tiled.heif", "uncompressed_row.heif", \
  "uncompressed_row_tiled.heif", "uncompressed_pix_tile_align.heif", \
  "uncompressed_comp_tile_align.heif", "uncompressed_row_tile_align.heif", \
  "uncompressed_tile_tiled.heif", "uncompressed_tile_tiled_align.heif"

#define YCBCR444_FILES "uncompressed_yuv444_comp_tiled.heif"
#define YCBCR422_FILES "uncompressed_yuv422_comp_tiled.heif"
#define YCBCR420_FILES "uncompressed_yuv420_comp_tiled.heif", "uncompressed_yuv420_mix.heif", "uncompressed_yuv420_mix_tiled.heif"
#define YCBCR_FILES YCBCR444_FILES, YCBCR422_FILES, YCBCR420_FILES

#define ALL_FILES RGB_FILES , YCBCR_FILES

void check_image_handle_size(struct heif_context *&context, const int expected_width, const int expected_height) {
  heif_image_handle *handle = get_primary_image_handle(context);
  int ispe_width = heif_image_handle_get_ispe_width(handle);
  REQUIRE(ispe_width == expected_width);
  int ispe_height = heif_image_handle_get_ispe_height(handle);
  REQUIRE(ispe_height == expected_height);
  int width = heif_image_handle_get_width(handle);
  REQUIRE(width == expected_width);
  int height = heif_image_handle_get_height(handle);
  REQUIRE(height == expected_height);

  heif_image_handle_release(handle);
}

TEST_CASE("check image handle size") {
  auto file = GENERATE(RGB_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_handle_size(context, 20, 10);
  heif_context_free(context);
}

TEST_CASE("check image handle size YCbCr") {
  auto file = GENERATE(YCBCR_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_handle_size(context, 20, 12);
  heif_context_free(context);
}

void check_image_handle_no_alpha(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);

  int has_alpha = heif_image_handle_has_alpha_channel(handle);
  REQUIRE(has_alpha == 0);

  heif_image_handle_release(handle);
}

TEST_CASE("check image handle no alpha channel") {
  auto file = GENERATE(ALL_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_handle_no_alpha(context);
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
  auto file = GENERATE(ALL_FILES);
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
  auto file = GENERATE(ALL_FILES);
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
  auto file = GENERATE(ALL_FILES);
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
  auto file = GENERATE(ALL_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_handle_no_metadata(context);
  heif_context_free(context);
}

void check_image_size_rgb(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_rgb(handle);

  REQUIRE(heif_image_has_channel(img, heif_channel_Y) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cb) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cr) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_R) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_G) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_B) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_Alpha) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_interleaved) == 0);
  int width = heif_image_get_primary_width(img);
  REQUIRE(width == 20);
  int height = heif_image_get_primary_height(img);
  REQUIRE(height == 10);
  width = heif_image_get_width(img, heif_channel_R);
  REQUIRE(width == 20);
  height = heif_image_get_height(img, heif_channel_R);
  REQUIRE(height == 10);
  width = heif_image_get_width(img, heif_channel_G);
  REQUIRE(width == 20);
  height = heif_image_get_height(img, heif_channel_G);
  REQUIRE(height == 10);
  width = heif_image_get_width(img, heif_channel_B);
  REQUIRE(width == 20);
  height = heif_image_get_height(img, heif_channel_B);
  REQUIRE(height == 10);

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

void check_image_size_ycbcr(struct heif_context *&context, const heif_chroma chroma, const int expected_width, const int expected_height, const int expected_chroma_width, const int expected_chroma_height) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_ycbcr(handle, chroma);
  REQUIRE(heif_image_has_channel(img, heif_channel_Y) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cb) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_Cr) == 1);
  REQUIRE(heif_image_has_channel(img, heif_channel_R) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_G) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_B) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_Alpha) == 0);
  REQUIRE(heif_image_has_channel(img, heif_channel_interleaved) == 0);
  int width = heif_image_get_primary_width(img);
  REQUIRE(width == expected_width);
  int height = heif_image_get_primary_height(img);
  REQUIRE(height == expected_height);
  width = heif_image_get_width(img, heif_channel_Y);
  REQUIRE(width == expected_width);
  height = heif_image_get_height(img, heif_channel_Y);
  REQUIRE(height == expected_height);
  width = heif_image_get_width(img, heif_channel_Cb);
  REQUIRE(width == expected_chroma_width);
  height = heif_image_get_height(img, heif_channel_Cb);
  REQUIRE(height == expected_chroma_height);
  width = heif_image_get_width(img, heif_channel_Cr);
  REQUIRE(width == expected_chroma_width);
  height = heif_image_get_height(img, heif_channel_Cr);
  REQUIRE(height == expected_chroma_height);

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

TEST_CASE("check image size RGB") {
  auto file = GENERATE(RGB_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_size_rgb(context);
  heif_context_free(context);
}

TEST_CASE("check image size YCbCr444") {
  auto file = GENERATE(YCBCR444_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_size_ycbcr(context, heif_chroma_444, 20, 12, 20, 12);
  heif_context_free(context);
}

TEST_CASE("check image size YCbCr422") {
  auto file = GENERATE(YCBCR422_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_size_ycbcr(context, heif_chroma_422, 20, 12, 10, 12);
  heif_context_free(context);
}

TEST_CASE("check image size YCbCr420") {
  auto file = GENERATE(YCBCR420_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_size_ycbcr(context, heif_chroma_420, 20, 12, 10, 6);
  heif_context_free(context);
}

void check_image_content_rgb(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_rgb(handle);

  int stride;
  const uint8_t *img_plane =
      heif_image_get_plane_readonly(img, heif_channel_R, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 10; row++) {
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
  }

  img_plane = heif_image_get_plane_readonly(img, heif_channel_G, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 10; row++) {
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
  }

  img_plane = heif_image_get_plane_readonly(img, heif_channel_B, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 10; row++) {
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
  }

  heif_image_release(img);
  heif_image_handle_release(handle);
}

void check_image_content_ycbcr444(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_ycbcr(handle, heif_chroma_444);

  int stride;
  const uint8_t *img_plane =
      heif_image_get_plane_readonly(img, heif_channel_Y, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 12; row++) {
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
  }

  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cb, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 12; row++) {
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
  }

  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cr, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 12; row++) {
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
  }

  heif_image_release(img);
  heif_image_handle_release(handle);
}

void check_image_content_ycbcr422(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_ycbcr(handle, heif_chroma_422);

  int stride;
  const uint8_t *img_plane =
      heif_image_get_plane_readonly(img, heif_channel_Y, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 12; row++) {
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
  }

  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cb, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 12; row++) {
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
  }

  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cr, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 12; row++) {
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
  }

  heif_image_release(img);
  heif_image_handle_release(handle);
}

void check_image_content_ycbcr420(struct heif_context *&context) {
  heif_image_handle *handle = get_primary_image_handle(context);
  heif_image *img = get_primary_image_ycbcr(handle, heif_chroma_420);

  int stride;
  const uint8_t *img_plane =
      heif_image_get_plane_readonly(img, heif_channel_Y, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 12; row++) {
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
  }

  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cb, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 6; row++) {
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
  }

  img_plane = heif_image_get_plane_readonly(img, heif_channel_Cr, &stride);
  REQUIRE(stride == 64);
  for (int row = 0; row < 6; row++) {
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
  }

  heif_image_release(img);
  heif_image_handle_release(handle);
}

TEST_CASE("check image content") {
  auto file = GENERATE(RGB_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_content_rgb(context);
  heif_context_free(context);
}

TEST_CASE("check image content YCbCr 444") {
  auto file = GENERATE(YCBCR444_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_content_ycbcr444(context);
  heif_context_free(context);
}

TEST_CASE("check image content YCbCr 422") {
  auto file = GENERATE(YCBCR422_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_content_ycbcr422(context);
  heif_context_free(context);
}

TEST_CASE("check image content YCbCr 420") {
  auto file = GENERATE(YCBCR420_FILES);
  auto context = get_context_for_test_file(file);
  INFO("file name: " << file);
  check_image_content_ycbcr420(context);
  heif_context_free(context);
}