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

#include "Idea_nxt_common.h"

#include <string.h>

const uint8_t pad[32] = {0xb7, 0xe1, 0x51, 0x62, 0x8a, 0xed, 0x2a, 0x6a,
                         0xbf, 0x71, 0x58, 0x80, 0x9c, 0xf4, 0xf3, 0xc7,
                         0x62, 0xe7, 0x16, 0x0f, 0x38, 0xb4, 0xda, 0x56,
                         0xa7, 0x84, 0xd9, 0x04, 0x51, 0x90, 0xcf, 0xef};

void nxt_p(const uint8_t *key, uint8_t l, uint8_t *pkey, uint16_t ek) {
  memcpy(pkey, key, l);
  memcpy(pkey + l, pad, (ek >> 3) - l);
}

void nxt_m(const uint8_t *pkey, uint8_t *mkey, uint16_t ek) {
  const uint8_t mkey_m2 = 0x6a;
  const uint8_t mkey_m1 = 0x76;

  int bound = (ek >> 3) - 1;

  mkey[0] = pkey[0] ^ (mkey_m1 + mkey_m2);
  mkey[1] = pkey[1] ^ (mkey[0] + mkey_m1);

  for (int i = 2; i <= bound; i++) {
    mkey[i] = pkey[i] ^ (mkey[i - 1] + mkey[i - 2]);
  }
}
