#include <stdio.h>
#include <string.h>
#include "vm.h"

void inicializarVM(char * nombreArchivo,TVM *VM){
    int c;
    uint8_t version;
    uint16_t tamanio_CS,masSignificativos,menosSignificativos,i=0;

    FILE * VMX = fopen(nombreArchivo,"rb"); // no valido ya que se valido antes

    //Piso los valores hasta que lee la cabecera
    while(i<TAMANIO_CABECERA  && !feof(VMX)){
        i++;
        c=fgetc(VMX);
    }
    version = (uint8_t) fgetc(VMX);
    masSignificativos = fgetc(VMX);
    menosSignificativos = fgetc(VMX);
    tamanio_CS = (masSignificativos << 8) | menosSignificativos;
    i=0;
    while(i<tamanio_CS){
        VM->memoria[i] = fgetc(VMX);
        i++;
    }
    //Inicia registro Claves
    // sujeto a cambios ya que si cambia el inicio del CS hay que buscar la forma en que no sea harcodeado
    VM->registros[26]=POSICION_CS;
    VM->registros[27]=POSICION_CS+i;
    VM->registros[3]=POSICION_CS;
    VM->registros[LAR] = 0;
    VM->registros[MAR] = 0;
    VM->registros[MBR] = 0;
    fclose(VMX);
    
}




void inicializarTablaDescriptores(TVM *VM){
  uint32_t CS = VM ->registros[26];
  uint32_t DS = VM ->registros[27];
  VM->tablaDescriptoresSegmentos[0] = (CS << 16) | (DS & 0xFFFF);
  VM->tablaDescriptoresSegmentos[1] = (DS << 16) | ((MEMORIA-DS) & 0xFFFF);
}


uint32_t obtenerDireccionFisica(TVM * MV, uint32_t direccionLogica,int *error){
    uint16_t segmento = direccionLogica >> 16; // Obtengo el 0001 o el 0000 del codigo de segmento
    uint32_t direccionBase = (MV->tablaDescriptoresSegmentos[segmento] >> 16) & 0xFFFF;
    uint32_t offSet = direccionLogica & 0x0000FFFF;
    uint32_t direccionFisica = direccionBase + offSet;
    uint32_t limiteSegmento = MV->tablaDescriptoresSegmentos[segmento] & 0xFFFF;


    if (direccionFisica < direccionBase || direccionFisica + 3 > limiteSegmento) {
        //error(1, )
        *error = 1;  
    }   
    return direccionFisica;
}

void error(int tipoError, int *error)
{
    switch(tipoError){
        case 1:
        case 2:
        case 3:
        case 4:
    }
}

char* operacionDessambler(uint8_t codOp) {
    switch(codOp) {
        case 0x00: return "SYS";
        case 0x01: return "JMP";
        case 0x02: return "JZ";
        case 0x03: return "JP";
        case 0x04: return "JN";
        case 0x05: return "JNZ";
        case 0x06: return "JNP";
        case 0x07: return "JNN";
        case 0x08: return "NOT";
        case 0x0F: return "STOP";
        case 0x10: return "MOV";
        case 0x11: return "ADD";
        case 0x12: return "SUB";
        case 0x13: return "MUL";
        case 0x14: return "DIV";
        case 0x15: return "CMP";
        case 0x16: return "SHL";
        case 0x17: return "SHR";
        case 0x18: return "SAR";
        case 0x19: return "AND";
        case 0x1A: return "OR";
        case 0x1B: return "XOR";
        case 0x1C: return "SWAP";
        case 0x1D: return "LDL";
        case 0x1E: return "LDH";
        case 0x1F: return "RND";
        default:   return "???";
    }
}



void interpretaInstruccion(TVM*MV, uint8_t instruccion) {
    MV ->registros[5] = 0x0;
    MV ->registros[6] = 0x0;
    // lo acomode para guardarlos en 32bits(sin el valor de los bytes leidos aun)
    // normalizo y luego shifteo
    MV ->registros[5] = ((instruccion >> 4) & 0x3) << 24;  // Ej:0x01000000 -> Con la lectura en main se volveria 0x01000033 
    MV ->registros[6] = ((instruccion >> 6) & 0x3) << 24; 
    MV ->registros[4] = instruccion & 0x1F;
}


uint8_t obtenerSumaBytes(TVM *MV){
    uint8_t op1 = (MV ->registros[5] >> 24) & 0x3;
    uint8_t op2 = (MV ->registros[6] >> 24) & 0x3;
    return op1+op2;
}

uint32_t cargarOperando(uint32_t reg, uint8_t *memoria, uint32_t direccion, uint8_t cantidadBytes)
{
    //Mantengo por las dudas el byte mas significativo
    uint32_t tipo = reg & 0xFF000000;
    uint32_t valor = 0;
    uint32_t inst;

    //Valor lo seteo en 0. En cada iteracion valor se corre 1 byte a la izquierda
    //Ej: declaro valor = 0x00000000
    //Paso 1 -> valor << 8 | memoria = 0x00000012
    //Paso 2 -> valor << 8 | memoria = 0x00001234
    // y asi
    for (int i = 0; i < cantidadBytes; i++)
    {
        valor = (valor << 8) | memoria[direccion + i + 1];
    }
    inst = tipo | valor; 

    return inst;
}

void MOV1(TVM *MV, uint32_t op1, uint32_t op2){
    set(MV, op1, get(MV, op2, sizeof(op2)));
}

uint32_t get(TVM *MV, uint32_t op, uint8_t cantBytes) {
    uint32_t TOperando = (op & 0xFF000000) >> 24;
    uint32_t valor;
    if (TOperando == MEMORIA) {
        int error = 0;
        uint32_t segmento = (MV->tablaDescriptoresSegmentos[0] >> 16) & 0xFFFF;
        uint32_t offset = op & 0xFFFF;
        uint32_t dirLogica = (segmento << 16) | offset;
        uint32_t dirFisica = obtenerDireccionFisica(MV, dirLogica, &error);
        uint32_t cantBytes = (op & 0xFF000000) >> 24;

        // Guardar dirección lógica en LAR
        MV->registros[LAR] = dirLogica;
        // Guardar cantidad de bytes (alto) y dirección física (bajo) en MAR
        MV->registros[MAR] = ((cantBytes << 16) & 0xFFFF0000) | (dirFisica & 0x0000FFFF);

        // Leer valor de memoria, byte a byte
        valor = 0;
        for (int i = 0; i < cantBytes; i++) {
            valor = (valor << 8) | MV->memoria[dirFisica + i];
        }

        // Guardar valor leído en MBR
        MV->registros[MBR] = valor;
    } else if (TOperando == REGISTRO) {
        // Leer directamente del registro
        valor = MV->registros[op];
        MV->registros[MBR] = valor;
    }else if (TOperando == INMEDIATO) {
        valor = op & 0x00FFFFFF; // valor inmediato (24 bits)
        MV->registros[MBR] = valor;
    }
    return valor;
}

void set(TVM *MV,uint32_t op1,uint32_t op2){
    uint32_t TOperando = (op1 & 0xFF000000) >> 24;
    if(TOperando == MEMORIA){
        int error = 0;
        uint32_t segmento = (MV->tablaDescriptoresSegmentos[0] >> 16) & 0xFFFF; // segmento de la dirección lógica
        uint32_t offset = op1 & 0xFFFF;           // offset de la dirección lógica
        uint32_t dirLogica = (segmento << 16) | offset;
        uint32_t dirFisica = obtenerDireccionFisica(MV, dirLogica, &error);
        uint32_t cantBytes = (op2 & 0xFF000000) >> 24;
        MV->registros[MAR] = ((cantBytes << 16) & HIGH_MASK) | (dirFisica & LOW_MASK); //Cargo en los 2 byte mas significativos la cantidad de bytes en memoria y en los menos significativos la direccion fisica
        MV->registros[MBR] = op2 & 0x0000FFFF; //Filtro los bytes que pertenecen al tipo de operando

        for(int i=0; i<cantBytes ; i++){
            //MV->memoria[(MV->tablaDescriptoresSegmentos[0]&LOW_MASK) + cantBytes + i] = (op2 >> 8) * (cantBytes - 1); //No estoy seguro si esta bien
            MV->memoria[dirFisica + i] = (op2 >> (8 * (cantBytes - 1 - i))) & 0xFF; //Copilot me dijo que lo haga asi
        }
    }else if(TOperando == REGISTRO){
        MV->registros[OP1] = op2 & 0x00FFFFFF; //Ej : OP2 = 02 00 00 01 -> si le aplico Mid-High Mask => 00 00 00 01
    }
}

