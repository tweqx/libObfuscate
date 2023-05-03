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

#include "CSPRNG.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Grostl512.h"
#include "Keccak512.h"
#include "Sha512.h"
#include "Skein512.h"

#define RESEED_COUNTDOWN 1000

void CSPRNG_set_seed(CSPRNG_DATA *pCd, ENUM_HASH hashE, const uint8_t *passw,
                     uint32_t nonce) {
  memset(pCd, 0, sizeof(CSPRNG_DATA));

  uint8_t inBuf[64];
  memset(inBuf, 0, 64);
  memcpy(inBuf, passw, MAX_PASSW_SIZE);
  uint32_t len = strlen((char *)inBuf);
  memcpy(&inBuf[len], &nonce, 4);
  len += 4;

  uint8_t hash[64];

  if (hashE == SHA512_HASH) {
    SHA512_DATA sha;

    Sha512_init(&sha);
    Sha512_data(&sha, inBuf, len);
    Sha512_finalize(&sha, hash);
  } else if (hashE == GROSTL512_HASH) {
    GROSTL512_DATA grostl;

    Grostl512_init(&grostl);
    Grostl512_data(&grostl, inBuf, len);
    Grostl512_finalize(&grostl, hash);
  } else if (hashE == KECCAK512_HASH) {
    KECCAK512_DATA keccak;

    Keccak512_init(&keccak);
    Keccak512_data(&keccak, inBuf, len);
    Keccak512_finalize(&keccak, hash);
  } else if (hashE == SKEIN512_HASH) {
    SKEIN512_DATA skein;

    Skein512_init(&skein);
    Skein512_data(&skein, inBuf, len);
    Skein512_finalize(&skein, hash);
  }

  Multi_single_setkey(&pCd->msd, RIJNDAEL_ALG, hash);

  memcpy(pCd->ctrBuf, &hash[MAX_PASSW_SIZE], DATA_BLOCK_SIZE);
  pCd->availCount = 0;
}

// auto setup with default
void CSPRNG_autoseed(CSPRNG_DATA *pCd, perc_callback_t backFunc, void *desc) {
  memset(pCd, 0, sizeof(CSPRNG_DATA));

  uint8_t passw[MAX_PASSW_SIZE + DATA_BLOCK_SIZE];

  FILE *random = fopen("/dev/random", "r");

  uint32_t to_read = MAX_PASSW_SIZE + DATA_BLOCK_SIZE;
  uint32_t left_to_read = to_read;

  while (left_to_read != 0) {
    uint32_t read = fread(passw + (to_read - left_to_read), left_to_read,
                          sizeof(uint8_t), random);

    if (backFunc)
      backFunc(desc, left_to_read / (float)to_read * 100);

    left_to_read -= read;
  }

  fclose(random);

  Multi_single_setkey(&pCd->msd, RIJNDAEL_ALG, passw);

  memcpy(pCd->ctrBuf, &passw[MAX_PASSW_SIZE], DATA_BLOCK_SIZE);
  pCd->availCount = 0;
}

// little endian
static void BlockInc(uint8_t *data) {
#if DATA_BLOCK_SIZE == 16
  uint32_t *as_dwords = (uint32_t *)data;

  if (as_dwords[0] != 0xffffffff)
    goto BlockInc_le0;
  if (as_dwords[1] != 0xffffffff)
    goto BlockInc_le1;
  if (as_dwords[2] != 0xffffffff)
    goto BlockInc_le2;

  as_dwords[3]++;
BlockInc_le2:
  as_dwords[2]++;
BlockInc_le1:
  as_dwords[1]++;
BlockInc_le0:
  as_dwords[0]++;
#else
#error
#endif
}

// auto reseeding leads to a bad chi-square result after some Mb of data!
uint8_t CSPRNG_get_byte(CSPRNG_DATA *pCd) {
  if (!pCd->availCount) {
    // random = AES-256(CTR)
    Multi_ECB_single_encrypt(&pCd->msd, RIJNDAEL_ALG, pCd->ctrBuf,
                             pCd->randBuf);
    BlockInc(pCd->ctrBuf);

    pCd->availCount = DATA_BLOCK_SIZE - 1;

    return *pCd->randBuf;
  } else {
    return pCd->randBuf[DATA_BLOCK_SIZE - (pCd->availCount--)];
  }
}

uint16_t CSPRNG_get_word(CSPRNG_DATA *pCd) {
  uint16_t retV = CSPRNG_get_byte(pCd);
  retV <<= 8;
  retV |= CSPRNG_get_byte(pCd);

  return retV;
}

uint32_t CSPRNG_get_dword(CSPRNG_DATA *pCd) {
  uint32_t retV = CSPRNG_get_word(pCd);
  retV <<= 16;
  retV |= CSPRNG_get_word(pCd);

  return retV;
}

#define REFRESH_COUNTDOWN 1000

OBFUNC_RETV CSPRNG_randomize(CSPRNG_DATA *pCd, const uint32_t len, uint8_t *buf,
                             perc_callback_t pFunc, void *pDesc,
                             test_callback_t tFunc, void *tDesc) {
  uint32_t tLen = len;
  uint8_t lastPerc = 0;
  uint16_t refCount = REFRESH_COUNTDOWN;

  while (tLen--) {
    *(buf++) = CSPRNG_get_byte(pCd);

    if (!refCount) {
      refCount = REFRESH_COUNTDOWN;

      if (pFunc) {
        uint8_t tmp = (len - tLen) / (float)len * 100;

        if (tmp > lastPerc) {
          lastPerc = tmp;
          pFunc(pDesc, lastPerc);
        }
      }
      if (tFunc && tFunc(tDesc)) {
        return OBFUNC_STOP;
      }
    }

    refCount--;
  }

  return OBFUNC_OK;
}

void CSPRNG_array_init(CSPRNG_DATA *pCd, uint32_t max, uint8_t *buf) {
  memset(buf, 0xFF, max);

  for (uint32_t index = 0; index < max; index++) {
    uint32_t rIndex;

    do {
      rIndex = CSPRNG_get_byte(pCd) % max;
    } while (buf[rIndex] != 0xFF);

    buf[rIndex] = index;
  }
}
