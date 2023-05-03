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

#ifndef TWOFISH_H
#define TWOFISH_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Twofish cipher
// ** 128bit block size
// ** 256bit key

#define Q_TABLES
#define M_TABLE
#define MK_TABLE
#define ONE_STEP

typedef struct {
  uint32_t k_len;
  uint32_t l_key[40];
  uint32_t s_key[4];

#ifdef Q_TABLES
  uint32_t qt_gen;
  uint8_t q_tab[2][256];
#endif

#ifdef M_TABLE
  uint32_t mt_gen;
  uint32_t m_tab[4][256];
#endif

#ifdef MK_TABLE
#ifdef ONE_STEP
  uint32_t mk_tab[4][256];
#else
  uint8_t sb[4][256];
#endif
#endif
} TWOFISH_DATA;

extern void Twofish_set_key(TWOFISH_DATA *pTfd, const uint32_t *in_key,
                            const uint32_t key_len);
extern void Twofish_encrypt(const TWOFISH_DATA *pTfd, const uint32_t *in_blk,
                            uint32_t *out_blk);
extern void Twofish_decrypt(const TWOFISH_DATA *pTfd, const uint32_t *in_blk,
                            uint32_t *out_blk);

#endif
