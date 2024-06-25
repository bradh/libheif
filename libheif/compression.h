/*
 * HEIF codec.
 * Copyright (c) 2022 Dirk Farin <dirk.farin@gmail.com>
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
#ifndef LIBHEIF_COMPRESSION_H
#define LIBHEIF_COMPRESSION_H

#include <vector>
#include <cinttypes>
#include <cstddef>

#include "libheif/error.h"

#if WITH_ZLIB_COMPRESSION
std::vector<uint8_t> deflate(const uint8_t* input, size_t size);

Error inflate_zlib(const std::vector<uint8_t>&compressed_input, std::vector<uint8_t> *output);
Error inflate_deflate(const std::vector<uint8_t>&, std::vector<uint8_t> *output);

#endif

#if HAVE_BROTLI

Error inflate_brotli(const std::vector<uint8_t> &compressed_input, std::vector<uint8_t> *output);

#endif

#endif //LIBHEIF_COMPRESSION_H
