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

#include "Multi.h"

#include <string.h>

static void BlockXor(uint8_t *data, const uint8_t *value) {
#if DATA_BLOCK_SIZE == 16
  uint32_t *data_as_dwords = (uint32_t *)data;
  uint32_t *value_as_dwords = (uint32_t *)value;

  data_as_dwords[0] ^= value_as_dwords[0];
  data_as_dwords[1] ^= value_as_dwords[1];
  data_as_dwords[2] ^= value_as_dwords[2];
  data_as_dwords[3] ^= value_as_dwords[3];
#else
#error
#endif
}

void Multi_setkey(MULTI_DATA *pMd, const uint8_t *iv, const uint8_t *passw1,
                  const uint8_t *passw2, uint32_t nonce) {
  uint8_t passw[MAX_ALG][MAX_PASSW_SIZE];
  uint8_t usedMap[MAX_ALG];
  memset(pMd, 0, sizeof(MULTI_DATA));

  // CSPRNG <- Skein512(passw2 + nonce)
  CSPRNG_set_seed(&pMd->cd, SKEIN512_HASH, passw2, nonce);

  // IVs
  memcpy(pMd->iv[0], iv, MAX_ALG * DATA_BLOCK_SIZE);

  // passw[] <- KDF4 : random ( hash( passw1 + nonce) )
  for (uint32_t index = 0; index < MAX_HASH; index++) {
    CSPRNG_DATA tmpCSPRNG;

    switch (index) {
    case 0:
      CSPRNG_set_seed(&tmpCSPRNG, SHA512_HASH, passw1, nonce);
      break;
    case 1:
      CSPRNG_set_seed(&tmpCSPRNG, GROSTL512_HASH, passw1, nonce);
      break;
    case 2:
      CSPRNG_set_seed(&tmpCSPRNG, KECCAK512_HASH, passw1, nonce);
      break;
    case 3:
      CSPRNG_set_seed(&tmpCSPRNG, SKEIN512_HASH, passw1, nonce);
      break;
    }

    for (uint32_t pIndex = 0; pIndex < (MAX_ALG / MAX_HASH); pIndex++) {
      for (uint32_t sIndex = 0; sIndex < MAX_PASSW_SIZE; sIndex++) {
        passw[index * (MAX_ALG / MAX_HASH) + pIndex][sIndex] =
            CSPRNG_get_byte(&tmpCSPRNG);
      }
    }
  }

  CSPRNG_array_init(&pMd->cd, MAX_ALG, usedMap);

  // subcipher setup
  Multi_single_setkey(&pMd->msd, ANUBIS_ALG, passw[usedMap[0]]);
  Multi_single_setkey(&pMd->msd, CAMELLIA_ALG, passw[usedMap[1]]);
  Multi_single_setkey(&pMd->msd, CAST256_ALG, passw[usedMap[2]]);
  Multi_single_setkey(&pMd->msd, CLEFIA_ALG, passw[usedMap[3]]);
  Multi_single_setkey(&pMd->msd, FROG_ALG, passw[usedMap[4]]);
  Multi_single_setkey(&pMd->msd, HIEROCRYPT3_ALG, passw[usedMap[5]]);
  Multi_single_setkey(&pMd->msd, IDEANXT128_ALG, passw[usedMap[6]]);
  Multi_single_setkey(&pMd->msd, MARS_ALG, passw[usedMap[7]]);
  Multi_single_setkey(&pMd->msd, RC6_ALG, passw[usedMap[8]]);
  Multi_single_setkey(&pMd->msd, RIJNDAEL_ALG, passw[usedMap[9]]);
  Multi_single_setkey(&pMd->msd, SAFERP_ALG, passw[usedMap[10]]);
  Multi_single_setkey(&pMd->msd, SC2000_ALG, passw[usedMap[11]]);
  Multi_single_setkey(&pMd->msd, SERPENT_ALG, passw[usedMap[12]]);
  Multi_single_setkey(&pMd->msd, SPEED_ALG, passw[usedMap[13]]);
  Multi_single_setkey(&pMd->msd, TWOFISH_ALG, passw[usedMap[14]]);
  Multi_single_setkey(&pMd->msd, UNICORNA_ALG, passw[usedMap[15]]);
}

#define REFRESH_COUNTDOWN 100

OBFUNC_RETV Multi_CBC_encrypt(MULTI_DATA *pMd, const uint32_t len, uint8_t *buf,
                              perc_callback_t pFunc, void *pDesc,
                              test_callback_t tFunc, void *tDesc) {
  uint32_t tLen = len;
  uint8_t lastPerc = 0;
  uint16_t refCount = REFRESH_COUNTDOWN;

  while (tLen >= DATA_BLOCK_SIZE) {
    uint8_t tmpIN[DATA_BLOCK_SIZE];
    uint8_t curAlg = CSPRNG_get_byte(&pMd->cd) % MAX_ALG;

    // OUT = encrypt( IN ^ IV_or_previous_out_block )
    memcpy(tmpIN, buf, DATA_BLOCK_SIZE);
    BlockXor(tmpIN, pMd->iv[curAlg]);

    Multi_ECB_single_encrypt(&pMd->msd, (ENUM_ALG)curAlg, tmpIN, buf);

    memcpy(pMd->iv[curAlg], buf, DATA_BLOCK_SIZE);

    buf += DATA_BLOCK_SIZE;
    tLen -= DATA_BLOCK_SIZE;

    if (!refCount) {
      refCount = REFRESH_COUNTDOWN;

      if (pFunc) {
        uint8_t perc = (len - tLen) / (float)len * 100;

        if (perc > lastPerc) {
          lastPerc = perc;
          pFunc(pDesc, lastPerc);
        }
      }

      if (tFunc && tFunc(tDesc))
        return OBFUNC_STOP;
    }

    refCount--;
  }

  return OBFUNC_OK;
}

OBFUNC_RETV Multi_CBC_decrypt(MULTI_DATA *pMd, const uint32_t len, uint8_t *buf,
                              perc_callback_t pFunc, void *pDesc,
                              test_callback_t tFunc, void *tDesc) {
  uint32_t tLen = len;
  uint8_t lastPerc = 0;
  uint16_t refCount = REFRESH_COUNTDOWN;

  while (tLen >= DATA_BLOCK_SIZE) {
    uint8_t tmpOUT[DATA_BLOCK_SIZE];
    uint8_t curAlg = CSPRNG_get_byte(&pMd->cd) % MAX_ALG;

    // OUT = decrypt( IN ) ^ IV_or_previous_in_block )
    Multi_ECB_single_decrypt(&pMd->msd, (ENUM_ALG)curAlg, buf, tmpOUT);

    BlockXor(tmpOUT, pMd->iv[curAlg]);
    memcpy(pMd->iv[curAlg], buf, DATA_BLOCK_SIZE);
    memcpy(buf, tmpOUT, DATA_BLOCK_SIZE);

    buf += DATA_BLOCK_SIZE;
    tLen -= DATA_BLOCK_SIZE;

    if (!refCount) {
      refCount = REFRESH_COUNTDOWN;

      if (pFunc) {
        uint8_t perc = (len - tLen) / (float)len * 100;

        if (perc > lastPerc) {
          lastPerc = perc;
          pFunc(pDesc, lastPerc);
        }
      }

      if (tFunc && tFunc(tDesc))
        return OBFUNC_STOP;
    }

    refCount--;
  }

  return OBFUNC_OK;
}
