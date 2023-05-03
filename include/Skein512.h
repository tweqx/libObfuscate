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

// Copyright 2022 tweqx

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

#ifndef SKEIN512_H
#define SKEIN512_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Skein hashing
// ** 512bit hash

#define SKEIN_MODIFIER_WORDS 2

#define SKEIN_512_STATE_WORDS 8
#define SKEIN_512_STATE_uint8_tS (8 * SKEIN_512_STATE_WORDS)
#define SKEIN_512_STATE_BITS (64 * SKEIN_512_STATE_WORDS)
#define SKEIN_512_BLOCK_uint8_tS (8 * SKEIN_512_STATE_WORDS)

typedef struct {
  uint32_t hashBitLen;              /* size of hash result, in bits */
  uint32_t bCnt;                    /* current byte count in buffer b[] */
  uint64_t T[SKEIN_MODIFIER_WORDS]; /* tweak words: T[0]=byte cnt, T[1]=flags */
} Skein_Ctxt_Hdr_t;

typedef struct {
  Skein_Ctxt_Hdr_t h;                /* common header context variables */
  uint64_t X[SKEIN_512_STATE_WORDS]; /* chaining variables */
  uint8_t
      b[SKEIN_512_BLOCK_uint8_tS]; /* partial block buffer (8-byte aligned) */
} SKEIN512_DATA;

extern void Skein512_init(SKEIN512_DATA *skein);
extern void Skein512_data(SKEIN512_DATA *skein, const uint8_t *buffer,
                          uint32_t len);
extern void Skein512_finalize(SKEIN512_DATA *skein, uint8_t *hash);

#endif
