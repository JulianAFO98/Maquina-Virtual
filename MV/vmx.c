
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "vm.h"
#include "funciones.h"

uint8_t esProgramaValido(char *nombreArchivo, char *cabeceraEsperada);
uint8_t comprobarExtension(char *nombreArchivo, char *extension);
uint8_t esArgumentoClave(const char *arg, const char *clave);

int main(int argc, char *argv[])
{
    TVM VM;
    uint32_t direccionFisicaIP;
    char nombreArchVMX[256];
    char nombreArchVMI[256];
    char vectorParametros[60000];
    uint32_t tamanioMemoria = MEMORIA;
    int8_t valido = 0;
    int8_t mostrarDisAssembler = 0;
    int cantParametros = 0;
    int cantCeldas = 0;
    int8_t esVMX1 = (argc > 1) ? comprobarExtension(argv[1], ".vmx") : 0;
    int8_t esVMI1 = (argc > 1) ? comprobarExtension(argv[1], ".vmi") : 0;
    int8_t esVMX2 = (argc > 2) ? comprobarExtension(argv[2], ".vmx") : 0;
    int8_t esVMI2 = (argc > 2) ? comprobarExtension(argv[2], ".vmi") : 0;

    if (argc > 1)
    {
        // el programa puede ejecutarse,comprueba primero que sea .vmx o .vmi, asi no lee un .txt por ejemplo
        if (esVMX1 || esVMI1)
        { // existe por lo menos algun archivo valido?
            if (esVMX1 && esVMI2)
            {
                strcpy(nombreArchVMX, argv[1]);
                strcpy(nombreArchVMI, argv[2]);
                valido = esProgramaValido(argv[1], "VMX25") && esProgramaValido(argv[2], "VMI25");
            }
            else if (esVMI1 && esVMX2)
            {
                strcpy(nombreArchVMI, argv[1]);
                strcpy(nombreArchVMX, argv[2]);
                valido = esProgramaValido(argv[1], "VMI25") && esProgramaValido(argv[2], "VMX25");
            }
            else if (esVMX1)
            { // Aca caeria el caso de la MV 1
                strcpy(nombreArchVMX, argv[1]);
                nombreArchVMI[0] = '\0';
                valido = esProgramaValido(argv[1], "VMX25");
            }
            else if (esVMI1)
            {
                strcpy(nombreArchVMI, argv[1]);
                nombreArchVMX[0] = '\0';
                valido = esProgramaValido(argv[1], "VMI25");
            }

            if (valido) // valido si las cabeceras internas de los archivos es valida "VMX25" o "VMI25"
            {
                int k = 0; // indice para vectorParametros

                for (int i = 0; i < argc; i++)
                {
                    if (esArgumentoClave(argv[i], "m"))
                        tamanioMemoria = atoi(argv[i] + 2);
                    if (strcmp(argv[i], "-d") == 0)
                    {
                        mostrarDisAssembler = 1;
                    }
                    // Si encontramos el flag -p
                    if (strcmp(argv[i], "-p") == 0)
                    {
                        // procesar los argumentos que vienen despues de -p por ejemplo si p estan en la pos 2 entonces recorre desde pos + 1
                        for (int l = i + 1; l < argc; l++)
                        {
                            cantParametros++;
                            // Copia caracter a caracter
                            for (int j = 0;; j++) // ciclo hasta encontrar un break
                            {
                                cantCeldas++;
                                vectorParametros[k++] = argv[l][j];
                                // printf("%c\n", vectorParametros[k - 1]);
                                if (argv[l][j] == '\0') // copio tambien el terminator
                                    break;
                            }
                        }
                    }
                }
                printf("Tamanio memoria -> %d\n", tamanioMemoria);
                // tener en cuenta si VMI
                if (*nombreArchVMX)
                {
                    inicializarVM(nombreArchVMX, &VM, tamanioMemoria, vectorParametros, cantParametros, cantCeldas);
                    // pasar vector de Parametros p
                }
                if (*nombreArchVMI)
                {
                    // manejar el VMI
                }
                int16_t inicioCS = (VM.tablaDescriptoresSegmentos[(VM.registros[CS] >> 16)] & HIGH_MASK) >> 16;
                uint32_t parteAltaCS = (VM.tablaDescriptoresSegmentos[(VM.registros[CS] >> 16)] & HIGH_MASK)>>16;
                uint32_t parteBajaCS = (VM.tablaDescriptoresSegmentos[(VM.registros[CS] >> 16)] & LOW_MASK);
                uint32_t finCS = parteBajaCS+parteAltaCS;
                printf("inicioCS %d\n", inicioCS);

                printf("finCS %d\n", finCS);
                //printf("IP -> %d\n", VM.registros[IP]);
                
                
                for (int i = 0; i < 70; i++)
                {
                    printf("Memoria %d 0x%02X\n", i, VM.memoria[i]);
                }
                
                if (mostrarDisAssembler)
                {
                    disassembler(&VM, finCS);
                    VM.error = 0;
                }
                while ((VM.registros[IP] <= finCS) && (VM.registros[IP] != -1) && (!VM.error))
                {
                    direccionFisicaIP = obtenerDireccionFisica(&VM, VM.registros[IP]); // obtener instruccion a partir de la IP Logica Reg[3] es el reg IP
                    interpretaInstruccion(&VM, VM.memoria[direccionFisicaIP]);
                    cargarAmbosOperandos(&VM, direccionFisicaIP);
                    if (!esSalto(VM.registros[OPC]) && VM.registros[IP] >= 0)
                        VM.registros[IP] += obtenerSumaBytes(&VM) + 1;

                    if (operaciones[VM.registros[OPC]] != NULL)
                    {
                        operaciones[VM.registros[OPC]](&VM);
                    }
                    else
                    {
                        VM.error = 3;
                    }
                }
                if (VM.error && VM.registros[IP] != -1)
                    mostrarError(VM.error);
                VM.memoria = NULL;
            }
            else
                printf("La cabecera del archivo no lleva VMX25 o el archivo no existe");
        }
        else
            printf("El archivo no tiene extension .vmx /.vmi o los argumentos se ingresaron de manera incorrecta");
    }
    else
        printf("Exceso de argumentos, verifique su operacion");
    return 0;
}

uint8_t esProgramaValido(char *nombreArchivo, char *cabeceraEsperada)
{
    char cabecera[TAMANIO_CABECERA]; // 5
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

uint8_t comprobarExtension(char *nombreArchivo, char *extension)
{
    uint16_t len = strlen(nombreArchivo);
    return len < 4 ? 0 : strcmp(nombreArchivo + (len - 4), extension) == 0;
}

uint8_t esArgumentoClave(const char *arg, const char *clave)
{
    size_t lenClave = strlen(clave);
    return (uint8_t)(strncmp(arg, clave, lenClave) == 0 && arg[lenClave] == '=');
}