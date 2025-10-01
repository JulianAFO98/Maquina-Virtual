MOV [3], 'd'
MOV [2], 'o'
MOV [1], 'o'
MOV [0], 'G'
MOV EDX, DS
ADD EDX, 3
LDH ECX, 1
LDL ECX, 4
MOV EAX, 0x2
SYS 0x2

