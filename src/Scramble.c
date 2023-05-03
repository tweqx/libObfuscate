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

#include "Scramble.h"

#include <stdlib.h>
#include <string.h>

static void Scramble_build_list(CSPRNG_DATA *pCd, const uint32_t count,
                                const uint32_t max, uint32_t *list) {
  uint32_t tCount = count;

  while (tCount) {
    do {
      list[0] = CSPRNG_get_dword(pCd) % max;
      list[1] = CSPRNG_get_dword(pCd) % max;
    } while (list[0] == list[1]);

    list += 2;
    tCount--;
  }
}

OBFUNC_RETV Scramble_seed(SCRAMBLE_DATA *pSd, const uint32_t len,
                          const uint8_t *passw, uint32_t nonce) {
  memset(pSd, 0, sizeof(SCRAMBLE_DATA));

  // CSPRNG <- Skein512(passw + nonce)
  CSPRNG_set_seed(&pSd->cd, SKEIN512_HASH, passw, nonce);

  pSd->len = len;

  // 50% scramble : (len/2)*2*sizeof(uint32_t)
  if ((pSd->list = (uint32_t *)malloc(((len >> 1) << 1) * sizeof(uint32_t)))) {
    Scramble_build_list(&pSd->cd, len >> 1, len, pSd->list);
    return OBFUNC_OK;
  } else {
    return OBFUNC_OUTOFMEM;
  }
}

void Scramble_end(SCRAMBLE_DATA *pSd) { TEST_AND_FREE(pSd->list); }

#define REFRESH_COUNTDOWN 10000

OBFUNC_RETV
Seg_scramble(SCRAMBLE_DATA *pSd, uint8_t *buf, perc_callback_t pFunc,
             void *pDesc, test_callback_t tFunc, void *tDesc) {
  uint32_t tot = pSd->len >> 1; // 50%
  uint32_t *pL = pSd->list;
  uint8_t lastPerc = 0;
  uint16_t refCount = REFRESH_COUNTDOWN;

  for (uint32_t index = 0; index < tot; index++) {
    uint32_t idx0 = *(pL++);
    uint32_t idx1 = *(pL++);

    uint8_t tmp = buf[idx0];
    buf[idx0] = buf[idx1];
    buf[idx1] = tmp;

    if (!refCount) {
      refCount = REFRESH_COUNTDOWN;

      if (pFunc) {
        uint8_t perc = index / (float)tot * 100;

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

OBFUNC_RETV Seg_descramble(SCRAMBLE_DATA *pSd, uint8_t *buf,
                           perc_callback_t pFunc, void *pDesc,
                           test_callback_t tFunc, void *tDesc) {
  uint32_t tot = pSd->len >> 1; // 50%
  uint32_t *pL = &pSd->list[((pSd->len >> 1) << 1) - 1];
  uint8_t lastPerc = 0;
  uint16_t refCount = REFRESH_COUNTDOWN;

  for (uint32_t index = 0; index < tot; index++) {
    uint32_t idx1 = *(pL--);
    uint32_t idx0 = *(pL--);

    uint8_t tmp = buf[idx0];
    buf[idx0] = buf[idx1];
    buf[idx1] = tmp;

    if (!refCount) {
      refCount = REFRESH_COUNTDOWN;

      if (pFunc) {
        uint8_t perc = index / (float)tot * 100;
        if (perc > lastPerc) {
          lastPerc = tmp;
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
