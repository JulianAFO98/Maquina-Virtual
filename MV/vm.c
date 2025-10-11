#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"

void inicializarVM(char *nombreArchivo, TVM *VM, uint32_t tamanioMemoria, char *vectorParametros, int argcParam, int celdasPS)
{
    int c;
    uint32_t acumuladorMemoria = 0, ultimoCero;
    uint8_t version;
    int offset = 0;        // posición de inicio de cada string
    int indicePuntero = 0; // índice del puntero que estamos guardando
    int bytesUsados = 0;   // cuenta de bytes recorridos en el bloque de strings
    uint16_t tamanio_CS, tamanio_DS, tamanio_ES, tamanio_SS, tamanio_KS, tamanioGenerico, masSignificativos, menosSignificativos, i = 0;

    FILE *VMX = fopen(nombreArchivo, "rb"); // no valido ya que se valido antes
    VM->memoria = (uint8_t *)malloc(tamanioMemoria * sizeof(uint8_t));

    // Piso los valores hasta que lee la cabecera
    while (i < TAMANIO_CABECERA && !feof(VMX))
    {
        i++;
        c = fgetc(VMX);
    }
    version = (uint8_t)fgetc(VMX);

    if (version == 1)
    {
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
        printf("reg CS -> 0x%08X\n", VM->registros[CS]);
        printf("reg DS -> 0x%08X\n", VM->registros[DS]);

        printf("Segmento CS -> 0x%08X\n", VM->tablaDescriptoresSegmentos[0]);
        printf("Segmento DS -> 0x%08X\n", VM->tablaDescriptoresSegmentos[1]);
    }
    else if (version == 2)
    {
        uint32_t contSegmentos = 0;
        // Lo declaro afuera del condicional porque igualmente si no existiera -p celdasPS = 0 y argcParam = 0
        int tamanio_PS = celdasPS + (argcParam * 4);
        // Esto es solo para el CS hay que hacerlo con el resto de segmentos
        VM->registros[PS] = -1;
        VM->registros[KS] = -1;
        VM->registros[CS] = -1;
        VM->registros[DS] = -1;
        VM->registros[ES] = -1;
        VM->registros[SS] = -1;

        for (int i = 0; i < 5; i++)
        {
            masSignificativos = fgetc(VMX);
            menosSignificativos = fgetc(VMX);
            printf("mas %X menos %X   ", masSignificativos, menosSignificativos);
            printf("\n");
            switch (i)
            {
            case 0:
                tamanio_CS = (masSignificativos << 8) | menosSignificativos;
                break;
            case 1:
                tamanio_DS = (masSignificativos << 8) | menosSignificativos;
                break;
            case 2:
                tamanio_ES = (masSignificativos << 8) | menosSignificativos;
                break;
            case 3:
                tamanio_SS = (masSignificativos << 8) | menosSignificativos;
                break;
            case 4:
                tamanio_KS = (masSignificativos << 8) | menosSignificativos;
                break;
            }
        }
        printf("Tamanio PS %X\n", tamanio_PS);
        printf("Tamanio KS %X\n", tamanio_KS);
        printf("Tamanio CS %X\n", tamanio_CS);
        printf("Tamanio DS %X\n", tamanio_DS);
        printf("Tamanio ES %X\n", tamanio_ES);
        printf("Tamanio SS %X\n", tamanio_SS);
        if (argcParam != 0)
        {
            VM->tablaDescriptoresSegmentos[contSegmentos++] = tamanio_PS;
            VM->registros[PS] = 0x0;
            if (tamanio_KS != 0)
            {
                VM->tablaDescriptoresSegmentos[contSegmentos++] = (VM->tablaDescriptoresSegmentos[0] << 16) | tamanio_KS;
                VM->registros[KS] = (contSegmentos - 1) << 16;
                VM->tablaDescriptoresSegmentos[contSegmentos++] =(VM->tablaDescriptoresSegmentos[contSegmentos - 2] << 16)+(VM->tablaDescriptoresSegmentos[contSegmentos - 2] & HIGH_MASK) | tamanio_CS;
            }else{
                VM->tablaDescriptoresSegmentos[contSegmentos++] = ((VM->tablaDescriptoresSegmentos[0] << 16) | tamanio_CS);
            }
            VM->registros[CS] = (contSegmentos - 1) << 16;
            if (tamanio_DS != 0)
            {
                VM->tablaDescriptoresSegmentos[contSegmentos++] = (VM->tablaDescriptoresSegmentos[contSegmentos - 2] << 16)+(VM->tablaDescriptoresSegmentos[contSegmentos - 2] & HIGH_MASK) | tamanio_DS;
                 VM->registros[DS] = (contSegmentos - 1) << 16;
            }
            if (tamanio_ES != 0)
            {
                VM->tablaDescriptoresSegmentos[contSegmentos++] = (VM->tablaDescriptoresSegmentos[contSegmentos - 2] << 16)+(VM->tablaDescriptoresSegmentos[contSegmentos - 2] & HIGH_MASK) | tamanio_ES ;
                VM->registros[ES] = (contSegmentos - 1) << 16;
            }
            VM->tablaDescriptoresSegmentos[contSegmentos++] = (VM->tablaDescriptoresSegmentos[contSegmentos - 2] << 16)+(VM->tablaDescriptoresSegmentos[contSegmentos - 2] & HIGH_MASK) | tamanio_SS ;
            VM->registros[SS] = (contSegmentos - 1) << 16;
            memcpy(VM->memoria, vectorParametros, tamanio_PS);
            for (int i = 0; i < tamanio_PS; i++)
            {
                bytesUsados++;
                if (vectorParametros[i] == '\0')
                {
                    uint32_t puntero = offset;

                    // posición donde escribir el puntero dentro del PS
                    // (los punteros se guardan al final del bloque)
                    uint32_t posDestino = tamanio_PS - (argcParam - indicePuntero) * 4;
                    VM->memoria[posDestino + 0] = (puntero >> 24) & 0xFF;
                    VM->memoria[posDestino + 1] = (puntero >> 16) & 0xFF;
                    VM->memoria[posDestino + 2] = (puntero >> 8) & 0xFF;
                    VM->memoria[posDestino + 3] = puntero & 0xFF;
                    indicePuntero++;
                    offset = i + 1; // el siguiente string empieza después del '\0'
                }
            }
        }
        else
        {
            if (tamanio_KS != 0)
            {
                VM->tablaDescriptoresSegmentos[contSegmentos++] =  (tamanio_KS & LOW_MASK);
                VM->registros[KS] = 0x0;
                VM->tablaDescriptoresSegmentos[contSegmentos++] = ((VM->tablaDescriptoresSegmentos[contSegmentos - 2] << 16) | (tamanio_CS & LOW_MASK));
            }else{
                VM->tablaDescriptoresSegmentos[contSegmentos++] = (tamanio_CS & LOW_MASK);                
            }
            VM->registros[CS] = (contSegmentos - 1) << 16;
            if (tamanio_DS != 0)
            {
                VM->tablaDescriptoresSegmentos[contSegmentos++] = (VM->tablaDescriptoresSegmentos[contSegmentos - 2] << 16)+(VM->tablaDescriptoresSegmentos[contSegmentos - 2] & HIGH_MASK) | tamanio_DS;
                VM->registros[DS] = (contSegmentos - 1) << 16;
            }
            if (tamanio_ES != 0)
            {
                VM->tablaDescriptoresSegmentos[contSegmentos++] = (VM->tablaDescriptoresSegmentos[contSegmentos - 2] << 16)+(VM->tablaDescriptoresSegmentos[contSegmentos - 2] & HIGH_MASK) | tamanio_ES ;
                VM->registros[ES] = (contSegmentos - 1) << 16;
            }
            VM->tablaDescriptoresSegmentos[contSegmentos++] = (VM->tablaDescriptoresSegmentos[contSegmentos - 2] << 16)+(VM->tablaDescriptoresSegmentos[contSegmentos - 2] & HIGH_MASK) | tamanio_SS ;
            VM->registros[SS] = (contSegmentos - 1) << 16;
        }
        
        fgetc(VMX);
        fgetc(VMX);
        int16_t inicioCS = (VM->tablaDescriptoresSegmentos[(VM->registros[CS] >> 16)] & HIGH_MASK) >> 16;
        for(int i=0;i<tamanio_CS;i++){
            VM->memoria[inicioCS+i] = fgetc(VMX);
        }
        if(tamanio_KS!=0){
             int16_t inicioKS = (VM->tablaDescriptoresSegmentos[(VM->registros[KS] >> 16)] & HIGH_MASK) >> 16;
            for(int i=0;i<tamanio_KS;i++){
                VM->memoria[inicioKS+i] = fgetc(VMX);
            }
        }

        printf("registros:\n");
        printf("reg PS -> 0x%08X\n", VM->registros[PS]);
        printf("reg KS -> 0x%08X\n", VM->registros[KS]);
        printf("reg CS -> 0x%08X\n", VM->registros[CS]);
        printf("reg DS -> 0x%08X\n", VM->registros[DS]);
        printf("reg ES -> 0x%08X\n", VM->registros[ES]);
        printf("reg SS -> 0x%08X\n", VM->registros[SS]);
        // preguntar
        //printf("Segmentos:\n");
        printf("Segmento PS -> 0x%08X\n", VM->tablaDescriptoresSegmentos[0]);
        printf("Segmento CS -> 0x%08X\n", VM->tablaDescriptoresSegmentos[1]);
        printf("Segmento DS -> 0x%08X\n", VM->tablaDescriptoresSegmentos[2]);
        printf("Segmento ES -> 0x%08X\n", VM->tablaDescriptoresSegmentos[3]);
        printf("Segmento SS -> 0x%08X\n", VM->tablaDescriptoresSegmentos[4]);
        uint16_t offsetEntryPoint = getEntryPointOffset(nombreArchivo);
        uint32_t ipLogica = VM->registros[CS]  | offsetEntryPoint;
        VM->registros[IP] = ipLogica;
    }
    //Normaliza registros
    for(int q=0xA;q<0x10;q++){
        VM->registros[q] = 0;
       // printf("E%XX = %d\n",q, VM->registros[q]);
    }

    VM->error = 0;
    VM->registros[LAR] = 0;
    VM->registros[MAR] = 0;
    VM->registros[MBR] = 0;
    fclose(VMX);
}

uint32_t obtenerDireccionFisica(TVM *MV, uint32_t direccionLogica)
{
    uint16_t segmento = direccionLogica >> 16 & LOW_MASK;  
    //printf("segmento 0x%08X\n",segmento);                                          // Obtengo el 0001 o el 0000 del codigo de segmento
    uint32_t direccionBase = (MV->tablaDescriptoresSegmentos[segmento] >> 16) & LOW_MASK; // 0xFFFF
    uint32_t tamanioSegmento = MV->tablaDescriptoresSegmentos[segmento]  & LOW_MASK;
    uint32_t offSet = direccionLogica & LOW_MASK;                                         // 0x0000FFFF
    uint32_t direccionFisica = direccionBase + offSet;
    //good
    uint32_t limiteSegmento = tamanioSegmento + direccionBase;
    //printf("limite segmento 0x%08X\n",limiteSegmento);
    //printf("c1 0x%08X\n",direccionFisica < direccionBase);
    //printf("c2 0x%08X\n",direccionFisica + 3 > limiteSegmento);
    
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
    case 0x4A:
        return "AL";
    case 0x8A:
        return "AH";
    case 0xCA:
        return "AX";
    case 0xB:
        return "EBX";
    case 0x4B:
        return "BL";
    case 0x8B:
        return "BH";
    case 0xCB:
        return "BX";
    case 0xC:
        return "ECX";
    case 0x4C:
        return "CL";
    case 0x8C:
        return "CH";
    case 0xCC:
        return "CX";
    case 0xD:
        return "EDX";
    case 0x4D:
        return "DL";
    case 0x8D:
        return "DH";
    case 0xCD:
        return "DX";
    case 0xE:
        return "EEX";
    case 0x4E:
        return "EL";
    case 0x8E:
        return "EH";
    case 0xEA:
        return "EX";
    case 0xF:
        return "EFX";
    case 0x4F:
        return "FL";
    case 0x8F:
        return "FH";
    case 0xCF:
        return "FX";
    case 0x10:
        return "AC";
    case 0x11:
        return "CC";
    case 0x1B:
        return "DS";
    case 0x1A:
        return "CS";
    case 0x1C:
        return "ES";
    case 0x1D:
        return "SS";
    case 0x1E:
        return "KS";
    case 0x1F:
        return "PS";
    }
}

uint32_t getEntryPointOffset(char *nombreArchivo)
{
    FILE *VMX = fopen(nombreArchivo, "rb"); // no valido ya que se valido antes
    int i = 0;
    int numByteMax = 16, masSignificativo, menosSignificativo;
    char c;
    while (i < numByteMax && !feof(VMX))
    {
        c = fgetc(VMX);
        if (i == 5 && c == 1)
        {
            return 0;
            fclose(VMX);
        }
        i++;
    }
    masSignificativo = fgetc(VMX);
    menosSignificativo = fgetc(VMX);
    fclose(VMX);
    int entryPointOffset = (masSignificativo << 16) | menosSignificativo;
    return entryPointOffset;
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
        uint32_t segmento = MV->registros[DS] & HIGH_MASK; // selector de segmento (ej: DS = 0001) //  27 // 0xFFFF
        int32_t offset = (int16_t)(op & LOW_MASK); // offset lógico 0xFFFF
        uint32_t regBase = (op >> 16) & ML_MASK; // registro base si hay (ej: 0D = EDX)  // 0xFF
        uint32_t dirLogica;
        if(regBase != 0){
            uint8_t sectorReg = (regBase & 0xF0) >> 4;
            if(sectorReg == 4){
                regBase = regBase & 0x0000000F;
                dirLogica = segmento | ((MV->registros[regBase] & 0xFF) + offset);

            }else if(sectorReg == 8){
                regBase = regBase & 0x0000000F;
                dirLogica = segmento  | ((MV->registros[regBase] >> 8) + offset);
            }else if(sectorReg == 12){
                regBase = regBase & 0x0000000F;
                dirLogica = segmento| ((MV->registros[regBase] & 0xFFFF) + offset);
            }else{
               dirLogica = segmento| (MV->registros[regBase] + offset);
            }
        }else{
            dirLogica = segmento| offset;
        }
        
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
        /*A partir del registro que quiero obtener de memoria deberia verifiar si el byte mas significativo
          pertenece a un sector especifico de dicho registro entonces le mando esa cantidad de bits que se
          quiere obtener.
          Por ejemplo si OP1 = 0100008A -> esto pertenece al 3er byte de EAX es decir -> EAX = 12345678 entonces AH = 56
          y si por ejemplo quiero hacer MOV AH, 6 => AH = 11 y EAX queda conformado como -> EAX = 12340B78
        */
        uint32_t reg = (op & ML_MASK); // 0xFF
        //printf("Operando -> 0X%08X\n", op);
        //printf("Registro -> %X\n", reg & 0x0000000F);
        uint32_t sectorReg = (op & 0x0000000F0) >> 4;
        //printf("Sector registros %X\n", sectorReg);
        //printf("Valor antes -> %X %d\n", MV->registros[reg], MV->registros[reg]);
        //Cuando entra en alguno de estos IF es mejor redefinir el registro ya que unicamente entra en las condiciones si son
        //registros de proposito general
        if (sectorReg == 4)
        { 
            reg = reg & 0x0000000F;
            printf("Registro -> %X\n", reg);
            printf("Antes %d\n", MV->registros[reg]);
            valor = (uint8_t)(MV->registros[reg] & ML_MASK);
            printf("Despues %d\n", valor);
        }
        else if (sectorReg == 8)
        { 
            reg = reg & 0x0000000F;
            printf("Resultado en GET antes 0x%08X\n", MV->registros[reg]);
            valor = (uint8_t)((MV->registros[reg] & 0xFF00) >> 8);
            printf("Resultado en GET dps 0x%08X\n", MV->registros[reg]);
        }
        else if (sectorReg == 12)
        { 
            reg = reg & 0x0000000F;
            valor = (uint16_t)(MV->registros[reg] & LOW_MASK);
        }
        else
        {
            valor = MV->registros[reg];
        }
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
        uint32_t segmento = (MV->registros[DS] & HIGH_MASK); // 0001 siempre // 0xFFFF
        int32_t offset = (int16_t)(op1 & LOW_MASK);               // offset de la dirección lógica // 0xFFFF
        uint32_t regBase = (op1 >> 16) & ML_MASK;                 // 0D si voy con EDX // 0xFF
        uint32_t dirLogica;
        if (regBase != 0){
            uint8_t sectorReg = (regBase & 0xF0) >> 4;
            if(sectorReg == 4){
                regBase = regBase & 0x0000000F;
                dirLogica = segmento | ((MV->registros[regBase] & 0xFF)+ offset);
            }else if(sectorReg == 8){
                regBase = regBase & 0x0000000F;
                dirLogica = segmento  | (((MV->registros[regBase] >> 8) & 0xFF)+ offset);
            }else if(sectorReg == 12){
                regBase = regBase & 0x0000000F;
                dirLogica = segmento| ((MV->registros[regBase] & 0xFFFF) + offset);
            }else{
               dirLogica = segmento| (MV->registros[regBase] + offset);
            }
        }
        else
            dirLogica = (segmento << 16) | offset;
        printf("Dir. Logica -> 0x%08X\n", dirLogica);
        uint32_t dirFisica = obtenerDireccionFisica(MV, dirLogica);
        printf("Dir. fisica -> 0x%08X\n", dirFisica);
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
        uint8_t sectorReg = (reg & 0xF0) >> 4;
        if (sectorReg == 4)
        { 
            reg = reg & 0x0000000F;
             printf("Resultado en SET antes 0x%08X\n", MV->registros[reg]);
            MV->registros[reg] = (MV->registros[reg] & 0xFFFFFF00) | (op2 & 0xFF) ;
            printf("Resultado en SET 0x%08X\n", MV->registros[reg]);
        }
        else if (sectorReg == 8)
        { 
            reg = reg & 0x0000000F;
            printf("Resultado en SET antes 0x%08X\n", MV->registros[reg]);
            MV->registros[reg] = (MV->registros[reg] & 0xFFFF00FF) | ((op2 << 8) & 0xFF00);
            printf("Resultado en SET 0x%08X\n", MV->registros[reg]);
        }
        else if (sectorReg == 12)
        { 
            reg = reg & 0x0000000F;
            MV->registros[reg] = (MV->registros[reg] & 0xFFFF0000) | ((op2) & 0xFFFF);
        }
        else
        {
            MV->registros[reg] = op2;
       }
    }
}


void disassembler(TVM *MV)
{
    uint32_t direccionFisicaIP;
    uint32_t op1, op2, opc;
    uint32_t tipo_operando;
    uint32_t tipo_operando2;
    uint32_t finCS =(MV->tablaDescriptoresSegmentos[(MV->registros[CS] >> 16)] & LOW_MASK)  + ((MV->tablaDescriptoresSegmentos[(MV->registros[CS] >> 16)] & HIGH_MASK)>>16);
    uint32_t reg1, reg2;
    uint32_t direccionActual =  obtenerDireccionFisica(MV,MV->registros[IP]);
    uint32_t sumaBytes = 0;

    while (direccionActual < finCS)
    {
        direccionFisicaIP = obtenerDireccionFisica(MV, direccionActual);

        // Paso puntero directo, no &MV
        interpretaInstruccionDisassembler(MV, MV->memoria[direccionFisicaIP], &op1, &op2, &opc);

        tipo_operando = (op1 >> 24) & 0x3;
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
        sumaBytes = tipo_operando + tipo_operando2;
        // Parte visual: imprime direccion, bytes y mnemónico
        printf("[%04X] ", direccionActual);

        for (int j = 0; j <= sumaBytes; j++)
            printf("%02X ", MV->memoria[direccionFisicaIP + j]);

        for (int j = 0; j < (6 - sumaBytes) * 3; j++)
            printf(" ");

        printf("%-6s ", operacionDisassembler(opc));

        // Ahora imprimir operandos
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
          //  printf("registro %X\n", registro);
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
            //ojo con esto, hardFIX para que se muestre bien el sys
            uint32_t opGenerico = op2 ? op2 : op1;
            uint32_t inmediato = get(MV, opGenerico, 4);
            printf("%d\n", inmediato);
        }
        else
            printf("\n");
        direccionActual += sumaBytes + 1;
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