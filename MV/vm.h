#include <stdint.h>
#define TAMANIO_CABECERA 5
#define POSICION_CS 0
#define CANT_REGISTROS 32
#define MEMORIA 16384
#define CANT_TABLA 8

typedef struct{
    uint8_t memoria[MEMORIA];
    uint32_t tablaDescriptoresSegmentos[CANT_TABLA]; // preguntar por CS Y DS en 00 00 00 00 y 00 01 00 00
    int32_t registros[CANT_REGISTROS];
} TVM;


void inicializarVM(char * nombreArchivo,TVM*MV, unsigned int *DS,unsigned int *CS);
void inicializarRegistros(TVM *MV,uint32_t CS,uint32_t DS);
void inicializarTablaDescriptores(TVM *VM,uint32_t CS,uint32_t DS);
int esProgramaValido(char * nombreArchivo);