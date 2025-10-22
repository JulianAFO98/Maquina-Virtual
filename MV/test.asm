\\INCLUDE "list.asm"
TEXTO EQU "hola"
lista_orig  equ     "lista original:\n"
head        equ     4
main: push bp 
      mov bp, sp
      sub sp, 4
      mov [bp-4], 6
      mov [edx], [bp-4]
      mov eax, 8
      ldh ecx, 4
      ldl ecx, 1
      sys 2 