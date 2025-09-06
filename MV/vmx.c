
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
    int error = 0;
    uint32_t direccionFisicaIP, cont = 0;
    if (argc >= 2 && argc < 4)
    {
        if (comprobarExtension(argv[1])) // comprueba primero que sea .vmx, asi no lee un .txt por ejemplo
        {
            if (esProgramaValido(argv[1])) //Esto ya dentro del archivo mira si lleva VMX25 en los primeros 5 bytes
            {
                inicializarVM(argv[1], &VM);
                inicializarTablaDescriptores(&VM);
                while (VM.registros[3] != -1 && !error && cont < 45)
                {                                                                             //
                    direccionFisicaIP = obtenerDireccionFisica(&VM, VM.registros[3], &error); // obtener instruccion a partir de la IP Logica Reg[3] es el reg IP
                    interpretaInstruccion(&VM, VM.memoria[direccionFisicaIP]);                // se puede cambiar y pasar solo la MV y la dire fisica
                    // if OP1 != 0 O OP2 != 0 {
                    // if OP1 == 0
                    // cargo OP1
                    // else
                    //  For Cargo OP y luego OP2
                    //}
                    // Operandos listos para operar
                    //operaciones[VM.registros[4]](&VM);
                    printf("%s\n",operacionDessambler(VM.registros[4]));
                    // Si hay algun JUMP cambiamos Ip sino sumamos como esta abajo
                    VM.registros[3] += obtenerSumaBytes(&VM) + 1; // Sumamos al IP los bytes a leer mas 1
                    cont++;
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
    if (VMX != NULL) {
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