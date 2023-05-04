# Copyright 2023 tweqx

# This file is part of LibrePuff.
#
# LibrePuff is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# LibrePuff is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with LibrePuff. If not, see <https://www.gnu.org/licenses/>.

SRC := Sha512.c Grostl512.c Keccak512.c Anubis.c Camellia.c Cast256.c Clefia.c
SRC += Frog.c Hierocrypt3.c Idea_nxt128.c Idea_nxt_common.c Mars.c Rc6.c
SRC += Rijndael.c Saferp.c Sc2000.c Serpent.c Skein512.c Speed.c Twofish.c
SRC += Unicorn-A.c MultiBase.c CSPRNG.c Scramble.c Multi.c

BUILD_DIR = ../build
INCLUDE_DIR = ../include
OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))

.PHONY: build
build: $(OBJ)
	gcc -shared -o $(BUILD_DIR)/libObfuscate.so $(OBJ) -fPIC

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(BUILD_DIR)
	gcc -c -Wall -fPIC $< -I$(INCLUDE_DIR) -o $@

.PHONY: clean
clean:
	-rm -r $(BUILD_DIR)
	-rm libObfuscate.so
