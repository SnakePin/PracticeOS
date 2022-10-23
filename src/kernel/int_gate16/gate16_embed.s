global __int_gate16_raw:data
global __int_gate16_raw_size:data

[SECTION .rodata]
__int_gate16_raw:
    incbin '../../out/kernel/int_gate16/gate16_raw.bin'
__int_gate16_raw_size EQU ($-__int_gate16_raw)
