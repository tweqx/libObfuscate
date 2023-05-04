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

#include <Grostl512.h>

#include "test_utils.h"

int main(int argc, char** argv) {
    initialize(argc, argv);

    GROSTL512_DATA grostl;
    Grostl512_init(&grostl);

    uint8_t data[0x100];
    randomize_data(data, 0x100);

    Grostl512_data(&grostl, data, sizeof(data));

    uint8_t hash[64];
    Grostl512_finalize(&grostl, hash);

    print_data(hash, sizeof(hash));
}
