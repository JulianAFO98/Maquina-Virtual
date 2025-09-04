#include <stdint.h>
#define TAMANIO_CABECERA 5
#define POSICION_CS 0
#define CANT_REGISTROS 32
#define MEMORIA 16384
#define CANT_TABLA 8

typedef struct{
    uint8_t memoria[MEMORIA]; // puede ser negativo??
    uint32_t tablaDescriptoresSegmentos[CANT_TABLA]; // preguntar por CS Y DS en 00 00 00 00 y 00 01 00 00
    uint32_t registros[CANT_REGISTROS]; // puede ser negativo??
} TVM;


void inicializarVM(char * nombreArchivo,TVM*MV, unsigned int *DS,unsigned int *CS);
void inicializaRegistros(TVM *MV);
void inicializarTablaDescriptores(TVM *VM);
int esProgramaValido(char * nombreArchivo);