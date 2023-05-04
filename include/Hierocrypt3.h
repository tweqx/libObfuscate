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

#ifndef HIEROCRYPT3_H
#define HIEROCRYPT3_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Hierocrypt3 cipher
// ** 128bit block size
// ** 256bit key

#define MAX_ROUND (8)

typedef uint8_t HC3_KS[MAX_ROUND + 1][8][4];

typedef struct {
	HC3_KS ks;
	HC3_KS dks;
} HIEROCRYPT3_DATA;

extern	void Hierocrypt3_set_key(HIEROCRYPT3_DATA *pHd,const uint8_t* key);
extern	void Hierocrypt3_encrypt(const HIEROCRYPT3_DATA *pHd,const uint8_t * in,uint8_t *out);
extern	void Hierocrypt3_decrypt(const HIEROCRYPT3_DATA *pHd,const uint8_t * in,uint8_t *out);

#endif
