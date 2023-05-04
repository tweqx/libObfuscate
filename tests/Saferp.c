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

#include <Saferp.h>

#include <stdio.h>
#include "test_utils.h"

int main(int argc, char** argv) {
    initialize(argc, argv);

    // Initializing...
    uint32_t key[8];
    randomize_data((uint8_t*)key, sizeof(key));

    SAFERP_DATA saferp;
    Saferp_set_key(&saferp, key, 256);

    uint32_t plaintext[4];
    uint32_t ciphertext[4];

    randomize_data((uint8_t*)plaintext, sizeof(plaintext));
    print_data((uint8_t*)plaintext, sizeof(plaintext));

    // Encrypting...
    Saferp_encrypt(&saferp, plaintext, ciphertext);
    print_data((uint8_t*)ciphertext, sizeof(ciphertext));

    // Decrypting
    Saferp_decrypt(&saferp, ciphertext, plaintext);
    print_data((uint8_t*)plaintext, sizeof(plaintext));
}
