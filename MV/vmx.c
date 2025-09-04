
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "vm.c"


int main(int argc, char *argv[]){
    TVM VM;

    uint32_t CS,DS;
    // TODO agregar validaciones de argumentos -d y filename.vmx 
    if(esProgramaValido(argv[1])){
        inicializarVM(argv[1],&VM,&DS,&CS);
        inicializarTablaDescriptores(&VM,CS,DS);
        inicializarRegistros(&VM);
        for (int i = 0;i<DS;i++){
            printf("%X, Operacion:%d\n",VM.memoria[i],i);
        }
    }else{
        printf("Ocurrio un error");
    }

    return 0;
}
