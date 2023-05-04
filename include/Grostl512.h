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

#ifndef GROSTL512_H
#define GROSTL512_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Grostl hashing
// ** 512bit hash

#define ROWS 8
#define LENGTHFIELDLEN ROWS
#define COLS1024 16
#define SIZE1024 (ROWS * COLS1024)
#define ROUNDS1024 14

typedef struct {
  uint8_t chaining[ROWS][COLS1024]; /* the actual state */
  uint64_t block_counter;           /* block counter */
  int hashbitlen;                   /* output length */
  uint8_t buffer[SIZE1024];         /* block buffer */
  int buf_ptr;                      /* buffer pointer */
  int bits_in_last_byte;            /* number of bits in incomplete byte */
  int columns;                      /* number of columns in state */
  int rounds;                       /* number of rounds in P and Q */
  int statesize;                    /* size of state (ROWS * columns) */
} GROSTL512_DATA;

extern void Grostl512_init(GROSTL512_DATA *grostl);
extern void Grostl512_data(GROSTL512_DATA *grostl, const uint8_t *buffer,
                           uint32_t len);
extern void Grostl512_finalize(GROSTL512_DATA *grostl, uint8_t *hash);

#endif
