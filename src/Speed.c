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

#include "Speed.h"

#include <string.h>

#define KEY_LEN_Duint8_t                                                       \
  (SPEED_KEY_LEN / 16)                /* length of a key in double bytes */
#define F_WD_LEN (SPEED_DATA_LEN / 8) /* length of a SPEED word in bits */
#define H_WD_LEN (F_WD_LEN / 2)       /* length of a half word  in bits */

#if SPEED_DATA_LEN == 256
#define F_WD_MASK 0xFFFFFFFFL
#define H_WD_MASK 0xFFFF
#define V_SHIFT 11
#define KB_SIZE (2 * SPEED_NO_OF_RND) /* size of kb in key scheduling */
#elif SPEED_DATA_LEN == 128
#define F_WD_MASK 0xFFFF
#define H_WD_MASK 0xFF
#define V_SHIFT 4
#define KB_SIZE SPEED_NO_OF_RND
#else /* SPEED_DATA_LEN == 64 */
#define F_WD_MASK 0xFF
#define H_WD_MASK 0xF
#define V_SHIFT 1
#define KB_SIZE (SPEED_NO_OF_RND / 2)
#endif

/* cyclically shift a SPEED internal data to the right by n bits */
#define rotate_data_right(x, n)                                                \
  ((((x)&F_WD_MASK) >> (n)) | (((x)&F_WD_MASK) << (F_WD_LEN - (n))))

/* cyclically shift a SPEED internal data to the left by n bits */
#define rotate_data_left(x, n)                                                 \
  ((((x)&F_WD_MASK) << (n)) | (((x)&F_WD_MASK) >> (F_WD_LEN - (n))))

/* GG is the non-linear function used in key scheduling */
#define GG(x2, x1, x0) ((x2) & (x1)) ^ ((x1) & (x0)) ^ ((x0) & (x2))

/* FF1 is a non-linear function used in Pass 1 */
#define FF1(x6, x5, x4, x3, x2, x1, x0)                                        \
  ((x6) & (x3)) ^ ((x5) & (x1)) ^ ((x4) & (x2)) ^ ((x1) & (x0)) ^ (x0)

/* FF2 is a non-linear function used in Pass 2 */
#define FF2(x6, x5, x4, x3, x2, x1, x0)                                        \
  ((x6) & (x4) & (x0)) ^ ((x4) & (x3) & (x0)) ^ ((x5) & (x2)) ^                \
      ((x4) & (x3)) ^ ((x4) & (x1)) ^ ((x3) & (x0)) ^ (x1)

/* FF3 is a non-linear function used in Pass 3 */
#define FF3(x6, x5, x4, x3, x2, x1, x0)                                        \
  ((x5) & (x4) & (x0)) ^ ((x6) & (x4)) ^ ((x5) & (x2)) ^ ((x3) & (x0)) ^       \
      ((x1) & (x0)) ^ (x3)

/* FF4 is a non-linear function used in Pass 4 */
#define FF4(x6, x5, x4, x3, x2, x1, x0)                                        \
  ((x6) & (x4) & (x2) & (x0)) ^ ((x6) & (x5)) ^ ((x4) & (x3)) ^                \
      ((x3) & (x2)) ^ ((x1) & (x0)) ^ (x2)

/* translates chars into SPEED internal words */
#if SPEED_DATA_LEN == 256
#define txt2itxt(txt, itxt)                                                    \
  {                                                                            \
    for (int i = 0; i < 8; i++) {                                              \
      itxt[i] = ((uint32_t)txt[4 * i + 0] <<  0) |                             \
                ((uint32_t)txt[4 * i + 1] <<  8) |                             \
                ((uint32_t)txt[4 * i + 2] << 16) |                             \
                ((uint32_t)txt[4 * i + 3] << 24);                              \
    }                                                                          \
  }
#elif SPEED_DATA_LEN == 128
#define txt2itxt(txt, itxt)                                                    \
  {                                                                            \
    for (int i = 0; i < 8; i++) {                                              \
      itxt[i] = ((uint32_t)txt[2 * i + 0] << 0) |                              \
                ((uint32_t)txt[2 * i + 1] << 8);                               \
    }                                                                          \
  }
#else
#define txt2itxt(txt, itxt)                                                    \
  {                                                                            \
    for (int i = 0; i < 8; i++) {                                              \
      itxt[i] = ((uint32_t)txt[1 * i + 0] << 0);                               \
    }                                                                          \
  }
#endif

/* translates SPEED internal words into chars */
#if SPEED_DATA_LEN == 256
#define itxt2txt(itxt, txt)                                                    \
  {                                                                            \
    for (int i = 0; i < 8; i++) {                                              \
      txt[4 * i + 0] = (uint8_t)((itxt[i] >>  0) & 0xFF);                      \
      txt[4 * i + 1] = (uint8_t)((itxt[i] >>  8) & 0xFF);                      \
      txt[4 * i + 2] = (uint8_t)((itxt[i] >> 16) & 0xFF);                      \
      txt[4 * i + 3] = (uint8_t)((itxt[i] >> 24) & 0xFF);                      \
    }                                                                          \
  }
#elif SPEED_DATA_LEN == 128
#define itxt2txt(itxt, txt)                                                    \
  {                                                                            \
    for (int i = 0; i < 8; i++) {                                              \
      txt[2 * i + 0] = (uint8_t)((itxt[i] >> 0) & 0xFF);                       \
      txt[2 * i + 1] = (uint8_t)((itxt[i] >> 8) & 0xFF);                       \
    }                                                                          \
  }
#else
#define itxt2txt(itxt, txt)                                                    \
  {                                                                            \
    for (int i = 0; i < 8; i++) {                                              \
      txt[1 * i + 0] = (uint8_t)((itxt[i] >> 0) & 0xFF);                       \
    }                                                                          \
  }
#endif

/* fractional part of sqrt(15), used in key scheduling */
#if SPEED_KEY_LEN == 48
static unsigned short sqrt15[3] = {0xDF7B, 0xD629, 0xE9DB};
#elif SPEED_KEY_LEN == 64
static unsigned short sqrt15[3] = {0x362F, 0x5D00, 0xF20F};
#elif SPEED_KEY_LEN == 80
static unsigned short sqrt15[3] = {0xC3D1, 0x1FD2, 0x589B};
#elif SPEED_KEY_LEN == 96
static unsigned short sqrt15[3] = {0x4312, 0x91EB, 0x718E};
#elif SPEED_KEY_LEN == 112
static unsigned short sqrt15[3] = {0xBF2A, 0x1E7D, 0xB257};
#elif SPEED_KEY_LEN == 128
static unsigned short sqrt15[3] = {0x77A6, 0x1654, 0x6B2A};
#elif SPEED_KEY_LEN == 144
static unsigned short sqrt15[3] = {0x0D9B, 0xA9D3, 0x668F};
#elif SPEED_KEY_LEN == 160
static unsigned short sqrt15[3] = {0x19BE, 0xF855, 0x6D98};
#elif SPEED_KEY_LEN == 176
static unsigned short sqrt15[3] = {0x022D, 0xE4E2, 0xD017};
#elif SPEED_KEY_LEN == 192
static unsigned short sqrt15[3] = {0xEA2F, 0x7572, 0xC3B5};
#elif SPEED_KEY_LEN == 208
static unsigned short sqrt15[3] = {0x1086, 0x480C, 0x3AA6};
#elif SPEED_KEY_LEN == 224
static unsigned short sqrt15[3] = {0x9CA0, 0x98F7, 0xD0E4};
#elif SPEED_KEY_LEN == 240
static unsigned short sqrt15[3] = {0x253C, 0xC901, 0x55F3};
#else /* SPEED_KEY_LEN == 256 */
static unsigned short sqrt15[3] = {0x9BF4, 0xF659, 0xD76C};
#endif

/* key scheduling */
void Speed_set_key(speed_ikey rndkey, const speed_key key) {
  uint32_t kb[KB_SIZE];

  /* Step 1: translates a user key into 2-byte data units */
  for (uint32_t i = 0; i < KEY_LEN_Duint8_t; i++) {
    kb[i] = ((uint32_t)key[2 * i + 0] << 0) |
            ((uint32_t)key[2 * i + 1] << 8);
  }

  uint32_t s0 = (uint32_t)sqrt15[0];
  uint32_t s1 = (uint32_t)sqrt15[1];
  uint32_t s2 = (uint32_t)sqrt15[2];

  /* Step 2: extends the key */
  for (uint32_t i = KEY_LEN_Duint8_t; i < KB_SIZE; i++) {
    uint32_t temp = GG(s2, s1, s0);
    temp = (temp << 5) | (temp >> 11);
    temp += s2 + kb[i % KEY_LEN_Duint8_t];
    temp = temp & 0xFFFF;

    s2 = s1;
    s1 = s0;
    s0 = temp;

    kb[i] = temp;
  }

  /* Step 3: translates 2-byte data into round keys (SPEED words) */
#if SPEED_DATA_LEN == 256 /* speed word length = 32 bits */
  for (uint32_t i = 0; i < KB_SIZE / 2; i++) {
    rndkey[i] = (kb[2 * i + 0] << 0) |
                (kb[2 * i + 1] << 16);
  }
#elif SPEED_DATA_LEN == 128 /* speed word length = 16 bits */
  memcpy(rndkey, kb, KB_SIZE * sizeof(uint32_t));
#else                       /* speed word length = 8 bits */
  for (uint32_t i = 0; i < KB_SIZE; i++) {
    rndkey[2 * i + 0] = (kb[i] >> 0) & 0xFF;
    rndkey[2 * i + 1] = (kb[i] >> 8) & 0xFF;
  }
#endif
}

/* encryption --- SPEED internal word oriented interface */
static void encrypt_rk(const speed_ikey rndkey, const speed_idata ipt,
                       speed_idata ict) {
  uint32_t t0 = ipt[0];
  uint32_t t1 = ipt[1];
  uint32_t t2 = ipt[2];
  uint32_t t3 = ipt[3];
  uint32_t t4 = ipt[4];
  uint32_t t5 = ipt[5];
  uint32_t t6 = ipt[6];
  uint32_t t7 = ipt[7];

  uint32_t k = 0; /* points to the first round key */

  /* Pass 1 */
  for (uint32_t i = 0; i < (SPEED_NO_OF_RND >> 2); i++) {
    uint32_t temp = FF1(t6, t5, t4, t3, t2, t1, t0);
    uint32_t vv = (((temp >> H_WD_LEN) + temp) & H_WD_MASK) >> V_SHIFT;
    temp = rotate_data_right(t7, H_WD_LEN - 1) +
           rotate_data_right(temp, vv) +
           rndkey[k++];

    t7 = t6;
    t6 = t5;
    t5 = t4;
    t4 = t3;
    t3 = t2;
    t2 = t1;
    t1 = t0;
    t0 = temp & F_WD_MASK;
  }

  /* Pass 2 */
  for (uint32_t i = 0; i < (SPEED_NO_OF_RND >> 2); i++) {
    uint32_t temp = FF2(t6, t5, t4, t3, t2, t1, t0);
    uint32_t vv = (((temp >> H_WD_LEN) + temp) & H_WD_MASK) >> V_SHIFT;
    temp = rotate_data_right(t7, H_WD_LEN - 1) +
           rotate_data_right(temp, vv) +
           rndkey[k++];

    t7 = t6;
    t6 = t5;
    t5 = t4;
    t4 = t3;
    t3 = t2;
    t2 = t1;
    t1 = t0;
    t0 = temp & F_WD_MASK;
  }

  /* Pass 3 */
  for (uint32_t i = 0; i < (SPEED_NO_OF_RND >> 2); i++) {
    uint32_t temp = FF3(t6, t5, t4, t3, t2, t1, t0);
    uint32_t vv = (((temp >> H_WD_LEN) + temp) & H_WD_MASK) >> V_SHIFT;
    temp = rotate_data_right(t7, H_WD_LEN - 1) +
           rotate_data_right(temp, vv) +
           rndkey[k++];

    t7 = t6;
    t6 = t5;
    t5 = t4;
    t4 = t3;
    t3 = t2;
    t2 = t1;
    t1 = t0;
    t0 = temp & F_WD_MASK;
  }

  /* Pass 4 */
  for (uint32_t i = 0; i < (SPEED_NO_OF_RND >> 2); i++) {
    uint32_t temp = FF4(t6, t5, t4, t3, t2, t1, t0);
    uint32_t vv = (((temp >> H_WD_LEN) + temp) & H_WD_MASK) >> V_SHIFT;
    temp = rotate_data_right(t7, H_WD_LEN - 1) +
           rotate_data_right(temp, vv) +
           rndkey[k++];

    t7 = t6;
    t6 = t5;
    t5 = t4;
    t4 = t3;
    t3 = t2;
    t2 = t1;
    t1 = t0;
    t0 = temp & F_WD_MASK;
  }

  ict[0] = t0;
  ict[1] = t1;
  ict[2] = t2;
  ict[3] = t3;
  ict[4] = t4;
  ict[5] = t5;
  ict[6] = t6;
  ict[7] = t7;
}

/* decryption --- SPEED internal word oriented interface */
static void decrypt_rk(const speed_ikey rndkey, const speed_idata ict,
                       speed_idata ipt) {
  uint32_t t0 = ict[0];
  uint32_t t1 = ict[1];
  uint32_t t2 = ict[2];
  uint32_t t3 = ict[3];
  uint32_t t4 = ict[4];
  uint32_t t5 = ict[5];
  uint32_t t6 = ict[6];
  uint32_t t7 = ict[7];

  uint32_t k = SPEED_NO_OF_RND - 1; /* points to the last round key */

  /* Inverse of Pass 4 */
  for (uint32_t i = 0; i < (SPEED_NO_OF_RND >> 2); i++) {
    uint32_t new_ipt7 = t0;

    t0 = t1;
    t1 = t2;
    t2 = t3;
    t3 = t4;
    t4 = t5;
    t5 = t6;
    t6 = t7;

    uint32_t temp = FF4(t6, t5, t4, t3, t2, t1, t0);
    uint32_t vv = (((temp >> H_WD_LEN) + temp) & H_WD_MASK) >> V_SHIFT;

    new_ipt7 = new_ipt7 - rotate_data_right(temp, vv) - rndkey[k--];

    t7 = rotate_data_left(new_ipt7, H_WD_LEN - 1);
  }

  /* Inverse of Pass 3 */
  for (uint32_t i = 0; i < (SPEED_NO_OF_RND >> 2); i++) {
    uint32_t new_ipt7 = t0;
    t0 = t1;
    t1 = t2;
    t2 = t3;
    t3 = t4;
    t4 = t5;
    t5 = t6;
    t6 = t7;

    uint32_t temp = FF3(t6, t5, t4, t3, t2, t1, t0);
    uint32_t vv = (((temp >> H_WD_LEN) + temp) & H_WD_MASK) >> V_SHIFT;

    new_ipt7 = new_ipt7 - rotate_data_right(temp, vv) - rndkey[k--];

    t7 = rotate_data_left(new_ipt7, H_WD_LEN - 1);
  }

  /* Inverse of Pass 2 */
  for (uint32_t i = 0; i < (SPEED_NO_OF_RND >> 2); i++) {
    uint32_t new_ipt7 = t0;
    t0 = t1;
    t1 = t2;
    t2 = t3;
    t3 = t4;
    t4 = t5;
    t5 = t6;
    t6 = t7;

    uint32_t temp = FF2(t6, t5, t4, t3, t2, t1, t0);
    uint32_t vv = (((temp >> H_WD_LEN) + temp) & H_WD_MASK) >> V_SHIFT;

    new_ipt7 = new_ipt7 - rotate_data_right(temp, vv) - rndkey[k--];

    t7 = rotate_data_left(new_ipt7, H_WD_LEN - 1);
  }

  /* Inverse of Pass 1 */
  for (uint32_t i = 0; i < (SPEED_NO_OF_RND >> 2); i++) {
    uint32_t new_ipt7 = t0;
    t0 = t1;
    t1 = t2;
    t2 = t3;
    t3 = t4;
    t4 = t5;
    t5 = t6;
    t6 = t7;

    uint32_t temp = FF1(t6, t5, t4, t3, t2, t1, t0);
    uint32_t vv = (((temp >> H_WD_LEN) + temp) & H_WD_MASK) >> V_SHIFT;

    new_ipt7 = new_ipt7 - rotate_data_right(temp, vv) - rndkey[k--];

    t7 = rotate_data_left(new_ipt7, H_WD_LEN - 1);
  }

  ipt[0] = t0;
  ipt[1] = t1;
  ipt[2] = t2;
  ipt[3] = t3;
  ipt[4] = t4;
  ipt[5] = t5;
  ipt[6] = t6;
  ipt[7] = t7;
}

/* encryption --- char oriented interface */
void Speed_encrypt(const speed_ikey rndkey, const speed_data pt,
                   speed_data ct) {
  speed_idata ipt;
  speed_idata ict;

  txt2itxt(pt, ipt);
  encrypt_rk(rndkey, ipt, ict);
  itxt2txt(ict, ct);
}

/* decryption --- char oriented interface */
void Speed_decrypt(const speed_ikey rndkey, const speed_data ct,
                   speed_data pt) {
  speed_idata ipt;
  speed_idata ict;

  txt2itxt(ct, ict);
  decrypt_rk(rndkey, ict, ipt);
  itxt2txt(ipt, pt);
}
