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

#include "Frog.h"

#define rotl(x, y) ((unsigned)(x) << (y) | (unsigned)(x) >> (32 - (y)))
#define rotr(x, y) ((unsigned)(x) >> (y) | (unsigned)(x) << (32 - (y)))

#define bswap(x) ((rotl(x, 8) & 0x00ff00ff) | (rotr(x, 8) & 0xff00ff00))

#define get_block(x)                                                           \
  {                                                                            \
    uint32_t *as_dwords = (uint32_t *)x;                                       \
    as_dwords[0] = bswap(in_blk[3]);                                           \
    as_dwords[1] = bswap(in_blk[2]);                                           \
    as_dwords[2] = bswap(in_blk[1]);                                           \
    as_dwords[3] = bswap(in_blk[0]);                                           \
  }

#define put_block(x)                                                           \
  {                                                                            \
    uint32_t *as_dwords = (uint32_t *)x;                                       \
    out_blk[3] = bswap(as_dwords[0]);                                          \
    out_blk[2] = bswap(as_dwords[1]);                                          \
    out_blk[1] = bswap(as_dwords[2]);                                          \
    out_blk[0] = bswap(as_dwords[3]);                                          \
  }

#define get_key(x, len)                                                        \
  {                                                                            \
    uint32_t *as_dwords = (uint32_t *)x;                                       \
                                                                               \
    as_dwords[4] = as_dwords[5] = as_dwords[6] = as_dwords[7] = 0;             \
                                                                               \
    switch ((((len) + 63) / 64)) {                                             \
    case 2:                                                                    \
      as_dwords[0] = bswap(in_key[3]);                                         \
      as_dwords[1] = bswap(in_key[2]);                                         \
      as_dwords[2] = bswap(in_key[1]);                                         \
      as_dwords[3] = bswap(in_key[0]);                                         \
      break;                                                                   \
    case 3:                                                                    \
      as_dwords[0] = bswap(in_key[5]);                                         \
      as_dwords[1] = bswap(in_key[4]);                                         \
      as_dwords[2] = bswap(in_key[3]);                                         \
      as_dwords[3] = bswap(in_key[2]);                                         \
      as_dwords[4] = bswap(in_key[1]);                                         \
      as_dwords[5] = bswap(in_key[0]);                                         \
      break;                                                                   \
    case 4:                                                                    \
      as_dwords[0] = bswap(in_key[7]);                                         \
      as_dwords[1] = bswap(in_key[6]);                                         \
      as_dwords[2] = bswap(in_key[5]);                                         \
      as_dwords[3] = bswap(in_key[4]);                                         \
      as_dwords[4] = bswap(in_key[3]);                                         \
      as_dwords[5] = bswap(in_key[2]);                                         \
      as_dwords[6] = bswap(in_key[1]);                                         \
      as_dwords[7] = bswap(in_key[0]);                                         \
    }                                                                          \
  }

#define ik_len 2304

static const uint8_t seed[256] = {
    113, 21,  232, 18,  113, 92,  63,  157, 124, 193, 166, 197, 126,
    56,  229, 229, 156, 162, 54,  17,  230, 89,  189, 87,  169, 0,
    81,  204, 8,   70,  203, 225, 160, 59,  167, 189, 100, 157, 84,
    11,  7,   130, 29,  51,  32,  45,  135, 237, 139, 33,  17,  221,
    24,  50,  89,  74,  21,  205, 191, 242, 84,  53,  3,   230,

    231, 118, 15,  15,  107, 4,   21,  34,  3,   156, 57,  66,  93,
    255, 191, 3,   85,  135, 205, 200, 185, 204, 52,  37,  35,  24,
    68,  185, 201, 10,  224, 234, 7,   120, 201, 115, 216, 103, 57,
    255, 93,  110, 42,  249, 68,  14,  29,  55,  128, 84,  37,  152,
    221, 137, 39,  11,  252, 50,  144, 35,  178, 190, 43,  162,

    103, 249, 109, 8,   235, 33,  158, 111, 252, 205, 169, 54,  10,
    20,  221, 201, 178, 224, 89,  184, 182, 65,  201, 10,  60,  6,
    191, 174, 79,  98,  26,  160, 252, 51,  63,  79,  6,   102, 123,
    173, 49,  3,   110, 233, 90,  158, 228, 210, 209, 237, 30,  95,
    28,  179, 204, 220, 72,  163, 77,  166, 192, 98,  165, 25,

    145, 162, 91,  212, 41,  230, 110, 6,   107, 187, 127, 38,  82,
    98,  30,  67,  225, 80,  208, 134, 60,  250, 153, 87,  148, 60,
    66,  165, 72,  29,  165, 82,  211, 207, 0,   177, 206, 13,  6,
    14,  92,  248, 60,  201, 132, 95,  35,  215, 118, 177, 121, 180,
    27,  83,  131, 26,  39,  46,  12,  0,   0,   0,   0,   0};

// top is 1 greater than that used in FROG specification
// this routine makes a permutation containing 'top' values

static void make_perm(uint8_t *ip, uint32_t top) {
  uint8_t ua[260];
  for (int i = 0; i < top; ++i)
    ua[i] = i;

  uint32_t ie = 0;
  uint32_t ne = top;

  for (int i = 0; i < top - 1; ++i) {
    ie = (ie + ip[i]) % ne;

    ip[i] = ua[ie];
    ne--;

    for (int j = ie; j < ne; ++j)
      ua[j] = ua[j + 1];
  }

  ip[top - 1] = ua[0];
}

static void make_ikey(key_str *kp) {
  for (int i = 0; i < 8; ++i) {
    make_perm(kp->f_key[i].k_spu, 256);

    for (int j = 0; j < 256; ++j)
      kp->i_key[i][kp->f_key[i].k_spu[j]] = j;

    make_perm(kp->f_key[i].k_bpu, 16);

    uint8_t ua[16];
    for (int j = 0; j < 16; ++j)
      ua[j] = 0;

    uint32_t ix = 0;
    for (int j = 0; j < 15; ++j) {
      if (!kp->f_key[i].k_bpu[ix]) {
        uint32_t k = ix;

        do {
          k = (k + 1) & 15;
        } while (ua[k]);

        kp->f_key[i].k_bpu[ix] = k;

        uint32_t ll = k;
        while (kp->f_key[i].k_bpu[ll] != k)
          ll = kp->f_key[i].k_bpu[ll];

        kp->f_key[i].k_bpu[ll] = 0;
      }

      ua[ix] = 1;
      ix = kp->f_key[i].k_bpu[ix];
    }

    for (int j = 0; j < 16; ++j)
      if (kp->f_key[i].k_bpu[j] == ((j + 1) & 15))
        kp->f_key[i].k_bpu[j] = (j + 2) & 15;
  }
}

#define f_rnd(j)                                                               \
  {                                                                            \
    uint32_t k = pp[j];                                                        \
    blk[j] = sp[blk[j] ^ xp[j]];                                               \
    blk[(j + 1) & 15] ^= blk[j];                                               \
    blk[k] ^= blk[j];                                                          \
  }

#define b_rnd(j)                                                               \
  {                                                                            \
    uint32_t ct = blk[j];                                                      \
    blk[pp[j]] ^= blk[j];                                                      \
    blk[(j + 1) & 15] ^= blk[j];                                               \
    blk[j] = sp[ct] ^ xp[j];                                                   \
  }

static void enc(const FROG_DATA *pFd, const uint32_t in_blk[4],
                uint32_t out_blk[4]) {
  uint8_t blk[16];

  *(uint32_t *)(blk +  0) = in_blk[0];
  *(uint32_t *)(blk +  4) = in_blk[1];
  *(uint32_t *)(blk +  8) = in_blk[2];
  *(uint32_t *)(blk + 12) = in_blk[3];

  for (int i = 0; i < 8; ++i) {
    uint8_t *xp = pFd->lkp->f_key[i].k_xbu;
    uint8_t *sp = pFd->lkp->f_key[i].k_spu;
    uint8_t *pp = pFd->lkp->f_key[i].k_bpu;

    f_rnd(0);
    f_rnd(1);
    f_rnd(2);
    f_rnd(3);
    f_rnd(4);
    f_rnd(5);
    f_rnd(6);
    f_rnd(7);
    f_rnd(8);
    f_rnd(9);
    f_rnd(10);
    f_rnd(11);
    f_rnd(12);
    f_rnd(13);
    f_rnd(14);
    f_rnd(15);
  }

  out_blk[0] = *(uint32_t *)(blk +  0);
  out_blk[1] = *(uint32_t *)(blk +  4);
  out_blk[2] = *(uint32_t *)(blk +  8);
  out_blk[3] = *(uint32_t *)(blk + 12);
}

/* initialise the key schedule from the user supplied key   */
void Frog_set_key(FROG_DATA *pFd, const uint32_t *in_key,
                  const uint32_t key_len) {
  uint8_t kb[32];

  get_key(kb, key_len);

  uint32_t a = 0, b = 0;
  for (int i = 0; i < ik_len; ++i) {
    ((uint8_t *)(pFd->sim_key.f_key))[i] = seed[a] ^ kb[b];

    a = (a + 1) % 251;
    b = (b + 1) % (key_len / 8);
  }

  make_ikey(&pFd->sim_key);
  pFd->lkp = &pFd->sim_key;

  kb[0] ^= (key_len / 8);

  for (int i = 0; i < ik_len / 16; ++i) {
    enc(pFd, (uint32_t *)kb, (uint32_t *)kb);

    for (int j = 0; j < 16; ++j)
      ((uint8_t *)(pFd->loc_key.f_key))[16 * i + j] = kb[j];
  }

  make_ikey(&pFd->loc_key);
  pFd->lkp = &pFd->loc_key;
}

/* encrypt a block of text  */
void Frog_encrypt(const FROG_DATA *pFd, const uint32_t in_blk[4],
                  uint32_t out_blk[4]) {
  uint8_t blk[16];

  get_block(blk);

  for (int i = 0; i < 8; ++i) {
    uint8_t *xp = pFd->lkp->f_key[i].k_xbu;
    uint8_t *sp = pFd->lkp->f_key[i].k_spu;
    uint8_t *pp = pFd->lkp->f_key[i].k_bpu;

    f_rnd(0);
    f_rnd(1);
    f_rnd(2);
    f_rnd(3);
    f_rnd(4);
    f_rnd(5);
    f_rnd(6);
    f_rnd(7);
    f_rnd(8);
    f_rnd(9);
    f_rnd(10);
    f_rnd(11);
    f_rnd(12);
    f_rnd(13);
    f_rnd(14);
    f_rnd(15);
  }

  put_block(blk);
}

/* decrypt a block of text  */
void Frog_decrypt(const FROG_DATA *pFd, const uint32_t in_blk[4],
                  uint32_t out_blk[4]) {
  uint8_t blk[16];

  get_block(blk);

  for (int i = 7; i >= 0; --i) {
    uint8_t *xp = pFd->lkp->f_key[i].k_xbu;
    uint8_t *sp = pFd->lkp->i_key[i];
    uint8_t *pp = pFd->lkp->f_key[i].k_bpu;

    b_rnd(15);
    b_rnd(14);
    b_rnd(13);
    b_rnd(12);
    b_rnd(11);
    b_rnd(10);
    b_rnd(9);
    b_rnd(8);
    b_rnd(7);
    b_rnd(6);
    b_rnd(5);
    b_rnd(4);
    b_rnd(3);
    b_rnd(2);
    b_rnd(1);
    b_rnd(0);
  }

  put_block(blk);
}
