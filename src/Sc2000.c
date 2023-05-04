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

#include "Sc2000.h"

#define T32(x) ((x)&ONE32)
#define ONE32 0xffffffffU
#define ROTL32(v, n) (T32((v) << (n)) | ((v) >> (32 - (n))))

/* S-boxes  (6-bit)(5-bit)  */
const uint32_t S6[64] = {
  47, 59, 25, 42, 15, 23, 28, 39, 26, 38, 36, 19, 60, 24, 29, 56,
  37, 63, 20, 61, 55,  2, 30, 44,  9, 10,  6, 22, 53, 48, 51, 11,
  62, 52, 35, 18, 14, 46,  0, 54, 17, 40, 27,  4, 31,  8,  5, 12,
   3, 16, 41, 34, 33,  7, 45, 49, 50, 58,  1, 21, 43, 57, 32, 13
};
const uint32_t S5[32] = {
  20, 26,  7, 31, 19, 12, 10, 15, 22, 30, 13, 14,  4, 24,  9, 18,
  27, 11,  1, 21,  6, 16,  2, 28, 23,  5,  8,  3,  0, 17, 29, 25
};

/* Bit-slice S-Box (4-bit) */
/* 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 */
static const uint32_t S4[16] = {2,  5, 10, 12, 7, 15, 1, 11,
                                13, 6, 0,  9,  4, 8,  3, 14};
static const uint32_t S4i[16] = {10, 6,  0, 14, 12, 1, 9,  4,
                                 13, 11, 2, 7,  3,  8, 15, 5};

/* M-Table */
static const uint32_t M[32] = {
  0xd0c19225, 0xa5a2240a, 0x1b84d250, 0xb728a4a1,
  0x6a704902, 0x85dddbe6, 0x766ff4a4, 0xecdfe128,
  0xafd13e94, 0xdf837d09, 0xbb27fa52, 0x695059ac,
  0x52a1bb58, 0xcc322f1d, 0x1844565b, 0xb4a8acf6,
  0x34235438, 0x6847a851, 0xe48c0cbb, 0xcd181136,
  0x9a112a0c, 0x43ec6d0e, 0x87d8d27d, 0x487dc995,
  0x90fb9b4b, 0xa1f63697, 0xfc513ed9, 0x78a37d93,
  0x8d16c5df, 0x9e0c8bbe, 0x3c381f7c, 0xe9fb0779
};

#define _A_ 0
#define _B_ 1
#define _C_ 2
#define _D_ 3
#define _X_ 0
#define _Y_ 1
#define _Z_ 2
#define _W_ 3

/* Order Table */
static const uint32_t Order[12][4] = {
    {_A_, _B_, _C_, _D_}, {_B_, _A_, _D_, _C_}, {_C_, _D_, _A_, _B_},
    {_D_, _C_, _B_, _A_}, {_A_, _C_, _D_, _B_}, {_B_, _D_, _C_, _A_},
    {_C_, _A_, _B_, _D_}, {_D_, _B_, _A_, _C_}, {_A_, _D_, _B_, _C_},
    {_B_, _C_, _A_, _D_}, {_C_, _B_, _D_, _A_}, {_D_, _A_, _C_, _B_}};

/* Index Table */
static const uint32_t Index[9][4] = {{0, 0, 0, 0}, {1, 1, 1, 1}, {2, 2, 2, 2},
                                     {0, 1, 0, 1}, {1, 2, 1, 2}, {2, 0, 2, 0},
                                     {0, 2, 0, 2}, {1, 0, 1, 0}, {2, 1, 2, 1}};

static void S_func(uint32_t a, uint32_t *b) {
  uint32_t q = (a >> 26) & 0x3F;
  uint32_t r = (a >> 21) & 0x1F;
  uint32_t s = (a >> 16) & 0x1F;
  uint32_t t = (a >> 11) & 0x1F;
  uint32_t u = (a >>  6) & 0x1F;
  uint32_t v = (a >>  0) & 0x3F;

  q = S6[q];
  r = S5[r];
  s = S5[s];
  t = S5[t];
  u = S5[u];
  v = S6[v];

  *b  = (q << 26);
  *b |= (r << 21);
  *b |= (s << 16);
  *b |= (t << 11);
  *b |= (u <<  6);
  *b |= (v <<  0);
}

static void M_func(uint32_t a, uint32_t *b) {
  *b = 0;

  for (int i = 31; i >= 0; i--) {
    if (a & 1)
      *b ^= M[i];

    a >>= 1;
  }
}

static void L_func(uint32_t a, uint32_t b, uint32_t mask,
                   uint32_t *c, uint32_t *d) {
  uint32_t imask = (mask ^ 0xFFFFFFFF);

  uint32_t s = a & mask;
  uint32_t t = b & imask;

  *c = s ^ b;
  *d = t ^ a;
}

static void F_func(uint32_t a, uint32_t b, uint32_t mask,
                   uint32_t *c, uint32_t *d) {
  uint32_t s, t;

  S_func(a, &s);
  M_func(s, &s);
  S_func(b, &t);
  M_func(t, &t);
  L_func(s, t, mask, c, d);
}

static void R_func(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t mask,
                   uint32_t *e, uint32_t *f, uint32_t *g, uint32_t *h) {
  uint32_t s, t;
  F_func(c, d, mask, &s, &t);

  *e = a ^ s;
  *f = b ^ t;
  *g = c;
  *h = d;
}

static void B_func(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                   uint32_t *e, uint32_t *f, uint32_t *g, uint32_t *h) {
  uint32_t m = 1;

  *e = 0;
  *f = 0;
  *g = 0;
  *h = 0;

  for (int i = 0; i < 32; i++) {
    /* T_func */
    uint32_t s = 0;
    if (a & m)
      s |= 8;
    if (b & m)
      s |= 4;
    if (c & m)
      s |= 2;
    if (d & m)
      s |= 1;

    uint32_t t = S4[s];
    if (t & 8)
      *e |= m;
    if (t & 4)
      *f |= m;
    if (t & 2)
      *g |= m;
    if (t & 1)
      *h |= m;

    m <<= 1;
  }
}

static void Bi_func(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                    uint32_t *e, uint32_t *f, uint32_t *g, uint32_t *h) {
  uint32_t m = 1;

  *e = 0;
  *f = 0;
  *g = 0;
  *h = 0;

  for (int i = 0; i < 32; i++) {
    /*T_func */
    uint32_t s = 0;
    if (a & m)
      s |= 8;
    if (b & m)
      s |= 4;
    if (c & m)
      s |= 2;
    if (d & m)
      s |= 1;

    uint32_t t = S4i[s];
    if (t & 8)
      *e |= m;
    if (t & 4)
      *f |= m;
    if (t & 2)
      *g |= m;
    if (t & 1)
      *h |= m;

    m <<= 1;
  }
}

static void I_func(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                   uint32_t ka, uint32_t kb, uint32_t kc, uint32_t kd,
                   uint32_t *e, uint32_t *f, uint32_t *g, uint32_t *h) {
  *e = a ^ ka;
  *f = b ^ kb;
  *g = c ^ kc;
  *h = d ^ kd;
}

static uint32_t make_one_imkey(uint32_t k1, uint32_t k2,
                               uint32_t i, uint32_t j) {
  uint32_t ka = k1;
  S_func(ka, &ka);
  M_func(ka, &ka);

  uint32_t kb = k2;
  S_func(kb, &kb);
  M_func(kb, &kb);

  uint32_t m = 4 * i + j;
  S_func(m, &m);
  M_func(m, &m);

  ka += m;
  ka &= 0xFFFFFFFF;
  kb *= (i + 1);
  kb &= 0xFFFFFFFF;
  ka ^= kb;

  S_func(ka, &ka);
  M_func(ka, &ka);

  return ka;
}

static void make_imkeys(const uint32_t *ukey, uint32_t keylength,
                        uint32_t imkey[4][3]) {
  uint32_t kl = ukey[0];
  uint32_t k2 = ukey[1];
  uint32_t k3 = ukey[2];
  uint32_t k4 = ukey[3];

  uint32_t k5 = ukey[4];
  uint32_t k6 = ukey[5];
  uint32_t k7 = ukey[6];
  uint32_t k8 = ukey[7];

  for (int i = 0; i < 3; i++) {
    imkey[_A_][i] = make_one_imkey(kl, k2, i, 0);
    imkey[_B_][i] = make_one_imkey(k3, k4, i, 1);
    imkey[_C_][i] = make_one_imkey(k5, k6, i, 2);
    imkey[_D_][i] = make_one_imkey(k7, k8, i, 3);
  }
}

static uint32_t make_one_ekey(uint32_t imkey[4][3], uint32_t t, uint32_t s) {
  uint32_t x = imkey[Order[t][_X_]][Index[s][_X_]];
  uint32_t y = imkey[Order[t][_Y_]][Index[s][_Y_]];
  uint32_t z = imkey[Order[t][_Z_]][Index[s][_Z_]];
  uint32_t w = imkey[Order[t][_W_]][Index[s][_W_]];

  x = ROTL32(x, 1);
  x += y;
  x &= 0xFFFFFFFF;
  z = ROTL32(z, 1);
  z -= w;
  z &= 0xFFFFFFFF;
  z = ROTL32(z, 1);
  x ^= z;

  return x;
}

static void make_ekeys(uint32_t imkey[4][3], uint32_t num_ekey,
                       uint32_t *ekey) {
  for (uint32_t n = 0; n < num_ekey; n++) {
    uint32_t t = (n + (n / 36)) % 12;
    uint32_t s = n % 9;

    ekey[n] = make_one_ekey(imkey, t, s);
  }
}

void Sc2000_set_key(uint32_t *ek, const uint32_t *in_key) {
  uint32_t imkey[4][3];

  /* make intermediate key */
  make_imkeys(in_key, 256, imkey);

  /* make extend key */
  make_ekeys(imkey, 64, ek);
}

void Sc2000_encrypt(const uint32_t *ek, const uint32_t *in, uint32_t *out) {
  uint32_t a = in[0];
  uint32_t b = in[1];
  uint32_t c = in[2];
  uint32_t d = in[3];

  I_func(a, b, c, d, ek[ 0], ek[ 1], ek[ 2], ek[ 3], &a, &b, &c, &d);
  B_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[ 4], ek[ 5], ek[ 6], ek[ 7], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x55555555, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x55555555, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[ 8], ek[ 9], ek[10], ek[11], &a, &b, &c, &d);
  B_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[12], ek[13], ek[14], ek[15], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x33333333, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x33333333, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[16], ek[17], ek[18], ek[19], &a, &b, &c, &d);
  B_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[20], ek[21], ek[22], ek[23], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x55555555, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x55555555, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[24], ek[25], ek[26], ek[27], &a, &b, &c, &d);
  B_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[28], ek[29], ek[30], ek[31], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x33333333, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x33333333, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[32], ek[33], ek[34], ek[35], &a, &b, &c, &d);
  B_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[36], ek[37], ek[38], ek[39], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x55555555, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x55555555, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[40], ek[41], ek[42], ek[43], &a, &b, &c, &d);
  B_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[44], ek[45], ek[46], ek[47], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x33333333, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x33333333, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[48], ek[49], ek[50], ek[51], &a, &b, &c, &d);
  B_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[52], ek[53], ek[54], ek[55], &a, &b, &c, &d);

  out[0] = a;
  out[1] = b;
  out[2] = c;
  out[3] = d;
}

void Sc2000_decrypt(const uint32_t *ek, const uint32_t *in, uint32_t *out) {
  uint32_t a = in[0];
  uint32_t b = in[1];
  uint32_t c = in[2];
  uint32_t d = in[3];

  I_func(a, b, c, d, ek[52], ek[53], ek[54], ek[55], &a, &b, &c, &d);
  Bi_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[48], ek[49], ek[50], ek[51], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x33333333, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x33333333, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[44], ek[45], ek[46], ek[47], &a, &b, &c, &d);
  Bi_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[40], ek[41], ek[42], ek[43], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x55555555, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x55555555, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[36], ek[37], ek[38], ek[39], &a, &b, &c, &d);
  Bi_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[32], ek[33], ek[34], ek[35], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x33333333, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x33333333, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[28], ek[29], ek[30], ek[31], &a, &b, &c, &d);
  Bi_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[24], ek[25], ek[26], ek[27], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x55555555, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x55555555, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[20], ek[21], ek[22], ek[23], &a, &b, &c, &d);
  Bi_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[16], ek[17], ek[18], ek[19], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x33333333, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x33333333, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[12], ek[13], ek[14], ek[15], &a, &b, &c, &d);
  Bi_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[ 8], ek[ 9], ek[10], ek[11], &a, &b, &c, &d);
  R_func(a, b, c, d, 0x55555555, &a, &b, &c, &d);
  R_func(c, d, a, b, 0x55555555, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[ 4], ek[ 5], ek[ 6], ek[ 7], &a, &b, &c, &d);
  Bi_func(a, b, c, d, &a, &b, &c, &d);
  I_func(a, b, c, d, ek[ 0], ek[ 1], ek[ 2], ek[ 3], &a, &b, &c, &d);

  out[0] = a;
  out[1] = b;
  out[2] = c;
  out[3] = d;
}
