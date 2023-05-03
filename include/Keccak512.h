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

#ifndef KECCAK512_H
#define KECCAK512_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Keccak hashing
// ** 512bit hash

#define cKeccakB 1600
#define cKeccakR 512

typedef struct {
  uint8_t state[cKeccakB / 8];
  int bitsInQueue;
} KECCAK512_DATA;

extern void Keccak512_init(KECCAK512_DATA *keccak);
extern void Keccak512_data(KECCAK512_DATA *keccak, const uint8_t *buffer,
                           uint32_t len);
extern void Keccak512_finalize(KECCAK512_DATA *keccak, uint8_t *hash);

#endif
