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

#include <Cast256.h>

#include "test_utils.h"

int main(int argc, char** argv) {
    initialize(argc, argv);

    // Initializing...
    uint32_t key[256 / 4];
    randomize_data((uint8_t*)key, 256);

    uint32_t cast256[96];
    Cast256_set_key(cast256, key, 256);

    uint32_t plaintext[4];
    uint32_t ciphertext[4];

    randomize_data((uint8_t*)plaintext, sizeof(plaintext));
    print_data((uint8_t*)plaintext, sizeof(plaintext));

    // Encrypting...
    Cast256_encrypt(cast256, plaintext, ciphertext);
    print_data((uint8_t*)ciphertext, sizeof(plaintext));

    // Decrypting
    Cast256_decrypt(cast256, ciphertext, plaintext);
    print_data((uint8_t*)plaintext, sizeof(plaintext));
}
