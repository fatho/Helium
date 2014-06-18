# libc functionality in kernel space

A small `libc` implementation which covers the basics needed for kernel development.

-   `string.h` ([implementation](@ref string.c)) contains the functions 
    known from the C standard library
-   `kstdio.h` ([implementation](@ref kstdio.c)) contains kernel level 
    (not yet input and) output functions with configurable backends.
    string formatting is implemented in @ref kstdio_fmt.c. 
    It currently has the following features:
    
    -   string formatting via @ref snprintf and @ref vsnprintf
    -   raw string output via @ref kputs and @ref kputsn
    -   raw character output via @ref kputchar
    -   formatted string output via @ref kprintf and @ref kvprintf
-   `kstdio_screen.h` ([implementation](@ref kstdio_screen.c)) implements a
    kstdio backend for direct screen output which directly accesses VGA memory.
    Features:
    
    -   colored output via @ref screen_push_color, @ref screen_pop_color, @ref screen_set_color
    -   respects ANSI escape color codes
    -   automated scrolling @ref screen_scroll
    -   clearing via @ref screen_clear
    
## TODO

-  `kstdlib.h` providing `kmalloc` and `kfree`
    -   requires working @ref pfa for heap memory
    -   requires working @ref vmm for heap mapping