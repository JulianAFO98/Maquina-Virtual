
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "vm.c"


int main(int argc, char *argv[]){
    TVM VM;
    int error=0;//el error toma un numero que nos da el error
    uint32_t CS,DS;
    // TODO agregar validaciones de argumentos -d y filename.vmx 
    if(esProgramaValido(argv[1])){
        inicializarVM(argv[1],&VM,&DS,&CS);
        inicializarTablaDescriptores(&VM,CS,DS);
        inicializarRegistros(&VM,DS,CS);
        while(VM.registros[3] != 0xFFFFFFFF && !error){
            //code
        }
    }else{
        printf("Ocurrio un error");
    }

    return 0;
}
