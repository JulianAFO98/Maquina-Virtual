#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"

void inicializarVM(char *nombreArchivo, TVM *VM, uint32_t tamanioMemoria,char * vectorParametros)
{
    int c;
    uint32_t acumuladorMemoria=0;
    uint8_t version;
    uint16_t tamanio_CS, masSignificativos, menosSignificativos, i = 0;
    VM->memoria =(uint8_t*) malloc(tamanioMemoria * sizeof(uint8_t));
    FILE *VMX = fopen(nombreArchivo, "rb"); // no valido ya que se valido antes

    // Piso los valores hasta que lee la cabecera
    while (i < TAMANIO_CABECERA && !feof(VMX))
    {
        i++;
        c = fgetc(VMX);
    }
    version = (uint8_t)fgetc(VMX);

    if(version == 1){
        masSignificativos = fgetc(VMX);
        menosSignificativos = fgetc(VMX);
        tamanio_CS = (masSignificativos << 8) | menosSignificativos;
        i = 0;
        while (i < tamanio_CS)
        {
            VM->memoria[i] = fgetc(VMX);
            i++;
        }
        VM->registros[CS] = 0x0; 
        VM->registros[DS] = (0x0001 << 16);
        VM->registros[IP] = POSICION_CS;
        VM->tablaDescriptoresSegmentos[0] = VM->registros[CS] | (tamanio_CS & LOW_MASK);
        VM->tablaDescriptoresSegmentos[1] = (VM->tablaDescriptoresSegmentos[0] << 16) | ((MEMORIA - tamanio_CS) & LOW_MASK); // fix 0xFFFF
    }else if(version == 2){
        uint32_t contSegmentos=0;
        if (vectorParametros[0] != '\0'){
           // VM->tablaDescriptoresSegmentos[contSegmentos++] = 
        }

    }




    VM->error = 0;
    VM->registros[LAR] = 0;
    VM->registros[MAR] = 0;
    VM->registros[MBR] = 0;
    fclose(VMX);
}

uint32_t obtenerDireccionFisica(TVM *MV, uint32_t direccionLogica)
{
    uint16_t segmento = direccionLogica >> 16;                                            // Obtengo el 0001 o el 0000 del codigo de segmento
    uint32_t direccionBase = (MV->tablaDescriptoresSegmentos[segmento] >> 16) & LOW_MASK; // 0xFFFF
    uint32_t offSet = direccionLogica & LOW_MASK;                                         // 0x0000FFFF
    uint32_t direccionFisica = direccionBase + offSet;
    uint32_t limiteSegmento = MV->tablaDescriptoresSegmentos[segmento] & LOW_MASK; // 0xFFFF
    if (direccionFisica < direccionBase || direccionFisica + 3 > limiteSegmento)
        MV->error = 1;
    return direccionFisica;
}

void mostrarError(uint8_t error)
{
    switch (error)
    {
    case 1:
        printf("Segmentation fault");
        break;
    case 2:
        printf("No es posible dividir por 0");
        break;
    case 3:
        printf("Operacion no valida");
        break;
    }
}

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

char *operandoDisassembler(uint8_t op)
{
    switch (op)
    {
    case 0x0:
        return "LAR";
    case 0x1:
        return "MAR";
    case 0x2:
        return "MBR";
    case 0x3:
        return "IP";
    case 0x4:
        return "OPC";
    case 0x5:
        return "OP1";
    case 0x6:
        return "OP2";
    case 0xA:
        return "EAX";
    case 0xB:
        return "EBX";
    case 0xC:
        return "ECX";
    case 0xD:
        return "EDX";
    case 0xE:
        return "EEX";
    case 0xF:
        return "EFX";
    case 0x10:
        return "EGX";
    case 0x1F:
        return "AC";
    case 0x2F:
        return "CC";
    case 0x1B:
        return "DS";
    case 0x1A:
        return "CS";
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

void interpretaInstruccionDisassembler(TVM *MV, uint8_t instruccion, uint32_t *op1, uint32_t *op2, uint32_t *opc)
{
    *op1 = 0x0;
    *op2 = 0x0;
    // lo acomode para guardarlos en 32bits(sin el valor de los bytes leidos aun)
    // normalizo y luego shifteo
    *op1 = ((instruccion >> 4) & 0x3) << 24; // Ej:0x01000000 -> Con la lectura en main se volveria 0x01000033
    *op2 = ((instruccion >> 6) & 0x3) << 24;
    *opc = instruccion & 0x1F;

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
    uint32_t tipo = reg & MH_MASK; // 0xFF000000
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

void cargarAmbosOperandos(TVM *VM, uint32_t direccionFisicaIP)
{
    uint8_t op1 = (VM->registros[OP1] >> 24) & 0x3;
    uint8_t op2 = (VM->registros[OP2] >> 24) & 0x3;
    if ((op1 != 0) && (op2 != 0))
    {
        uint32_t auxDireccion = direccionFisicaIP; // Variable auxiliar para no operar directamente desde direccionFisicaIP
        if (op2 != 0)
            VM->registros[OP2] = cargarOperando(VM->registros[OP2], VM->memoria, direccionFisicaIP, op2);
        auxDireccion += op2;
        if (op1 != 0)
            VM->registros[OP1] = cargarOperando(VM->registros[OP1], VM->memoria, direccionFisicaIP + op2, op1);
    }
    else
    {
        if ((op1 == 0) && (op2 != 0))
        {
            VM->registros[OP1] = VM->registros[OP2];
            VM->registros[OP1] = cargarOperando(
                VM->registros[OP1],
                VM->memoria,
                direccionFisicaIP,
                op2);
            VM->registros[OP2] = 0x0;
        }
    }
}

int32_t get(TVM *MV, uint32_t op, uint8_t cantBytes)
{

    uint32_t TOperando = (op & MH_MASK) >> 24; // tipo de operando // 0xFF000000
    uint32_t valor = 0;

    if (TOperando == TMEMORIA)
    {
        int error = 0;
        uint32_t segmento = (MV->registros[DS] >> 16) & LOW_MASK; // selector de segmento (ej: DS = 0001) //  27 // 0xFFFF

        int32_t offset = (int16_t)(op & LOW_MASK); // offset lógico 0xFFFF

        uint32_t regBase = (op >> 16) & ML_MASK; // registro base si hay (ej: 0D = EDX)  // 0xFF

        uint32_t dirLogica;
        if (regBase != 0)
            dirLogica = MV->registros[regBase] + offset;
        else
            dirLogica = (segmento << 16) | offset;

        uint32_t dirFisica = obtenerDireccionFisica(MV, dirLogica);
        for (int i = 0; i < cantBytes; i++)
        {
            valor = (valor << 8) | MV->memoria[dirFisica + i];
        }

        MV->registros[LAR] = dirLogica;
        MV->registros[MAR] = ((cantBytes << 16) & HIGH_MASK) | (dirFisica & LOW_MASK);
        MV->registros[MBR] = valor;
    }
    else if (TOperando == TREGISTRO)
    {
        uint32_t reg = op & ML_MASK; // 0xFF
        valor = MV->registros[reg];
    }
    else if (TOperando == TINMEDIATO)
    {
        int16_t inmediato16 = (int16_t)(op & LOW_MASK); // tomo los 16 bits bajos y extiendo signo
        valor = (int32_t)inmediato16;
    }

    return (int32_t)valor;
}

int esSalto(uint32_t codOp)
{
    switch (codOp)
    {
    case 0x01: // JMP
    case 0x02: // JZ
    case 0x03: // JP
    case 0x04: // JN
    case 0x05: // JNZ
    case 0x06: // JNP
    case 0x07: // JNN
        return 1;
    default:
        return 0;
    }
}

void set(TVM *MV, uint32_t op1, uint32_t op2)
{
    uint32_t TOperando = (op1 & MH_MASK) >> 24; // podriamos usar el Operando ya guardado en la MV // 0xFF000000
    if (TOperando == TMEMORIA)
    {
        uint32_t segmento = (MV->registros[DS] >> 16) & LOW_MASK; // 0001 siempre // 0xFFFF
        int32_t offset = (int16_t)(op1 & LOW_MASK);               // offset de la dirección lógica // 0xFFFF
        uint32_t regBase = (op1 >> 16) & ML_MASK;                 // 0D si voy con EDX // 0xFF
        uint32_t dirLogica;
        if (regBase != 0)
            dirLogica = (segmento << 16) | (MV->registros[regBase] + offset);
        else
            dirLogica = (segmento << 16) | offset;
        uint32_t dirFisica = obtenerDireccionFisica(MV, dirLogica);
        uint32_t cantBytes = 4; // deberia ser 4 siempre sujeto a cambios por parte 2
        MV->registros[LAR] = dirLogica;
        MV->registros[MAR] = ((0x0004 << 16) & HIGH_MASK) | (dirFisica & LOW_MASK); // Cargo en los 2 byte mas significativos la cantidad de bytes en memoria y en los menos significativos la direccion fisica
        MV->registros[MBR] = op2;                                                   // 0x00FFFFFF     // ojo aca le saque la mascara   // Filtro los bytes que pertenecen al tipo de operando
        // se puede extraer y hacer un  prodimiento asigna memoria con LAR MAR Y MRB
        uint32_t valor = (uint32_t)op2;
        for (int i = 0; i < cantBytes; i++)
        {
            MV->memoria[dirFisica + i] = (valor >> (8 * (cantBytes - 1 - i))) & 0xFF;
        }
    }
    else if (TOperando == TREGISTRO)
    {
        uint32_t reg = op1 & AH_MASK; // 0x00FFFFFF
        MV->registros[reg] = op2;
    }
}

void disassembler(TVM *MV, uint32_t finCS)
{
    uint32_t direccionFisicaIP;
    uint32_t op1, op2, opc;
    uint32_t tipo_operando;
    uint32_t tipo_operando2;
    uint32_t reg1, reg2;
    uint32_t direccionActual = MV->registros[IP];
    uint32_t sumaBytes=0;

    while(direccionActual<finCS)
    {
        direccionFisicaIP = obtenerDireccionFisica(MV, direccionActual);
        

        // 🔹 Paso puntero directo, no &MV
        interpretaInstruccionDisassembler(MV, MV->memoria[direccionFisicaIP], &op1, &op2, &opc);

        tipo_operando  = (op1 >> 24) & 0x3;
        tipo_operando2 = (op2 >> 24) & 0x3;
        

        // Caso: ambos operandos existen
        if ((tipo_operando != 0) && (tipo_operando2 != 0))
        {
            uint32_t valor = 0;
            for (int j = 0; j < tipo_operando2; j++)
                valor = (valor << 8) | MV->memoria[direccionFisicaIP + j + 1];
            op2 = (op2 & MH_MASK) | valor;

 
            valor = 0;
            for (int j = 0; j < tipo_operando; j++)
                valor = (valor << 8) | MV->memoria[direccionFisicaIP + tipo_operando2 + j + 1];
            op1 = (op1 & MH_MASK) | valor;
        }
        else if ((tipo_operando == 0) && (tipo_operando2 != 0))
        {
            uint32_t valor = 0;
            for (int j = 0; j < tipo_operando2; j++)
                valor = (valor << 8) | MV->memoria[direccionFisicaIP + j + 1];

            op1 = (op2 & MH_MASK) | valor;
            op2 = 0x0;
        }

        reg1 = op1 & ML_MASK;
        reg2 = op2 & ML_MASK;
        sumaBytes =  tipo_operando + tipo_operando2;
        // 🔹 Parte visual: imprime direccion, bytes y mnemónico
        printf("[%04X] ", direccionActual);

        for (int j = 0; j <= sumaBytes; j++)
            printf("%02X ", MV->memoria[direccionFisicaIP + j]);

        for (int j = 0; j < (6 - sumaBytes) * 3; j++)
            printf(" ");

        printf("%-6s ", operacionDisassembler(opc));

        // 🔹 Ahora imprimir operandos
        uint32_t registro;

        // Primer operando
        if (tipo_operando == TMEMORIA)
        {
            registro = op1 >> 16;
            int32_t offset = (int8_t)(op1 & ML_MASK);
            printf("[");
            printf("%s", operandoDisassembler(registro));
            if (offset != 0)
                printf("%+d", offset);
            printf("], ");
        }
        else if (tipo_operando == TREGISTRO)
        {
            registro = op1 & ML_MASK;
            printf("%s, ", operandoDisassembler(registro));
        }

        // Segundo operando
        if (tipo_operando2 == TMEMORIA)
        {
            registro = op2 >> 16;
            int32_t offset = (int8_t)(op2 & ML_MASK);
            printf("[");
            printf("%s", operandoDisassembler(registro));
            if (offset != 0)
                printf("%+d", offset);
            printf("]\n");
        }
        else if (tipo_operando2 == TREGISTRO)
        {
            registro = op2 & ML_MASK;
            printf("%s\n", operandoDisassembler(registro));
        }
        else if (tipo_operando2 == TINMEDIATO)
        {
            uint32_t inmediato = get(MV, op2, 4);
            printf("%d\n", inmediato);
        }
        else
            printf("\n");
        direccionActual+=sumaBytes+1;
    }
}


void setAC(TVM *VM, int32_t value)
{
    VM->registros[AC] = value;
}

void setCC(TVM *MV, uint32_t resultado)
{
    MV->registros[CC] = 0;
    int32_t valor_casteado = (int32_t)resultado;
    // Se supone que seguimos buenas practicas, pero en nuestro proyecto no seguimos esas normas
    if (valor_casteado == 0)
    {
        MV->registros[CC] = 0x40000000;
    }
    else
    {
        if (valor_casteado < 0)
        {
            MV->registros[CC] = 0x80000000;
        }
        else
        {
            MV->registros[CC] = 0x0;
        }
    }
}

void imprimirBinario32(uint32_t valor)
{
    int totalBits = 32;
    int start = 0;      // bandera: 0 = aun no se encontró 1, 1 = ya se encontró
    int todosCeros = 1; // para saber si el número era 0
    printf("0b");
    for (int i = totalBits - 1; i >= 0; i--)
    {
        int bit = (valor & (1u << i)) ? 1 : 0;

        if (bit == 1)
        {
            start = 1;      // a partir de ahora imprimimos todo
            todosCeros = 0; // no era cero
        }

        if (start)
        {
            printf("%d", bit);

            if (i % 4 == 0 && i != 0)
            {
                printf(" ");
            }
        }
    }

    if (todosCeros)
    {
        printf("0");
    }
}