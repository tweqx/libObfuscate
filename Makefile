.PHONY: build
build:
	$(MAKE) -C src/

.PHONY: tests
tests:
	$(MAKE) -C tests/

.PHONY: clean
clean:
	$(MAKE) -C src/ clean
	$(MAKE) -C tests/ clean


