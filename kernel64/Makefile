.PHONY: install all build link clean

# makefile for kernel64

# output

TARGET     := helium64
TARGET_BIN := $(TARGET).bin

# Directories
SOURCE_DIR := ./src
INC_DIR    := ./inc
BUILD_DIR  := ./build

# Files

OBJECTS    := $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.c.o,$(shell find $(SOURCE_DIR) -name "*.c"))
OBJECTS    += $(patsubst $(SOURCE_DIR)/%.s,$(BUILD_DIR)/%.s.o,$(shell find $(SOURCE_DIR) -name "*.s"))

# Toolchain
CC         := x86_64-elf-gcc
AS         := nasm
LD         := x86_64-elf-gcc

# Toolchain flags

LDFILE     := linker.ld

CCFLAGS    := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(INC_DIR) -m64
ASFLAGS    := -64
LDFLAGS    := -T $(LDFILE) -ffreestanding -O2 -nostdlib -lgcc -m64

all: build link

install: all
	@ mkdir -p $(PREFIX)/boot
	@ mkdir -p $(PREFIX)/include
	@ cp $(BUILD_DIR)/$(TARGET_BIN) $(PREFIX)/boot

build: $(OBJECTS)

link: $(BUILD_DIR)/$(TARGET_BIN)

$(BUILD_DIR)/$(TARGET_BIN): $(OBJECTS)
	echo $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# build c file
$(BUILD_DIR)/%.c.o: $(SOURCE_DIR)/%.c
	@ mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

# build assembly file
$(BUILD_DIR)/%.s.o: $(SOURCE_DIR)/%.s
	@ mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	@ echo "Cleaning $(BUILD_DIR) ..."
	@ rm -r -f $(BUILD_DIR)/*
