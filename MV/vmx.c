
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "vm.c"


int main(int argc, char *argv[]){
    TVM VM;
    int error=0;//el error toma un numero que nos da el error
    uint32_t CS,DS,instruccion;
    // TODO agregar validaciones de argumentos -d y filename.vmx 
    if(esProgramaValido(argv[1])){
        inicializarVM(argv[1],&VM);
        inicializarTablaDescriptores(&VM);

        while(VM.registros[3] != 0xF && !error){
           // VM.registros[3] = obtenerDireccionFisica(&VM,,&error) // obtener instruccion
        }
        uint32_t a = obtenerDireccionFisica(&VM,0x00000008,&error);
    }else{
        printf("Ocurrio un error");
    }

    return 0;
}
