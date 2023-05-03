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

#ifndef SCRAMBLE_H
#define SCRAMBLE_H

// ** Thread-safe implementation

// ** Scramble data segment
// ** Based on CSPRNG

// "passw" = password, byte[MAX_PASSW_SIZE]
// "nonce" = cryptographic nonce (number used once)

#include "CSPRNG.h"

typedef struct {
  CSPRNG_DATA cd;
  uint32_t len;
  uint32_t *list;
} SCRAMBLE_DATA;

extern OBFUNC_RETV Scramble_seed(SCRAMBLE_DATA *pSd, const uint32_t len,
                                 const uint8_t *passw, uint32_t nonce);
extern void Scramble_end(SCRAMBLE_DATA *pSd);

extern OBFUNC_RETV Seg_scramble(SCRAMBLE_DATA *pSd, uint8_t *buf,
                                perc_callback_t pFunc, void *pDesc,
                                test_callback_t tFunc, void *tDesc);
extern OBFUNC_RETV Seg_descramble(SCRAMBLE_DATA *pSd, uint8_t *buf,
                                  perc_callback_t pFunc, void *pDesc,
                                  test_callback_t tFunc, void *tDesc);

#endif
