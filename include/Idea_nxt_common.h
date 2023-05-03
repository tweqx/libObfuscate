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

#ifndef IDEA_NXT_COMMON_H
#define IDEA_NXT_COMMON_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Idea-Nxt cipher
// ** 128bit block size
// ** 256bit key

/*
 * These macros define which algorithms are used. You can comment one of the
 * two macros if you don't want to use both algorithms.
 */
#define USE_NXT64
#define USE_NXT128

/*
 * NXT64 macros
 */
#ifdef USE_NXT64

#if 1
#define NXT64_UNROLL_LOOPS
#endif

#endif /* USE_NXT64 */

/*
 * NXT128 macros
 */
#ifdef USE_NXT128

#if 1
#define NXT128_UNROLL_LOOPS
#endif

#endif /* USE_NXT128 */

#define UNPACK32(x, str)                                                       \
  {                                                                            \
    *((str) + 3) = (uint8_t)((x));                                             \
    *((str) + 2) = (uint8_t)((x) >> 8);                                        \
    *((str) + 1) = (uint8_t)((x) >> 16);                                       \
    *((str)) = (uint8_t)((x) >> 24);                                           \
  }

#define PACK32(str, x)                                                         \
  {                                                                            \
    *(x) = (*((str) + 3)) | (*((str) + 2) << 8) | (*((str) + 1) << 16) |       \
           (*((str)) << 24);                                                   \
  }

#define IRRED_POLY 0x1f9

#define LFSR(reg, lfsr_value)                                                  \
  do {                                                                         \
    *reg = *reg << 1;                                                          \
    if (*reg & 0x1000000)                                                      \
      *reg ^= 0x100001b;                                                       \
    lfsr_value = *reg;                                                         \
  } while (0)

#define NXT_OR(x) (x << 16) ^ (x >> 16) ^ (x & 0x0000ffff);

#define NXT_IO(x) (x << 16) ^ (x >> 16) ^ (x & 0xffff0000);

extern const uint8_t pad[32];

void nxt_p(const uint8_t *key, uint8_t l, uint8_t *pkey, uint16_t ek);
void nxt_m(const uint8_t *pkey, uint8_t *mkey, uint16_t ek);

#endif
