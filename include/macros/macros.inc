%define magicbreak xchg bx, bx
%macro halt 0
    cli
.halt_loop:
    hlt
    jmp .halt_loop
%endmacro

%define LOADER_VMA 0x0000000000100000
%define KERNEL_VMA 0xFFFFFFFF80000000
