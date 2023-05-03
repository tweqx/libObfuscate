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

#ifndef CSPRNG_H
#define CSPRNG_H

#include <stdint.h>

#include "CommonDef.h"
#include "MultiBase.h"

// ** Thread-safe implementation

// ** Cryptographically Secure Pseudo Random Number Generator (by construction)
// ** Based on AES-256 + CTR

// "hashE" = hashing function
// "passw" = password, byte[MAX_PASSW_SIZE]
// "nonce" = cryptographic nonce (number used once)

typedef enum {
  SHA512_HASH = 0,
  GROSTL512_HASH,
  KECCAK512_HASH,
  SKEIN512_HASH
} ENUM_HASH;

#define MAX_HASH 4

typedef struct {
  MULTI_STATIC_DATA msd;
  uint8_t ctrBuf[DATA_BLOCK_SIZE];
  uint8_t randBuf[DATA_BLOCK_SIZE];
  uint32_t availCount;
} CSPRNG_DATA;

extern void CSPRNG_set_seed(CSPRNG_DATA *pCd, ENUM_HASH hashE,
                            const uint8_t *passw, uint32_t nonce);
extern void CSPRNG_autoseed(CSPRNG_DATA *pCd, perc_callback_t backFunc,
                            void *desc);

extern uint8_t CSPRNG_get_byte(CSPRNG_DATA *pCd);
extern uint16_t CSPRNG_get_word(CSPRNG_DATA *pCd);
extern uint32_t CSPRNG_get_dword(CSPRNG_DATA *pCd);
extern OBFUNC_RETV CSPRNG_randomize(CSPRNG_DATA *pCd, const uint32_t len,
                                    uint8_t *buf, perc_callback_t pFunc,
                                    void *pDesc, test_callback_t tFunc,
                                    void *tDesc);
extern void CSPRNG_array_init(CSPRNG_DATA *pCd, uint32_t max, uint8_t *buf);

#endif
