#include "kernel/idris_rts/idris_heap.h"
#include "kernel/idris_rts/idris_rts.h"

#include <stddef.h>
#include "kernel/klibc/kstdio.h"
#include "kernel/klibc/kstdlib.h"
#include "kernel/panic.h"

void alloc_heap(Heap * h, size_t heap_size) 
{
    char * mem = kmalloc(heap_size);
    if (mem == NULL) {
        kpanicf("RTS ERROR: Unable to allocate heap. Requested %d bytes.\n", (int)heap_size);
    }

    h->heap = mem;
    h->next = h->heap;
    h->end  = h->heap + heap_size;

    h->size   = heap_size;
    h->growth = heap_size;

    h->old = NULL;
}

void free_heap(Heap * h) {
    kfree(h->heap);

    if (h->old != NULL) { 
        kfree(h->old);
    }
}

// TODO: more testing
/******************** Heap testing ********************************************/
void heap_check_underflow(Heap * heap) {
    if (!(heap->heap <= heap->next)) {
       kpanicf("RTS ERROR: HEAP UNDERFLOW <bot %p> <cur %p>\n", heap->heap, heap->next);
    }
}

void heap_check_overflow(Heap * heap) {
    if (!(heap->next <= heap->end)) {
       kpanicf("RTS ERROR: HEAP OVERFLOW <cur %p> <end %p>\n", heap->next, heap->end);
    }
}

int is_valid_ref(VAL v) {
    return (v != NULL) && !(ISINT(v));
}

int ref_in_heap(Heap * heap, VAL v) { 
    return ((VAL)heap->heap <= v) && (v < (VAL)heap->next);
}

// Checks three important properties:
// 1. Closure.
//      Check if all pointers in the _heap_ points only to heap. 
// 2. Unidirectionality. (if compact gc)
//      All references in the heap should be are unidirectional. In other words,
//      more recently allocated closure can point only to earlier allocated one.
// 3. After gc there should be no forward references.
//
void heap_check_pointers(Heap * heap) {
    char* scan = NULL;
  
    size_t item_size = 0;
    for(scan = heap->heap; scan < heap->next; scan += item_size) {
       item_size = *((size_t*)scan);
       VAL heap_item = (VAL)(scan + sizeof(size_t));

       switch(GETTY(heap_item)) {
       case CON:
           {
             int ar = ARITY(heap_item);
             int i = 0;
             for(i = 0; i < ar; ++i) {
                 VAL ptr = heap_item->info.c.args[i];

                 if (is_valid_ref(ptr)) {
                     // Check for closure.
                     if (!ref_in_heap(heap, ptr)) {
                         kpanicf("RTS ERROR: heap closure broken. "\
                                 "<HEAP %p %p %p> <REF %p>\n", heap->heap, heap->next, heap->end, ptr);
                     }
#if 0 // TODO macro
                     // Check for unidirectionality.
                     if (!(ptr < heap_item)) {
                         kpanicf("RTS ERROR: heap unidirectionality broken:" \
                                 "<CON %p> <FIELD %p>\n", heap_item, ptr);
                     }
#endif
                 }
             }
             break;
           }
       case FWD:
           // Check for artifacts after cheney gc.
           kpanic("RTS ERROR: FWD in working heap.\n");
           break;
       default:
           break;
       }
    }
}

void heap_check_all(Heap * heap)
{
    heap_check_underflow(heap);
    heap_check_overflow(heap);
    heap_check_pointers(heap);
}
