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

#include <Clefia.h>

#include "test_utils.h"

int main(int argc, char** argv) {
    initialize(argc, argv);

    // Initializing...
    uint8_t key[256];
    randomize_data(key, sizeof(key));

    CLEFIA_DATA clefia;
    Clefia_set_key(&clefia, key, 256);

    uint8_t plaintext[16];
    uint8_t ciphertext[16];

    randomize_data(plaintext, sizeof(plaintext));
    print_data(plaintext, sizeof(plaintext));

    // Encrypting...
    Clefia_encrypt(&clefia, plaintext, ciphertext);
    print_data(ciphertext, sizeof(ciphertext));

    // Decrypting
    Clefia_decrypt(&clefia, ciphertext, plaintext);
    print_data(plaintext, sizeof(plaintext));
}
