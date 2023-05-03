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

#include "Serpent.h"

#define rotl(x, y) ((unsigned)(x) << (y) | (unsigned)(x) >> (32 - (y)))
#define rotr(x, y) ((unsigned)(x) >> (y) | (unsigned)(x) << (32 - (y)))

#define bswap(x) ((rotl(x, 8) & 0x00ff00ff) | (rotr(x, 8) & 0xff00ff00))

#define sb0(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = a ^ d;                                                       \
    uint32_t t2 = a & d;                                                       \
    uint32_t t3 = c ^ t1;                                                      \
    uint32_t t6 = b & t1;                                                      \
    uint32_t t4 = b ^ t3;                                                      \
    uint32_t t10 = ~t3;                                                        \
    h = t2 ^ t4;                                                               \
    uint32_t t7 = a ^ t6;                                                      \
    uint32_t t14 = ~t7;                                                        \
    uint32_t t8 = c | t7;                                                      \
    uint32_t t11 = t3 ^ t7;                                                    \
    g = t4 ^ t8;                                                               \
    uint32_t t12 = h & t11;                                                    \
    f = t10 ^ t12;                                                             \
    e = t12 ^ t14;                                                             \
  }

/* 15 terms */

#define ib0(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = ~a;                                                          \
    uint32_t t2 = a ^ b;                                                       \
    uint32_t t3 = t1 | t2;                                                     \
    uint32_t t4 = d ^ t3;                                                      \
    uint32_t t7 = d & t2;                                                      \
    uint32_t t5 = c ^ t4;                                                      \
    uint32_t t8 = t1 ^ t7;                                                     \
    g = t2 ^ t5;                                                               \
    uint32_t t11 = a & t4;                                                     \
    uint32_t t9 = g & t8;                                                      \
    uint32_t t14 = t5 ^ t8;                                                    \
    f = t4 ^ t9;                                                               \
    uint32_t t12 = t5 | f;                                                     \
    h = t11 ^ t12;                                                             \
    e = h ^ t14;                                                               \
  }

/* 14 terms!  */

#define sb1(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = ~a;                                                          \
    uint32_t t2 = b ^ t1;                                                      \
    uint32_t t3 = a | t2;                                                      \
    uint32_t t4 = d | t2;                                                      \
    uint32_t t5 = c ^ t3;                                                      \
    g = d ^ t5;                                                                \
    uint32_t t7 = b ^ t4;                                                      \
    uint32_t t8 = t2 ^ g;                                                      \
    uint32_t t9 = t5 & t7;                                                     \
    h = t8 ^ t9;                                                               \
    uint32_t t11 = t5 ^ t7;                                                    \
    f = h ^ t11;                                                               \
    uint32_t t13 = t8 & t11;                                                   \
    e = t5 ^ t13;                                                              \
  }

/* 17 terms */

#define ib1(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = a ^ d;                                                       \
    uint32_t t2 = a & b;                                                       \
    uint32_t t3 = b ^ c;                                                       \
    uint32_t t4 = a ^ t3;                                                      \
    uint32_t t5 = b | d;                                                       \
    uint32_t t7 = c | t1;                                                      \
    h = t4 ^ t5;                                                               \
    uint32_t t8 = b ^ t7;                                                      \
    uint32_t t11 = ~t2;                                                        \
    uint32_t t9 = t4 & t8;                                                     \
    f = t1 ^ t9;                                                               \
    uint32_t t13 = t9 ^ t11;                                                   \
    uint32_t t12 = h & f;                                                      \
    g = t12 ^ t13;                                                             \
    uint32_t t15 = a & d;                                                      \
    uint32_t t16 = c ^ t13;                                                    \
    e = t15 ^ t16;                                                             \
  }

/* 16 terms */

#define sb2(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = ~a;                                                          \
    uint32_t t2 = b ^ d;                                                       \
    uint32_t t3 = c & t1;                                                      \
    uint32_t t13 = d | t1;                                                     \
    e = t2 ^ t3;                                                               \
    uint32_t t5 = c ^ t1;                                                      \
    uint32_t t6 = c ^ e;                                                       \
    uint32_t t7 = b & t6;                                                      \
    uint32_t t10 = e | t5;                                                     \
    h = t5 ^ t7;                                                               \
    uint32_t t9 = d | t7;                                                      \
    uint32_t t11 = t9 & t10;                                                   \
    uint32_t t14 = t2 ^ h;                                                     \
    g = a ^ t11;                                                               \
    uint32_t t15 = g ^ t13;                                                    \
    f = t14 ^ t15;                                                             \
  }

/* 16 terms */

#define ib2(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = b ^ d;                                                       \
    uint32_t t2 = ~t1;                                                         \
    uint32_t t3 = a ^ c;                                                       \
    uint32_t t4 = c ^ t1;                                                      \
    uint32_t t7 = a | t2;                                                      \
    uint32_t t5 = b & t4;                                                      \
    uint32_t t8 = d ^ t7;                                                      \
    uint32_t t11 = ~t4;                                                        \
    e = t3 ^ t5;                                                               \
    uint32_t t9 = t3 | t8;                                                     \
    uint32_t t14 = d & t11;                                                    \
    h = t1 ^ t9;                                                               \
    uint32_t t12 = e | h;                                                      \
    f = t11 ^ t12;                                                             \
    uint32_t t15 = t3 ^ t12;                                                   \
    g = t14 ^ t15;                                                             \
  }

/* 17 terms */

#define sb3(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = a ^ c;                                                       \
    uint32_t t2 = d ^ t1;                                                      \
    uint32_t t3 = a & t2;                                                      \
    uint32_t t4 = d ^ t3;                                                      \
    uint32_t t5 = b & t4;                                                      \
    g = t2 ^ t5;                                                               \
    uint32_t t7 = a | g;                                                       \
    uint32_t t8 = b | d;                                                       \
    uint32_t t11 = a | d;                                                      \
    uint32_t t9 = t4 & t7;                                                     \
    f = t8 ^ t9;                                                               \
    uint32_t t12 = b ^ t11;                                                    \
    uint32_t t13 = g ^ t9;                                                     \
    uint32_t t15 = t3 ^ t8;                                                    \
    h = t12 ^ t13;                                                             \
    uint32_t t16 = c & t15;                                                    \
    e = t12 ^ t16;                                                             \
  }

/* 16 term solution that performs less well than 17 term one
   in my environment (PPro/PII)

#define sb3(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    t1 = a ^ b;                                                                \
    t2 = a & c;                                                                \
    t3 = a | d;                                                                \
    t4 = c ^ d;                                                                \
    t5 = t1 & t3;                                                              \
    t6 = t2 | t5;                                                              \
    g = t4 ^ t6;                                                               \
    t8 = b ^ t3;                                                               \
    t9 = t6 ^ t8;                                                              \
    t10 = t4 & t9;                                                             \
    e = t1 ^ t10;                                                              \
    t12 = g & e;                                                               \
    f = t9 ^ t12;                                                              \
    t14 = b | d;                                                               \
    t15 = t4 ^ t12;                                                            \
    h = t14 ^ t15;                                                             \
  }
*/

/* 17 terms */

#define ib3(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = b ^ c;                                                       \
    uint32_t t2 = b | c;                                                       \
    uint32_t t3 = a ^ c;                                                       \
    uint32_t t7 = a ^ d;                                                       \
    uint32_t t4 = t2 ^ t3;                                                     \
    uint32_t t5 = d | t4;                                                      \
    uint32_t t9 = t2 ^ t7;                                                     \
    e = t1 ^ t5;                                                               \
    uint32_t t8 = t1 | t5;                                                     \
    uint32_t t11 = a & t4;                                                     \
    g = t8 ^ t9;                                                               \
    uint32_t t12 = e | t9;                                                     \
    f = t11 ^ t12;                                                             \
    uint32_t t14 = a & g;                                                      \
    uint32_t t15 = t2 ^ t14;                                                   \
    uint32_t t16 = e & t15;                                                    \
    h = t4 ^ t16;                                                              \
  }

/* 15 terms */

#define sb4(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = a ^ d;                                                       \
    uint32_t t2 = d & t1;                                                      \
    uint32_t t3 = c ^ t2;                                                      \
    uint32_t t4 = b | t3;                                                      \
    h = t1 ^ t4;                                                               \
    uint32_t t6 = ~b;                                                          \
    uint32_t t7 = t1 | t6;                                                     \
    e = t3 ^ t7;                                                               \
    uint32_t t9 = a & e;                                                       \
    uint32_t t10 = t1 ^ t6;                                                    \
    uint32_t t11 = t4 & t10;                                                   \
    g = t9 ^ t11;                                                              \
    uint32_t t13 = a ^ t3;                                                     \
    uint32_t t14 = t10 & g;                                                    \
    f = t13 ^ t14;                                                             \
  }

/* 17 terms */

#define ib4(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = c ^ d;                                                       \
    uint32_t t2 = c | d;                                                       \
    uint32_t t3 = b ^ t2;                                                      \
    uint32_t t4 = a & t3;                                                      \
    f = t1 ^ t4;                                                               \
    uint32_t t6 = a ^ d;                                                       \
    uint32_t t7 = b | d;                                                       \
    uint32_t t8 = t6 & t7;                                                     \
    h = t3 ^ t8;                                                               \
    uint32_t t10 = ~a;                                                         \
    uint32_t t11 = c ^ h;                                                      \
    uint32_t t12 = t10 | t11;                                                  \
    e = t3 ^ t12;                                                              \
    uint32_t t14 = c | t4;                                                     \
    uint32_t t15 = t7 ^ t14;                                                   \
    uint32_t t16 = h | t10;                                                    \
    g = t15 ^ t16;                                                             \
  }

/* 16 terms */

#define sb5(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = ~a;                                                          \
    uint32_t t2 = a ^ b;                                                       \
    uint32_t t3 = a ^ d;                                                       \
    uint32_t t4 = c ^ t1;                                                      \
    uint32_t t5 = t2 | t3;                                                     \
    e = t4 ^ t5;                                                               \
    uint32_t t7 = d & e;                                                       \
    uint32_t t8 = t2 ^ e;                                                      \
    uint32_t t10 = t1 | e;                                                     \
    f = t7 ^ t8;                                                               \
    uint32_t t11 = t2 | t7;                                                    \
    uint32_t t12 = t3 ^ t10;                                                   \
    uint32_t t14 = b ^ t7;                                                     \
    g = t11 ^ t12;                                                             \
    uint32_t t15 = f & t12;                                                    \
    h = t14 ^ t15;                                                             \
  }

/* 16 terms */

#define ib5(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = ~c;                                                          \
    uint32_t t2 = b & t1;                                                      \
    uint32_t t3 = d ^ t2;                                                      \
    uint32_t t4 = a & t3;                                                      \
    uint32_t t5 = b ^ t1;                                                      \
    h = t4 ^ t5;                                                               \
    uint32_t t7 = b | h;                                                       \
    uint32_t t8 = a & t7;                                                      \
    f = t3 ^ t8;                                                               \
    uint32_t t10 = a | d;                                                      \
    uint32_t t11 = t1 ^ t7;                                                    \
    e = t10 ^ t11;                                                             \
    uint32_t t13 = a ^ c;                                                      \
    uint32_t t14 = b & t10;                                                    \
    uint32_t t15 = t4 | t13;                                                   \
    g = t14 ^ t15;                                                             \
  }

/* 15 terms */

#define sb6(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = ~a;                                                          \
    uint32_t t2 = a ^ d;                                                       \
    uint32_t t3 = b ^ t2;                                                      \
    uint32_t t4 = t1 | t2;                                                     \
    uint32_t t5 = c ^ t4;                                                      \
    f = b ^ t5;                                                                \
    uint32_t t13 = ~t5;                                                        \
    uint32_t t7 = t2 | f;                                                      \
    uint32_t t8 = d ^ t7;                                                      \
    uint32_t t9 = t5 & t8;                                                     \
    g = t3 ^ t9;                                                               \
    uint32_t t11 = t5 ^ t8;                                                    \
    e = g ^ t11;                                                               \
    uint32_t t14 = t3 & t11;                                                   \
    h = t13 ^ t14;                                                             \
  }

/* 15 terms */

#define ib6(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = ~a;                                                          \
    uint32_t t2 = a ^ b;                                                       \
    uint32_t t3 = c ^ t2;                                                      \
    uint32_t t4 = c | t1;                                                      \
    uint32_t t5 = d ^ t4;                                                      \
    uint32_t t13 = d & t1;                                                     \
    f = t3 ^ t5;                                                               \
    uint32_t t7 = t3 & t5;                                                     \
    uint32_t t8 = t2 ^ t7;                                                     \
    uint32_t t9 = b | t8;                                                      \
    h = t5 ^ t9;                                                               \
    uint32_t t11 = b | h;                                                      \
    e = t8 ^ t11;                                                              \
    uint32_t t14 = t3 ^ t11;                                                   \
    g = t13 ^ t14;                                                             \
  }

/* 17 terms */

#define sb7(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = ~c;                                                          \
    uint32_t t2 = b ^ c;                                                       \
    uint32_t t3 = b | t1;                                                      \
    uint32_t t4 = d ^ t3;                                                      \
    uint32_t t5 = a & t4;                                                      \
    uint32_t t7 = a ^ d;                                                       \
    h = t2 ^ t5;                                                               \
    uint32_t t8 = b ^ t5;                                                      \
    uint32_t t9 = t2 | t8;                                                     \
    uint32_t t11 = d & t3;                                                     \
    f = t7 ^ t9;                                                               \
    uint32_t t12 = t5 ^ f;                                                     \
    uint32_t t15 = t1 | t4;                                                    \
    uint32_t t13 = h & t12;                                                    \
    g = t11 ^ t13;                                                             \
    uint32_t t16 = t12 ^ g;                                                    \
    e = t15 ^ t16;                                                             \
  }

/* 17 terms */

#define ib7(a, b, c, d, e, f, g, h)                                            \
  {                                                                            \
    uint32_t t1 = a & b;                                                       \
    uint32_t t2 = a | b;                                                       \
    uint32_t t3 = c | t1;                                                      \
    uint32_t t4 = d & t2;                                                      \
    h = t3 ^ t4;                                                               \
    uint32_t t6 = ~d;                                                          \
    uint32_t t7 = b ^ t4;                                                      \
    uint32_t t8 = h ^ t6;                                                      \
    uint32_t t11 = c ^ t7;                                                     \
    uint32_t t9 = t7 | t8;                                                     \
    f = a ^ t9;                                                                \
    uint32_t t12 = d | f;                                                      \
    e = t11 ^ t12;                                                             \
    uint32_t t14 = a & h;                                                      \
    uint32_t t15 = t3 ^ f;                                                     \
    uint32_t t16 = e ^ t14;                                                    \
    g = t15 ^ t16;                                                             \
  }

#define k_xor(r, a, b, c, d)                                                   \
  a ^= l_key[4 * r +  8];                                                      \
  b ^= l_key[4 * r +  9];                                                      \
  c ^= l_key[4 * r + 10];                                                      \
  d ^= l_key[4 * r + 11]

#define k_set(r, a, b, c, d)                                                   \
  a = l_key[4 * r +  8];                                                       \
  b = l_key[4 * r +  9];                                                       \
  c = l_key[4 * r + 10];                                                       \
  d = l_key[4 * r + 11]

#define k_get(r, a, b, c, d)                                                   \
  l_key[4 * r +  8] = a;                                                       \
  l_key[4 * r +  9] = b;                                                       \
  l_key[4 * r + 10] = c;                                                       \
  l_key[4 * r + 11] = d

/* the linear transformation and its inverse    */

#define rot(a, b, c, d)                                                        \
  a = rotl(a, 13);                                                             \
  c = rotl(c, 3);                                                              \
  d ^= c ^ (a << 3);                                                           \
  b ^= a ^ c;                                                                  \
  d = rotl(d, 7);                                                              \
  b = rotl(b, 1);                                                              \
  a ^= b ^ d;                                                                  \
  c ^= d ^ (b << 7);                                                           \
  a = rotl(a, 5);                                                              \
  c = rotl(c, 22)

#define irot(a, b, c, d)                                                       \
  c = rotr(c, 22);                                                             \
  a = rotr(a, 5);                                                              \
  c ^= d ^ (b << 7);                                                           \
  a ^= b ^ d;                                                                  \
  d = rotr(d, 7);                                                              \
  b = rotr(b, 1);                                                              \
  d ^= c ^ (a << 3);                                                           \
  b ^= a ^ c;                                                                  \
  c = rotr(c, 3);                                                              \
  a = rotr(a, 13)

void Serpent_set_key(uint32_t *l_key, const uint32_t *in_key,
                     const uint32_t key_len) {
  if (key_len < 0 || key_len > 256)
    return;

  uint32_t i = 0;
  uint32_t lk = (key_len + 31) / 32;

  while (i < lk) {
    l_key[i] = bswap(in_key[lk - i - 1]);
    i++;
  }

  if (key_len < 256) {
    while (i < 8)
      l_key[i++] = 0;

    i = key_len / 32;
    lk = 1 << key_len % 32;

    l_key[i] = (l_key[i] & (lk - 1)) | lk;
  }

  for (i = 0; i < 132; ++i) {
    lk = l_key[i] ^ l_key[i + 3] ^ l_key[i + 5] ^ l_key[i + 7] ^ 0x9e3779b9 ^ i;

    l_key[i + 8] = (lk << 11) | (lk >> 21);
  }

  uint32_t a, b, c, d, e, f, g, h;

  k_set(0, a, b, c, d);
  sb3(a, b, c, d, e, f, g, h);
  k_get(0, e, f, g, h);
  k_set(1, a, b, c, d);
  sb2(a, b, c, d, e, f, g, h);
  k_get(1, e, f, g, h);
  k_set(2, a, b, c, d);
  sb1(a, b, c, d, e, f, g, h);
  k_get(2, e, f, g, h);
  k_set(3, a, b, c, d);
  sb0(a, b, c, d, e, f, g, h);
  k_get(3, e, f, g, h);
  k_set(4, a, b, c, d);
  sb7(a, b, c, d, e, f, g, h);
  k_get(4, e, f, g, h);
  k_set(5, a, b, c, d);
  sb6(a, b, c, d, e, f, g, h);
  k_get(5, e, f, g, h);
  k_set(6, a, b, c, d);
  sb5(a, b, c, d, e, f, g, h);
  k_get(6, e, f, g, h);
  k_set(7, a, b, c, d);
  sb4(a, b, c, d, e, f, g, h);
  k_get(7, e, f, g, h);
  k_set(8, a, b, c, d);
  sb3(a, b, c, d, e, f, g, h);
  k_get(8, e, f, g, h);
  k_set(9, a, b, c, d);
  sb2(a, b, c, d, e, f, g, h);
  k_get(9, e, f, g, h);
  k_set(10, a, b, c, d);
  sb1(a, b, c, d, e, f, g, h);
  k_get(10, e, f, g, h);
  k_set(11, a, b, c, d);
  sb0(a, b, c, d, e, f, g, h);
  k_get(11, e, f, g, h);
  k_set(12, a, b, c, d);
  sb7(a, b, c, d, e, f, g, h);
  k_get(12, e, f, g, h);
  k_set(13, a, b, c, d);
  sb6(a, b, c, d, e, f, g, h);
  k_get(13, e, f, g, h);
  k_set(14, a, b, c, d);
  sb5(a, b, c, d, e, f, g, h);
  k_get(14, e, f, g, h);
  k_set(15, a, b, c, d);
  sb4(a, b, c, d, e, f, g, h);
  k_get(15, e, f, g, h);
  k_set(16, a, b, c, d);
  sb3(a, b, c, d, e, f, g, h);
  k_get(16, e, f, g, h);
  k_set(17, a, b, c, d);
  sb2(a, b, c, d, e, f, g, h);
  k_get(17, e, f, g, h);
  k_set(18, a, b, c, d);
  sb1(a, b, c, d, e, f, g, h);
  k_get(18, e, f, g, h);
  k_set(19, a, b, c, d);
  sb0(a, b, c, d, e, f, g, h);
  k_get(19, e, f, g, h);
  k_set(20, a, b, c, d);
  sb7(a, b, c, d, e, f, g, h);
  k_get(20, e, f, g, h);
  k_set(21, a, b, c, d);
  sb6(a, b, c, d, e, f, g, h);
  k_get(21, e, f, g, h);
  k_set(22, a, b, c, d);
  sb5(a, b, c, d, e, f, g, h);
  k_get(22, e, f, g, h);
  k_set(23, a, b, c, d);
  sb4(a, b, c, d, e, f, g, h);
  k_get(23, e, f, g, h);
  k_set(24, a, b, c, d);
  sb3(a, b, c, d, e, f, g, h);
  k_get(24, e, f, g, h);
  k_set(25, a, b, c, d);
  sb2(a, b, c, d, e, f, g, h);
  k_get(25, e, f, g, h);
  k_set(26, a, b, c, d);
  sb1(a, b, c, d, e, f, g, h);
  k_get(26, e, f, g, h);
  k_set(27, a, b, c, d);
  sb0(a, b, c, d, e, f, g, h);
  k_get(27, e, f, g, h);
  k_set(28, a, b, c, d);
  sb7(a, b, c, d, e, f, g, h);
  k_get(28, e, f, g, h);
  k_set(29, a, b, c, d);
  sb6(a, b, c, d, e, f, g, h);
  k_get(29, e, f, g, h);
  k_set(30, a, b, c, d);
  sb5(a, b, c, d, e, f, g, h);
  k_get(30, e, f, g, h);
  k_set(31, a, b, c, d);
  sb4(a, b, c, d, e, f, g, h);
  k_get(31, e, f, g, h);
  k_set(32, a, b, c, d);
  sb3(a, b, c, d, e, f, g, h);
  k_get(32, e, f, g, h);
}

void Serpent_encrypt(const uint32_t *l_key, const uint32_t *in_blk,
                     uint32_t *out_blk) {
  uint32_t a = bswap(in_blk[3]);
  uint32_t b = bswap(in_blk[2]);
  uint32_t c = bswap(in_blk[1]);
  uint32_t d = bswap(in_blk[0]);

  uint32_t e, f, g, h;

  k_xor(0, a, b, c, d);
  sb0(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(1, e, f, g, h);
  sb1(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(2, a, b, c, d);
  sb2(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(3, e, f, g, h);
  sb3(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(4, a, b, c, d);
  sb4(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(5, e, f, g, h);
  sb5(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(6, a, b, c, d);
  sb6(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(7, e, f, g, h);
  sb7(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(8, a, b, c, d);
  sb0(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(9, e, f, g, h);
  sb1(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(10, a, b, c, d);
  sb2(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(11, e, f, g, h);
  sb3(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(12, a, b, c, d);
  sb4(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(13, e, f, g, h);
  sb5(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(14, a, b, c, d);
  sb6(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(15, e, f, g, h);
  sb7(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(16, a, b, c, d);
  sb0(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(17, e, f, g, h);
  sb1(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(18, a, b, c, d);
  sb2(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(19, e, f, g, h);
  sb3(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(20, a, b, c, d);
  sb4(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(21, e, f, g, h);
  sb5(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(22, a, b, c, d);
  sb6(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(23, e, f, g, h);
  sb7(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(24, a, b, c, d);
  sb0(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(25, e, f, g, h);
  sb1(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(26, a, b, c, d);
  sb2(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(27, e, f, g, h);
  sb3(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(28, a, b, c, d);
  sb4(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(29, e, f, g, h);
  sb5(e, f, g, h, a, b, c, d);
  rot(a, b, c, d);
  k_xor(30, a, b, c, d);
  sb6(a, b, c, d, e, f, g, h);
  rot(e, f, g, h);
  k_xor(31, e, f, g, h);
  sb7(e, f, g, h, a, b, c, d);
  k_xor(32, a, b, c, d);

  out_blk[3] = bswap(a);
  out_blk[2] = bswap(b);
  out_blk[1] = bswap(c);
  out_blk[0] = bswap(d);
}

void Serpent_decrypt(const uint32_t *l_key, const uint32_t *in_blk,
                     uint32_t *out_blk) {
  uint32_t a = bswap(in_blk[3]);
  uint32_t b = bswap(in_blk[2]);
  uint32_t c = bswap(in_blk[1]);
  uint32_t d = bswap(in_blk[0]);

  uint32_t e, f, g, h;

  k_xor(32, a, b, c, d);
  ib7(a, b, c, d, e, f, g, h);
  k_xor(31, e, f, g, h);
  irot(e, f, g, h);
  ib6(e, f, g, h, a, b, c, d);
  k_xor(30, a, b, c, d);
  irot(a, b, c, d);
  ib5(a, b, c, d, e, f, g, h);
  k_xor(29, e, f, g, h);
  irot(e, f, g, h);
  ib4(e, f, g, h, a, b, c, d);
  k_xor(28, a, b, c, d);
  irot(a, b, c, d);
  ib3(a, b, c, d, e, f, g, h);
  k_xor(27, e, f, g, h);
  irot(e, f, g, h);
  ib2(e, f, g, h, a, b, c, d);
  k_xor(26, a, b, c, d);
  irot(a, b, c, d);
  ib1(a, b, c, d, e, f, g, h);
  k_xor(25, e, f, g, h);
  irot(e, f, g, h);
  ib0(e, f, g, h, a, b, c, d);
  k_xor(24, a, b, c, d);
  irot(a, b, c, d);
  ib7(a, b, c, d, e, f, g, h);
  k_xor(23, e, f, g, h);
  irot(e, f, g, h);
  ib6(e, f, g, h, a, b, c, d);
  k_xor(22, a, b, c, d);
  irot(a, b, c, d);
  ib5(a, b, c, d, e, f, g, h);
  k_xor(21, e, f, g, h);
  irot(e, f, g, h);
  ib4(e, f, g, h, a, b, c, d);
  k_xor(20, a, b, c, d);
  irot(a, b, c, d);
  ib3(a, b, c, d, e, f, g, h);
  k_xor(19, e, f, g, h);
  irot(e, f, g, h);
  ib2(e, f, g, h, a, b, c, d);
  k_xor(18, a, b, c, d);
  irot(a, b, c, d);
  ib1(a, b, c, d, e, f, g, h);
  k_xor(17, e, f, g, h);
  irot(e, f, g, h);
  ib0(e, f, g, h, a, b, c, d);
  k_xor(16, a, b, c, d);
  irot(a, b, c, d);
  ib7(a, b, c, d, e, f, g, h);
  k_xor(15, e, f, g, h);
  irot(e, f, g, h);
  ib6(e, f, g, h, a, b, c, d);
  k_xor(14, a, b, c, d);
  irot(a, b, c, d);
  ib5(a, b, c, d, e, f, g, h);
  k_xor(13, e, f, g, h);
  irot(e, f, g, h);
  ib4(e, f, g, h, a, b, c, d);
  k_xor(12, a, b, c, d);
  irot(a, b, c, d);
  ib3(a, b, c, d, e, f, g, h);
  k_xor(11, e, f, g, h);
  irot(e, f, g, h);
  ib2(e, f, g, h, a, b, c, d);
  k_xor(10, a, b, c, d);
  irot(a, b, c, d);
  ib1(a, b, c, d, e, f, g, h);
  k_xor(9, e, f, g, h);
  irot(e, f, g, h);
  ib0(e, f, g, h, a, b, c, d);
  k_xor(8, a, b, c, d);
  irot(a, b, c, d);
  ib7(a, b, c, d, e, f, g, h);
  k_xor(7, e, f, g, h);
  irot(e, f, g, h);
  ib6(e, f, g, h, a, b, c, d);
  k_xor(6, a, b, c, d);
  irot(a, b, c, d);
  ib5(a, b, c, d, e, f, g, h);
  k_xor(5, e, f, g, h);
  irot(e, f, g, h);
  ib4(e, f, g, h, a, b, c, d);
  k_xor(4, a, b, c, d);
  irot(a, b, c, d);
  ib3(a, b, c, d, e, f, g, h);
  k_xor(3, e, f, g, h);
  irot(e, f, g, h);
  ib2(e, f, g, h, a, b, c, d);
  k_xor(2, a, b, c, d);
  irot(a, b, c, d);
  ib1(a, b, c, d, e, f, g, h);
  k_xor(1, e, f, g, h);
  irot(e, f, g, h);
  ib0(e, f, g, h, a, b, c, d);
  k_xor(0, a, b, c, d);

  out_blk[3] = bswap(a);
  out_blk[2] = bswap(b);
  out_blk[1] = bswap(c);
  out_blk[0] = bswap(d);
}
