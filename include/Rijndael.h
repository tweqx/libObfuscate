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

#ifndef RIJNDAEL_H
#define RIJNDAEL_H

#include <stdint.h>

// ** Thread-safe implementation

// ** Aes cipher
// ** 128bit block size
// ** 256bit key

#define RKLENGTH(keybits) ((keybits) / 8 + 28)

extern int Rijndael_set_key_encrypt(uint32_t *rk, const uint8_t *key,
                                    const int keybits);
extern int Rijndael_set_key_decrypt(uint32_t *rk, const uint8_t *key,
                                    const int keybits);
extern void Rijndael_encrypt(const uint32_t *rk, const int nrounds,
                             const uint8_t *plaintext, uint8_t *ciphertext);
extern void Rijndael_decrypt(const uint32_t *rk, const int nrounds,
                             const uint8_t *ciphertext, uint8_t *plaintext);

#endif
