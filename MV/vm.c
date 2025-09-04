#include <stdio.h>
#include <string.h>
#include "vm.h"

#define TAMANIO_CABECERA 5
#define POSICION_CS 0


void inicializarVM(char * nombreArchivo,uint8_t *memoria,unsigned int *DS,unsigned int *CS){
    int c;
    uint8_t version;
    uint16_t tamanio_CS,masSignificativos,menosSignificativos,i=0;

    FILE * VMX = fopen(nombreArchivo,"rb"); // no valido ya que se valido antes

    //Piso los valores hasta que lee la cabecera
    while(i<TAMANIO_CABECERA  && !feof(VMX)){
        i++;
        c=fgetc(VMX);
    }
    
    
    version = (uint8_t) fgetc(VMX);
    masSignificativos = fgetc(VMX);
    menosSignificativos = fgetc(VMX);
    tamanio_CS = (masSignificativos << 8) | menosSignificativos;
    i=0;
    while(i<tamanio_CS){
        memoria[i] = fgetc(VMX);
        i++;
    }
    *CS = POSICION_CS;
    *DS = POSICION_CS+i;
    
    fclose(VMX);
    
}


int esProgramaValido(char * nombreArchivo){
    char cabeceraEsperada[] = "VMX25";
    char cabecera[TAMANIO_CABECERA + 1];
    int i = 0;
    FILE * VMX = fopen(nombreArchivo,"rb");
    if(VMX == NULL){
        printf("El archivo no existe");
    }else{
         while(i<TAMANIO_CABECERA  && !feof(VMX)){
            cabecera[i] = fgetc(VMX);
            i++;
        }
        cabecera[i] = '\0';
        fclose(VMX);
    }
     return strcmp(cabeceraEsperada,cabecera) == 0;
}


