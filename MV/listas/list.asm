\\include  "heap.asm"
; Singly linked list

; Estructura del nodo:
;    +------------+
;  0 |   entero   |  (4 bytes)
;    +------------+
; +4 | *siguiente |  (4 bytes)
;    +------------+
nodo_size   equ     8
val         equ     0
sig         equ     4

;----------------------------------------
; crea un nuevo nodo de la lista 
; parámetros: +8 valor entero
;----------------------------------------
; invocación:
; push  <valor entero>
; call  nodo_nuevo
; add   sp, 4
; << eax puntero al nuevo nodo
;----------------------------------------
nodo_nuevo:     push    bp
                mov     bp, sp

                push    nodo_size
                call    alloc
                add     sp, 4      

                cmp     eax, null
                jz      nodo_nuevo_fin

                mov     [eax+val], [bp+8]
                mov     [eax+sig], null

nodo_nuevo_fin: mov     sp, bp
                pop     bp
                ret 


;----------------------------------------
; imprime una lista de valores enteros
; parámentros: +8 puntero a primer nodo
;----------------------------------------
; invocación:
; push  <nodo*>
; call  list_print
; add   sp, 4
;----------------------------------------
list_print:     push    bp
                mov     bp, sp
                push    eax
                push    ecx
                push    edx

                ldh     ecx, 4
                ldl     ecx, 1
                mov     eax, 1
                mov     edx, [bp+8]
list_print_imp: cmp     edx, null
                jz      list_print_fin
                sys     0x2
                mov     edx, [edx+sig]
                jmp     list_print_imp

list_print_fin: pop     edx
                pop     ecx
                pop     eax
                mov     sp, bp
                pop     bp
                ret


;----------------------------------------
; insertar un nodo en una lista ordenada
; parametro +8: doble puntero a la lista
; parametro +12: puntero al nodo a insertar
;----------------------------------------
; invocación:
; push <nodo* nuevo_nodo>
; push <nodo** head>
; call insert_nodo
; add  sp, 8
;----------------------------------------
insert_nodo:    push    bp
                mov     bp,sp
                push    eax
                push    ebx
                push    edx
                
                mov     edx, [bp+8]     ; doble puntero a la lista
                mov     ebx, [edx]      ; puntero a la lista
                mov     eax, [bp+12]    ; puntero al nodo a insertar

                cmp     ebx, null
                jz      insert_actual

                cmp     [eax+val], [ebx+val]    ; si el nodo a insertar es menor que el primero
                jnp     insert_actual   ; insertar al principio

                add     ebx, sig
                push    eax
                push    ebx
                call    insert_nodo
                add     sp, 8
                jmp     insert_fin

insert_actual:  mov     [eax+sig], [edx]   ; insertar al principio
                mov     [edx], eax
                jmp     insert_fin
insert_fin:     pop     edx
                pop     ebx
                pop     eax
                mov     sp,bp
                pop     bp
                ret




;----------------------------------------
; busca un nodo en una lista
; parametro +8: doble puntero a la lista
; parametro +12: valor de n
;----------------------------------------
; invocación:
; push <valor>
; push <nodo** head>
; call buscar_nodo
; add  sp, 8
;----------------------------------------
;probar
;buscar_nodo:    push bp
;                mov bp,sp
;                push edx
;                push ebx
;                mov edx,[bp+8]
;                mov ebx,[edx]
;                cmp ebx,null
;                jz buscar_nodo_fin
;                cmp [ebx+val],[bp+12]
;                jz buscar_nodo_fin
;                add ebx,sig
;                push [bp+12]
;                push ebx
;                call buscar_nodo
;                add sp,8
;buscar_nodo_fin: mov eax,[edx];paso a eax el puntero simple,podria ser ebx
;                pop ebx
;                pop edx
;                mov sp,bp
;                pop bp
;                ret


;----------------------------------------
; Remover nodo con valor pasado como parámetro
; parametro +8: doble puntero a la lista
; parametro +12: valor de n
; El nodo queda flotando por alli
;----------------------------------------
; invocación:
; push <valor>
; push <nodo** head>
; call buscar_nodo
; add  sp, 8
;----------------------------------------

remover_de_lista:   push bp
                    mov bp,sp
                    push edx
                    push ebx
                    mov edx,[bp+8];edx tiene doble puntero
                    mov ebx,[edx];ebx puntero simple
                    cmp ebx,null
                    jz remover_de_lista_fin
                     add ebx,sig
                     cmp ebx,null
                     jz remover_de_lista_fin
                     mov ebx,[ebx]
                     cmp [ebx+val],[bp+12]
                     jz remover_nodo
                     mov ebx,[edx]
                     add ebx,sig
                     push [bp+12]
                     push ebx
                     call remover_de_lista
                     add sp,8
                     jmp remover_de_lista_fin
remover_nodo:       mov edx,[edx]
                     mov [edx+sig],[ebx+sig]
remover_de_lista_fin: pop ebx
                    pop edx
                    mov sp,bp
                    pop bp
                    ret

;----------------------------------------
; Intercala dos listas generando una nueva
; parametro +8: doble puntero a la lista nueva
; parametro +12: puntero simple a lista 1
; parametro +16: puntero simple a lista 2
;----------------------------------------
; invocación:
; push <nodo* head1>
; push <nodo* head2>
; push <nodo** head_nuevo>
; call intercalar_listas_con_lista_nueva
; add  sp, 12
;----------------------------------------
intercalar_listas_con_lista_nueva: push bp
mov bp,sp
push edx
push ecx
push eex
mov edx,[bp+8]
mov ecx,[bp+12];ecx puntero a lista1
mov eex,[bp+16];eex puntero a lista2
cmp eex,ecx; ambos null?
jz fin_intercalar_con_lista_nueva
cmp eex,null
jz insertar_primer_valor
cmp ecx,null
jz insertar_segundo_valor
cmp [ecx+val],[eex+val];si ningun valor es nulo
jp insertar_segundo_valor;eex es menor o da 0
insertar_primer_valor: push [ecx+val]
call nodo_nuevo
add sp,4
mov ecx,[ecx+sig]
jmp insertar_intercalado
insertar_segundo_valor: push [eex+val]
call nodo_nuevo
add sp,4 
mov eex,[eex+sig]
insertar_intercalado: mov [edx],eax
add eax,sig
push ecx
push eex
push eax
call intercalar_listas_con_lista_nueva
add  sp, 12
fin_intercalar_con_lista_nueva: pop eex
pop ecx
pop edx
mov sp,bp
pop bp 
ret



;----------------------------------------
; Intercala dos listas reacomodando los nodos
; parametro +8: doble puntero a la lista 1 que sera la modificada
; parametro +12: doble puntero  a lista 2
;----------------------------------------
; invocación:
; push <nodo** head2>
; push <nodo** head1>
; call intercalar_modificando
; add  sp, 8
;----------------------------------------
intercalar_modificando: push bp
mov bp,sp
push eax
push edx
push ebx
push ecx
push eex
push efx
mov edx,[bp+8];edx = doble puntero primer lista
mov ebx,[bp+12];ebx = doble puntero segunda lista
mov ecx,[edx];puntero simple a lista 1
mov eex,[ebx];puntero simple a lista 2

cmp ecx,eex; -1 - -1 = 0 ambos null, me voy
jz fin_intercalar_modificando

cmp eex,null;segunda lista null?
jz fin_intercalar_modificando

cmp ecx,null;primera lista null?
jz insertar_intercalado_restante

;ninguna es null, comparo
cmp [ecx+val],[eex+val]
jp acomodar_puntero_segunda_lista

acomodar_puntero_primer_lista: mov efx,eex;
mov [ebx],[eex+sig]
mov [efx+sig],[ecx+sig];
mov [ecx+sig],efx;
mov ecx,[ecx+sig]
jmp siguiente_llamada_acomodar

acomodar_puntero_segunda_lista: mov efx,ecx;
mov [edx],eex
mov [ebx],[eex+sig];
mov [eex+sig],efx
jmp siguiente_llamada_acomodar


insertar_intercalado_restante: mov [edx],[ebx]

siguiente_llamada_acomodar: push ebx
mov eax, ecx        
add eax, sig        
push eax
call intercalar_modificando
add sp,8

fin_intercalar_modificando: pop efx
pop eex
pop ecx
pop ebx
pop edx
pop eax
mov sp,bp
pop bp
ret


;----------------------------------------
; Invierte la lista
; parametro +8: doble puntero a la lista que sera invertida
;----------------------------------------
; invocación:
; push <nodo** head>
; call invertir_lista
; add  sp, 4
;----------------------------------------

invertir_lista: push bp
mov bp,sp
push edx
push ebx
push ecx
push eex
mov edx,[bp+8]
mov ebx,[edx]
cmp ebx,null;esta vacia la lista?
jz fin_invertir
cmp [ebx+sig],null;tiene siguiente el nodo actual?
jz fin_invertir
mov ecx,[ebx+sig];tomo el siguiente
mov [ebx+sig],null;el primer nodo ahora es el ultimo
bucle_invertir: cmp ecx,null;mientras no me caiga
jz acomodar_cabecera
mov eex,[ecx+sig];guardo la direccion al siguiente
mov [ecx+sig],ebx
mov ebx,ecx
mov ecx,eex
jmp bucle_invertir
acomodar_cabecera: mov [edx],ebx
fin_invertir: pop eex
pop ecx
pop ebx
pop edx
mov sp,bp
pop bp
ret
