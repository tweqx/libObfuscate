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

TESTS := Sha512 Grostl512 Keccak512 Anubis Camellia Cast256 Clefia Frog
TESTS += Hierocrypt3 Idea_nxt128 Mars Rc6 Rijndael Saferp Sc2000 Serpent
TESTS += Skein512 Speed Twofish Unicorn-A CSPRNG Scramble Multi

SRC = $(patsubst %,%.c,$(TESTS))
INCLUDE = ../include
LIBRARY_PATH = ../build
BUILD_DIR = binaries

.PHONY: all
all: $(TESTS)

$(TESTS):
	@mkdir -p $(BUILD_DIR)

	# Compile on Linux, on Windows
	gcc -g -I$(INCLUDE) $@.c test_utils.c -L$(LIBRARY_PATH) -lObfuscate -o binaries/$@
	i686-w64-mingw32-gcc -I$(INCLUDE) $@.c test_utils.c -L. -lObfuscate -o binaries/$@.exe

	@test_seed=$$(date +%N); \
	echo "Test $@ using seed '$${test_seed}'..."; \
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(LIBRARY_PATH) ./binaries/$@ $${test_seed} > /tmp/test_$${test_seed}_linux; \
	wine ./binaries/$@.exe $$test_seed > /tmp/test_$${test_seed}_windows; \
	diff --strip-trailing-cr /tmp/test_$${test_seed}_linux /tmp/test_$${test_seed}_windows

	@echo "Test passed!"

.PHONY: clean
clean:
	-rm $(patsubst %,$(BUILD_DIR)/%,$(TESTS))
	-rm $(patsubst %,$(BUILD_DIR)/%.exe,$(TESTS))
	-rm /tmp/test_*
