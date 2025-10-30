\\STACK 1024
\\include  "list.asm"

primer_lista  equ     "primer_lista:\n"
segunda_lista  equ     "segunda_lista:\n"
lista_final equ     "lista intercalada:\n"
head        equ     4
head2       equ     8
head_lista_intercalar equ 12
main:       push    bp
            mov     bp, sp
            sub     sp, 4 ; head  (variable local)
            sub     sp, 4; head2
            sub     sp, 4; head_lista_intercalar
            push    ebx

            sys     0xF

            call    heap_init ;inicializa heap

            ; declaro un lista simplemente enlazada (sll)
            ; head->10->20->30->(null)
            mov     [bp-head], null     ; nodo* head = null
            mov     ebx, bp
            sub     ebx, head           ; ebx = &head

            sys     0xF
            
            push    10
            call    nodo_nuevo
            add     sp, 4
            mov     [ebx], eax          ; head = nodo_nuevo(10);
            mov     ebx,[ebx]           ;
            add     ebx,sig             ; ebx = &nodo->sig

            sys     0xF            

            push    20
            call    nodo_nuevo
            add     sp, 4
            mov     [ebx], eax          ; nodo->sig = nodo_nuevo(11);
            mov     ebx,[ebx]           ;
            add     ebx,sig             ; &nodo11->sig   

            sys     0xF
            
            push    30
            call    nodo_nuevo
            add     sp, 4
            mov     [ebx], eax          ; nodo11->sig = nodo_nuevo(12)

            sys     0xF
            
            ; mensaje lista 1
            mov     edx, ks
            add     edx, primer_lista
            sys     0x4

            ; imprimir la lista
            push    [bp-head]
            call    list_print          ; list_print (head)
            add     sp, 4       

            sys     0xF

            ;armo segunda lista
            ; declaro un lista simplemente enlazada (sll)
            mov     [bp-head2], null     ; nodo* head = null
            mov     ebx, bp
            sub     ebx, head2           ; ebx = &head2

            sys     0xF
            
            push    2
            call    nodo_nuevo
            add     sp, 4
            mov     [ebx], eax          ; head = nodo_nuevo(10);
            mov     ebx,[ebx]           ;
            add     ebx,sig             ; ebx = &nodo->sig

            sys     0xF            

            push    16
            call    nodo_nuevo
            add     sp, 4
            mov     [ebx], eax          ; nodo->sig = nodo_nuevo(11);
            mov     ebx,[ebx]           ;
            add     ebx,sig             ; &nodo11->sig   

            sys     0xF
            
            push    78
            call    nodo_nuevo
            add     sp, 4
            mov     [ebx], eax  

            
            ; mensaje segunda_lista
            mov     edx, ks
            add     edx, segunda_lista
            sys     0x4

            ; imprimir la lista
            push    [bp-head2]
            call    list_print          ; list_print (head)
            add     sp, 4  

             ; mensaje lista intercalada
            mov     edx, ks
            add     edx, lista_final
            sys     0x4
            ;intercalamos listas
            mov ebx, bp
            sub ebx,head2
            push ebx
            mov ebx, bp
            sub ebx,head
            push ebx
            call intercalar_modificando
            add sp,8

          

            ; imprimir la lista
            push    [bp-head]
            call    list_print          ; list_print (head)
            add     sp, 4

            pop     ebx
            mov     sp, bp 
            pop     bp
            ret



            

