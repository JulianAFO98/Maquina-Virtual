# Documentación - Máquina Virtual Didáctica (MV)

## Descripción General

**MV** es una máquina virtual didáctica compilada como ejecutable (.exe) para Windows que interpreta y ejecuta código ASM personalizado. La VM está diseñada para enseñanza y simulación de arquitectura de computadores, permitiendo ejecutar programas ensambladores didácticos compilados a formato binario.

---

## Componentes Principales

### 1. **vmx.c** (Intérprete Principal)
Es el núcleo de la máquina virtual. Compilado a `.exe`, interpreta archivos:
- **`.vmx`** - Archivos ejecutables compilados (código binario)
- **`.vmi`** - Archivos de inicialización (datos de memoria)

### 2. **vm.h** (Definiciones)
Define la arquitectura virtual:
- **32 registros** (0-31): EAX, EBX, ECX, EDX, etc.
- **Memoria configurable** (por defecto 16KB)
- **Tabla de descriptores de segmentos** (8 segmentos)
- **Registros especiales**:
  - `IP` (Instruction Pointer) - Contador de programa
  - `SP` (Stack Pointer) - Puntero de pila
  - `BP` (Base Pointer) - Puntero de base
  - `PS` (Program Status) - Estado del programa
  - `CC` (Condition Code) - Banderas de condición

### 3. **funciones.c** (Operaciones)
Implementa 32 instrucciones disponibles:
- **Control**: SYS, JMP, JZ, JP, JN, CALL, RET, STOP
- **Datos**: MOV, PUSH, POP, LDL, LDH, SWAP
- **Aritmética**: ADD, SUB, MUL, DIV
- **Lógica**: AND, OR, XOR, NOT
- **Desplazamiento**: SHL, SHR, SAR
- **Especial**: CMP, RND

### 4. **Elementos Auxiliares**
- **list.asm, heap.asm** - Librerías para listas y asignación de memoria
- **Carpetas de ejemplos** - `samples/`, `listas/`, `arboles/` contienen programas de demostración

---

## Cómo Usar

### Compilación del Código

Primero, compila tu código ASM a formato binario usando un ensamblador compatible:

```bash
ensamblador miPrograma.asm -o miPrograma.vmx
```

### Ejecución Básica

```bash
vmx.exe miPrograma.vmx
```

Ejecuta el programa en la VM.

### Parámetros Disponibles

#### **-d (Disassembler)**
Muestra el código ensamblador disassemblado antes de ejecutar:

```bash
vmx.exe miPrograma.vmx -d
```

**Salida típica:**
```
[0000] SYS
[0002] MOV EAX, 10
[0006] ADD EBX, EAX
[000A] PUSH EAX
```

#### **-M (Memoria)**
Especifica el tamaño de memoria en bytes (por defecto 16384):

```bash
vmx.exe miPrograma.vmx -m32768
```

Asigna 32KB de memoria a la VM.

#### **-p (Parámetros)**
Pasa parámetros al programa (strings o números). Los parámetros se cargan en memoria:

```bash
vmx.exe miPrograma.vmx -p "hola" 42 "mundo"
```

**Combinación de parámetros:**
```bash
vmx.exe miPrograma.vmx miPrograma.vmi -d -m8192 -p param1 param2
```

### Archivos de Entrada

- **`.vmx`** - Código compilado (ejecutable)
- **`.vmi`** - Datos iniciales (opcional, se carga en memoria antes de ejecutar)

Puede pasar ambos archivos en cualquier orden:
```bash
vmx.exe programa.vmx datos.vmi
vmx.exe datos.vmi programa.vmx
```

---

## ASM Didáctico - Instrucciones Básicas

### Formato de Instrucción

```plaintext
[ETIQUETA:] INSTRUCCION [OPERANDO1] [, OPERANDO2]
```

### Operandos

- **Registros**: `EAX`, `EBX`, `ECX`, `EDX`, etc.
- **Inmediatos**: Números directos (ej: `10`, `0xFF`)
- **Memoria**: `[dirección]` o `[registro]`
- **Null**: Valor nulo

### Instrucciones Frecuentes

#### **MOV - Movimiento**
Copia datos entre registros o memoria:
```asm
mov eax, 10         ; EAX = 10
mov ebx, eax        ; EBX = EAX
mov [100], ecx      ; Almacena ECX en dirección 100
```

#### **ADD/SUB - Operaciones Aritméticas**
```asm
add eax, 5          ; EAX += 5
sub ebx, eax        ; EBX -= EAX
```

#### **PUSH/POP - Pila**
```asm
push eax            ; Apila valor de EAX
pop ebx             ; Desapila a EBX
```

#### **JMP - Salto Incondicional**
```asm
jmp inicio          ; Salta a etiqueta 'inicio'
```

#### **JZ/JNZ - Saltos Condicionales**
```asm
jz fin              ; Salta si última comparación fue cero
jnz loop_mas        ; Salta si no fue cero
```

#### **CMP - Comparación**
Compara dos valores y establece banderas:
```asm
cmp eax, 10         ; Compara EAX con 10
jz igual            ; Si son iguales, salta
```

#### **CALL/RET - Subrutinas**
```asm
call funcion        ; Llama subrutina (apila dirección de retorno)
ret                 ; Retorna (desapila y continúa)
```

#### **SYS - Operación del Sistema**
Entrada/salida e interacción:
```asm
sys 0x1             ; Lee entrada
sys 0x3             ; Escribe salida
```

#### **STOP - Fin de Programa**
```asm
stop                ; Termina ejecución
```

### Ejemplo Completo

```asm
main:   mov eax, 5      ; EAX = 5
        mov ebx, 3      ; EBX = 3
        add eax, ebx    ; EAX = EAX + EBX (8)
        push eax        ; Apila resultado
        sys 0x3         ; Escribe resultado
        stop            ; Fin
```

---

## Banderas de Condición (Condition Code)

Se actualizan con operaciones aritméticas y comparaciones:

- **Zero Flag (Z)** - 1 si resultado es 0
- **Positive Flag (P)** - 1 si resultado > 0
- **Negative Flag (N)** - 1 si resultado < 0

Se usan con saltos condicionales: `JZ`, `JP`, `JN`, `JNZ`, etc.

---

## Estructura de Memoria

```
+------------------+ <- 0x0000
|  Segmento de Código
|  (Programa compilado)
+------------------+
|  Datos estáticos
+------------------+
|  Pila (STACK)
|  (crece hacia abajo)
+------------------+
|  Heap
|  (memoria dinámica)
+------------------+ <- Final (según -m)
```

---

## Ejemplo de Uso Práctico

### Programa simple que suma dos números

**suma.asm:**
```asm
main:   mov eax, 15
        mov ebx, 7
        add eax, ebx    ; EAX = 22
        stop
```

**Compilar y ejecutar:**
```bash
vmx.exe suma.vmx -d
```

**Salida con disassembler:**
```
[0000] MOV EAX, 15
[0004] MOV EBX, 7
[0008] ADD EAX, EBX
[000C] STOP
```

---

## Información Técnica

- **Arquitectura**: x86-64 simulada (32 registros, 32 bits)
- **Memoria**: Configurable (default 16KB)
- **Instrucciones**: 32 operaciones disponibles
- **Segmentos**: Soporte para 8 segmentos de memoria
- **Formato binario**: Cabecera "VMX25" para ejecutables

---

## Resolución de Problemas

| Problema | Causa | Solución |
|----------|-------|----------|
| "Cabecera inválida" | Archivo .vmx corrupto o mal compilado | Recompila desde .asm |
| Parámetro no reconocido | Opción mal escrita | Usa `-d`, `-m` o `-p` (minúsculas) |
| Stack overflow | Pila muy pequeña | Aumenta memoria con `-m` |
| Segmentation fault | Acceso a memoria inválida | Revisa direcciones en código |

---

## Carpetas Incluidas

- **`samples/`** - Ejemplos básicos compilados (.vmx)
- **`listas/`** - Programas con estructuras de datos (listas enlazadas)
- **`arboles/`** - Programas con árboles binarios
- **`.asm`** - Código fuente ensamblador
- **`.vmi`** - Archivos de datos/inicialización

---

**Versión**: MV2 2025 | **Lenguaje**: ASM Didáctico | **Plataforma**: Windows
