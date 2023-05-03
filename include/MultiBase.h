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

#ifndef MULTIBASE_H
#define MULTIBASE_H

// ** Thread-safe implementation

// ** Wrapping of implemented ciphers
// ** 128bit block size	(DATA_BLOCK_SIZE<<3)
// ** 2x 256bit keys	(MAX_PASSW_SIZE<<3)
// ** Electronic Codebook (ECB)

// "curAlg" = cipher function
// "passw" = password, byte[MAX_PASSW_SIZE]

#define	MIN_PASSW_SIZE 8
#define	MAX_PASSW_SIZE 32

#include "Anubis.h"
#include "Camellia.h"
#include "Cast256.h"
#include "Clefia.h"
#include "Frog.h"
#include "Hierocrypt3.h"
#include "Idea_nxt128.h"
#include "Mars.h"
#include "Unicorn-A.h"
#include "Rc6.h"
#include "Rijndael.h"
#include "Saferp.h"
#include "Sc2000.h"
#include "Serpent.h"
#include "Speed.h"
#include "Sha512.h"
#include "Speed.h"
#include "Twofish.h"

typedef struct {
  ANUBIS_DATA key;
} ANUBIS_STATIC_DATA;

typedef struct {
  uint8_t key[34 * 16];
} CAMELLIA_STATIC_DATA;

typedef struct {
  uint32_t key[96];
} CAST256_STATIC_DATA;

typedef struct {
  CLEFIA_DATA key;
} CLEFIA_STATIC_DATA;

typedef struct {
  FROG_DATA key;
} FROG_STATIC_DATA;

typedef struct {
  HIEROCRYPT3_DATA key;
} HIEROCRYPT3_STATIC_DATA;

typedef struct {
  nxt128_ctx key;
} IDEANXT128_STATIC_DATA;

typedef struct {
  uint32_t key[40];
  uint32_t vk[47];
} MARS_STATIC_DATA;

typedef struct {
  uint32_t key[44];
} RC6_STATIC_DATA;

typedef struct {
  uint32_t key[RKLENGTH(256)];
  int nrounds;
} RIJNDAEL_STATIC_DATA;

typedef struct {
  SAFERP_DATA key;
} SAFERP_STATIC_DATA;

typedef struct {
  uint32_t key[64];
} SC2000_STATIC_DATA;

typedef struct {
  uint32_t key[140];
} SERPENT_STATIC_DATA;

typedef struct {
  speed_ikey key;
} SPEED_STATIC_DATA;

typedef struct {
  TWOFISH_DATA key;
} TWOFISH_STATIC_DATA;

typedef struct {
  uint8_t key[288];
} UNICORNA_STATIC_DATA;

typedef struct {
  ANUBIS_STATIC_DATA anubis;
  CAMELLIA_STATIC_DATA camellia;
  CAST256_STATIC_DATA cast256;
  CLEFIA_STATIC_DATA clefia;
  FROG_STATIC_DATA frog;
  HIEROCRYPT3_STATIC_DATA hc3;
  IDEANXT128_STATIC_DATA ideanxt128;
  MARS_STATIC_DATA mars;
  RC6_STATIC_DATA rc6;
  RIJNDAEL_STATIC_DATA rijndael_enc;
  RIJNDAEL_STATIC_DATA rijndael_dec;
  SAFERP_STATIC_DATA saferp;
  SC2000_STATIC_DATA sc2000;
  SERPENT_STATIC_DATA serpent;
  SPEED_STATIC_DATA speed;
  TWOFISH_STATIC_DATA twofish;
  UNICORNA_STATIC_DATA unicorn;
} MULTI_STATIC_DATA;

typedef enum {
  ANUBIS_ALG = 0,
  CAMELLIA_ALG,
  CAST256_ALG,
  CLEFIA_ALG,
  FROG_ALG,
  HIEROCRYPT3_ALG,
  IDEANXT128_ALG,
  MARS_ALG,
  RC6_ALG,
  RIJNDAEL_ALG,
  SAFERP_ALG,
  SC2000_ALG,
  SERPENT_ALG,
  SPEED_ALG,
  TWOFISH_ALG,
  UNICORNA_ALG
} ENUM_ALG;

extern void Multi_single_setkey(MULTI_STATIC_DATA *pMsd, const ENUM_ALG curAlg,
                                const uint8_t *passw);
extern void Multi_ECB_single_encrypt(const MULTI_STATIC_DATA *pMsd,
                                     const ENUM_ALG curAlg,
                                     const uint8_t *inBuf, uint8_t *outBuf);
extern void Multi_ECB_single_decrypt(const MULTI_STATIC_DATA *pMsd,
                                     const ENUM_ALG curAlg,
                                     const uint8_t *inBuf, uint8_t *outBuf);

#endif
