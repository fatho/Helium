.PHONY: all loader kernel64 clean install run

CURDIR          := $(shell pwd)
PREFIX          := $(CURDIR)/dist
export PREFIX

all: loader kernel64

install: all
	@ echo "Installing to $(PREFIX)"
	@ $(MAKE) -C loader/ install
	@ $(MAKE) -C kernel64/ install
	@ mkdir -p $(PREFIX)/boot/grub
	@ cp config/grub.cfg $(PREFIX)/boot/grub/grub.cfg
	@ sync


loader:
	@ echo "Building loader"
	@ $(MAKE) -C loader/

kernel32:
	@ echo "Building kernel64"
	@ $(MAKE) -C kernel64/

clean:
	@ echo "Cleaning"
	@ rm -r -f $(PREFIX)/boot/*
	@ $(MAKE) -C loader/ clean
	@ $(MAKE) -C kernel64/ clean

run: install
	@ $(EMU) $(EMU_ARGS)
