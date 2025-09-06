
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "vm.c"


int main(int argc, char *argv[]){
    TVM VM;
    int error=0;//el error toma un numero que nos da el error (Por ejemplo podemos poner error = 1 Segmentation fault , error = 2 Se fue todo a la verg) 
    uint32_t direccionFisicaIP,cont = 0; // el cont lo puse nomas para que termine el bucle, ya que aun no tengo condicion de corte correcta
    // TODO agregar validaciones de argumentos -d y filename.vmx 
    if(esProgramaValido(argv[1])){
        inicializarVM(argv[1],&VM);
        inicializarTablaDescriptores(&VM);
        while(VM.registros[3] != -1 && !error && cont<45){ // 
            direccionFisicaIP = obtenerDireccionFisica(&VM,VM.registros[3],&error);// obtener instruccion a partir de la IP Logica Reg[3] es el reg IP
            interpretaInstruccion(&VM,VM.memoria[direccionFisicaIP]); // se puede cambiar y pasar solo la MV y la dire fisica
            //if OP1 != 0 O OP2 != 0 {
                //if OP1 == 0 
                    //cargo OP1
                //else
                    // For Cargo OP y luego OP2
            //}
            //Operandos listos para operar
            //Array Punteros[OPC]
            // Si hay algun JUMP cambiamos Ip sino sumamos como esta abajo
            VM.registros[3]+=obtenerSumaBytes(&VM)+1; // Sumamos al IP los bytes a leer mas 1
            cont++;
        }

    }else{
        printf("Ocurrio un error");
    }
    return 0;
}
