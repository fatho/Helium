#  use intel syntax
.intel_syntax noprefix
# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Multiboot Header
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Temporary stack
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 0x4000 # 16 KiB
stack_top:

# Code Section
.section .text
.global boot_bsp
.type boot_bsp, @function
# entry point for loader. BSP is Bootstrap Processor
boot_bsp:

	# Setup temporary stack (.bootstrap_stack)
	mov esp, stack_top	; multiboot

	call multiboot_init	; parse multiboot structure

	# magic breakpoint for Bochs
	xchg bx, bx
	cli
	# halt
	jmp $

# Set the size of the boot_bsp symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size boot_bsp, . - boot_bsp
