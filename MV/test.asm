NUMERO EQU "12345"

main: PUSH BP
MOV BP,SP
MOV EDX,KS
ADD EDX,NUMERO
SYS 4

PUSH EDX
CALL CONVERTIR_A_ENTERO
ADD SP,4
MOV EDX,EAX
SYS 4
MOV SP,BP
POP BP
STOP


CONVERTIR_A_ENTERO: PUSH BP
    MOV BP,SP
    PUSH EDX
    SUB SP,4; BP-8 tendra mi n
    SUB SP,4; BP-12 tendra mi signo 
    SUB SP,4; BP-16 lo uso para el calculo del - '0'
    MOV EDX,[BP+8];EDX tiene el puntero a string
    MOV [BP-8], 2 ; n = 0
    MOV [BP-12],-1 ; signo= -1
    SYS 4
    MOV EDX,DS;
    MOV [EDX],[BP-8]
    LDL ECX,1
    LDH ECX,4
    MOV EAX,1
    SYS 2
    POP EDX
    ADD SP,12
    MOV EAX,[BP+8]
    MOV SP,BP
    POP BP
    
