#include "funciones.h"
#include <stdlib.h>
#include <stdio.h>

void (*operaciones[32])(TVM *MV) = {
    SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, NOT,
    NULL, NULL, NULL, NULL, NULL, NULL,
    STOP, MOV, ADD, SUB, MUL, DIV, CMP, SHL,
    SHR, SAR, AND, OR, XOR, SWAP, LDL, LDH, RND};

void SYS(TVM *MV)
{
    uint32_t error = 0;
    uint32_t valor = 0;
    uint32_t dirFisica = obtenerDireccionFisica(MV,MV->registros[EDX],&error);
    int32_t op1 = get(MV, MV->registros[OP1], 4); // 0x1 READ 0x2 WRITE
    uint32_t bytesWR = MV->registros[ECX] & 0xFFFF0000; // obtengo los datos del LDH
    uint32_t cuantasVeces = MV->registros[ECX] & 0x0000FFFF; // obtengo los datos del LDL
    uint32_t formato = MV->registros[EAX]; // formato decimal 0x01 formato 0x02 caracter formato 0x04 octal formato 0x08 hexa formato 0x10 binario
    for (uint32_t i = 0; i < cuantasVeces; i++)
    {
        if (op1 == 0x1)
        {
            if (formato == 0x01)
                scanf("%d", &valor);
            else if (formato == 0x02)
            {
                char c;
                scanf(" %c", &c);
                valor = (uint32_t)c;
            }
            else if (formato == 0x04)
                scanf("%o", &valor);
            else if (formato == 0x08)
                scanf("%x", &valor);
            else if (formato == 0x10)
            {
                char bin[65];
                scanf("%64s", bin);
                valor = strtol(bin, NULL, 2);
            }
            for (int i = 0; i < 4; i++)
                MV->memoria[dirFisica + i] = (valor >> (8 * (3 - i))) & 0xFF;
        }
        else if (op1 == 0x2)
        {
            for (int i = 0; i < 4; i++)
                valor = (valor << 8) | MV->memoria[dirFisica + i];
            if (formato == 0x01)
                printf("%d", valor);
            else if (formato == 0x02)
                printf("%c", (char)valor);
            else if (formato == 0x04)
                printf("%o", valor);
            else if (formato == 0x08)
                printf("%X", valor);
            else if (formato == 0x10)
                for (int j = bytesWR * 8 - 1; j >= 0; j--)
                    printf("%d", (valor >> j) & 1);
            printf("\n");
        }
    }
}
void JMP(TVM *MV)
{
    int32_t op1 = MV->registros[OP1];
    MV->registros[IP] = get(MV, op1, 4);
}
void JZ(TVM *MV)
{
    if (MV->registros[CC] & 0x40000000){
        MV->registros[IP] = get(MV,MV->registros[OP1],4);
    } else {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void JP(TVM *MV)
{
    if(MV->registros[CC] & 0x40000000){
        MV->registros[IP] = get(MV,MV->registros[OP1], 4);
    } else {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void JN(TVM *MV)
{
    if(MV->registros[CC] < 0){
        MV->registros[IP] = get(MV,MV->registros[OP1], 4);
    } else {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void JNZ(TVM *MV)
{
    printf("Ejecutando JNZ\n");
    if (!(MV->registros[CC] & 0x10000000)) {
        MV->registros[IP] = get(MV, MV->registros[OP1], 4);
    } else {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void JNP(TVM *MV)
{
    if(MV->registros[CC] <= 0){
        MV->registros[IP] = get(MV,MV->registros[OP1], 4);
    } else {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void JNN(TVM *MV)
{
    if(MV->registros[CC] >= 0){
        MV->registros[IP] = get(MV,MV->registros[OP1], 4);
    } else {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void NOT(TVM *MV)
{
}
void STOP(TVM *MV)
{
    MV->registros[IP] = -1;
}
void MOV(TVM *MV)
{
    set(MV, MV->registros[OP1], get(MV, MV->registros[OP2], 4));
}
void ADD(TVM *MV)
{
    uint32_t op1 = MV->registros[OP1]; // operando destino
    uint32_t op2 = MV->registros[OP2]; // operando fuente
    int32_t val1 = get(MV, op1, 4);
    int32_t val2 = get(MV, op2, 4);
    int32_t res = val1 + val2;
    setCC(MV,res);
    set(MV, op1, res);
}
void SUB(TVM *MV)
{
   uint32_t op1 = MV->registros[OP1]; 
    uint32_t op2 = MV->registros[OP2]; 
    int32_t val1 = get(MV, op1, 4);
    int32_t val2 = get(MV, op2, 4);
    int32_t res = val1 - val2;
    setCC(MV,res);
    set(MV, op1, res);
}
void MUL(TVM *MV)
{
    uint32_t op1 = MV->registros[OP1]; 
    uint32_t op2 = MV->registros[OP2]; 
    int32_t val1 = get(MV, op1, 4);
    int32_t val2 = get(MV, op2, 4);
    int32_t res = val1 * val2;
    setCC(MV,res);
    set(MV, op1, res);
}
void DIV(TVM *MV)
{
    uint32_t op1 = MV->registros[OP1]; 
    uint32_t op2 = MV->registros[OP2]; 
    int32_t val1 = get(MV, op1, 4);
    int32_t val2 = get(MV, op2, 4);
    //Controlar div 0
    int32_t res = val1 / val2;
    set(MV, op1, res);
    setCC(MV,res);
    setAC(MV,val1 % val2);
}
void CMP(TVM *MV)
{
    uint32_t op1 = MV->registros[OP1]; 
    uint32_t op2 = MV->registros[OP2]; 
    int32_t val1 = get(MV, op1, 4);
    int32_t val2 = get(MV, op2, 4);
    int32_t res = val1 - val2;
    setCC(MV,res);
}
void SHL(TVM *MV)
{
    // code
}
void SHR(TVM *MV)
{
    // code
}
void SAR(TVM *MV)
{
    // code
}
void AND(TVM *MV)
{
    // code
}
void OR(TVM *MV)
{
    // code
}
void XOR(TVM *MV)
{
    // code
}
void SWAP(TVM *MV)
{
    // code
}
void LDL(TVM *MV)
{
    int32_t op1 = MV->registros[OP1];
    int32_t op2 = MV->registros[OP2];
    int32_t reg = MV->registros[OP1] & 0xFFFF;
    MV->registros[reg] = (MV->registros[reg] & 0xFFFF0000) | (op2 & 0xFFFF);
}
void LDH(TVM *MV)
{
    int32_t reg = MV->registros[OP1] & 0xFFFF;
    int32_t valor = MV->registros[OP2] & 0xFFFF;
    MV->registros[reg] = (MV->registros[reg] & 0x0000FFFF) | (valor << 16);
}
void RND(TVM *MV)
{
    // code
}