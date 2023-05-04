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

#include "MultiBase.h"

#include <string.h>

void Multi_single_setkey(MULTI_STATIC_DATA *pMsd, const ENUM_ALG curAlg,
                         const uint8_t *passw) {
  switch (curAlg) {
  case ANUBIS_ALG:
    memset(&pMsd->anubis, 0, sizeof(ANUBIS_STATIC_DATA));
    Anubis_set_key(&pMsd->anubis.key, passw);
    break;

  case CAMELLIA_ALG:
    memset(&pMsd->camellia, 0, sizeof(CAMELLIA_STATIC_DATA));
    Camellia_set_key(pMsd->camellia.key, MAX_PASSW_SIZE << 3, passw);
    break;

  case CAST256_ALG:
    memset(&pMsd->cast256, 0, sizeof(CAST256_STATIC_DATA));
    Cast256_set_key(pMsd->cast256.key, (uint32_t *)passw, MAX_PASSW_SIZE << 3);
    break;

  case CLEFIA_ALG:
    memset(&pMsd->clefia, 0, sizeof(CLEFIA_STATIC_DATA));
    Clefia_set_key(&pMsd->clefia.key, passw, MAX_PASSW_SIZE << 3);
    break;

  case FROG_ALG:
    memset(&pMsd->frog, 0, sizeof(FROG_STATIC_DATA));
    Frog_set_key(&pMsd->frog.key, (uint32_t *)passw, MAX_PASSW_SIZE << 3);
    break;

  case HIEROCRYPT3_ALG:
    memset(&pMsd->hc3, 0, sizeof(HIEROCRYPT3_STATIC_DATA));
    Hierocrypt3_set_key(&pMsd->hc3.key, passw);
    break;

  case IDEANXT128_ALG:
    memset(&pMsd->ideanxt128, 0, sizeof(IDEANXT128_STATIC_DATA));
    Ideanxt128_set_key(&pMsd->ideanxt128.key, passw, MAX_PASSW_SIZE << 3);
    break;

  case MARS_ALG:
    memset(&pMsd->mars, 0, sizeof(MARS_STATIC_DATA));

    pMsd->mars.vk[0] = 0x09d0c479;
    pMsd->mars.vk[1] = 0x28c8ffe0;
    pMsd->mars.vk[2] = 0x84aa6c39;
    pMsd->mars.vk[3] = 0x9dad7287;
    pMsd->mars.vk[4] = 0x7dff9be3;
    pMsd->mars.vk[5] = 0xd4268361;
    pMsd->mars.vk[6] = 0xc96da1d4;

    Mars_set_key(pMsd->mars.key, pMsd->mars.vk, (uint32_t *)passw,
                 MAX_PASSW_SIZE << 3);
    break;

  case RC6_ALG:
    memset(&pMsd->rc6, 0, sizeof(RC6_STATIC_DATA));
    Rc6_set_key(pMsd->rc6.key, (uint32_t *)passw, MAX_PASSW_SIZE << 3);
    break;

  case RIJNDAEL_ALG:
    memset(&pMsd->rijndael_enc, 0, sizeof(RIJNDAEL_STATIC_DATA));
    memset(&pMsd->rijndael_dec, 0, sizeof(RIJNDAEL_STATIC_DATA));
    pMsd->rijndael_enc.nrounds = Rijndael_set_key_encrypt(
        pMsd->rijndael_enc.key, passw, MAX_PASSW_SIZE << 3);

    pMsd->rijndael_dec.nrounds = Rijndael_set_key_decrypt(
        pMsd->rijndael_dec.key, passw, MAX_PASSW_SIZE << 3);
    break;

  case SAFERP_ALG:
    memset(&pMsd->saferp, 0, sizeof(SAFERP_STATIC_DATA));
    Saferp_set_key(&pMsd->saferp.key, (uint32_t *)passw, MAX_PASSW_SIZE << 3);
    break;

  case SC2000_ALG:
    memset(&pMsd->sc2000, 0, sizeof(SC2000_STATIC_DATA));
    Sc2000_set_key(pMsd->sc2000.key, (uint32_t *)passw);
    break;

  case SERPENT_ALG:
    memset(&pMsd->serpent, 0, sizeof(SERPENT_STATIC_DATA));
    Serpent_set_key(pMsd->serpent.key, (uint32_t *)passw, MAX_PASSW_SIZE << 3);
    break;

  case SPEED_ALG:
    memset(&pMsd->speed, 0, sizeof(SPEED_STATIC_DATA));
    Speed_set_key(pMsd->speed.key, passw);
    break;

  case TWOFISH_ALG:
    memset(&pMsd->twofish, 0, sizeof(TWOFISH_STATIC_DATA));
    Twofish_set_key(&pMsd->twofish.key, (uint32_t *)passw, MAX_PASSW_SIZE << 3);
    break;

  case UNICORNA_ALG:
    memset(&pMsd->unicorn, 0, sizeof(UNICORNA_STATIC_DATA));
    Unicorn_set_key(pMsd->unicorn.key, passw);
    break;
  }
}

void Multi_ECB_single_encrypt(const MULTI_STATIC_DATA *pMsd,
                              const ENUM_ALG curAlg, const uint8_t *inBuf,
                              uint8_t *outBuf) {
  switch (curAlg) {
  case ANUBIS_ALG:
    Anubis_encrypt(&pMsd->anubis.key, inBuf, outBuf);
    break;
  case CAMELLIA_ALG:
    Camellia_encrypt(pMsd->camellia.key, MAX_PASSW_SIZE << 3, inBuf, outBuf);
    break;
  case CAST256_ALG:
    Cast256_encrypt(pMsd->cast256.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case CLEFIA_ALG:
    Clefia_encrypt(&pMsd->clefia.key, inBuf, outBuf);
    break;
  case FROG_ALG:
    Frog_encrypt(&pMsd->frog.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case HIEROCRYPT3_ALG:
    Hierocrypt3_encrypt(&pMsd->hc3.key, inBuf, outBuf);
    break;
  case IDEANXT128_ALG:
    Ideanxt128_encrypt(&pMsd->ideanxt128.key, inBuf, outBuf);
    break;
  case MARS_ALG:
    Mars_encrypt(pMsd->mars.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case RC6_ALG:
    Rc6_encrypt(pMsd->rc6.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case RIJNDAEL_ALG:
    Rijndael_encrypt(pMsd->rijndael_enc.key, pMsd->rijndael_enc.nrounds, inBuf,
                     outBuf);
    break;
  case SAFERP_ALG:
    Saferp_encrypt(&pMsd->saferp.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case SC2000_ALG:
    Sc2000_encrypt(pMsd->sc2000.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case SERPENT_ALG:
    Serpent_encrypt(pMsd->serpent.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case SPEED_ALG:
    Speed_encrypt(pMsd->speed.key, inBuf, outBuf);
    break;
  case TWOFISH_ALG:
    Twofish_encrypt(&pMsd->twofish.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case UNICORNA_ALG:
    Unicorn_encrypt(pMsd->unicorn.key, inBuf, outBuf);
    break;
  }
}

void Multi_ECB_single_decrypt(const MULTI_STATIC_DATA *pMsd,
                              const ENUM_ALG curAlg, const uint8_t *inBuf,
                              uint8_t *outBuf) {
  switch (curAlg) {
  case ANUBIS_ALG:
    Anubis_decrypt(&pMsd->anubis.key, inBuf, outBuf);
    break;
  case CAMELLIA_ALG:
    Camellia_decrypt(pMsd->camellia.key, MAX_PASSW_SIZE << 3, inBuf, outBuf);
    break;
  case CAST256_ALG:
    Cast256_decrypt(pMsd->cast256.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case CLEFIA_ALG:
    Clefia_decrypt(&pMsd->clefia.key, inBuf, outBuf);
    break;
  case FROG_ALG:
    Frog_decrypt(&pMsd->frog.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case HIEROCRYPT3_ALG:
    Hierocrypt3_decrypt(&pMsd->hc3.key, inBuf, outBuf);
    break;
  case IDEANXT128_ALG:
    Ideanxt128_decrypt(&pMsd->ideanxt128.key, inBuf, outBuf);
    break;
  case MARS_ALG:
    Mars_decrypt(pMsd->mars.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case RC6_ALG:
    Rc6_decrypt(pMsd->rc6.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case RIJNDAEL_ALG:
    Rijndael_decrypt(pMsd->rijndael_dec.key, pMsd->rijndael_dec.nrounds, inBuf,
                     outBuf);
    break;
  case SAFERP_ALG:
    Saferp_decrypt(&pMsd->saferp.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case SC2000_ALG:
    Sc2000_decrypt(pMsd->sc2000.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case SERPENT_ALG:
    Serpent_decrypt(pMsd->serpent.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case SPEED_ALG:
    Speed_decrypt(pMsd->speed.key, inBuf, outBuf);
    break;
  case TWOFISH_ALG:
    Twofish_decrypt(&pMsd->twofish.key, (uint32_t *)inBuf, (uint32_t *)outBuf);
    break;
  case UNICORNA_ALG:
    Unicorn_decrypt(pMsd->unicorn.key, inBuf, outBuf);
    break;
  }
}
