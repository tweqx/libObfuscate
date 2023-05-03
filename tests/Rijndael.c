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

#include <Rijndael.h>

#include "test_utils.h"

int main(int argc, char** argv) {
    initialize(argc, argv);

    // Initializing...
    uint8_t enc_key[256 / 8];
    uint8_t dec_key[256 / 8];
    randomize_data(enc_key, 256 / 8);
    randomize_data(dec_key, 256 / 8);

    uint32_t enc_rijndael[RKLENGTH(256)];
    uint32_t dec_rijndael[RKLENGTH(256)];
    int enc_rounds = Rijndael_set_key_encrypt(enc_rijndael, enc_key, 256);
    int dec_rounds = Rijndael_set_key_decrypt(dec_rijndael, dec_key, 256);

    uint8_t plaintext[16];
    uint8_t ciphertext[16];

    randomize_data(plaintext, sizeof(plaintext));
    print_data(plaintext, sizeof(plaintext));

    // Encrypting...
    Rijndael_encrypt(enc_rijndael, enc_rounds, plaintext, ciphertext);
    print_data(ciphertext, sizeof(plaintext));

    // Decrypting
    Rijndael_decrypt(dec_rijndael, dec_rounds, ciphertext, plaintext);
    print_data(plaintext, sizeof(plaintext));
}
