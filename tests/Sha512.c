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

#include <Sha512.h>

#include "test_utils.h"

int main(int argc, char** argv) {
    initialize(argc, argv);

    SHA512_DATA sha;
    Sha512_init(&sha);

    uint8_t data[0x100];
    randomize_data(data, 0x100);

    Sha512_data(&sha, data, sizeof(data));

    uint8_t hash[64];
    Sha512_finalize(&sha, hash);

    print_data(hash, sizeof(hash));
}
