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

#ifndef SPEED_H
#define SPEED_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Speed cipher
// ** 128bit block size
// ** 256bit key

#define SPEED_KEY_LEN 256
#define SPEED_DATA_LEN 128
#define SPEED_NO_OF_RND (20 * 8) // rc6=20 rounds, rc6->speed=(*8)

#define SPEED_DATA_LEN_uint8_t                                                 \
  (SPEED_DATA_LEN / 8) /* no. of bytes in a p/c-text */
#define SPEED_KEY_LEN_uint8_t (SPEED_KEY_LEN / 8) /* no. of bytes in a key */

typedef uint8_t speed_key[SPEED_KEY_LEN_uint8_t];   /* for user key */
typedef uint8_t speed_data[SPEED_DATA_LEN_uint8_t]; /* for p/c-text */

typedef uint32_t speed_ikey[SPEED_NO_OF_RND]; /* for round keys */
typedef uint32_t speed_idata[8];              /* for internal p/c-text */

extern void Speed_set_key(speed_ikey rndkey, const speed_key key);
extern void Speed_encrypt(const speed_ikey rndkey, const speed_data pt,
                          speed_data ct);
extern void Speed_decrypt(const speed_ikey rndkey, const speed_data ct,
                          speed_data pt);

#endif
