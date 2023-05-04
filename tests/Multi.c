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

#include <Multi.h>

#include <stdio.h>
#include "test_utils.h"

int main(int argc, char** argv) {
    initialize(argc, argv);

    // Initializing...
    uint8_t iv[256];
    uint8_t passw1[32];
    uint8_t passw2[32];
    randomize_data(iv, sizeof(iv));
    randomize_data(passw1, sizeof(passw1));
    randomize_data(passw2, sizeof(passw2));

    MULTI_DATA multi;
    Multi_setkey(&multi, iv, passw1, passw2, 84946433);

    uint8_t buffer[0x100];

    randomize_data(buffer, sizeof(buffer));
    print_data(buffer, sizeof(buffer));

    // Encrypting...
    Multi_CBC_encrypt(&multi, 0x100, buffer, NULL, NULL, NULL, NULL);
    print_data(buffer, sizeof(buffer));

    // Decrypting
    Multi_CBC_decrypt(&multi, 0x100, buffer, NULL, NULL, NULL, NULL);
    print_data(buffer, sizeof(buffer));
}
