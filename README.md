Helium
======

A small 64 bit OS with a micro-kernel.

## Done

1. Read Multiboot Info
    - memory map
    - module table
    
## Partially Done

2. Setup Page Frame Allocator
    - watermark
    - revisit for more sophisticated implementation

## TODO

3. Setup Virtual Memory Allocator
    - decide where to map paging structures
4. CPU features like I/O Ports, MSRs, etc. 
    - may be needed be earlier steps too
5. Parse ACPI and SMP Tables
    - needs virtual memory management (at least for ACPI)
6. Enable I/O APICs and Local APIC of BSP
7. Boot Application Processors
8. Keyboard
9. User Mode processes
10. Virtual Consoles
