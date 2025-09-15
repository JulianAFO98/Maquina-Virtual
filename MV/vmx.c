
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "vm.h"
#include "funciones.h"

uint8_t esProgramaValido(char *nombreArchivo);
uint8_t comprobarExtension(char *nombreArchivo);

int main(int argc, char *argv[])
{
    TVM VM;
    uint32_t direccionFisicaIP;
    if (argc >= 2 && argc < 4)
    {
        if (comprobarExtension(argv[1])) // comprueba primero que sea .vmx, asi no lee un .txt por ejemplo
        {
            if (esProgramaValido(argv[1])) // Esto ya dentro del archivo mira si lleva VMX25 en los primeros 5 bytes
            {
                inicializarVM(argv[1], &VM);
                while (VM.registros[IP] >= 0 && !VM.error)
                {      
                    direccionFisicaIP = obtenerDireccionFisica(&VM, VM.registros[IP]); // obtener instruccion a partir de la IP Logica Reg[3] es el reg IP
                    interpretaInstruccion(&VM, VM.memoria[direccionFisicaIP]);
                    cargarAmbosOperandos(&VM,direccionFisicaIP);
                    if(operaciones[VM.registros[OPC]] != NULL){
                        operaciones[VM.registros[OPC]](&VM);
                    }else{
                        VM.error = 3;
                    }
                    if(argc > 2 && strcmp(argv[2],"-d")==0){
                        disassembler(&VM, direccionFisicaIP);
                    }
                    if (!esSalto(VM.registros[OPC]) && VM.registros[IP] >= 0) 
                        VM.registros[IP] += obtenerSumaBytes(&VM) + 1;

                }
                if (VM.error && VM.registros[IP] != -1)
                   mostrarError(VM.error);
                
            }
            else
                printf("La cabecera del archivo no lleva VMX25 o el archivo no existe");
        }
        else
            printf("El archivo no tiene extension .vmx o los argumentos se ingresaron de manera incorrecta");
    }
    else
        printf("Exceso de argumentos, verifique su operacion");
    return 0;
}

uint8_t esProgramaValido(char *nombreArchivo)
{
    const char cabeceraEsperada[] = "VMX25";
    char cabecera[TAMANIO_CABECERA];
    FILE *VMX = fopen(nombreArchivo, "rb");
    uint8_t valido = 0;
    if (VMX != NULL)
    {
        fread(cabecera, 1, TAMANIO_CABECERA, VMX);
        valido = memcmp(cabecera, cabeceraEsperada, TAMANIO_CABECERA) == 0; // compara byte a byte, para no usar strcmp, mas seguro
        fclose(VMX);
    }
    return valido;
}

uint8_t comprobarExtension(char *nombreArchivo)
{
    uint16_t len = strlen(nombreArchivo);
    return len < 4 ? 0 : strcmp(nombreArchivo + (len - 4), ".vmx") == 0;
}
