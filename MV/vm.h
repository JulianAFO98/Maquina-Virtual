#include <stdint.h>
#define TAMANIO_CABECERA 5
#define POSICION_CS 0

#define CANT_REGISTROS 32
#define MEMORIA 16384
#define CANT_TABLA 8

/*typedef struct {
    uint8_t memoria[CANT_REGISTROS]; // puede ser negativo??
    uint32_t tablaDescriptoresSegmentos[CANT_TABLA]; // preguntar por CS Y DS en 00 00 00 00 y 00 01 00 00
    uint32_t registros[CANT_TABLA]; // puede ser negativo??



}vm;*/


void inicializarVM(char * nombreArchivo,uint8_t *memoria, unsigned int *DS,unsigned int *CS);
void inicializaRegistros(char *nombreArchivo, uint32_t *registros);
int esProgramaValido(char * nombreArchivo);