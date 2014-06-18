# Page Frame Allocation {#pfa}

## Requirements

- allocation contiguous pages for memory mapped IO
- allocation of page tables in the first 4 GiB of physical memory

## Memory Layout

- `0x00001000` - `0x00002000` 16 bit boot code (for application processors)
- `0x00100000` - `0x00101000` 32 and 64 bit boot code and data
- `0x00101000` - `0x00102000` GDT
- `0x00102000` - `0x00103000` IDT
- `0x00103000` - `0x0010c000` page tables
    - ...
- ...
- `0x0010f000` - `0x0011a000` 64 bit kernel code (.text)
- `0x0011a000` - `0x00136000` 64 bit kernel data (.data & .bss)

## Watermark Allocator

The watermark allocator is used in the early stages for allocation of
contiguous blocks, such as the page tables for initial mappings.
Freeing the pages is possible, after the stack based allocator has been
initialized. The stack allocator needs a working @ref vmm
and interrupts for reacting to page faults.