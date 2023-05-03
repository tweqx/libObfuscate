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

#ifndef ANUBIS_H
#define ANUBIS_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Anubis cipher
// ** 128bit block size
// ** 256bit key

#define MIN_N 4
#define MAX_N 10
#define MIN_ROUNDS (8 + MIN_N)
#define MAX_ROUNDS (8 + MAX_N)
#define MIN_KEYSIZEB (4 * MIN_N)
#define MAX_KEYSIZEB (4 * MAX_N)
#define BLOCKSIZE 128
#define BLOCKSIZEB (BLOCKSIZE / 8)

/*
 * The KEYSIZEB macro should be redefined for each allowed key size
 * in order to use the NESSIE test vector generator program.
 * Valid sizes (in bytes) are 16, 20, 24, 28, 32, 36, and 40.
 */
#define KEYSIZEB 32

typedef struct {
  int keyBits; // this field must be initialized before the NESSIEkeysetup call
  int R;
  uint32_t roundKeyEnc[MAX_ROUNDS + 1][4];
  uint32_t roundKeyDec[MAX_ROUNDS + 1][4];
} ANUBIS_DATA;

extern void Anubis_set_key(ANUBIS_DATA *pAd, const uint8_t *key);
extern void Anubis_encrypt(const ANUBIS_DATA *pAd, const uint8_t *plaintext,
                           uint8_t *ciphertext);
extern void Anubis_decrypt(const ANUBIS_DATA *pAd, const uint8_t *ciphertext,
                           uint8_t *plaintext);

#endif
