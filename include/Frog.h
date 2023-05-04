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

#ifndef FROG_H
#define FROG_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Frog cipher
// ** 128bit block size
// ** 256bit key

typedef struct {
  uint8_t k_xbu[16];
  uint8_t k_spu[256];
  uint8_t k_bpu[16];
} k_str;

typedef struct {
  k_str f_key[8];
  uint8_t i_key[8][256];
} key_str;

typedef struct {
  key_str loc_key;
  key_str sim_key;
  key_str *lkp;
} FROG_DATA;

extern void Frog_set_key(FROG_DATA *pFd, const uint32_t *in_key,
                         const uint32_t key_len);
extern void Frog_encrypt(const FROG_DATA *pFd, const uint32_t in_blk[4],
                         uint32_t out_blk[4]);
extern void Frog_decrypt(const FROG_DATA *pFd, const uint32_t in_blk[4],
                         uint32_t out_blk[4]);

#endif
