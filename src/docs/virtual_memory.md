# Virtual Memory Management {#vmm}

## Reference Counting

The virtual memory manager uses reference counting to keep track of the number of elements set in a page table.
The number is stored in bits 54 to 62 (incl.) (9 bits) of the page table entries.

## Memory Layout

- `0x00000000 00000000` - `0x00000000 00200000` identity mapping
- `0xFFFFFF80 00000000` - `0xFFFFFF81 00000000` mapping of first 4 GiB of physical memory
    - page tables must be allocated in the first 4 GiB
- `0xFFFFFFFF 80000000` - `0xFFFFFFFF FFFFFFFF` kernel space (highest 2 GiB of virtual memory)
    - `0xFFFFFFFF 80000000` - `0xFFFFFFFF 80200000` mapping of first 2 MiB of physical memory (kernel code)