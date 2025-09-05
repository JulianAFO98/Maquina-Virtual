#include <stdio.h>
#include <string.h>
#include "vm.h"

void inicializarVM(char * nombreArchivo,TVM *VM){
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
        VM->memoria[i] = fgetc(VMX);
        i++;
    }
    //Inicia registro Claves
    // sujeto a cambios ya que si cambia el inicio del CS hay que buscar la forma en que no sea harcodeado
    VM->registros[26]=POSICION_CS;
    VM->registros[27]=POSICION_CS+i;
    VM->registros[3]=POSICION_CS;
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

void inicializarTablaDescriptores(TVM *VM){
  uint32_t CS = VM ->registros[26];
  uint32_t DS = VM ->registros[27];
  VM->tablaDescriptoresSegmentos[0] = (CS << 16) | (DS & 0xFFFF);
  VM->tablaDescriptoresSegmentos[1] = (DS << 16) | ((MEMORIA-DS) & 0xFFFF);
}


// En construccion, Primero quiero tratar OP OP1 y OP2 para ver como hacer esto bien
uint32_t obtenerDireccionFisica(TVM * MV, uint32_t direccionLogica,int *error){
    uint16_t segmento = direccionLogica >> 16; // Obtengo el 0001 o el 0000 del codigo de segmento
    uint32_t direccionBase = (MV->tablaDescriptoresSegmentos[segmento] >> 16) & 0xFFFF; //Quizas es buena idea convertir las mascaras en constantes
    uint32_t offSet = direccionLogica & 0x0000FFFF;
    uint32_t direccionFisica = direccionBase + offSet;
    //Esto esta bien hasta aca dentro de todo
    uint32_t limiteSegmento = direccionLogica & 0xFFFF0000;

    printf("El numero en hexadecimal es: 0x%08X\n", direccionFisica);
}