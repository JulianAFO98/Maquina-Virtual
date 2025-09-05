
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "vm.c"


int main(int argc, char *argv[]){
    TVM VM;
    int error=0;//el error toma un numero que nos da el error (Por ejemplo podemos poner error = 1 Segmentation fault , error = 2 Se fue todo a la verg) 
    uint8_t OP1,OP2,OP;
    uint32_t CS,DS,direccionFisicaIP,cont = 0; // el cont lo puse nomas para que termine el bucle, ya que aun no tengo condicion de corte correcta
    // TODO agregar validaciones de argumentos -d y filename.vmx 
    if(esProgramaValido(argv[1])){
        inicializarVM(argv[1],&VM);
        inicializarTablaDescriptores(&VM);
        while(VM.registros[3] != 0xF && !error && cont<45){
            direccionFisicaIP = obtenerDireccionFisica(&VM,VM.registros[3],&error);// obtener instruccion a partir de la IP Logica Reg[3] es el reg IP
            printf("%X\n",VM.memoria[direccionFisicaIP]);
            VM.registros[3]+=1;
            cont++;
        }
        uint32_t a = obtenerDireccionFisica(&VM,0x00000008,&error);
    }else{
        printf("Ocurrio un error");
    }
    uint8_t inst = 0b10110001;
    interpretaInstruccion(inst);

    return 0;
}
