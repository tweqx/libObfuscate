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

#ifndef MULTI_H
#define MULTI_H

// ** Thread-safe implementation

// ** Wrapping of implemented ciphers
// ** 128bit block size	(DATA_BLOCK_SIZE<<3)
// ** 2x 256bit keys	(MAX_PASSW_SIZE<<3)
// ** Cypher-Block-Chaining (CBC)

// "iv" = Inizialization Vectors (IVs), byte[MAX_ALG][DATA_BLOCK_SIZE]
// "passw1" = password1, byte[MAX_PASSW_SIZE]
// "passw2" = password2, byte[MAX_PASSW_SIZE]
// "nonce" = cryptographic nonce (number used once)

#include "CSPRNG.h"
#include "CommonDef.h"
#include "MultiBase.h"

typedef struct {
  MULTI_STATIC_DATA msd;
  CSPRNG_DATA cd;
  uint8_t iv[MAX_ALG][DATA_BLOCK_SIZE];
} MULTI_DATA;

extern void Multi_setkey(MULTI_DATA *pMd, const uint8_t *iv,
                         const uint8_t *passw1, const uint8_t *passw2,
                         uint32_t nonce);

extern OBFUNC_RETV Multi_CBC_encrypt(MULTI_DATA *pMd, const uint32_t len,
                                     uint8_t *buf, perc_callback_t pFunc,
                                     void *pDesc, test_callback_t tFunc,
                                     void *tDesc);
extern OBFUNC_RETV Multi_CBC_decrypt(MULTI_DATA *pMd, const uint32_t len,
                                     uint8_t *buf, perc_callback_t pFunc,
                                     void *pDesc, test_callback_t tFunc,
                                     void *tDesc);

#endif
