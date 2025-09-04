
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "vm.h"
#include "vm.c"


int main(int argc, char *argv[]){
   /* uint8_t memoria[16384]; // puede ser negativo??
    uint32_t tablaDescriptoresSegmentos[8]; // preguntar por CS Y DS en 00 00 00 00 y 00 01 00 00
    uint32_t registros[32]; // puede ser negativo??*/

    TVM *VM;

    

    unsigned int CS,DS;
    // TODO agregar validaciones de argumentos -d y filename.vmx 
    if(esProgramaValido(argv[1])){
        inicializarVM(argv[1],VM->memoria,&DS,&CS);
        for (int i = 0;i<DS;i++){
            printf("%X, Operacion:%d\n",VM->memoria[i],i);
        }
    }else{
        printf("Ocurrio un error");
    }

    return 0;
}
