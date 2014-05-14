.PHONY: install all build link clean

# makefile for kernel64

# output

TARGET     := helium64
TARGET_BIN := $(TARGET).sys

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
LD         := x86_64-elf-ld

# Toolchain flags

LDFILE     := linker.ld

INC        := -I$(INC_DIR) -I../include
CCFLAGS    := -std=gnu99 -mcmodel=kernel -ffreestanding -O2 -Wall -Wextra $(INC) -m64
ASFLAGS    := -f elf64
LDFLAGS    := -T $(LDFILE) -z max-page-size=0x1000 -Map $(BUILD_DIR)/$(TARGET).map

all: build link

install: all
	@ echo "Installing kernel to $(PREFIX)"
	@ mkdir -p $(PREFIX)/boot
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
