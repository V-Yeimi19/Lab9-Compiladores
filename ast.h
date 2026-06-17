#ifndef AST_H
#define AST_H

// =============================================================================
// ast.h — Definición del Árbol de Sintaxis Abstracta (AST)
// =============================================================================
// Jerarquía de nodos:
//
//   Exp (abstracta)
//     ├── NumberExp   — literal entero
//     ├── IdExp       — variable
//     ├── BinaryExp   — operación binaria (+, -, *, /, **, <)
//     ├── ListSize    — lista inicializada con un tamanio
//     ├── ListVals    — lista inicializada con valores iniciales
//     └── FcallExp    — llamada a función
//
//   Stm (abstracta)
//     ├── AssignStm   — asignación: ID = Exp
//     ├── PrintStm    — print(Exp)
//     ├── ReturnStm   — return(Exp)
//     ├── IfStm       — if CE then Body [else Body] endif
//     └── WhileStm    — while CE do Body endwhile
//
//   VarDec            — var tipo var1, var2, ...
//   Body              — VarDec* Stm*
//   FunDec            — fun tipo nombre(params) Body endfun
//   Program           — VarDec* FunDec*
// =============================================================================

#include <list>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

class Visitor;
class VarDec;
class Body;
class StructDec;
class StructNewExp;
class FieldAccessExp;
class ExpMatrix2D;
class Matrix2DIndex;
class AddrOfExp;
class DerefExp;

// =============================================================================
// Operadores binarios
// =============================================================================

enum BinaryOp {
  PLUS_OP,  // +
  MINUS_OP, // -
  MUL_OP,   // *
  DIV_OP,   // /
  POW_OP,   // **
  LE_OP,    // <
  GT_OP,    // >
  LEQ_OP,   // <=
  GEQ_OP,   // >=
  EQ_OP,    // ==
  NE_OP,    // !=
  AND_OP,   // &&
  OR_OP     // ||
};

// =============================================================================
// Expresiones
// =============================================================================

class Exp {
public:
  virtual int accept(Visitor *visitor) = 0;
  virtual int computeAddress(Visitor *visitor) {
    throw std::runtime_error(
        "Compute adress called on a non lvalue expression");
  };
  virtual ~Exp() = 0;
  static std::string binopToChar(BinaryOp op);
};

// ---- Expresión binaria ----
class BinaryExp : public Exp {
public:
  Exp *left;
  Exp *right;
  BinaryOp op;
  BinaryExp(Exp *l, Exp *r, BinaryOp op);
  int accept(Visitor *visitor) override;
  ~BinaryExp();
};

// ---- Expresión unaria (NOT) ----
class UnaryExp : public Exp {
public:
  Exp *operand;
  // Por ahora solo soportamos NOT, pero la estructura es extensible
  UnaryExp(Exp *operand);
  int accept(Visitor *visitor) override;
  ~UnaryExp();
};

// ---- Literal numérico entero ----
class NumberExp : public Exp {
public:
  int value;
  NumberExp(int v);
  int accept(Visitor *visitor) override;
  ~NumberExp();
};

// ---- Identificador (variable) ----
class IdExp : public Exp {
public:
  std::string value;
  IdExp(const std::string &v);
  int accept(Visitor *visitor) override;
  int computeAddress(Visitor *visitor) override;
  ~IdExp();
};

// Expresion de lista con tamano inicial:  new ID [CE] ----
class ExpListSize : public Exp {
public:
  Exp *size;
  std::string type;
  ExpListSize(std::string t, Exp *s);
  int accept(Visitor *visitor) override;
  ~ExpListSize();
};
// Expresion de list con valores iniciales : new ID {CE (, CE)*} ----
class ExpListVals : public Exp {
public:
  std::string type;
  std::vector<Exp *> values;
  ExpListVals(std::string t);
  int accept(Visitor *visitor) override;
  ~ExpListVals();
};

// ---- Llamada a función ----
class FcallExp : public Exp {
public:
  std::string nombre;
  std::vector<Exp *> argumentos;
  FcallExp() = default;
  int accept(Visitor *visitor) override;
  ~FcallExp() = default;
};

// ---- Acceso a campo de struct: obj.field ----
class FieldAccessExp : public Exp {
public:
  std::string obj;
  std::string field;
  FieldAccessExp(const std::string &obj, const std::string &field);
  int accept(Visitor *v) override;
  int computeAddress(Visitor *v) override;
  ~FieldAccessExp() = default;
};

// ---- Creación de struct en heap: new StructName ----
class StructNewExp : public Exp {
public:
  std::string structName;
  explicit StructNewExp(const std::string &n);
  int accept(Visitor *v) override;
  ~StructNewExp() = default;
};

// ---- Asignación y acceso a matriz 2D: new type[rows][cols], m[row][col] ----

// new type[rows][cols]  —  2D matrix heap allocation
class ExpMatrix2D : public Exp {
public:
  std::string type;
  Exp *rows;
  Exp *cols;
  ExpMatrix2D(const std::string &t, Exp *r, Exp *c);
  int accept(Visitor *v) override;
  ~ExpMatrix2D();
};

// m[row][col]  —  2D matrix element access
class Matrix2DIndex : public Exp {
public:
  std::string name;
  Exp *row;
  Exp *col;
  Matrix2DIndex(const std::string &n, Exp *r, Exp *c);
  int accept(Visitor *v) override;
  int computeAddress(Visitor *v) override;
  ~Matrix2DIndex();
};

// ---- Llamada indice de lista ----
class IndexExp : public Exp {
public:
  std::string name;
  Exp *index;
  IndexExp(const std::string &name, Exp *index);
  int accept(Visitor *v) override;
  int computeAddress(Visitor *visitor) override;
  ~IndexExp();
};

// &id  —  address-of operator: yields the address of a variable
class AddrOfExp : public Exp {
public:
  std::string var;
  explicit AddrOfExp(const std::string &v);
  int accept(Visitor *v) override;
  ~AddrOfExp() = default;
};

// *expr  —  pointer dereference (read value or write target)
class DerefExp : public Exp {
public:
  Exp *ptr;
  explicit DerefExp(Exp *p);
  int accept(Visitor *v) override;
  int computeAddress(Visitor *v) override;
  ~DerefExp();
};

// =============================================================================
// Sentencias
// =============================================================================

class Stm {
public:
  virtual int accept(Visitor *visitor) = 0;
  virtual ~Stm() = 0;
};

// ---- Asignación: ID = Exp ----
class AssignStm : public Stm {
public:
  Exp *target;
  Exp *e;
  AssignStm(Exp *target, Exp *expr);
  int accept(Visitor *visitor) override;
  ~AssignStm();
};

// ---- Impresión: print(Exp) ----
class PrintStm : public Stm {
public:
  Exp *e;
  PrintStm(Exp *e);
  int accept(Visitor *visitor) override;
  ~PrintStm();
};

// ---- Retorno: return(Exp) ----
class ReturnStm : public Stm {
public:
  Exp *e;
  ReturnStm() {}
  int accept(Visitor *visitor) override;
  ~ReturnStm() {}
};

// ---- Condicional: if CE then Body [else Body] endif ----
class IfStm : public Stm {
public:
  Exp *condition;
  Body *then;
  Body *els; // Puede ser nullptr si no hay rama else
  IfStm(Exp *condition, Body *then, Body *els);
  int accept(Visitor *visitor) override;
  ~IfStm() {}
};

// ---- Bucle: while CE do Body endwhile ----
class WhileStm : public Stm {
public:
  Exp *condition;
  Body *b;
  WhileStm(Exp *condition, Body *b);
  int accept(Visitor *visitor) override;
  ~WhileStm() {}
};

// ---- Bucle do-while: do Body while CE endwhile ----
class DoWhileStm : public Stm {
public:
  Body *b;
  Exp *condition;
  DoWhileStm(Body *b, Exp *condition);
  int accept(Visitor *visitor) override;
  ~DoWhileStm() {}
};

// ---- Break ----
class BreakStm : public Stm {
public:
  BreakStm() {}
  int accept(Visitor *visitor) override;
  ~BreakStm() {}
};

// ---- Caso en switch ----
class CaseStm {
public:
  int value;             // Valor del case (constante numérica)
  std::list<Stm *> body; // Cuerpo del case
  CaseStm(int value) : value(value) {}
  ~CaseStm() {}
};

// ---- Switch: switch Exp case NUM Body ... default Body? endswitch ----
class SwitchStm : public Stm {
public:
  Exp *e;                        // Expresión a evaluar
  std::list<CaseStm *> cases;    // Lista de casos
  std::list<Stm *> default_body; // Cuerpo del default (opcional)
  SwitchStm(Exp *e);
  int accept(Visitor *visitor) override;
  ~SwitchStm() {}
};

// =============================================================================
// Declaraciones y estructura del programa
// =============================================================================

// ---- Declaración de variables: var tipo var1, var2, ... ----
class VarDec {
public:
  std::string type;
  std::list<std::string> vars;
  VarDec();
  int accept(Visitor *visitor);
  ~VarDec();
};

// ---- Cuerpo: VarDec* Stm* ----
class Body {
public:
  std::list<VarDec *> declarations;
  std::list<Stm *> StmList;
  Body();
  int accept(Visitor *visitor);
  ~Body();
};

// ---- Declaración de función: fun tipo nombre(params) Body endfun ----
class FunDec {
public:
  std::string nombre;
  std::string tipo; // Tipo de retorno
  Body *cuerpo;
  std::vector<std::string> Ptipos;   // Tipos de los parámetros
  std::vector<std::string> Pnombres; // Nombres de los parámetros
  FunDec() = default;
  int accept(Visitor *visitor);
  ~FunDec() = default;
};

// ---- Campo de un struct ----
struct StructField {
  std::string type;
  std::string name;
};

// ---- Declaración de struct ----
class StructDec {
public:
  std::string name;
  std::vector<StructField> fields;
  StructDec() = default;
  int accept(Visitor *v);
  ~StructDec() = default;
};

// ---- Programa: StructDec* VarDec* FunDec* ----
class Program {
public:
  std::list<StructDec *> sdlist;
  std::list<VarDec *> vdlist;
  std::list<FunDec *> fdlist;
  Program() = default;
  int accept(Visitor *visitor);
  ~Program() = default;
};

#endif // AST_H
