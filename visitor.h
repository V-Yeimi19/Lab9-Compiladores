#ifndef VISITOR_H
#define VISITOR_H

// =============================================================================
// visitor.h — Definición de los visitantes del AST
// =============================================================================
// Se implementan dos visitantes sobre el patrón Visitor:
//
//   TypeCheckerVisitor — Análisis semántico:
//     · Verifica que las variables usadas hayan sido declaradas.
//     · Verifica que las funciones llamadas existan.
//     · Verifica que las llamadas tengan el número correcto de argumentos.
//     · Cuenta las variables locales de cada función para la generación.
//
//   GenCodeVisitor — Generación de código x86-64 (AT&T syntax):
//     · Emite ensamblador para cada nodo del AST.
//     · Usa TypeCheckerVisitor para obtener metadatos (tamaño de frame, etc.).
// =============================================================================

#include "ast.h"
#include "environment.h"
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
class BinaryExp;
class Body;
class BreakStm;
class DoWhileStm;
class FunDec;
class FcallExp;
class IdExp;
class IndexExp;
class IfStm;
class NumberExp;
class PrintStm;
class Program;
class ReturnStm;
class Stm;
class SwitchStm;
class UnaryExp;
class VarDec;
class WhileStm;
class ExpListSize;
class ExpListVals;
class WhileStm;
class StructDec;
class StructNewExp;
class FieldAccessExp;
class ExpMatrix2D;
class Matrix2DIndex;
class AddrOfExp;
class DerefExp;

// =============================================================================
// StructInfo — información de un tipo struct (campos y sus índices)
// =============================================================================

struct StructInfo {
  std::vector<std::string> fieldNames;
  std::vector<std::string> fieldTypes;
  int numFields() const { return (int)fieldNames.size(); }
  int fieldIndex(const std::string &name) const {
    for (int i = 0; i < (int)fieldNames.size(); i++)
      if (fieldNames[i] == name)
        return i;
    return -1;
  }
};

// =============================================================================
// Clase base abstracta Visitor
// =============================================================================

class Visitor {
public:
  virtual int visit(BinaryExp *exp) = 0;
  virtual int visit(NumberExp *exp) = 0;
  virtual int visit(IdExp *exp) = 0;
  virtual int computeAddress(IdExp *exp) = 0;
  virtual int visit(UnaryExp *exp) = 0;
  virtual int visit(IndexExp *exp) = 0;
  virtual int computeAddress(IndexExp *exp) = 0;
  virtual int visit(Program *p) = 0;
  virtual int visit(PrintStm *stm) = 0;
  virtual int visit(WhileStm *stm) = 0;
  virtual int visit(DoWhileStm *stm) = 0;
  virtual int visit(IfStm *stm) = 0;
  virtual int visit(AssignStm *stm) = 0;
  virtual int visit(ExpListSize *stm) = 0;
  virtual int visit(ExpListVals *stm) = 0;
  virtual int visit(BreakStm *stm) = 0;
  virtual int visit(SwitchStm *stm) = 0;
  virtual int visit(Body *body) = 0;
  virtual int visit(VarDec *vd) = 0;
  virtual int visit(FcallExp *fc) = 0;
  virtual int visit(ReturnStm *r) = 0;
  virtual int visit(FunDec *fd) = 0;
  virtual int visit(StructDec *sd) = 0;
  virtual int visit(StructNewExp *sn) = 0;
  virtual int visit(FieldAccessExp *fa) = 0;
  virtual int computeAddress(FieldAccessExp *fa) = 0;
  virtual int visit(ExpMatrix2D *m) = 0;
  virtual int visit(Matrix2DIndex *m) = 0;
  virtual int computeAddress(Matrix2DIndex *m) = 0;
  virtual int visit(AddrOfExp *a) = 0;
  virtual int visit(DerefExp *d) = 0;
  virtual int computeAddress(DerefExp *d) = 0;
};

// =============================================================================
// TypeCheckerVisitor — Análisis semántico
// =============================================================================

class TypeCheckerVisitor : public Visitor {
public:
  // Mapa función → número total de variables en el frame (parámetros + locales)
  std::unordered_map<std::string, int> funcontador;

  // Mapa función → número de parámetros (para verificar aridad en llamadas)
  std::unordered_map<std::string, int> funAridad;

  // Contador de variables locales del cuerpo actual (acumulado por
  // visit(VarDec))
  int locales;

  // Entorno de variables: lleva el alcance (scope) actual
  Environment<int> entorno;

  // Nombre de la función que se está analizando (para mensajes de error)
  std::string funcionActual;

  // Mapa de tipos struct definidos
  std::unordered_map<std::string, StructInfo> structDefs;

  // Mapa variable → nombre de tipo (para acceso a campos de struct)
  std::unordered_map<std::string, std::string> varTypes;

  // Punto de entrada del análisis semántico
  int TypeChecker(Program *program);

  // ---- Visitas ----
  int visit(BinaryExp *exp) override;
  int visit(NumberExp *exp) override;
  int visit(IdExp *exp) override;
  int computeAddress(IdExp *exp) override;
  int visit(UnaryExp *exp) override;
  int visit(IndexExp *exp) override;
  int computeAddress(IndexExp *exp) override;
  int visit(Program *p) override;
  int visit(PrintStm *stm) override;
  int visit(AssignStm *stm) override;
  int visit(ExpListSize *stm) override;
  int visit(ExpListVals *stm) override;
  int visit(WhileStm *stm) override;
  int visit(DoWhileStm *stm) override;
  int visit(IfStm *stm) override;
  int visit(BreakStm *stm) override;
  int visit(SwitchStm *stm) override;
  int visit(Body *body) override;
  int visit(VarDec *vd) override;
  int visit(FcallExp *fc) override;
  int visit(ReturnStm *r) override;
  int visit(FunDec *fd) override;
  int visit(StructDec *sd) override;
  int visit(StructNewExp *sn) override;
  int visit(FieldAccessExp *fa) override;
  int computeAddress(FieldAccessExp *fa) override;
  int visit(ExpMatrix2D *m) override;
  int visit(Matrix2DIndex *m) override;
  int computeAddress(Matrix2DIndex *m) override;
  int visit(AddrOfExp *a) override;
  int visit(DerefExp *d) override;
  int computeAddress(DerefExp *d) override;
};

// =============================================================================
// GenCodeVisitor — Generación de código ensamblador x86-64
// =============================================================================

class GenCodeVisitor : public Visitor {
private:
  std::ostream &out; // Stream de salida (archivo .s)

public:
  TypeCheckerVisitor tipos; // Resultado del análisis semántico

  // Mapa función → tamaño del frame (obtenido del TypeChecker)
  std::unordered_map<std::string, int> funcontador;

  // Mapa variable local → offset desde %rbp
  std::unordered_map<std::string, int> memoria;

  // Conjunto de variables globales
  std::unordered_map<std::string, bool> memoriaGlobal;

  int offset = -8;               // Próximo offset disponible en el frame local
  int labelcont = 0;             // Contador de etiquetas únicas para if/while
  bool entornoFuncion = false;   // ¿Estamos dentro de una función?
  std::string currentBreakLabel; // Etiqueta a la que deben saltar los break
                                 // dentro del ciclo actual
  std::string nombreFuncion;     // Nombre de la función actual

  // Mapa de tipos struct definidos (copiado del TypeChecker)
  std::unordered_map<std::string, StructInfo> structDefs;

  // Mapa variable → nombre de tipo (para acceso a campos de struct)
  std::unordered_map<std::string, std::string> varTypes;

  // Número de columnas (constante) de cada variable matriz, para indexar
  // m[i][j] como i*cols + j sobre un arreglo plano.
  std::unordered_map<std::string, int> matrixCols;

  // Estado de la asignación en curso: nombre de la variable destino si es un
  // lvalue simple ("" si no), y bandera que indica que la expresión derecha ya
  // emitió la asignación completa (inicializadores 'new ...{}').
  std::string assignTargetName;
  bool assignHandled = false;

  GenCodeVisitor(std::ostream &out) : out(out) {}

  // Emite la carga del puntero/valor de una variable en %rax (global o local).
  void emitLoadVar(const std::string &name);
  // Emite el almacenamiento de %rax en una variable (global o local).
  void emitStoreToVar(const std::string &name);

  // Punto de entrada de la generación
  int generar(Program *program);

  // ---- Visitas ----
  int visit(BinaryExp *exp) override;
  int visit(NumberExp *exp) override;
  int visit(IdExp *exp) override;
  int computeAddress(IdExp *exp) override;
  int visit(UnaryExp *exp) override;
  int visit(IndexExp *exp) override;
  int computeAddress(IndexExp *exp) override;
  int visit(Program *p) override;
  int visit(PrintStm *stm) override;
  int visit(AssignStm *stm) override;
  int visit(ExpListSize *stm) override;
  int visit(ExpListVals *stm) override;
  int visit(WhileStm *stm) override;
  int visit(DoWhileStm *stm) override;
  int visit(IfStm *stm) override;
  int visit(BreakStm *stm) override;
  int visit(SwitchStm *stm) override;
  int visit(Body *body) override;
  int visit(VarDec *vd) override;
  int visit(FcallExp *fc) override;
  int visit(ReturnStm *r) override;
  int visit(FunDec *fd) override;
  int visit(StructDec *sd) override;
  int visit(StructNewExp *sn) override;
  int visit(FieldAccessExp *fa) override;
  int computeAddress(FieldAccessExp *fa) override;
  int visit(ExpMatrix2D *m) override;
  int visit(Matrix2DIndex *m) override;
  int computeAddress(Matrix2DIndex *m) override;
  int visit(AddrOfExp *a) override;
  int visit(DerefExp *d) override;
  int computeAddress(DerefExp *d) override;
};

#endif // VISITOR_H
