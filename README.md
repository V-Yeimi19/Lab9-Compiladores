# Compilador Fun — CS3402 Compiladores, UTEC 2026-1

Compilador para el lenguaje **Fun** (archivos `.fun`) que genera ensamblador x86-64 en sintaxis AT&T para Linux. El compilador está escrito en C++ e implementa todas las fases de manera manual: análisis léxico, análisis sintáctico, análisis semántico y generación de código.

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

## Casos de prueba (`inputs/`)

| Archivo | Qué prueba |
|---------|------------|
| `input1.txt` | Bucle `while` con acumulador y `break` condicional |
| `input2.txt` | Condicional `if-else` con operador `\|\|` |
| `input3.txt` | Bucle `while` con `break` al llegar a un valor |
| `input4.txt` | `switch` con `case`, `break` y `default` |
| `input5.txt` | `do-while` anidado con `switch` interno |
| `input6.txt` | Condicional `if-then` con expresión lógica compuesta (`&&` y `\|\|`) |
| `input7.txt` | `do-while` con `switch` y variable de control externa |
| `input8.txt` | Todos los operadores de comparación (`<`, `<=`, `>`, `>=`, `==`, `!=`) |
| `input9.txt` | `if` anidado con condición compuesta `&&` |
| `input10.txt` | `do-while` con `if` y `switch` combinados, condición con `\|\|` |
| `input11.txt` | Arreglo 1D con `new int[N]`, acceso por índice y expresiones aritméticas como valores |
| `input12.txt` | Arreglo 1D con `new int[N]` y asignación de elementos individuales |
| `input13.txt` | Arreglo 1D con `new int{v1, v2, v3}` (valores iniciales literales) |
| `input14.txt` | Arreglo 1D global con `new int{expr1, expr2}` (valores iniciales como expresiones) |
| `input15.txt` | Struct básico (`Point` con campos `x` e `y`): `new`, asignación de campos y lectura |
| `input16.txt` | Struct con función que recibe un struct como argumento y accede a sus campos |
| `input17.txt` | Matriz 2D con `new int[3][3]`, asignación y lectura de elementos |
| `input18.txt` | Matriz 2D llenada con bucles `while` anidados y lectura de posiciones específicas |
| `input19.txt` | Punteros básicos: `&var`, `*p` para lectura y `*p = expr` para escritura |
| `input20.txt` | Múltiples punteros: aritmética con valores desreferenciados y escritura a través de puntero |

---

## Estructura de archivos

```
.
├── main.cpp          # Punto de entrada del compilador
├── scanner.h / .cpp  # Analizador léxico
├── token.h / .cpp    # Definición y representación de tokens
├── parser.h / .cpp   # Parser de descenso recursivo
├── ast.h / .cpp      # Nodos del AST
├── visitor.h / .cpp  # TypeCheckerVisitor y GenCodeVisitor
├── environment.h     # Tabla de símbolos con alcance léxico
├── inputs/           # Archivos de entrada .fun y .s generados
├── outputs/          # Copias de los .s generados por run_all_inputs.py
├── specs/            # Especificaciones de implementación
└── run_all_inputs.py # Script para compilar todos los inputs
```
