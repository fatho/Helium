# Virtual Memory Management {#vmm}

## Memory Layout

- `0x00000000 00000000` - `0x00000000 00200000` identity mapping
- `0xFFFFFF80 00000000` - `0xFFFFFF81 00000000` mapping of first 4 GiB of physical memory
    - page tables must be allocated in the first 4 GiB
- `0xFFFFFFFF 80000000` - `0xFFFFFFFF FFFFFFFF` kernel space (highest 2 GiB of virtual memory)
    - `0xFFFFFFFF 80000000` - `0xFFFFFFFF 80200000` mapping of first 2 MiB of physical memory (kernel code)