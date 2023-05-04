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

#include "Keccak512.h"

#include <string.h>

#define cKeccakR_SizeInBytes (cKeccakR / 8)

#ifndef crypto_hash_BYTES
	#ifdef cKeccakFixedOutputLengthInBytes
		#define crypto_hash_BYTES cKeccakFixedOutputLengthInBytes
	#else
		#define crypto_hash_BYTES cKeccakR_SizeInBytes
	#endif
#endif
#if crypto_hash_BYTES > cKeccakR_SizeInBytes
	#error "Full squeezing not yet implemented"
#endif

#define IS_BIG_ENDIAN 4321    /* byte 0 is most significant (mc68k) */
#define IS_LITTLE_ENDIAN 1234 /* byte 0 is least significant (i386) */
#define PLATFORM_BYTE_ORDER                                                    \
  IS_LITTLE_ENDIAN /* WARNING: This implementation works on little-endian      \
                      platforms. Support for big-endinanness is implemented,   \
                      but not tested. */

#if (cKeccakB == 1600)
	typedef uint64_t tKeccakLane;
	#define cKeccakNumberOfRounds 24
#elif (cKeccakB == 800)
	typedef uint32_t tKeccakLane;
	#define cKeccakNumberOfRounds 22
#elif (cKeccakB == 400)
	typedef uint16_t tKeccakLane;
	#define cKeccakNumberOfRounds 20
#elif (cKeccakB == 200)
	typedef uint8_t tKeccakLane;
	#define cKeccakNumberOfRounds 18
#else
	#error "Unsupported Keccak-f width"
#endif

#define cKeccakLaneSizeInBits (sizeof(tKeccakLane) * 8)

#define ROL(a, offset)                                                         \
  (tKeccakLane)((((tKeccakLane)a) << ((offset) % cKeccakLaneSizeInBits)) ^     \
                (((tKeccakLane)a) >> (cKeccakLaneSizeInBits -                  \
                                      ((offset) % cKeccakLaneSizeInBits))))

static void KeccakF(tKeccakLane *state, const tKeccakLane *in, int laneCount);

static const tKeccakLane KeccakF_RoundConstants[cKeccakNumberOfRounds] = {
    0x0000000000000001,
    0x0000000000008082,
    0x800000000000808a,
    0x8000000080008000,
    0x000000000000808b,
    0x0000000080000001,
    0x8000000080008081,
    0x8000000000008009,
    0x000000000000008a,
    0x0000000000000088,
    0x0000000080008009,
    0x000000008000000a,
    0x000000008000808b,
    0x800000000000008b,
    0x8000000000008089,
    0x8000000000008003,
    0x8000000000008002,
    0x8000000000000080
#if (cKeccakB >= 400)
    ,
    0x000000000000800a,
    0x800000008000000a
#if (cKeccakB >= 800)
    ,
    0x8000000080008081,
    0x8000000000008080
#if (cKeccakB == 1600)
    ,
    0x0000000080000001,
    0x8000000080008008
#endif
#endif
#endif
};

static const uint16_t KeccakF_RotationConstants[25] = {
    1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
    27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44};

static const uint16_t KeccakF_PiLane[25] = {10, 7,  11, 17, 18, 3,  5,  16,
                                            8,  21, 24, 4,  15, 23, 19, 13,
                                            12, 2,  20, 14, 22, 9,  6,  1};

static const uint16_t KeccakF_Mod5[10] = {0, 1, 2, 3, 4, 0, 1, 2, 3, 4};

static void KeccakF(tKeccakLane *state, const tKeccakLane *in, int laneCount) {
  tKeccakLane BC[5];

#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN) || (cKeccakB == 200)
  while (--laneCount >= 0)
    state[laneCount] ^= in[laneCount];
#else
  while (--laneCount >= 0) {
	tKeccakLane temp = 0;
    for (x = 0; x < sizeof(tKeccakLane); ++x) {
      temp <<= 8;
      temp |= ((char *)&in[laneCount])[x];
    }
    state[laneCount] = temp;
  }
#endif

#define round laneCount
  for (round = 0; round < cKeccakNumberOfRounds; ++round) {
    // Theta
    for (uint16_t x = 0; x < 5; ++x) {
      BC[x] = state[x] ^ state[5 + x] ^ state[10 + x] ^ state[15 + x] ^
              state[20 + x];
    }
    for (uint16_t x = 0; x < 5; ++x) {
      tKeccakLane temp = BC[KeccakF_Mod5[x + 4]] ^ ROL(BC[KeccakF_Mod5[x + 1]], 1);

      for (uint16_t y = 0; y < 25; y += 5)
        state[y + x] ^= temp;
    }

    // Rho Pi
    tKeccakLane temp = state[1];
    for (uint16_t x = 0; x < 24; ++x) {
      BC[0] = state[KeccakF_PiLane[x]];
      state[KeccakF_PiLane[x]] = ROL(temp, KeccakF_RotationConstants[x]);
      temp = BC[0];
    }

    //	Chi
    for (uint16_t y = 0; y < 25; y += 5) {
      BC[0] = state[y + 0];
      BC[1] = state[y + 1];
      BC[2] = state[y + 2];
      BC[3] = state[y + 3];
      BC[4] = state[y + 4];

      for (uint16_t x = 0; x < 5; ++x) {
        state[y + x] =
            BC[x] ^ (~BC[KeccakF_Mod5[x + 1]] & BC[KeccakF_Mod5[x + 2]]);
      }
    }

    //	Iota
    state[0] ^= KeccakF_RoundConstants[round];
  }
#undef round
}

void Keccak512_init(KECCAK512_DATA *keccak) {
  keccak->bitsInQueue = 0;
  memset(keccak->state, 0, sizeof(keccak->state));
}

void Keccak512_data(KECCAK512_DATA *keccak, const uint8_t *buffer, uint32_t len) {
  if ((keccak->bitsInQueue < 0) || ((keccak->bitsInQueue % 8) != 0)) {
    /* Final() already called or bits already in queue not modulo 8.	*/
    return;
  }

  /* If already data in queue, continue queuing first */
  for (/* empty */; (len >= 8) && (keccak->bitsInQueue != 0); len -= 8) {
    keccak->state[keccak->bitsInQueue / 8] ^= *(buffer++);
    if ((keccak->bitsInQueue += 8) == cKeccakR) {
      KeccakF((tKeccakLane *)keccak->state, 0, 0);
      keccak->bitsInQueue = 0;
    }
  }

  /* Absorb complete blocks */
  for (/* empty */; len >= cKeccakR;
       len -= cKeccakR, buffer += cKeccakR_SizeInBytes) {
    KeccakF((tKeccakLane *)keccak->state, (const tKeccakLane *)buffer,
            cKeccakR_SizeInBytes / sizeof(tKeccakLane));
  }

  /* Queue remaining data bytes */
  for (/* empty */; len >= 8; len -= 8, keccak->bitsInQueue += 8)
    keccak->state[keccak->bitsInQueue / 8] ^= *(buffer++);

  /* Queue eventual remaining data bits plus add first padding bit */
  if (len != 0) {
    keccak->state[keccak->bitsInQueue / 8] ^= (*buffer >> (8 - len));
    keccak->bitsInQueue += (int)len;
  }
}

void Keccak512_finalize(KECCAK512_DATA *keccak, uint8_t *hash) {
  if (keccak->bitsInQueue < 0) {
    /* Final() already called. */
    return;
  }

  // Padding
  if (keccak->bitsInQueue + 1 == cKeccakR_SizeInBytes * 8) {
    keccak->state[cKeccakR_SizeInBytes - 1] ^= 0x80;
    KeccakF((tKeccakLane *)keccak->state, 0, 0);
  } else {
    keccak->state[keccak->bitsInQueue / 8] ^= 1 << (keccak->bitsInQueue % 8);
  }
  keccak->state[cKeccakR_SizeInBytes - 1] ^= 0x80;
  KeccakF((tKeccakLane *)keccak->state, 0, 0);

  // Output
  uint16_t i;

  for (int hashbytelen = 64; hashbytelen != 0; hash += i, hashbytelen -= i) {
    i = (hashbytelen < cKeccakR_SizeInBytes) ? hashbytelen
                                             : cKeccakR_SizeInBytes;

#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN) || (cKeccakB == 200)
    memcpy(hash, keccak->state, i);
#else
    for (unsigned int offset = 0; offset < i; offset += sizeof(tKeccakLane)) {
      for (unsigned int j = 0; j < sizeof(tKeccakLane); ++j) {
        hash[offset + j] =
            keccak->state[offset + (sizeof(tKeccakLane) - 1) - j];
      }
    }
#endif

    if (i != hashbytelen)
      KeccakF((tKeccakLane *)keccak->state, 0, 0);
  }

  keccak->bitsInQueue = -1; /* flag final state */
}
