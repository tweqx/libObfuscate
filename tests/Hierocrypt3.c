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

#include <Hierocrypt3.h>

#include "test_utils.h"

int main(int argc, char** argv) {
    initialize(argc, argv);

    // Initializing...
    uint8_t key[32];
    randomize_data(key, sizeof(key));

    HIEROCRYPT3_DATA hierocrypt3;
    Hierocrypt3_set_key(&hierocrypt3, key);

    uint8_t plaintext[16];
    uint8_t ciphertext[16];

    randomize_data(plaintext, sizeof(plaintext));
    print_data(plaintext, sizeof(plaintext));

    // Encrypting...
    Hierocrypt3_encrypt(&hierocrypt3, plaintext, ciphertext);
    print_data(ciphertext, sizeof(ciphertext));

    // Decrypting
    Hierocrypt3_decrypt(&hierocrypt3, ciphertext, plaintext);
    print_data(plaintext, sizeof(plaintext));
}
