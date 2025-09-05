#include <stdint.h>
#define TAMANIO_CABECERA 5
#define POSICION_CS 0
#define CANT_REGISTROS 32
#define MEMORIA 16384
#define CANT_TABLA 8
#define MNEMONICO 4

typedef struct{
    uint8_t memoria[MEMORIA];
    uint32_t tablaDescriptoresSegmentos[CANT_TABLA];
    int32_t registros[CANT_REGISTROS];
} TVM;


void inicializarVM(char * nombreArchivo,TVM*MV);
void inicializarTablaDescriptores(TVM *VM);
int esProgramaValido(char * nombreArchivo);
uint32_t obtenerDireccionFisica(TVM * MV, uint32_t direccionLogica,int * error);
char *operacion(uint8_t codOp);
void interpretaInstruccion(uint8_t instruccion);
