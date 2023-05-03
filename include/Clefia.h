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

#ifndef CLEFIA_H
#define CLEFIA_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Clefia cipher
// ** 128bit block size
// ** 256bit key

typedef struct {
  int r;
  uint8_t rk[8 * 26 + 16];
} CLEFIA_DATA;

extern void Clefia_set_key(CLEFIA_DATA *pCd, const uint8_t *skey,
                           const int key_bitlen);
extern void Clefia_encrypt(const CLEFIA_DATA *pCd, const uint8_t *pt,
                           uint8_t *ct);
extern void Clefia_decrypt(const CLEFIA_DATA *pCd, const uint8_t *ct,
                           uint8_t *pt);

#endif
