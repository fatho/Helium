.PHONY: install all build link clean

################################################################################
# MAKEFILE FOR HELIUM LOADER
################################################################################

# DEFINITIONS
# ===========

# output

TARGET     := heliumldr
TARGET_BIN := $(TARGET).sys

# Directories
SOURCE_DIR := ./src
INC_DIR    := ./inc
BUILD_DIR  := ./build

# Files

OBJECTS    := $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.c.o,$(shell find $(SOURCE_DIR) -name "*.c"))
OBJECTS    += $(patsubst $(SOURCE_DIR)/%.s,$(BUILD_DIR)/%.s.o,$(shell find $(SOURCE_DIR) -name "*.s"))

# Toolchain
CC         := i686-elf-gcc
AS         := nasm
LD         := i686-elf-gcc

# Toolchain flags

LDFILE     := linker.ld

CCFLAGS    := -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INC_DIR)
ASFLAGS    := -f elf32
LDFLAGS    := -T $(LDFILE) -ffreestanding -O2 -nostdlib -lgcc -m32


# TARGETS
# =======

all: build link

install: all
	@ echo "Installing loader to $(PREFIX)"
	@ mkdir -p $(PREFIX)/boot
	@ cp $(BUILD_DIR)/$(TARGET_BIN) $(PREFIX)/boot

build: $(OBJECTS)

link: $(BUILD_DIR)/$(TARGET_BIN)

# final loader binary: build/heliumldr.sys
$(BUILD_DIR)/$(TARGET_BIN): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# build c files
$(BUILD_DIR)/%.c.o: $(SOURCE_DIR)/%.c
	@ mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

# build assembly files
$(BUILD_DIR)/%.s.o: $(SOURCE_DIR)/%.s
	@ mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# clean intermediate files
clean:
	@ echo "Cleaning $(BUILD_DIR) ..."
	@ rm -r -f $(BUILD_DIR)/*
