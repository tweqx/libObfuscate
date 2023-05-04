///////////////////////////////////////////////
//
// **************************
// ** ENGLISH - 10/Jul/2017 **
//
// Project: libObfuscate v2.00
//
// This software is released under:
// * LGPL 3.0: "www.gnu.org/licenses/lgpl.html"
//
// You're free to copy, distribute and make commercial use
// of this software under the following conditions:
// * You cite the author and copyright owner: "www.embeddedsw.net"
// * You provide a link to the Homepage: "www.embeddedsw.net/libobfuscate.html"
//
///////////////////////////////////////////////

// Copyright 2023 tweqx

/* This file is part of LibrePuff.

  LibrePuff is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version.

  LibrePuff is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LibrePuff. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef IDEANXT128_H
#define IDEANXT128_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Idea-Nxt cipher
// ** 128bit block size
// ** 256bit key

// min 16 ... max 255
#define NXT128_TOTAL_ROUNDS 16

typedef struct {
  uint32_t rk[NXT128_TOTAL_ROUNDS * 4];
} nxt128_ctx;

extern void Ideanxt128_set_key(nxt128_ctx *ctx, const uint8_t *key,
                               const uint16_t key_len);
extern void Ideanxt128_encrypt(const nxt128_ctx *ctx, const uint8_t *in,
                               uint8_t *out);
extern void Ideanxt128_decrypt(const nxt128_ctx *ctx, const uint8_t *in,
                               uint8_t *out);

#endif
