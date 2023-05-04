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

#ifndef SHA512_H
#define SHA512_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Sha2 hashing
// ** 512bit hash

typedef struct {
  uint32_t inputLen;
  uint64_t A, B, C, D, E, F, G, H;
  uint64_t totalLen;
  uint8_t input[128];
} SHA512_DATA;

extern void Sha512_init(SHA512_DATA *sha);
extern void Sha512_data(SHA512_DATA *sha, const void *buffer, uint32_t len);
extern void Sha512_finalize(SHA512_DATA *sha, uint8_t *hash);

#endif
