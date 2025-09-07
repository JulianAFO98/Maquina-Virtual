#include "funciones.h"
#include <stdlib.h>


void (*operaciones[32])(TVM *MV) = {
    SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, NOT,
    NULL, NULL, NULL, NULL, NULL, NULL,
    STOP, MOV, ADD, SUB, MUL, DIV, CMP, SHL,
    SHR, SAR, AND, OR, XOR, SWAP, LDL, LDH, RND
};


void SYS(TVM *MV){
    //code
}
void JMP(TVM *MV){
    //code
}
void JZ(TVM *MV){
    //code
}
void JP(TVM *MV){
    //code
}
void JN(TVM *MV){
    //code
}
void JNZ(TVM *MV){
    //code
}
void JNP(TVM *MV){
    //code
}
void JNN(TVM *MV){
    //code
}
void NOT(TVM *MV){
    //code
}
void STOP(TVM *MV){
    //code
}
void MOV(TVM *MV){
   set(MV, MV->registros[OP1], get(MV,MV->registros[OP2],4));
}
void ADD(TVM *MV){
    //code
}
void SUB(TVM *MV){
    //code
}
void MUL(TVM *MV){
    //code
}
void DIV(TVM *MV){
    //code
}
void CMP(TVM *MV){
    //code
}
void SHL(TVM *MV){
    //code
}
void SHR(TVM *MV){
    //code
}
void SAR(TVM *MV){
    //code
}
void AND(TVM *MV){
    //code
}
void OR(TVM *MV){
    //code
}
void XOR(TVM *MV){
    //code
}
void SWAP(TVM *MV){
    //code
}
void LDL(TVM *MV){
    //code
}
void LDH(TVM *MV){
    //code
}
void RND(TVM *MV){
    //code
}