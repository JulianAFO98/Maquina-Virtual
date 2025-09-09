#include <stdio.h>
#include <string.h>
#include "vm.h"

void inicializarVM(char *nombreArchivo, TVM *VM)
{
    int c;
    uint8_t version;
    uint16_t tamanio_CS, masSignificativos, menosSignificativos, i = 0;

    FILE *VMX = fopen(nombreArchivo, "rb"); // no valido ya que se valido antes

    // Piso los valores hasta que lee la cabecera
    while (i < TAMANIO_CABECERA && !feof(VMX))
    {
        i++;
        c = fgetc(VMX);
    }
    version = (uint8_t)fgetc(VMX);
    masSignificativos = fgetc(VMX);
    menosSignificativos = fgetc(VMX);
    tamanio_CS = (masSignificativos << 8) | menosSignificativos;
    i = 0;
    while (i < tamanio_CS)
    {
        VM->memoria[i] = fgetc(VMX);
        i++;
    }
    // Inicia registro Claves
    //  sujeto a cambios ya que si cambia el inicio del CS hay que buscar la forma en que no sea harcodeado
    VM->registros[CS] = 0x0; // hot fix
    VM->registros[DS] = (0x0001 << 16);
    VM->registros[IP] = POSICION_CS;
    VM->tablaDescriptoresSegmentos[0] = VM->registros[CS] | (tamanio_CS & 0xFFFF);
    VM->tablaDescriptoresSegmentos[1] = (VM->tablaDescriptoresSegmentos[0] << 16) | ((MEMORIA - tamanio_CS) & 0xFFFF); // fix
    VM->registros[LAR] = 0;
    VM->registros[MAR] = 0;
    VM->registros[MBR] = 0;
    fclose(VMX);
}

uint32_t obtenerDireccionFisica(TVM *MV, uint32_t direccionLogica, int *error)
{
    uint16_t segmento = direccionLogica >> 16; // Obtengo el 0001 o el 0000 del codigo de segmento
    uint32_t direccionBase = (MV->tablaDescriptoresSegmentos[segmento] >> 16) & 0xFFFF;
    uint32_t offSet = direccionLogica & 0x0000FFFF;
    uint32_t direccionFisica = direccionBase + offSet;
    uint32_t limiteSegmento = MV->tablaDescriptoresSegmentos[segmento] & 0xFFFF;

    if (direccionFisica < direccionBase || direccionFisica + 3 > limiteSegmento)
    {
        // error(1, )
        *error = 1;
    }
    return direccionFisica;
}
/*
void error(int tipoError, int *error)
{
    switch(tipoError){
        case 1:
        case 2:
        case 3:
        case 4:
    }
}
*/

char *operacionDisassembler(uint8_t codOp)
{
    switch (codOp)
    {
    case 0x00:
        return "SYS";
    case 0x01:
        return "JMP";
    case 0x02:
        return "JZ";
    case 0x03:
        return "JP";
    case 0x04:
        return "JN";
    case 0x05:
        return "JNZ";
    case 0x06:
        return "JNP";
    case 0x07:
        return "JNN";
    case 0x08:
        return "NOT";
    case 0x0F:
        return "STOP";
    case 0x10:
        return "MOV";
    case 0x11:
        return "ADD";
    case 0x12:
        return "SUB";
    case 0x13:
        return "MUL";
    case 0x14:
        return "DIV";
    case 0x15:
        return "CMP";
    case 0x16:
        return "SHL";
    case 0x17:
        return "SHR";
    case 0x18:
        return "SAR";
    case 0x19:
        return "AND";
    case 0x1A:
        return "OR";
    case 0x1B:
        return "XOR";
    case 0x1C:
        return "SWAP";
    case 0x1D:
        return "LDL";
    case 0x1E:
        return "LDH";
    case 0x1F:
        return "RND";
    default:
        return "???";
    }
}

void interpretaInstruccion(TVM *MV, uint8_t instruccion)
{
    MV->registros[OP1] = 0x0;
    MV->registros[OP2] = 0x0;
    // lo acomode para guardarlos en 32bits(sin el valor de los bytes leidos aun)
    // normalizo y luego shifteo
    MV->registros[OP1] = ((instruccion >> 4) & 0x3) << 24; // Ej:0x01000000 -> Con la lectura en main se volveria 0x01000033
    MV->registros[OP2] = ((instruccion >> 6) & 0x3) << 24;
    MV->registros[OPC] = instruccion & 0x1F;
}

uint8_t obtenerSumaBytes(TVM *MV)
{
    uint8_t op1 = (MV->registros[OP1] >> 24) & 0x3;
    uint8_t op2 = (MV->registros[OP2] >> 24) & 0x3;
    return op1 + op2;
}

uint32_t cargarOperando(uint32_t reg, uint8_t *memoria, uint32_t direccion, uint8_t cantidadBytes)
{
    // Mantengo por las dudas el byte mas significativo
    uint32_t tipo = reg & 0xFF000000;
    uint32_t valor = 0;
    uint32_t inst;

    // Valor lo seteo en 0. En cada iteracion valor se corre 1 byte a la izquierda
    // Ej: declaro valor = 0x00000000
    // Paso 1 -> valor << 8 | memoria = 0x00000012
    // Paso 2 -> valor << 8 | memoria = 0x00001234
    //  y asi
    for (int i = 0; i < cantidadBytes; i++)
    {
        valor = (valor << 8) | memoria[direccion + i + 1];
    }
    inst = tipo | valor;

    return inst;
}


uint32_t get(TVM *MV, uint32_t op, uint8_t cantBytes) {

    uint32_t TOperando = (op & 0xFF000000) >> 24; // tipo de operando
    uint32_t valor = 0;

    if (TOperando == TMEMORIA) {
        int error = 0;
        uint32_t segmento = (MV->registros[27] >> 16) & 0xFFFF; // selector de segmento (ej: DS = 0001)

        uint32_t offset   = op & 0xFFFF;                        // offset lógico

        uint32_t regBase  = (op >> 16) & 0xFF;                  // registro base si hay (ej: 0D = EDX)



        uint32_t dirLogica;
        if (regBase != 0)
            dirLogica =  MV->registros[regBase] + offset;
        else
            dirLogica = (segmento << 16) | offset;
        
        uint32_t dirFisica = obtenerDireccionFisica(MV, dirLogica, &error);

        // reconstruyo valor desde memoria (big endian)
        for (int i = 0; i < cantBytes; i++) {
            valor = (valor << 8) | MV->memoria[dirFisica + i];
        }

        MV->registros[LAR] = dirLogica;
        MV->registros[MAR] = ((cantBytes << 16) & HIGH_MASK) | (dirFisica & LOW_MASK);
        MV->registros[MBR] = valor;
    }
    else if (TOperando == REGISTRO) {
        uint32_t reg = op & 0xFF;
        valor = MV->registros[reg];
    }
    else if (TOperando == INMEDIATO) {
        valor = op & 0x00FFFFFF;
    }

    return valor;
}






void set(TVM *MV, uint32_t op1, uint32_t op2)
{
    uint32_t TOperando = (op1 & 0xFF000000) >> 24; // podriamos usar el Operando ya guardado en la MV
    if (TOperando == TMEMORIA)
    {
        int error = 0;
        uint32_t segmento = (MV->registros[DS] >> 16) & 0xFFFF; // 0001 siempre 
        uint32_t offset = op1 & 0xFFFF;                         // offset de la dirección lógica
        uint32_t regBase = (op1 >> 16) & 0xFF;                  //0D si voy con EDX
        uint32_t dirLogica;
        if (regBase != 0)
            dirLogica = (segmento << 16 ) | (MV->registros[regBase] + offset) ;
        else
            dirLogica = (segmento << 16) | offset;
        uint32_t dirFisica = obtenerDireccionFisica(MV, dirLogica, &error);
        uint32_t cantBytes = 4; // deberia ser 4 siempre sujeto a cambios por parte 2
        MV->registros[LAR] = dirLogica;
        MV->registros[MAR] = ((0x0004 << 16) & HIGH_MASK) | (dirFisica & LOW_MASK); // Cargo en los 2 byte mas significativos la cantidad de bytes en memoria y en los menos significativos la direccion fisica
        MV->registros[MBR] = op2 & 0x00FFFFFF;                                      // Filtro los bytes que pertenecen al tipo de operando
        // se puede extraer y hacer un  prodimiento asigna memoria con LAR MAR Y MRB
        int32_t valor = MV->registros[MBR];
        for (int i = 0; i < cantBytes; i++)
            MV->memoria[dirFisica + i] = (valor >> (8 * (cantBytes - 1 - i))) & 0xFF;
    }
    else if (TOperando == REGISTRO)
    {
        uint32_t reg = op1 & 0x00FFFFFF;
        MV->registros[reg] = op2 & 0x00FFFFFF; // Ej : OP2 = 02 00 00 01 -> si le aplico Mid-High Mask => 00 00 00 01
    }
}


void setAC(TVM *VM,int32_t value){
  VM->registros[AC] = value;
}


void setCC(TVM *MV,uint32_t value){
  uint32_t esNegativo = (int32_t) value < 0;
  uint32_t esCero = value == 0;
  MV->registros[CC] = esNegativo << 31;
  MV->registros[CC] = (esCero << 30) | MV->registros[CC];
}