
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "vm.c"


int main(int argc, char *argv[]){
    TVM VM;
    uint8_t  sumaBytesLeidos;
    int error=0;//el error toma un numero que nos da el error (Por ejemplo podemos poner error = 1 Segmentation fault , error = 2 Se fue todo a la verg) 
    uint32_t direccionFisicaIP,cont = 0; // el cont lo puse nomas para que termine el bucle, ya que aun no tengo condicion de corte correcta
    // TODO agregar validaciones de argumentos -d y filename.vmx 
    if(esProgramaValido(argv[1])){
        inicializarVM(argv[1],&VM);
        inicializarTablaDescriptores(&VM);
        while(VM.registros[3] != 0xF && !error && cont<45){
            direccionFisicaIP = obtenerDireccionFisica(&VM,VM.registros[3],&error);// obtener instruccion a partir de la IP Logica Reg[3] es el reg IP
            interpretaInstruccion(&VM,VM.memoria[direccionFisicaIP]); // se puede cambiar y pasar solo la MV y la dire fisica
            sumaBytesLeidos = obtenerSumaBytes(&VM);
            printf("%02X\n",VM.registros[3]);
            // TODO Hacer lectura selectiva aqui, OPC Esta cargado Pero Los operandos solo tienen Cargados la cantidad de bytes a leer
            // Habria que hacer algun for con algun if para leer si la suma de bytes es == 0 no leer nada,1 leer solo en OP1 ,2 leer en ambos y asi

            
            
            VM.registros[3]+=sumaBytesLeidos+1; // Sumamos al IP los bytes a leer mas 1
            cont++;
        }

    }else{
        printf("Ocurrio un error");
    }
    return 0;
}
