#include "funciones.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

void (*operaciones[32])(TVM *MV) = {
    SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, NOT,
    NULL, NULL, PUSH, POP, CALL, RET,
    STOP, MOV, ADD, SUB, MUL, DIV, CMP, SHL,
    SHR, SAR, AND, OR, XOR, SWAP, LDL, LDH, RND};

void leerSYS(TVM *MV, uint32_t dirFisica, uint32_t formato, uint32_t bytesWR)
{
    int32_t valor = 0;
    char c;
    printf("[%04X] ", dirFisica);
    if (formato == 0x01)
        scanf("%d", &valor);
    else if (formato == 0x02)
        scanf("%d", &valor);
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
    for (int i = 0; i < bytesWR; i++)
        MV->memoria[dirFisica + i] = (valor >> (8 * (3 - i))) & ML_MASK; // 0xFF
}

void escribirSYS(TVM *MV, uint32_t dirFisica, uint32_t formato, uint32_t bytesWR, uint32_t cuantasVeces)
{
    int32_t valor = 0;
    char buffer[256];
    int pos = 0;
    // construimos el valor
    for (int i = 0; i < bytesWR; i++){
        uint8_t byte = MV->memoria[dirFisica + i];
        valor = (valor << 8) | byte;
        buffer[pos++] = isprint(byte) ? (char)byte : '.';
    }
    buffer[pos] = '\0';
    printf("[%04X] ", dirFisica);
    if (formato & 0b10000)
    {
        // arreglar esto printf("binario! valor:\n",valor);
        imprimirBinario32(valor);
    }
    if (formato & 0b1000)
        printf("0x%X ", valor);
    if (formato & 0b100)
        printf("0o%o ", valor);
    if (formato & 0b10)
    { 
        printf("%.*s ", pos, buffer);     
    }
    if (formato & 0b1)
        printf("%d ", valor);
    printf("\n");
}


void stringWrite(TVM *MV, uint32_t dirFisica){
    char palabra [256];
    int i = 0;
    printf("[%04X] ",dirFisica);
    while(MV->memoria[dirFisica + i] != 0 &&  i < sizeof(palabra) - 1){
        palabra[i]=MV->memoria[dirFisica + i];
        i++;
    }
    palabra[i]= '\0';
    printf("%s",palabra);
}

void SYS(TVM *MV)
{
  
    uint32_t dirFisica = obtenerDireccionFisica(MV, MV->registros[EDX]);
    int32_t op1 = get(MV, MV->registros[OP1], 4);              // 0x1 READ 0x2 WRITE
    uint32_t bytesWR = (MV->registros[ECX] & HIGH_MASK) >> 16; // obtengo los datos del LDH
    uint32_t cuantasVeces = MV->registros[ECX] & LOW_MASK;     // obtengo los datos del LDL
    uint32_t formato = MV->registros[EAX];                     // formato decimal 0x01 formato 0x02 caracter formato 0x04 octal formato 0x08 hexa formato 0x10 binario
    
    if(op1 == 0x4){
        stringWrite(MV, dirFisica);
    }else {
        for (uint32_t i = 0; i < cuantasVeces; i++)
        {
            if (op1 == 0x1)
                leerSYS(MV, dirFisica, formato, bytesWR);
            else if (op1 == 0x2)
            {
                escribirSYS(MV, dirFisica, formato, bytesWR, cuantasVeces);
            }else if(op1 == 0x7){
                printf("\033[H\033[2J");
            }
            dirFisica += bytesWR;
        }
    }
}

void JMP(TVM *MV)
{
    int32_t op1 = MV->registros[OP1];
    //printf("Operando 1 -> 0x%08X\n", op1);
    //printf("Get Operando 1 0x%08X\n", get(MV, op1, 4));
    MV->registros[IP] = MV->registros[CS] | get(MV, op1, 4);
}
void JZ(TVM *MV)
{
    if (MV->registros[CC] & 0x40000000)
    {
        MV->registros[IP] = MV->registros[CS] | get(MV, MV->registros[OP1], 4);
    }
    else
    {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void JP(TVM *MV)
{
    if (MV->registros[CC] == 0)
    {
        MV->registros[IP] = MV->registros[CS] | get(MV, MV->registros[OP1], 4);
    }
    else
    {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void JN(TVM *MV)
{
    if (MV->registros[CC] == 0x80000000)
    {

        MV->registros[IP] = MV->registros[CS] | get(MV, MV->registros[OP1], 4);
    }
    else
    {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void JNZ(TVM *MV)
{
    if (MV->registros[CC] != 0x40000000)
    {
        MV->registros[IP] = MV->registros[CS] | get(MV, MV->registros[OP1], 4);
    }
    else
    {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void JNP(TVM *MV)
{
    if (MV->registros[CC] != 0)
    {
        MV->registros[IP] = MV->registros[CS] | get(MV, MV->registros[OP1], 4);
    }
    else
    {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void JNN(TVM *MV)
{
    if (MV->registros[CC] != 0x80000000)
    {
        MV->registros[IP] = MV->registros[CS] | get(MV, MV->registros[OP1], 4);
    }
    else
    {
        MV->registros[IP] += obtenerSumaBytes(MV) + 1;
    }
}
void NOT(TVM *MV)
{
    int32_t val = get(MV, MV->registros[OP1], 4);
    int32_t valNegado = ~val;
    set(MV, MV->registros[OP1], valNegado);
    setCC(MV, valNegado);
}
void STOP(TVM *MV)
{
    MV->registros[IP] = -1;
}
void MOV(TVM *MV)
{
    printf("\n------MOV-------\n");
    printf("Operando 1 -> 0x%08X\n", MV->registros[OP1]);
    printf("Operando 2 -> 0x%08X\n", MV->registros[OP2]);
    printf("Get Operando 2 0x%08X\n", get(MV, MV->registros[OP2], 4));
    printf("--------------\n");
    set(MV, MV->registros[OP1], get(MV, MV->registros[OP2], 4));
}
void ADD(TVM *MV)
{

    uint32_t op1 = MV->registros[OP1]; // operando destino
    uint32_t op2 = MV->registros[OP2]; // operando fuente
    int32_t val1 = get(MV, op1, 4);
    int32_t val2 = get(MV, op2, 4);
    int32_t res = val1 + val2;
    setCC(MV, res);
    set(MV, op1, res);
}

void SUB(TVM *MV)
{
    uint32_t op1 = MV->registros[OP1];
    uint32_t op2 = MV->registros[OP2];
    int32_t val1 = get(MV, op1, 4);
    int32_t val2 = get(MV, op2, 4);
    int32_t res = val1 - val2;
    setCC(MV, res);
    set(MV, op1, res);
}
void MUL(TVM *MV)
{
    uint32_t op1 = MV->registros[OP1];
    uint32_t op2 = MV->registros[OP2];
    int32_t val1 = get(MV, op1, 4);
    int32_t val2 = get(MV, op2, 4);
    int32_t res = val1 * val2;
    setCC(MV, res);
    set(MV, op1, res);
}
void DIV(TVM *MV)
{
    uint32_t op1 = MV->registros[OP1];
    uint32_t op2 = MV->registros[OP2];
    int32_t val1 = get(MV, op1, 4);
    int32_t val2 = get(MV, op2, 4);
    if (val2 != 0)
    {
        int32_t res = val1 / val2;
        set(MV, op1, res);
        setCC(MV, res);
        setAC(MV, val1 % val2);
    }
    else
    {
        MV->error = 2;
    }
}
void CMP(TVM *MV)
{
    int32_t res;
    uint32_t op1 = MV->registros[OP1];
    uint32_t op2 = MV->registros[OP2];
    int32_t val1 = (int32_t)get(MV, op1, 4);
    int32_t val2 = (int32_t)get(MV, op2, 4);
    res = val1 - val2;
    setCC(MV, res);
}

void SHL(TVM *MV)
{
    int32_t op1 = get(MV, MV->registros[OP1], 4);
    int32_t op2 = get(MV, MV->registros[OP2], 4);
    printf("op1 %X\n", op1);
    printf("op2 %X\n", op2);
    uint32_t desplazado = op1 << op2;
    printf("desplazado 0x%08X\n", desplazado);
    set(MV, MV->registros[OP1], desplazado);
}
void SHR(TVM *MV)
{
    // verificar mejor de todas formas este metodo
    int32_t op1 = get(MV, MV->registros[OP1], 4);
    int32_t op2 = get(MV, MV->registros[OP2], 4);
    uint32_t desplazado = op1;
    desplazado >>= op2;
    set(MV, MV->registros[OP1], desplazado);
}
void SAR(TVM *MV)
{
    // Checkear por las dudas
    int32_t op1 = get(MV, MV->registros[OP1], 4);
    int32_t op2 = get(MV, MV->registros[OP2], 4);
    uint32_t desplazado = op1 >> op2;
    set(MV, MV->registros[OP1], desplazado);
}
void AND(TVM *MV)
{
    int32_t op1 = get(MV, MV->registros[OP1], 4);
    //printf("Operando 1 -> 0x%08X\n", op1);
    int32_t op2 = get(MV, MV->registros[OP2], 4);
    //printf("Operando 2 -> 0x%08X\n", op2);
    uint32_t res = op1 & op2;
    //printf("Resultado -> 0x%08X\n", res);
    set(MV, MV->registros[OP1], res);
    //printf("OP1 0x%08X\n",MV->registros[OP1]);
}
void OR(TVM *MV)
{

    int32_t op1 = get(MV, MV->registros[OP1], 4);
    int32_t op2 = get(MV, MV->registros[OP2], 4);
    uint32_t res = op1 | op2;
    set(MV, MV->registros[OP1], res);
}
void XOR(TVM *MV)
{
    int32_t op1 = get(MV, MV->registros[OP1], 4);
    int32_t op2 = get(MV, MV->registros[OP2], 4);
    uint32_t res = op1 ^ op2;
    set(MV, MV->registros[OP1], res);
}
void SWAP(TVM *MV)
{
    int32_t op1 = get(MV, MV->registros[OP1], 4);
    int32_t op2 = get(MV, MV->registros[OP2], 4);
    set(MV, MV->registros[OP2], op1); // operando y valor
    set(MV, MV->registros[OP1], op2);
}
void LDL(TVM *MV)
{
    int32_t op1 = get(MV, MV->registros[OP1], 4) & HIGH_MASK;
    int32_t op2 = get(MV, MV->registros[OP2], 4) & LOW_MASK;
    op1 = op1 | op2;
    set(MV, MV->registros[OP1], op1);
}
void LDH(TVM *MV)
{
    int32_t op1 = get(MV, MV->registros[OP1], 4) & LOW_MASK;
    int32_t op2 = ((get(MV, MV->registros[OP2], 4) & LOW_MASK) << 16);
    op1 = op1 | op2;
    set(MV, MV->registros[OP1], op1);
}
void RND(TVM *MV)
{
    srand(time(NULL));
    int random = rand() % get(MV, MV->registros[OP2], 4);
    set(MV, MV->registros[OP1], random);
}

void PUSH(TVM *MV)
{
    //printf("SP antes de PUSH -> 0x%08X\n", MV->registros[SP]);
    int32_t op1 = get(MV, MV->registros[OP1], 4);
    printf("\nPUSH 0x%08X\n",op1);
    MV->registros[SP] -= 4;
    uint32_t dirFisica = obtenerDireccionFisica(MV, MV->registros[SP]);
    for (int i = 0; i < 4; i++)
    {
        MV->memoria[dirFisica + i] = (op1 >> (8 * (3 - i))) & ML_MASK; // 0xFF
    }
    
}

void POP(TVM *MV)
{
    uint32_t dirFisica = obtenerDireccionFisica(MV, MV->registros[SP]);
    int32_t valor = 0;
    for (int i = 0; i < 4; i++)
    {
        valor = (valor << 8) | MV->memoria[dirFisica + i];
    }
    set(MV, MV->registros[OP1], valor);
    MV->registros[SP] += 4;
}

void CALL(TVM *MV)
{
    int32_t op1 = get(MV, MV->registros[OP1], 4);
    MV->registros[SP] -= 4;
    uint32_t dirFisica = obtenerDireccionFisica(MV, MV->registros[SP]);
    uint32_t retorno = MV->registros[IP];
    for (int i = 0; i < 4; i++)
    {
        MV->memoria[dirFisica + i] = (retorno >> (8 * (3 - i))) & ML_MASK; // 0xFF
    }
    
    MV->registros[IP] = MV->registros[CS] | op1;
}

void RET(TVM *MV)
{
    uint32_t dirFisica = obtenerDireccionFisica(MV, MV->registros[SP]);
    int32_t valor = 0;
    for (int i = 0; i < 4; i++)
    {
        valor = (valor << 8) | MV->memoria[dirFisica + i];
    }
    MV->registros[IP] = valor;
    MV->registros[SP] += 4;
}