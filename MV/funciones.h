#ifndef FUNCIONES_H // esto es como para evitar que se rompa al traer vm.h por varias referencias a la misma funcion
#define FUNCIONES_H

#include "vm.h"
#include <time.h>
void leerSYS(TVM *MV, uint32_t dirFisica, uint32_t formato, uint32_t bytesWR);
void escribirSYS(TVM *MV, uint32_t dirFisica, uint32_t formato, uint32_t bytesWR, uint32_t cuantasVeces);
void stringWrite(TVM *MV, uint32_t dirFisica);
void stringRead(TVM *MV, uint32_t dirFisica);
void generarVMI(TVM *MV);
void SYS_Breakpoint(TVM *MV);
void SYS(TVM *MV);
void JMP(TVM *MV);
void JZ(TVM *MV);
void JP(TVM *MV);
void JN(TVM *MV);
void JNZ(TVM *MV);
void JNP(TVM *MV);
void JNN(TVM *MV);
void NOT(TVM *MV);
void STOP(TVM *MV);
void MOV(TVM *MV);
void ADD(TVM *MV);
void SUB(TVM *MV);
void MUL(TVM *MV);
void DIV(TVM *MV);
void CMP(TVM *MV);
void SHL(TVM *MV);
void SHR(TVM *MV);
void SAR(TVM *MV);
void AND(TVM *MV);
void OR(TVM *MV);
void XOR(TVM *MV);
void SWAP(TVM *MV);
void LDL(TVM *MV);
void LDH(TVM *MV);
void RND(TVM *MV);
void RET(TVM *MV);
void CALL(TVM *MV);
void POP(TVM *MV);
void PUSH(TVM *MV);

extern void (*operaciones[32])(TVM *MV); // array de punteros a funciones

#endif 
