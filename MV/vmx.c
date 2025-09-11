
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "vm.h"
#include "funciones.h"

uint8_t esProgramaValido(char *nombreArchivo);
uint8_t comprobarExtension(char *nombreArchivo);
uint32_t cargarOperando(uint32_t, uint8_t *, uint32_t, uint8_t);

int main(int argc, char *argv[])
{
    TVM VM;
    int error = 0;
    uint32_t direccionFisicaIP;
    if (argc >= 2 && argc < 4)
    {
        if (comprobarExtension(argv[1])) // comprueba primero que sea .vmx, asi no lee un .txt por ejemplo
        {
            if (esProgramaValido(argv[1])) // Esto ya dentro del archivo mira si lleva VMX25 en los primeros 5 bytes
            {
                inicializarVM(argv[1], &VM);
                while (VM.registros[IP] >= 0 && !error)
                {                       
                    direccionFisicaIP = obtenerDireccionFisica(&VM, VM.registros[IP], &error); // obtener instruccion a partir de la IP Logica Reg[3] es el reg IP
                    interpretaInstruccion(&VM, VM.memoria[direccionFisicaIP]);
                    uint8_t op1 = (VM.registros[OP1] >> 24) & 0x3;
                    uint8_t op2 = (VM.registros[OP2] >> 24) & 0x3;
                    if ((op1 != 0) && (op2 != 0))
                    {
                        uint32_t auxDireccion = direccionFisicaIP; // Variable auxiliar para no operar directamente desde direccionFisicaIP
                        if (op2 != 0)
                        {
                            VM.registros[OP2] = cargarOperando(VM.registros[OP2], VM.memoria, direccionFisicaIP, op2);
                            auxDireccion += op2;
                        }

                        if (op1 != 0)
                        {
                            VM.registros[OP1] = cargarOperando(VM.registros[OP1], VM.memoria, direccionFisicaIP + op2, op1);
                        }
                    }
                    else
                    {
                        if ((op1 == 0) && (op2 != 0))
                        {
                            VM.registros[OP1] = VM.registros[OP2];
                            VM.registros[OP1] = cargarOperando(
                                VM.registros[OP1],
                                VM.memoria,
                                direccionFisicaIP,
                                op2);
                            VM.registros[OP2] = 0x0;
                        }
                    }
                    operaciones[VM.registros[OPC]](&VM);
                    disassembler(&VM, direccionFisicaIP);
                    if (!esSalto(VM.registros[OPC]) && VM.registros[IP] >= 0) {
                   
                        VM.registros[IP] += obtenerSumaBytes(&VM) + 1;
                    }
                 
                }
                if (error == 1 && VM.registros[IP] != -1)
                {
                    printf("Segmentation fault");
                }
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
