%define magicbreak xchg bx, bx
%macro halt 0
    cli
.halt_loop:
    hlt
    jmp .halt_loop
%endmacro