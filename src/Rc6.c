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

#include "Rc6.h"

#define rotl(x, y) ((unsigned)(x) << (y) | (unsigned)(x) >> (32 - (y)))
#define rotr(x, y) ((unsigned)(x) >> (y) | (unsigned)(x) << (32 - (y)))

#define f_rnd(i, a, b, c, d)                                                   \
  {                                                                            \
    uint32_t u = rotl(d * (d + d + 1), 5);                                     \
    uint32_t t = rotl(b * (b + b + 1), 5);                                     \
    a = rotl(a ^ t, u) + l_key[i + 0];                                         \
    c = rotl(c ^ u, t) + l_key[i + 1];                                         \
  }

#define i_rnd(i, a, b, c, d)                                                   \
  {                                                                            \
    uint32_t u = rotl(d * (d + d + 1), 5);                                     \
    uint32_t t = rotl(b * (b + b + 1), 5);                                     \
    c = rotr(c - l_key[i + 1], t) ^ u;                                         \
    a = rotr(a - l_key[i + 0], u) ^ t;                                         \
  }

void Rc6_set_key(uint32_t *l_key, const uint32_t *in_key,
                 const uint32_t key_len) {
  l_key[0] = 0xb7e15163;
  for (int k = 1; k < 44; ++k)
    l_key[k] = l_key[k - 1] + 0x9e3779b9;

  uint32_t l[8];
  for (int k = 0; k < key_len / 32; ++k)
    l[k] = in_key[k];

  uint32_t t = (key_len / 32) - 1; // t = (key_len / 32);
  uint32_t a = 0, b = 0, i = 0, j = 0;

  for (int k = 0; k < 132; ++k) {
    a = rotl(l_key[i] + a + b, 3);
    b += a;
    b = rotl(l[j] + b, b);

    l_key[i] = a;
    l[j] = b;

    i = (i == 43 ? 0 : i + 1); // i = (i + 1) % 44;
    j = (j == t ? 0 : j + 1);  // j = (j + 1) % t;
  }
}

void Rc6_encrypt(const uint32_t *l_key, const uint32_t *in_blk,
                 uint32_t *out_blk) {
  uint32_t a = in_blk[0];
  uint32_t b = in_blk[1] + l_key[0];
  uint32_t c = in_blk[2];
  uint32_t d = in_blk[3] + l_key[1];

  f_rnd(2, a, b, c, d);
  f_rnd(4, b, c, d, a);
  f_rnd(6, c, d, a, b);
  f_rnd(8, d, a, b, c);
  f_rnd(10, a, b, c, d);
  f_rnd(12, b, c, d, a);
  f_rnd(14, c, d, a, b);
  f_rnd(16, d, a, b, c);
  f_rnd(18, a, b, c, d);
  f_rnd(20, b, c, d, a);
  f_rnd(22, c, d, a, b);
  f_rnd(24, d, a, b, c);
  f_rnd(26, a, b, c, d);
  f_rnd(28, b, c, d, a);
  f_rnd(30, c, d, a, b);
  f_rnd(32, d, a, b, c);
  f_rnd(34, a, b, c, d);
  f_rnd(36, b, c, d, a);
  f_rnd(38, c, d, a, b);
  f_rnd(40, d, a, b, c);

  out_blk[0] = a + l_key[42];
  out_blk[1] = b;
  out_blk[2] = c + l_key[43];
  out_blk[3] = d;
}

void Rc6_decrypt(const uint32_t *l_key, const uint32_t *in_blk,
                 uint32_t *out_blk) {
  uint32_t d = in_blk[3];
  uint32_t c = in_blk[2] - l_key[43];
  uint32_t b = in_blk[1];
  uint32_t a = in_blk[0] - l_key[42];

  i_rnd(40, d, a, b, c);
  i_rnd(38, c, d, a, b);
  i_rnd(36, b, c, d, a);
  i_rnd(34, a, b, c, d);
  i_rnd(32, d, a, b, c);
  i_rnd(30, c, d, a, b);
  i_rnd(28, b, c, d, a);
  i_rnd(26, a, b, c, d);
  i_rnd(24, d, a, b, c);
  i_rnd(22, c, d, a, b);
  i_rnd(20, b, c, d, a);
  i_rnd(18, a, b, c, d);
  i_rnd(16, d, a, b, c);
  i_rnd(14, c, d, a, b);
  i_rnd(12, b, c, d, a);
  i_rnd(10, a, b, c, d);
  i_rnd(8, d, a, b, c);
  i_rnd(6, c, d, a, b);
  i_rnd(4, b, c, d, a);
  i_rnd(2, a, b, c, d);

  out_blk[3] = d - l_key[1];
  out_blk[2] = c;
  out_blk[1] = b - l_key[0];
  out_blk[0] = a;
}
