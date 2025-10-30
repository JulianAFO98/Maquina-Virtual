\\include  "binario.asm"
IN_ORDER equ "in order:"
PRE_ORDER equ "pre order:"
POST_ORDER equ "post order:"
INGRESE_NUMERO equ "Ingrese un numero:\n"
EXISTE equ "existe el numero\n"
NO_EXISTE equ "no existe el numero\n"


root  equ    4
MAIN:   push bp
        mov bp, sp
        sub sp, 4 ; root de ABB
        push eax
        push edx
        
        call    heap_init    ; inicializa memoria dinámica

        mov edx, bp
        sub edx, root        ; edx = &root
        mov [edx], null      ; inicializo root = null

        push 20
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8

        push 20
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8

        push 10
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8

        push 15
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8

        push 30
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8

        push 25
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8


        mov edx,ks
        add edx,IN_ORDER
        sys 4

        mov edx,bp
        sub edx,root
        push [edx]
        call inorder
        add sp, 4


        mov edx,ks
        add edx,PRE_ORDER
        sys 4

        mov edx,bp
        sub edx,root
        push [edx]
        call preorder
        add sp, 4

        mov edx,ks
        add edx,POST_ORDER
        sys 4

        mov edx,bp
        sub edx,root
        push [edx]
        call postorder
        add sp, 4

        mov edx,ks
        add edx,INGRESE_NUMERO
        sys 4

        mov edx,ds
        mov eax,1
        ldl ecx,1
        ldh ecx,4
        sys 1

        mov edx,bp
        sub edx,root
        push [0]
        push [edx]
        call existe_en_arbol
        add sp, 8



        mov edx,ks
        cmp eax,0
        jz no_encontrado
encontrado: add edx,EXISTE
        sys 4
        jmp continuar
no_encontrado: add edx,NO_EXISTE
        sys 4

continuar:        mov edx,bp
        sub edx,root
        push [edx]
        call niveles_arbol
        add sp, 4

        mov edx,ds
        mov [edx],eax
        mov eax,1
        ldl ecx,1
        ldh ecx,4
        sys 2


main_end:   pop edx
        pop eax
        mov sp,bp
        pop bp
        stop