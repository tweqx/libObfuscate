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

#include "Sha512.h"

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define ROTATE(x, n) (((x) >> (n)) | ((x) << (64 - (n))))
#define SHIFT(x, n) ((x) >> (n))

/*
 * Helper macros used by the SHA-512 computation.
 */
#define CH(x, y, z) (((x) & (y)) ^ ((~(x)) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SUM0(x) (ROTATE((x), 28) ^ ROTATE((x), 34) ^ ROTATE((x), 39))
#define SUM1(x) (ROTATE((x), 14) ^ ROTATE((x), 18) ^ ROTATE((x), 41))
#define RHO0(x) (ROTATE((x),  1) ^ ROTATE((x),  8) ^ SHIFT((x), 7))
#define RHO1(x) (ROTATE((x), 19) ^ ROTATE((x), 61) ^ SHIFT((x), 6))

/*
 * Constants used in each of the SHA-512 rounds.
 */
static const uint64_t K[80] = {
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f,
    0xe9b5dba58189dbbc, 0x3956c25bf348b538, 0x59f111f1b605d019,
    0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242,
    0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
    0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
    0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 0x2de92c6f592b0275,
    0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f,
    0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
    0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc,
    0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6,
    0x92722c851482353b, 0xa2bfe8a14cf10364, 0xa81a664bbc423001,
    0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
    0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99,
    0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
    0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc,
    0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915,
    0xc67178f2e372532b, 0xca273eceea26619c, 0xd186b8c721c0c207,
    0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba,
    0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
    0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
    0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

void Sha512_init(SHA512_DATA *sha) {
    sha->inputLen = 0;

    sha->A = 0x6a09e667f3bcc908;
    sha->B = 0xbb67ae8584caa73b;
    sha->C = 0x3c6ef372fe94f82b;
    sha->D = 0xa54ff53a5f1d36f1;
    sha->E = 0x510e527fade682d1;
    sha->F = 0x9b05688c2b3e6c1f;
    sha->G = 0x1f83d9abfb41bd6b;
    sha->H = 0x5be0cd19137e2179;

    sha->totalLen = 0;
}

static void ProcessBlock(SHA512_DATA *sha, const uint8_t *block) {
    uint64_t W[80];
    uint64_t a, b, c, d, e, f, g, h;

    /* Unpack the block into 80 64-bit words */
    for (int t = 0; t < 16; ++t) {
        W[t] = (uint64_t)block[t * 8 + 0] << 56 | (uint64_t)block[t * 8 + 1] << 48 |
               (uint64_t)block[t * 8 + 2] << 40 | (uint64_t)block[t * 8 + 3] << 32 |
               (uint64_t)block[t * 8 + 4] << 24 | (uint64_t)block[t * 8 + 5] << 16 |
               (uint64_t)block[t * 8 + 6] << 8  | (uint64_t)block[t * 8 + 7] << 0;
    }

    for (int t = 16; t < 80; ++t) {
       W[t] = RHO1(W[t - 2]) + W[t - 7] + RHO0(W[t - 15]) + W[t - 16];
    }

    /* Load the SHA-512 state into local variables */
    a = sha->A;
    b = sha->B;
    c = sha->C;
    d = sha->D;
    e = sha->E;
    f = sha->F;
    g = sha->G;
    h = sha->H;

    /* Perform 80 rounds of hash computations */
    for (int t = 0; t < 80; ++t) {
        uint64_t temp1 = h + SUM1(e) + CH(e, f, g) + K[t] + W[t];
        uint64_t temp2 = SUM0(a) + MAJ(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    /* Combine the previous SHA-512 state with the new state */
    sha->A += a;
    sha->B += b;
    sha->C += c;
    sha->D += d;
    sha->E += e;
    sha->F += f;
    sha->G += g;
    sha->H += h;
}

void Sha512_data(SHA512_DATA *sha, const void *buffer, uint32_t len) {
  uint32_t templen;

  /* Add to the total length of the input stream */
  sha->totalLen += len;

  /* Copy the blocks into the input buffer and process them */
  while (len > 0) {
    if (!sha->inputLen && len >= 128) {
      /* Short cut: no point copying the data twice */
      ProcessBlock(sha, (const uint8_t *)buffer);
      buffer = (const uint8_t *)buffer + 128;
      len -= 128;
    } else {
      templen = len;
      if (templen > (128 - sha->inputLen)) {
        templen = 128 - sha->inputLen;
      }
      memcpy(sha->input + sha->inputLen, buffer, templen);
      if ((sha->inputLen += templen) >= 128) {
        ProcessBlock(sha, sha->input);
        sha->inputLen = 0;
      }
      buffer = (const uint8_t *)buffer + templen;
      len -= templen;
    }
  }
}

static void WriteLong(uint8_t* buf, uint64_t value) {
  buf[0] = value >> 56;
  buf[1] = value >> 48;
  buf[2] = value >> 40;
  buf[3] = value >> 32;
  buf[4] = value >> 24;
  buf[5] = value >> 16;
  buf[6] = value >>  8;
  buf[7] = value >>  0;
}

void Sha512_finalize(SHA512_DATA *sha, uint8_t *hash) {
  /* Compute the final hash if necessary */
  if (hash) {
    /* Pad the input data to a multiple of 1024 bits */
    if (sha->inputLen >= (128 - 16)) {
      /* Need two blocks worth of padding */
      sha->input[sha->inputLen++] = 0x80;
      while (sha->inputLen < 128) {
        sha->input[sha->inputLen++] = 0x00;
      }
      ProcessBlock(sha, sha->input);
      sha->inputLen = 0;
    } else {
      /* Need one block worth of padding */
      sha->input[sha->inputLen++] = 0x80;
    }
    while (sha->inputLen < (128 - 16)) {
      sha->input[sha->inputLen++] = 0x00;
    }

    memset(sha->input + (128 - 16), 0, 8);

    uint64_t totalBits = sha->totalLen << 3;
    WriteLong(sha->input + (128 - 8), totalBits);


    ProcessBlock(sha, sha->input);

    /* Write the final hash value to the supplied buffer */
    WriteLong(hash +  0, sha->A);
    WriteLong(hash +  8, sha->B);
    WriteLong(hash + 16, sha->C);
    WriteLong(hash + 24, sha->D);
    WriteLong(hash + 32, sha->E);
    WriteLong(hash + 40, sha->F);
    WriteLong(hash + 48, sha->G);
    WriteLong(hash + 56, sha->H);
  }
}
