inicializacion: MOV [100],3 ; n = 3
MOV [104],3 ; m = 3
MOV [108],1
MOV [112],5
MOV [116],7
MOV [120],2
MOV [124],-5
MOV [128],-3
MOV [132],2
MOV [136],-2
MOV [140],1


MOV ECX,0 ;//j = 0

forj: 	CMP ECX,[104]
	JNN fin
	MOV EDX,ECX
	MUL EDX,4
	ADD EDX,DS
	ADD EDX,10  ; en [10,14,18] voy a guardar mis maximos
	MOV [EDX],0x0000FFFF  ; vec[j] = inf (0x7FFFFFFF)
	MOV EBX,0 ; i = 0

fori: CMP EBX,[100]
	JNN sigcol
	MOV EAX,EBX
	MUL EAX,[104]
	ADD EAX,ECX
	MUL EAX,4
	ADD EAX,DS
	ADD EAX,108
	CMP [EDX],[EAX]
	JNP nocambio
	MOV [EDX],[EAX]
nocambio: ADD EBX,1
	JMP fori

sigcol: ADD ECX,1
	JMP forj

fin: MOV EDX,DS
    MOV EAX,0x01
	LDL ECX,1
	LDH ECX,4
    ADD EDX,10
	SYS 2
    ADD EDX,10
	SYS 2
    ADD EDX,10
	SYS 2
    STOP