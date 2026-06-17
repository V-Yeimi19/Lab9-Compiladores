# Lab9 - Compiladores
Compilador que genera ensamblador x86-64 en sintaxis AT&T para Linux. El compilador está escrito en C++ e implementa todas las fases de manera manual: análisis léxico, análisis sintáctico, análisis semántico y generación de código.

---

## Compilar y ejecutar

### 1. Compilar el compilador

```bash
g++ main.cpp scanner.cpp token.cpp parser.cpp ast.cpp visitor.cpp -o a.out
```

### 2. Compilar un archivo Fun

```bash
./a.out inputs/input1.txt
# Produce: inputs/input1.s
```

El compilador imprime en stdout el nombre del archivo generado y `Compilación exitosa.` si no hubo errores.

### 3. Ensamblar y ejecutar el ensamblador generado

```bash
gcc inputs/input1.s -o prog && ./prog
```

### 4. Ejecutar todos los inputs de prueba

```bash
python3 run_all_inputs.py
```

El script compila los inputs del 1 al 20 y mueve los archivos `.s` generados a la carpeta `outputs/`.

---

## Referencia del lenguaje Fun

### Tipos

| Tipo | Descripción |
|------|-------------|
| `int` | Entero de 64 bits |
| `list` | Arreglo dinámico en heap (puntero a memoria asignada con `malloc`) |
| `ptr` | Puntero crudo de 64 bits |
| `NombreStruct` | Variable de tipo struct (contiene un puntero al heap) |

### Variables

```fun
var int x, y       # una o más variables enteras
var list arr       # lista (puntero a arreglo en heap)
var ptr p          # puntero crudo
var Point pt       # variable de tipo struct
```

Las variables globales se declaran fuera de cualquier función, antes de las declaraciones de funciones.

### Funciones

```fun
fun int sumar(int a, int b)
    var int resultado
    resultado = a + b
    return(resultado)
endfun

fun int main()
    print(sumar(3, 4))
    return(0)
endfun
```

- El tipo de retorno se declara después de `fun`.
- Los parámetros se listan como `tipo nombre` separados por comas.
- Las variables locales se declaran con `var` al inicio del cuerpo.
- El cuerpo termina con `endfun`.
- El punto de entrada del programa es la función `main`.

### Sentencias

#### Asignación

```fun
x = expr
arr[i] = expr          # elemento de arreglo 1D
mat[i][j] = expr       # elemento de matriz 2D
obj.campo = expr       # campo de struct
*p = expr              # escritura a través de puntero
```

#### Impresión

```fun
print(expr)
```

Imprime el valor entero de `expr` seguido de un salto de línea, usando `printf` internamente.

#### Retorno

```fun
return(expr)
```

#### Condicional

```fun
if condicion then
    # cuerpo then
endif

if condicion then
    # cuerpo then
else
    # cuerpo else
endif
```

#### Bucle while

```fun
while condicion do
    # cuerpo
endwhile
```

#### Bucle do-while

```fun
do
    # cuerpo
dowhile condicion
enddo
```

#### Break

```fun
break
```

Sale del bucle (`while` o `do-while`) actual. Internamente genera un salto al label de fin del bucle.

#### Switch

```fun
switch expr
case 1
    print(100)
    break
case 2
    print(200)
    break
default
    print(999)
endswitch
```

El `switch` compara el valor de `expr` contra cada constante entera. El `break` dentro de un `case` evita caer al siguiente caso.

### Expresiones

#### Aritméticas

| Operador | Descripción |
|----------|-------------|
| `+` | Suma |
| `-` | Resta |
| `*` | Multiplicación |
| `/` | División entera |
| `**` | Potencia |

#### Comparación

| Operador | Descripción |
|----------|-------------|
| `<` | Menor que |
| `>` | Mayor que |
| `<=` | Menor o igual |
| `>=` | Mayor o igual |
| `==` | Igual |
| `!=` | Distinto |

#### Lógicas

| Operador | Descripción |
|----------|-------------|
| `&&` | AND lógico |
| `\|\|` | OR lógico |
| `!` | NOT lógico (unario) |

#### Llamada a función

```fun
f(arg1, arg2)
```

#### Acceso a arreglo 1D

```fun
arr[i]
```

#### Acceso a matriz 2D

```fun
mat[i][j]
```

#### Acceso a campo de struct

```fun
obj.campo
```

#### Operador dirección-de

```fun
&variable
```

Devuelve la dirección en memoria de `variable`.

#### Desreferencia de puntero

```fun
*p
```

Devuelve el valor almacenado en la dirección apuntada por `p`.

#### Asignación de memoria dinámica

```fun
new int[N]          # arreglo 1D de N enteros
new int{v1, v2, v3} # arreglo 1D con valores iniciales
new int[filas][cols] # matriz 2D de filas×cols enteros
new NombreStruct     # struct en heap
```

#### Literales

- Números enteros: `0`, `42`, `-1`
- `true` equivale a `1`
- `false` equivale a `0`

### Structs

```fun
struct Point
    var int x
    var int y
endstruct

fun int main()
    var Point p
    p = new Point
    p.x = 10
    p.y = 20
    print(p.x)
    print(p.y)
    return(0)
endfun
```

- Los structs se declaran antes de las variables globales y funciones.
- `new NombreStruct` asigna memoria en el heap (`numCampos * 8` bytes).
- Las variables de tipo struct contienen un puntero al bloque asignado.
- El acceso a campos usa notación punto: `obj.campo`.
- Al pasar un struct a una función, se pasa el puntero (8 bytes).

### Punteros

```fun
fun int main()
    var int x
    var ptr p
    x = 42
    p = &x
    print(*p)
    *p = 100
    print(x)
    return(0)
endfun
```

- `var ptr p` declara una variable que almacena una dirección de 64 bits.
- `&variable` obtiene la dirección de una variable local o global.
- `*p` lee el valor en la dirección almacenada en `p`.
- `*p = expr` escribe en la dirección almacenada en `p`.

### Matrices 2D

```fun
fun int main()
    var list mat
    var int i, j
    mat = new int[3][4]
    mat[0][0] = 1
    mat[2][3] = 99
    print(mat[0][0])
    print(mat[2][3])
    return(0)
endfun
```

- `new int[filas][cols]` asigna `(filas*cols + 2) * 8` bytes.
- Los primeros dos slots del bloque almacenan `filas` y `cols` (encabezado).
- Los elementos se acceden con `mat[i][j]` (fórmula: `elemento = ptr[2 + i*cols + j]`).

---

## Arquitectura del compilador

El compilador sigue un pipeline de cinco etapas:

```
Fuente (.fun) → Scanner → Parser → AST → TypeChecker → GenCodeVisitor → .s
```

### Etapa 1: Scanner (`scanner.h`, `scanner.cpp`, `token.h`, `token.cpp`)

Analizador léxico escrito a mano. Lee el texto fuente carácter a carácter y produce tokens uno a la vez mediante `Scanner::nextToken()`.

- Reconoce palabras clave: `fun`, `endfun`, `var`, `if`, `then`, `else`, `endif`, `while`, `do`, `endwhile`, `dowhile`, `enddo`, `switch`, `case`, `default`, `endswitch`, `return`, `print`, `break`, `new`, `true`, `false`, `struct`, `endstruct`.
- Reconoce operadores: `+`, `-`, `*`, `/`, `**`, `<`, `>`, `<=`, `>=`, `==`, `!=`, `&&`, `||`, `!`, `=`, `[`, `]`, `(`, `)`, `{`, `}`, `,`, `.`, `&`.
- Ignora espacios en blanco y comentarios con `#`.

### Etapa 2: Parser (`parser.h`, `parser.cpp`)

Parser de descenso recursivo. El método `Parser::parseProgram()` construye el AST completo (un `Program*`) a partir del stream de tokens.

- Gramática: `Program → StructDec* VarDec* FunDec*`
- Lanza `std::exception` ante errores de sintaxis.
- Implementa prioridad de operadores mediante la jerarquía `parseCE → parseE → parseT → parseF`.

### Etapa 3: AST (`ast.h`, `ast.cpp`)

Árbol de Sintaxis Abstracta. Dos jerarquías abstractas principales:

- **`Exp`** — expresiones: `NumberExp`, `IdExp`, `BinaryExp`, `UnaryExp`, `FcallExp`, `IndexExp`, `ExpListSize`, `ExpListVals`, `FieldAccessExp`, `StructNewExp`, `ExpMatrix2D`, `Matrix2DIndex`, `AddrOfExp`, `DerefExp`.
- **`Stm`** — sentencias: `AssignStm`, `PrintStm`, `ReturnStm`, `IfStm`, `WhileStm`, `DoWhileStm`, `BreakStm`, `SwitchStm`.
- Nodos estructurales: `VarDec`, `Body`, `FunDec`, `StructDec`, `Program`.

Todos los nodos implementan el patrón **Visitor** mediante el método `accept(Visitor*)`.

### Etapa 4: TypeCheckerVisitor (`visitor.h`, `visitor.cpp`)

Análisis semántico. Se ejecuta automáticamente desde `GenCodeVisitor::generar()` antes de emitir código.

- Verifica que las variables usadas estén declaradas (mediante `Environment<int>`).
- Verifica que las funciones llamadas existan y tengan la aridad correcta.
- Verifica que los campos de struct accedidos pertenezcan al tipo correspondiente.
- Calcula el tamaño del frame (`funcontador`) y la aridad (`funAridad`) de cada función, datos que usa el generador de código para calcular el prólogo/epílogo.
- Mantiene un mapa `structDefs` con los tipos struct y `varTypes` con el tipo de cada variable.

### Etapa 5: GenCodeVisitor (`visitor.h`, `visitor.cpp`)

Generación de código x86-64 AT&T. El método `GenCodeVisitor::generar(program)` llama primero al TypeChecker y luego recorre el AST emitiendo instrucciones al archivo `.s`.

- Todos los valores son de 64 bits (`movq`, `%rax`, etc.).
- Las variables locales se ubican en posiciones fijas relativas a `%rbp` (el offset comienza en `-8` y decrementa de 8 en 8 por cada variable).
- Los labels de control de flujo se numeran con `labelcont` para evitar colisiones.

**Tabla de símbolos auxiliar** (`environment.h`): `Environment<T>` implementa una tabla de símbolos con alcance léxico como una pila de `unordered_map`. Los scopes internos opacan a los externos.

---

## Convenciones de generación de código

### Marco de pila (stack frame)

Cada función genera el siguiente prólogo y epílogo estándar:

```asm
nombre_funcion:
    pushq %rbp
    movq  %rsp, %rbp
    subq  $N, %rsp          # N = número_de_variables * 8
    ...
    movq  %rbp, %rsp
    popq  %rbp
    ret
```

Las variables locales y parámetros se almacenan en posiciones `offset(%rbp)` donde `offset` es negativo y decrece de 8 en 8.

### Convención de llamada — System V AMD64

Los primeros seis argumentos enteros se pasan en registros, en orden:

```
%rdi  %rsi  %rdx  %rcx  %r8  %r9
```

El valor de retorno se devuelve en `%rax`. El compilador sigue esta convención para todas las llamadas a funciones Fun y para las llamadas a funciones de la biblioteca C (`printf`, `malloc`).

### Instrucción print

```asm
.data
print_fmt: .string "%ld \n"

# En el cuerpo de print(expr):
leaq print_fmt(%rip), %rdi
movq %rax, %rsi             # %rax contiene el valor a imprimir
call printf@PLT
```

### Asignación dinámica de memoria

- `new int[N]` llama a `malloc(N * 8)`.
- `new int{v1, v2, ...}` llama a `malloc(count * 8)` y almacena cada valor.
- `new NombreStruct` llama a `malloc(numCampos * 8)`.
- `new int[filas][cols]` llama a `malloc((filas*cols + 2) * 8)` y escribe el encabezado `{filas, cols}` en los primeros dos slots.

### Layout de la matriz 2D en memoria

```
ptr[0]           → filas   (offset 0)
ptr[1]           → cols    (offset 8)
ptr[2 + i*cols + j] → elemento (i, j)
```

El acceso `mat[i][j]` carga `cols` desde `ptr[1]`, calcula `i*cols + j + 2` y desreferencia esa posición.

### Marcado de stack no ejecutable

Todos los archivos `.s` generados incluyen al final:

```asm
.section .note.GNU-stack,"",@progbits
```

Esto le indica al enlazador que la pila no debe ser ejecutable.

### Break en bucles

`GenCodeVisitor` mantiene el campo `currentBreakLabel` con la etiqueta de fin del bucle actual. La sentencia `break` emite `jmp currentBreakLabel`.

---

## Detalles de implementación por feature

### Feature 1: Structs

#### Parsing

- **Token nuevo**: `STRUCT`, `ENDSTRUCT` (añadidos en `token.h`/`token.cpp` y reconocidos en `scanner.cpp`).
- **Token para acceso de campo**: `DOT` (`.`).
- **Nodos AST**: 
  - `StructDec` — declaración de un tipo struct con lista de campos.
  - `StructNewExp` — expresión `new NombreStruct` que asigna memoria en heap.
  - `FieldAccessExp` — acceso de lectura/escritura a campo (`obj.field`).
- **Modificaciones a `parser.cpp`**:
  - `parseProgram()` ahora maneja `StructDec*` antes de `VarDec*` y `FunDec*`.
  - `parseStructDec()` parsea la declaración de un struct: `struct ID (var tipo campo)+ endstruct`.
  - `parseStm()` detecta `obj.field = expr` (acceso a campo en asignación).
  - `parseF()` detecta `obj.field` en expresiones y `new StructName`.

#### Análisis semántico

- **TypeCheckerVisitor**:
  - `structDefs` — mapa `nombre_struct → StructInfo` (lista de nombres y tipos de campos).
  - `varTypes` — mapa `nombre_variable → tipo` (para resolver tipos de objetos struct).
  - `visit(StructDec*)` registra la definición del struct.
  - `visit(StructNewExp*)` verifica que el struct exista.
  - `visit(FieldAccessExp*)` verifica que la variable sea un struct conocido y que el campo exista.

#### Generación de código

- **`visit(StructNewExp*)`**: emite `movq $(numCampos * 8), %rdi; call malloc@PLT`. El puntero se queda en `%rax`.
- **`visit(FieldAccessExp*)`**: 
  - Carga el puntero al struct (desde memoria local/global).
  - Carga el valor del campo en el offset `(fieldIndex * 8)` del bloque.
- **`computeAddress(FieldAccessExp*)`**: 
  - Guarda el valor a escribir.
  - Carga el puntero al struct.
  - Escribe el valor en la dirección `struct_ptr + (fieldIndex * 8)`.

#### Layout en memoria

Cada struct ocupa `numCampos * 8` bytes en el heap. Los campos se organizan secuencialmente:

```
struct Point {int x; int y;}  →  8 bytes (x) + 8 bytes (y) = 16 bytes totales
offset 0: x
offset 8: y
```

---

### Feature 2: Matrices 2D

#### Parsing

- **Nodos AST**:
  - `ExpMatrix2D` — expresión `new int[rows][cols]`.
  - `Matrix2DIndex` — acceso `mat[i][j]` (lectura/escritura).
- **Modificaciones a `parser.cpp`**:
  - En `parseF()`, tras `new int[dimA]`, se verifica si hay un segundo `[dimB]`. Si existe, se crea `ExpMatrix2D`; si no, `ExpListSize` (1D).
  - En `parseStm()` y `parseF()`, el acceso `m[i][j]` se detecta tras `m[i]` cuando aparece un segundo `[`.

#### Análisis semántico

- **TypeCheckerVisitor**:
  - `visit(ExpMatrix2D*)` verifica ambas expresiones de dimensión.
  - `visit(Matrix2DIndex*)` verifica que la variable sea un arreglo y que los índices sean válidos.

#### Generación de código

- **`visit(ExpMatrix2D*)`**:
  ```
  1. Evaluar rows → %rax, push
  2. Evaluar cols → %rax, push
  3. Calcular (rows*cols + 2) * 8 bytes
  4. malloc()
  5. Escribir rows en ptr[0]
  6. Escribir cols en ptr[1]
  ```
  El valor de retorno (puntero) se queda en `%rax`.

- **`visit(Matrix2DIndex*)`** (lectura):
  ```
  1. Cargar puntero base en %rbx
  2. Cargar cols desde ptr[1] en %r10
  3. Evaluar row → %rax
  4. Calcular offset = row * cols + col + 2
  5. Saltar 3 (offset*8) bytes
  6. Desreferenciar: movq (%rbx + offset*8), %rax
  ```

- **`computeAddress(Matrix2DIndex*)`** (escritura):
  ```
  1. Guardar valor a escribir
  2. Cargar puntero base y cols (igual a arriba)
  3. Calcular la dirección de destino
  4. Escribir el valor
  ```

#### Layout en memoria

```
new int[3][4]  →  (3*4 + 2) * 8 = 160 bytes

ptr[0]      = 3          (rows)
ptr[1]      = 4          (cols)
ptr[2]      = mat[0][0]
ptr[3]      = mat[0][1]
ptr[4]      = mat[0][2]
ptr[5]      = mat[0][3]
ptr[6]      = mat[1][0]
...
ptr[15]     = mat[2][3]
```

Fórmula de acceso: `elemento(i,j) = ptr[2 + i*cols + j]`

---

### Feature 3: Punteros

#### Parsing

- **Token nuevo**: `ADDR` (`&`, cuando no va seguido de otro `&` que sería `&&`).
- **Nodos AST**:
  - `AddrOfExp` — expresión `&variable` (operador unario de dirección).
  - `DerefExp` — expresión `*ptr` (operador unario de desreferencia).
- **Modificaciones a `parser.cpp`**:
  - En `parseF()`, el `match(Token::ADDR)` crea `AddrOfExp`.
  - En `parseF()`, el `check(Token::MUL)` seguido de recursión crea `DerefExp` (porque en este contexto `*` no puede ser multiplicación).
  - En `parseStm()`, se añade un caso `*expr = rhs` para asignaciones a través de puntero.
  - En `parseBody()`, `isStmStart` incluye `Token::MUL` para reconocer `*ptr = ...` como inicio de sentencia.

#### Análisis semántico

- **TypeCheckerVisitor**:
  - `visit(AddrOfExp*)` verifica que la variable exista.
  - `visit(DerefExp*)` recorre la expresión del puntero.
  - `computeAddress(DerefExp*)` es un no-op (el cálculo de la dirección ocurre en el generador).

#### Generación de código

- **`visit(AddrOfExp*)`**: emite `leaq var(%rip)` o `leaq var(%rbp)` según sea global o local. El resultado está en `%rax`.
- **`visit(DerefExp*)`** (lectura):
  ```
  1. Evaluar la expresión del puntero → %rax (contiene una dirección)
  2. movq (%rax), %rax  (desreferenciar)
  ```
- **`computeAddress(DerefExp*)`** (escritura):
  ```
  1. Guardar valor a escribir (pushq %rax)
  2. Evaluar puntero → %rax
  3. Mover dirección a %rcx (movq %rax, %rcx)
  4. Restaurar valor (popq %rax)
  5. Escribir (movq %rax, (%rcx))
  ```

#### Validación

- **Variable `var ptr p`**: se trata como una variable normal de 64 bits. No requiere token especial; `ptr` es un ID común que se usa como tipo.
- **Direcciones de variables locales**: se obtienen con `leaq offset(%rbp)`.
- **Direcciones de variables globales**: se obtienen con `leaq var(%rip)` (RIP-relative addressing).
- **Escritura indirecta**: permite modificar variables locales indirectamente a través de punteros.

---
