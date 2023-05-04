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

#include <CSPRNG.h>

#include "test_utils.h"

int main(int argc, char** argv) {
    initialize(argc, argv);

    // Initializing...
    uint8_t seed[32];
    randomize_data(seed, sizeof(seed));

    CSPRNG_DATA csprng;
    CSPRNG_set_seed(&csprng, SKEIN512_HASH, seed, 108876846);

    uint8_t buffer[0x100];

    CSPRNG_array_init(&csprng, sizeof(buffer), buffer);
    print_data(buffer, sizeof(buffer));
}
