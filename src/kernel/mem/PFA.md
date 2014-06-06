# Page Frame Allocation

## Watermark Allocator

The watermark allocator is used in the early stages for allocation of
contiguous blocks, such as the page tables for initial mappings.
Freeing the pages is possible, after the stack based allocator has been
initialized. The stack allocator needs a working [virtual memory management](VMM.md)
and interrupts for reacting to page faults.