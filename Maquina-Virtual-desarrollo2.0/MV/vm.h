#ifndef VM_H
#define VM_H

#include <stdint.h>
#define TAMANIO_CABECERA 5
#define POSICION_CS 0
#define CANT_REGISTROS 32
#define MEMORIA 16384
#define CANT_TABLA 8
#define MNEMONICO 4


#define HIGH_MASK 0xFFFF0000 //2 Bytes mas significativos
#define LOW_MASK 0x0000FFFF //2 Bytes menos significativos
#define MH_MASK 0xFF000000 // Mid high mask -> byte mas significativo
#define ML_MASK 0x000000FF // MId low mask -> byte menos significativo
#define AH_MASK 0x00FFFFFF
//Quizas sea mejor agruparlos en un typedef
#define TREGISTRO 0x01
#define TINMEDIATO 0x02
#define TMEMORIA 0x03
#define KS 30
#define ES 28
#define SS 29
#define PS 31
#define CS 26
#define DS 27
#define AC 16
#define CC 17
#define LAR 0
#define MAR 1
#define MBR 2
#define IP 3
#define OPC 4
#define OP1 5
#define OP2 6
#define SP 7
#define BP 8
#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFX 15
#define EGX 16



typedef struct{
    uint8_t error;
    char vmi[256];
    char vmx[256];
    uint8_t banderaBreakPoint;
    uint32_t tamanioMemoria;
    uint8_t *memoria;
    uint32_t tablaDescriptoresSegmentos[CANT_TABLA];
    int32_t registros[CANT_REGISTROS];
} TVM;


void inicializarVM(char * nombreArchivo,TVM*MV, uint32_t tamanioMemoria,char * vectorParametros, int, int);
void inicializarVMPorVMI(char * nombreArchVMI, TVM *MV);
uint32_t obtenerDireccionFisica(TVM * MV, uint32_t direccionLogica);
uint8_t obtenerSumaBytes(TVM *MV);
char *operacionDisassembler(uint8_t codOp);
void interpretaInstruccion(TVM *VM,uint8_t instruccion);
uint32_t cargarOperando(uint32_t, uint8_t *, uint32_t, uint8_t);
void cargarAmbosOperandos(TVM *MV,uint32_t direccionFisicaIP);
int esSalto(uint32_t codOp);
void disassembler(TVM *MV);
void mostrarError(uint8_t error);
void imprimirBinario32(uint32_t valor);
void interpretaInstruccionDisassembler(TVM *MV, uint8_t instruccion,uint32_t *op1, uint32_t *op2,uint32_t * opc);
uint32_t getEntryPointOffset(char *);


//Funciones especiales de seteo y obtencion de datos
int32_t get(TVM *MV, uint32_t op,uint8_t num);
void set(TVM *MV, uint32_t op1, uint32_t op2);
void setAC(TVM *VM,int32_t value);
void setCC(TVM *MV,uint32_t value);
#endif 