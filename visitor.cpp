// =============================================================================
// visitor.cpp — Implementación de TypeCheckerVisitor y GenCodeVisitor
// =============================================================================

#include "visitor.h"
#include "ast.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

// =============================================================================
// Despacho del patrón Visitor (accept en cada nodo del AST)
// =============================================================================

int BinaryExp::accept(Visitor *v) { return v->visit(this); }
int NumberExp::accept(Visitor *v) { return v->visit(this); }
int IdExp::accept(Visitor *v) { return v->visit(this); }
int IdExp::computeAddress(Visitor *v) { return v->computeAddress(this); }
int Program::accept(Visitor *v) { return v->visit(this); }
int PrintStm::accept(Visitor *v) { return v->visit(this); }
int AssignStm::accept(Visitor *v) { return v->visit(this); }
int ExpListSize::accept(Visitor *v) { return v->visit(this); }
int ExpListVals::accept(Visitor *v) { return v->visit(this); }
int IfStm::accept(Visitor *v) { return v->visit(this); }
int WhileStm::accept(Visitor *v) { return v->visit(this); }
int Body::accept(Visitor *v) { return v->visit(this); }
int VarDec::accept(Visitor *v) { return v->visit(this); }
int FcallExp::accept(Visitor *v) { return v->visit(this); }
int FunDec::accept(Visitor *v) { return v->visit(this); }
int ReturnStm::accept(Visitor *v) { return v->visit(this); }
int DoWhileStm::accept(Visitor *v) { return v->visit(this); }
int BreakStm::accept(Visitor *v) { return v->visit(this); }
int SwitchStm::accept(Visitor *v) { return v->visit(this); }
int UnaryExp::accept(Visitor *v) { return v->visit(this); }
int IndexExp::accept(Visitor *v) { return v->visit(this); }
int IndexExp ::computeAddress(Visitor *v) { return v->computeAddress(this); }
int StructDec::accept(Visitor *v) { return v->visit(this); }
int StructNewExp::accept(Visitor *v) { return v->visit(this); }
int FieldAccessExp::accept(Visitor *v) { return v->visit(this); }
int FieldAccessExp::computeAddress(Visitor *v) { return v->computeAddress(this); }
int ExpMatrix2D::accept(Visitor *v) { return v->visit(this); }
int Matrix2DIndex::accept(Visitor *v) { return v->visit(this); }
int Matrix2DIndex::computeAddress(Visitor *v) { return v->computeAddress(this); }
int AddrOfExp::accept(Visitor *v)          { return v->visit(this); }
int DerefExp::accept(Visitor *v)           { return v->visit(this); }
int DerefExp::computeAddress(Visitor *v)   { return v->computeAddress(this); }

// =============================================================================
// Implementación de accept()
// =============================================================================

// =============================================================================
// TypeCheckerVisitor — Análisis semántico
// =============================================================================
// Responsabilidades:
//   1. Registrar las funciones y su aridad antes de analizar los cuerpos.
//   2. Verificar que cada variable usada haya sido declarada en el scope.
//   3. Verificar que cada función llamada exista y reciba el número correcto
//      de argumentos.
//   4. Contar las variables locales de cada función para que GenCodeVisitor
//      pueda reservar el espacio correcto en el stack frame.
// =============================================================================

// -----------------------------------------------------------------------------
// TypeChecker — punto de entrada
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::TypeChecker(Program *program) {
  // Pasada 0: registrar definiciones de struct
  for (auto sd : program->sdlist) {
    sd->accept(this);
  }

  // Primera pasada: registrar todas las funciones y su aridad.
  // Esto permite llamadas hacia adelante (funciones que se usan antes de
  // ser definidas en el orden textual del fuente).
  for (auto fd : program->fdlist) {
    funAridad[fd->nombre] = static_cast<int>(fd->Pnombres.size());
  }

  // Segunda pasada: analizar cada función
  for (auto fd : program->fdlist) {
    fd->accept(this);
  }

  return 0;
}

// -----------------------------------------------------------------------------
// visit(FunDec) — analiza una declaración de función
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(FunDec *fd) {
  funcionActual = fd->nombre;
  locales = 0;
  int parametros = static_cast<int>(fd->Pnombres.size());

  // Abrir scope de la función
  entorno.add_level();

  // Registrar los parámetros como variables del scope actual
  for (int i = 0; i < (int)fd->Pnombres.size(); i++) {
    entorno.add_var(fd->Pnombres[i], 0);
    varTypes[fd->Pnombres[i]] = fd->Ptipos[i];
  }

  // Analizar el cuerpo
  fd->cuerpo->accept(this);

  // Cerrar scope
  entorno.remove_level();

  // Guardar el tamaño total del frame (parámetros + locales)
  funcontador[fd->nombre] = parametros + locales;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(Body) — analiza declaraciones y sentencias del cuerpo
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(Body *body) {
  entorno.add_level();

  for (auto dec : body->declarations)
    dec->accept(this);
  for (auto stm : body->StmList)
    stm->accept(this);

  entorno.remove_level();
  return 0;
}

// -----------------------------------------------------------------------------
// visit(VarDec) — registra las variables declaradas y cuenta los locales
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(VarDec *vd) {
  for (auto &nombre : vd->vars) {
    if (entorno.check(nombre)) {
      std::cerr << "[TypeChecker] Advertencia: la variable '" << nombre
                << "' ya fue declarada en este scope"
                << " (en función '" << funcionActual << "').\n";
    }
    entorno.add_var(nombre, 0);
    varTypes[nombre] = vd->type;
    locales++;
  }
  return 0;
}

// -----------------------------------------------------------------------------
// visit(IdExp) — verifica que la variable esté declarada
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(IdExp *exp) {
  if (!entorno.check(exp->value)) {
    throw std::runtime_error("[TypeChecker] Variable no declarada: '" +
                             exp->value + "' usada en la función '" +
                             funcionActual + "'");
  }
  return 0;
}

// -----------------------------------------------------------------------------
// (IdExp) — verifica que la variable esté declarada
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::computeAddress(IdExp *exp) { return 0; }

// -----------------------------------------------------------------------------
// visit(AssignStm) — verifica variable y evalúa expresión
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(AssignStm *stm) { return 0; }

// -----------------------------------------------------------------------------
// visit(ExpListSize) — verifica variable y evalúa expresión
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(ExpListSize *StmListSize) { return 0; }

// -----------------------------------------------------------------------------
// visit(ExpListVals) — verifica variable y evalúa expresión
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(ExpListVals *StmListVals) { return 0; }

// -----------------------------------------------------------------------------
// visit(FcallExp) — verifica existencia de la función y aridad
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(FcallExp *fcall) {
  // Verificar que la función exista
  if (funAridad.find(fcall->nombre) == funAridad.end()) {
    throw std::runtime_error("[TypeChecker] Función no definida: '" +
                             fcall->nombre + "' llamada en '" + funcionActual +
                             "'");
  }

  // Verificar número de argumentos
  int esperados = funAridad[fcall->nombre];
  int recibidos = static_cast<int>(fcall->argumentos.size());
  if (recibidos != esperados) {
    throw std::runtime_error("[TypeChecker] La función '" + fcall->nombre +
                             "' espera " + std::to_string(esperados) +
                             " argumento(s), pero se pasaron " +
                             std::to_string(recibidos));
  }

  // Verificar cada argumento
  for (auto arg : fcall->argumentos)
    arg->accept(this);

  return 0;
}

// -----------------------------------------------------------------------------
// visit(IfStm) — cuenta máximo de locales entre ramas
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(IfStm *stm) {
  stm->condition->accept(this);

  int base = locales;

  // Rama then
  locales = 0;
  stm->then->accept(this);
  int maxLocales = locales;

  // Rama else (opcional)
  if (stm->els) {
    locales = 0;
    stm->els->accept(this);
    maxLocales = std::max(maxLocales, locales);
  }

  // El frame necesita el máximo de ambas ramas
  locales = base + maxLocales;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(WhileStm)
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(WhileStm *stm) {
  stm->condition->accept(this);
  stm->b->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(PrintStm)
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(PrintStm *stm) {
  stm->e->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(ReturnStm)
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(ReturnStm *r) {
  r->e->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// Nodos que no requieren verificación semántica adicional
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(BinaryExp *exp) {
  exp->left->accept(this);
  exp->right->accept(this);
  return 0;
}

int TypeCheckerVisitor::visit(UnaryExp *exp) {
  exp->operand->accept(this);
  return 0;
}
int TypeCheckerVisitor::visit(IndexExp *exp) {
  // TODO
  return 0;
}

int TypeCheckerVisitor::computeAddress(IndexExp *exp) { return 0; }

int TypeCheckerVisitor::visit(NumberExp *exp) { return 0; }
int TypeCheckerVisitor::visit(Program *p) { return 0; }

int TypeCheckerVisitor::visit(DoWhileStm *stm) {
  stm->condition->accept(this);
  stm->b->accept(this);
  return 0;
}

int TypeCheckerVisitor::visit(BreakStm *stm) { return 0; }

int TypeCheckerVisitor::visit(SwitchStm *stm) {
  stm->e->accept(this);
  for (auto c : stm->cases) {
    for (auto s : c->body)
      s->accept(this);
  }
  for (auto s : stm->default_body)
    s->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(StructDec) — registra la definición del struct
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(StructDec *sd) {
  StructInfo info;
  for (auto &f : sd->fields) {
    info.fieldNames.push_back(f.name);
    info.fieldTypes.push_back(f.type);
  }
  structDefs[sd->name] = info;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(StructNewExp) — verifica que el struct esté definido
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(StructNewExp *sn) {
  if (structDefs.count(sn->structName) == 0) {
    throw std::runtime_error("[TypeChecker] Struct no definido: '" +
                             sn->structName + "'");
  }
  int nFields = structDefs[sn->structName].numFields();
  if (!sn->initValues.empty() && (int)sn->initValues.size() != nFields) {
    throw std::runtime_error(
        "[TypeChecker] 'new " + sn->structName + "' recibió " +
        std::to_string(sn->initValues.size()) + " valores pero el struct tiene " +
        std::to_string(nFields) + " campos");
  }
  for (Exp *v : sn->initValues)
    v->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(FieldAccessExp) — verifica que el campo exista en el struct
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(FieldAccessExp *fa) {
  if (varTypes.count(fa->obj) == 0) {
    throw std::runtime_error("[TypeChecker] Variable '" + fa->obj +
                             "' sin tipo registrado");
  }
  std::string structType = varTypes[fa->obj];
  if (structDefs.count(structType) == 0) {
    throw std::runtime_error("[TypeChecker] Tipo '" + structType +
                             "' no es un struct conocido");
  }
  if (structDefs[structType].fieldIndex(fa->field) < 0) {
    throw std::runtime_error("[TypeChecker] Campo '" + fa->field +
                             "' no existe en el struct '" + structType + "'");
  }
  return 0;
}

int TypeCheckerVisitor::computeAddress(FieldAccessExp *fa) { return 0; }

// -----------------------------------------------------------------------------
// visit(ExpMatrix2D) — visita rows y cols
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(ExpMatrix2D *m) {
  m->rows->accept(this);
  m->cols->accept(this);
  for (Exp *v : m->initValues)
    v->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(Matrix2DIndex) — verifica que la variable esté declarada
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(Matrix2DIndex *m) {
  if (!entorno.check(m->name))
    throw std::runtime_error("[TypeChecker] Variable no declarada: '" + m->name + "'");
  m->row->accept(this);
  m->col->accept(this);
  return 0;
}

int TypeCheckerVisitor::computeAddress(Matrix2DIndex *m) { return 0; }

// -----------------------------------------------------------------------------
// visit(AddrOfExp) — verifica que la variable esté declarada
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(AddrOfExp *a) {
  if (!entorno.check(a->var))
    throw std::runtime_error("[TypeChecker] Variable no declarada: '" + a->var +
                             "' usada en '" + funcionActual + "'");
  return 0;
}

// -----------------------------------------------------------------------------
// visit(DerefExp) — verifica el puntero interno
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(DerefExp *d) {
  d->ptr->accept(this);
  return 0;
}

int TypeCheckerVisitor::computeAddress(DerefExp *d) { return 0; }

// =============================================================================
// GenCodeVisitor — Generación de código ensamblador x86-64 (AT&T syntax)
// =============================================================================
// Convenciones usadas:
//   · Registros de argumentos: %rdi, %rsi, %rdx, %rcx, %r8, %r9
//   · Resultado de expresiones en %rax
//   · Variables locales: offsets negativos desde %rbp
//   · Variables globales: símbolos en .data con acceso RIP-relativo
//   · printf para print: formato en print_fmt ("%ld \n")
// =============================================================================

// -----------------------------------------------------------------------------
// generar — punto de entrada de la generación
// -----------------------------------------------------------------------------

int GenCodeVisitor::generar(Program *program) {
  tipos.TypeChecker(program);
  funcontador = tipos.funcontador;
  structDefs = tipos.structDefs;
  varTypes = tipos.varTypes;
  program->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(Program)
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(Program *program) {
  // Sección de datos
  out << ".data\n";
  out << "print_fmt: .string \"%ld \\n\"\n";

  // Registrar definiciones de struct
  for (auto sd : program->sdlist)
    sd->accept(this);

  // Recolectar nombres de variables globales
  for (auto dec : program->vdlist)
    dec->accept(this);

  // Emitir las etiquetas .quad para las globales
  for (auto &[var, _] : memoriaGlobal)
    out << var << ": .quad 0\n";

  // Sección de texto (código)
  out << "\n.text\n";

  for (auto fd : program->fdlist)
    fd->accept(this);

  // Marca el stack como no ejecutable (requerido por el linker moderno)
  out << "\n.section .note.GNU-stack,\"\",@progbits\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(VarDec) — registra variables en memoria (global o local)
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(VarDec *stm) {
  for (auto &var : stm->vars) {
    if (!entornoFuncion) {
      memoriaGlobal[var] = true;
    } else {
      memoria[var] = offset;
      offset -= 8;
    }
    varTypes[var] = stm->type;
  }
  return 0;
}

// -----------------------------------------------------------------------------
// visit(NumberExp) — carga un inmediato en %rax
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(NumberExp *exp) {
  out << "  movq $" << exp->value << ", %rax\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(ExpListSize) — evalúa la expresión para reservar el espacio apropiado y
// dejarlo en rax
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(ExpListSize *stm) {
  // TEST
  // Lista de Int: malloc de 8*n bytes
  stm->size->accept(this);
  out << "  movq $8, %rcx\n";
  out << "  imulq %rcx, %rax\n";
  out << "  movq %rax, %rdi\n"
      << "  call malloc@PLT\n";
  // El puntero se deja en rax
  return 0;
}

// -----------------------------------------------------------------------------
// visit(ExpListVals) — evalúa las expresiones expresión y las almacena en los
// espacios reservados de memoria y luego regresa el puntero
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(ExpListVals *stm) {
  // Lista con valores: malloc de n*8 bytes, guardar el puntero en la variable
  // destino y luego escribir cada elemento como var[i] = values[i].
  int n = (int)stm->values.size();
  out << "  movq $" << n << ", %rax\n";
  out << "  movq $8, %rcx\n";
  out << "  imulq %rcx, %rax\n";
  out << "  movq %rax, %rdi\n";
  out << "  call malloc@PLT\n";
  emitStoreToVar(assignTargetName);
  for (int i = 0; i < n; ++i) {
    stm->values[i]->accept(this); // valor → %rax
    out << "  pushq %rax\n";
    out << "  movq $" << i << ", %rax\n"; // índice
    out << "  movq %rax, %rdi\n";
    out << "  popq %rax\n";
    out << "  movq %rax, %rcx\n";
    emitLoadVar(assignTargetName);
    out << "  movq %rcx, (%rax, %rdi, 8)\n";
  }
  assignHandled = true;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(IdExp) — carga el valor de una variable en %rax
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(IdExp *exp) {
  if (memoriaGlobal.count(exp->value))
    out << "  movq " << exp->value << "(%rip), %rax\n";
  else
    out << "  movq " << memoria[exp->value] << "(%rbp), %rax\n";
  return 0;
}

// visit(UnaryExp) — operador unario NOT
int GenCodeVisitor::visit(UnaryExp *exp) {
  exp->operand->accept(this);
  int lbl = labelcont++;
  out << "  cmpq $0, %rax\n";
  out << "  je not_true_" << lbl << "\n";
  out << "  movq $0, %rax\n";
  out << "  jmp not_end_" << lbl << "\n";
  out << "not_true_" << lbl << ":\n";
  out << "  movq $1, %rax\n";
  out << "not_end_" << lbl << ":\n";
  return 0;
}

// emitLoadVar / emitStoreToVar — acceso uniforme a una variable (global/local)
void GenCodeVisitor::emitLoadVar(const std::string &name) {
  if (memoriaGlobal.count(name))
    out << "  movq " << name << "(%rip), %rax\n";
  else
    out << "  movq " << memoria[name] << "(%rbp), %rax\n";
}

void GenCodeVisitor::emitStoreToVar(const std::string &name) {
  if (memoriaGlobal.count(name))
    out << "  movq %rax, " << name << "(%rip)\n";
  else
    out << "  movq %rax, " << memoria[name] << "(%rbp)\n";
}

// visit(IndexExp) — lectura de elemento de lista: índice escalado (%rax,%rdi,8)
int GenCodeVisitor::visit(IndexExp *exp) {
  exp->index->accept(this);     // índice → %rax
  out << "  movq %rax, %rdi\n"; // índice → %rdi
  emitLoadVar(exp->name);       // puntero base → %rax
  out << "  movq (%rax, %rdi, 8), %rax\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(BinaryExp) — evalúa left y right, aplica operador
// Convención: left en %rax, right en %rcx
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(BinaryExp *exp) {
  exp->left->accept(this);
  out << "  pushq %rax\n";
  exp->right->accept(this);
  out << "  movq %rax, %rcx\n";
  out << "  popq %rax\n";

  switch (exp->op) {
  case PLUS_OP:
    out << "  addq %rcx, %rax\n";
    break;
  case MINUS_OP:
    out << "  subq %rcx, %rax\n";
    break;
  case MUL_OP:
    out << "  imulq %rcx, %rax\n";
    break;
  case DIV_OP:
    // División entera con signo: idivq usa %rdx:%rax / %rcx
    out << "  cqto\n";       // sign-extend %rax → %rdx:%rax
    out << "  idivq %rcx\n"; // cociente en %rax
    break;
  case LE_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  setl %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case GT_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  setg %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case LEQ_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  setle %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case GEQ_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  setge %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case EQ_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  sete %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case NE_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  setne %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case AND_OP:
    out << "  andq %rcx, %rax\n";
    break;
  case OR_OP:
    out << "  orq %rcx, %rax\n";
    break;
  }
  return 0;
}

// -----------------------------------------------------------------------------
// visit(AssignStm) — evalúa expresión y almacena resultado
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(AssignStm *stm) {
  // Exponer el nombre del destino si es una variable simple, para que los
  // inicializadores 'new ...{}' emitan el patrón guardar-puntero + escribir.
  assignTargetName = stm->target->lvalueName();
  assignHandled = false;
  stm->e->accept(this);
  // Si la expresión derecha no resolvió ya la asignación completa, almacenar.
  if (!assignHandled)
    stm->target->computeAddress(this);
  assignTargetName.clear();
  assignHandled = false;
  return 0;
}

// -----------------------------------------------------------------------------
// computeAdress(IdExp) — Asigna el rax hacia la posicion correcta de memoria
// -----------------------------------------------------------------------------

int GenCodeVisitor::computeAddress(IdExp *id) {
  if (memoriaGlobal.count(id->value)) {
    out << "  movq %rax, " << id->value << "(%rip)\n";
  } else {
    int off = memoria[id->value];
    out << "  movq %rax, " << off << "(%rbp)\n";
  }
  return 0;
}

// -----------------------------------------------------------------------------
// computeAdress(IndexExp) — Asigna el rax hacia la posicion correcta de memoria
// -----------------------------------------------------------------------------

int GenCodeVisitor::computeAddress(IndexExp *idx) {
  // Not perfect as it usses a third register rdi
  // OPTIMIZE
  out << "  pushq %rax\n";

  idx->index->accept(this);

  out << "  movq %rax, %rdi\n"  // Saves the index in rdi
      << "  popq %rax\n"        // pops the index to have the value to assign
      << "  movq %rax, %rcx\n"; // Assigns the exp to rcx
  if (memoriaGlobal.count(idx->name)) {
    out << "  movq " << idx->name
        << "(%rip), %rax\n"; // Assigns the array in memory to the index
  } else {
    int off = memoria[idx->name];
    out << "  movq " << off
        << "(%rbp), %rax\n"; // Assigns the array in memory to the index
  }
  out << "  movq %rcx, (%rax, %rdi, 8)\n"; // asssigns the rcx wo the  array
                                           // in the correct offset

  return 0;
}

// -----------------------------------------------------------------------------
// visit(PrintStm) — imprime un entero con printf
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(PrintStm *stm) {
  stm->e->accept(this);
  out << "  movq %rax, %rsi\n";
  out << "  leaq print_fmt(%rip), %rdi\n";
  out << "  movq $0, %rax\n";
  out << "  call printf@PLT\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(Body) — procesa declaraciones y sentencias
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(Body *b) {
  for (auto dec : b->declarations)
    dec->accept(this);
  for (auto stm : b->StmList)
    stm->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(IfStm) — emite bloque if-then-else con etiquetas únicas
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(IfStm *stm) {
  int lbl = labelcont++;
  stm->condition->accept(this);
  out << "  cmpq $0, %rax\n";
  out << "  je else_" << lbl << "\n";
  stm->then->accept(this);
  out << "  jmp endif_" << lbl << "\n";
  out << "else_" << lbl << ":\n";
  if (stm->els)
    stm->els->accept(this);
  out << "endif_" << lbl << ":\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(WhileStm) — emite bucle while con etiquetas únicas
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(WhileStm *stm) {

  int lbl = labelcont++;

  std::string oldBreak = currentBreakLabel;
  currentBreakLabel = "endwhile_" + std::to_string(lbl);

  out << "while_" << lbl << ":\n";

  stm->condition->accept(this);

  out << "  cmpq $0, %rax\n";
  out << "  je endwhile_" << lbl << "\n";

  stm->b->accept(this);

  out << "  jmp while_" << lbl << "\n";

  out << "endwhile_" << lbl << ":\n";

  currentBreakLabel = oldBreak;

  return 0;
}

// -----------------------------------------------------------------------------
// visit(ReturnStm) — salta al epílogo de la función
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(ReturnStm *stm) {
  stm->e->accept(this);
  out << "  jmp .end_" << nombreFuncion << "\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(FunDec) — emite prólogo, cuerpo y epílogo de una función
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(FunDec *f) {
  entornoFuncion = true;
  memoria.clear();
  offset = -8;
  nombreFuncion = f->nombre;

  const std::vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx",
                                            "%rcx", "%r8",  "%r9"};

  // ---- Prólogo ----
  out << "\n.globl " << f->nombre << "\n";
  out << f->nombre << ":\n";
  out << "  pushq %rbp\n";
  out << "  movq %rsp, %rbp\n";
  out << "  subq $" << funcontador[f->nombre] * 8 << ", %rsp\n";

  // Guardar parámetros en el frame local
  int nParams = static_cast<int>(f->Pnombres.size());
  for (int i = 0; i < nParams; i++) {
    memoria[f->Pnombres[i]] = offset;
    varTypes[f->Pnombres[i]] = f->Ptipos[i];
    out << "  movq " << argRegs[i] << ", " << offset << "(%rbp)\n";
    offset -= 8;
  }

  // Registrar variables locales declaradas (ajusta 'offset' y 'memoria')
  for (auto dec : f->cuerpo->declarations)
    dec->accept(this);

  // ---- Cuerpo ----
  for (auto stm : f->cuerpo->StmList)
    stm->accept(this);

  // ---- Epílogo ----
  out << ".end_" << f->nombre << ":\n";
  out << "  leave\n";
  out << "  ret\n";

  entornoFuncion = false;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(FcallExp) — emite una llamada a función
// Argumentos en registros según la ABI System V x86-64
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(FcallExp *exp) {
  const std::vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx",
                                            "%rcx", "%r8",  "%r9"};

  int nArgs = static_cast<int>(exp->argumentos.size());
  for (int i = 0; i < nArgs; i++) {
    exp->argumentos[i]->accept(this);
    out << "  movq %rax, " << argRegs[i] << "\n";
  }
  out << "  call " << exp->nombre << "\n";
  return 0;
}

int GenCodeVisitor::visit(DoWhileStm *stm) {

  int lbl = labelcont++;

  std::string oldBreak = currentBreakLabel;
  currentBreakLabel = "endwhile_" + std::to_string(lbl);

  out << "dowhile_" << lbl << ":\n";

  stm->b->accept(this);

  stm->condition->accept(this);

  out << "  cmpq $0, %rax\n";
  out << "  jne dowhile_" << lbl << "\n";

  out << "endwhile_" << lbl << ":\n";

  currentBreakLabel = oldBreak;

  return 0;
}

int GenCodeVisitor::visit(BreakStm *stm) {

  if (currentBreakLabel.empty()) {
    std::cerr << "Error: break fuera de while, do-while o switch\n";
    exit(1);
  }

  out << "  jmp " << currentBreakLabel << "\n";

  return 0;
}

int GenCodeVisitor::visit(SwitchStm *stm) {

  int lbl = labelcont++;

  stm->e->accept(this);

  out << "  movq %rax, %r10\n";

  for (auto c : stm->cases) {

    out << "  movq $" << c->value << ", %rax\n";
    out << "  cmpq %rax, %r10\n";
    out << "  je case_" << lbl << "_" << c->value << "\n";
  }

  if (!stm->default_body.empty())
    out << "  jmp default_" << lbl << "\n";
  else
    out << "  jmp endswitch_" << lbl << "\n";

  std::string oldBreak = currentBreakLabel;
  currentBreakLabel = "endswitch_" + std::to_string(lbl);

  for (auto c : stm->cases) {

    out << "case_" << lbl << "_" << c->value << ":\n";

    for (auto s : c->body)
      s->accept(this);

    out << "  jmp endswitch_" << lbl << "\n";
  }

  if (!stm->default_body.empty()) {

    out << "default_" << lbl << ":\n";

    for (auto s : stm->default_body)
      s->accept(this);
  }

  currentBreakLabel = oldBreak;

  out << "endswitch_" << lbl << ":\n";

  return 0;
}

// -----------------------------------------------------------------------------
// visit(StructDec) — registra la definición del struct (sin emitir código)
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(StructDec *sd) {
  StructInfo info;
  for (auto &f : sd->fields) {
    info.fieldNames.push_back(f.name);
    info.fieldTypes.push_back(f.type);
  }
  structDefs[sd->name] = info;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(StructNewExp) — aloca un struct en el heap via malloc
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(StructNewExp *sn) {
  int sz = structDefs[sn->structName].numFields() * 8;
  out << "  movq $" << sz << ", %rdi\n";
  out << "  call malloc@PLT\n";
  if (sn->initValues.empty())
    return 0; // puntero en %rax → lo almacena AssignStm
  // Con inicializadores: guardar el puntero y escribir cada campo (índice = i)
  emitStoreToVar(assignTargetName);
  for (size_t i = 0; i < sn->initValues.size(); ++i) {
    sn->initValues[i]->accept(this); // valor → %rax
    out << "  pushq %rax\n";
    out << "  movq $" << i << ", %rax\n"; // índice de campo
    out << "  movq %rax, %rdi\n";
    out << "  popq %rax\n";
    out << "  movq %rax, %rcx\n";
    emitLoadVar(assignTargetName);
    out << "  movq %rcx, (%rax, %rdi, 8)\n";
  }
  assignHandled = true;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(FieldAccessExp) — lee un campo del struct
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(FieldAccessExp *fa) {
  // Acceso a campo = indexación escalada con el índice del campo (struct ≡ lista)
  int idx = structDefs[varTypes[fa->obj]].fieldIndex(fa->field);
  out << "  movq $" << idx << ", %rdi\n"; // índice de campo → %rdi
  emitLoadVar(fa->obj);                   // puntero al struct → %rax
  out << "  movq (%rax, %rdi, 8), %rax\n";
  return 0;
}

// -----------------------------------------------------------------------------
// computeAddress(FieldAccessExp) — escribe un valor en un campo del struct
// %rax contiene el valor a asignar (viene de AssignStm)
// -----------------------------------------------------------------------------

int GenCodeVisitor::computeAddress(FieldAccessExp *fa) {
  // Escritura de campo = escritura escalada con el índice del campo.
  // El valor a asignar llega en %rax (desde AssignStm).
  int idx = structDefs[varTypes[fa->obj]].fieldIndex(fa->field);
  out << "  pushq %rax\n";               // valor a asignar
  out << "  movq $" << idx << ", %rax\n"; // índice de campo
  out << "  movq %rax, %rdi\n";
  out << "  popq %rax\n";
  out << "  movq %rax, %rcx\n";          // valor → %rcx
  emitLoadVar(fa->obj);                  // puntero al struct → %rax
  out << "  movq %rcx, (%rax, %rdi, 8)\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(ExpMatrix2D) — aloca (rows*cols+2)*8 bytes, guarda rows y cols en cabecera
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(ExpMatrix2D *m) {
  int cols = m->cols->constValue();
  // Registrar columnas (constante) de la variable destino para indexar luego.
  if (!assignTargetName.empty())
    matrixCols[assignTargetName] = cols;
  // malloc de rows*cols*8 bytes — arreglo plano, sin cabecera.
  m->cols->accept(this); // cols → %rax (segunda dimensión, evaluada primero)
  out << "  pushq %rax\n";
  m->rows->accept(this); // rows → %rax
  out << "  popq %rcx\n"; // cols → %rcx
  out << "  imulq %rcx, %rax\n";
  out << "  salq $3, %rax\n"; // *8
  out << "  movq %rax, %rdi\n";
  out << "  call malloc@PLT\n";
  if (m->initValues.empty())
    return 0; // puntero en %rax → lo almacena AssignStm
  // Con inicializadores: guardar el puntero e inicializar en orden row-major
  // como m[row][col] = valor, con índice lineal row*cols + col.
  emitStoreToVar(assignTargetName);
  for (size_t k = 0; k < m->initValues.size(); ++k) {
    int row = (int)k / cols;
    int col = (int)k % cols;
    m->initValues[k]->accept(this); // valor → %rax
    out << "  pushq %rax\n";
    out << "  movq $" << row << ", %rax\n";
    out << "  pushq %rax\n";
    out << "  movq $" << col << ", %rax\n";
    out << "  movq %rax, %rdi\n";
    out << "  popq %rax\n";
    out << "  movq $" << cols << ", %rcx\n";
    out << "  imulq %rcx, %rax\n";
    out << "  addq %rdi, %rax\n";
    out << "  movq %rax, %rdi\n";
    out << "  popq %rcx\n";
    emitLoadVar(assignTargetName);
    out << "  movq %rcx, (%rax, %rdi, 8)\n";
  }
  assignHandled = true;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(Matrix2DIndex) — lee element at [row*cols + col + 2]
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(Matrix2DIndex *m) {
  // Lectura m[row][col] sobre arreglo plano: índice lineal row*cols + col,
  // con cols constante conocido de la declaración.
  int cols = matrixCols[m->name];
  m->row->accept(this); // row → %rax
  out << "  pushq %rax\n";
  m->col->accept(this); // col → %rax
  out << "  movq %rax, %rdi\n"; // col → %rdi
  out << "  popq %rax\n";       // row → %rax
  out << "  movq $" << cols << ", %rcx\n";
  out << "  imulq %rcx, %rax\n"; // row*cols
  out << "  addq %rdi, %rax\n";  // + col
  out << "  movq %rax, %rdi\n";  // índice lineal → %rdi
  emitLoadVar(m->name);          // puntero base → %rax
  out << "  movq (%rax, %rdi, 8), %rax\n";
  return 0;
}

// -----------------------------------------------------------------------------
// computeAddress(Matrix2DIndex) — stores value (in %rax) to element [row*cols+col+2]
// -----------------------------------------------------------------------------

int GenCodeVisitor::computeAddress(Matrix2DIndex *m) {
  // Escritura m[row][col] = valor (en %rax) sobre arreglo plano.
  int cols = matrixCols[m->name];
  out << "  pushq %rax\n";       // valor a almacenar
  m->row->accept(this);          // row → %rax
  out << "  pushq %rax\n";
  m->col->accept(this);          // col → %rax
  out << "  movq %rax, %rdi\n";  // col → %rdi
  out << "  popq %rax\n";        // row → %rax
  out << "  movq $" << cols << ", %rcx\n";
  out << "  imulq %rcx, %rax\n"; // row*cols
  out << "  addq %rdi, %rax\n";  // + col
  out << "  movq %rax, %rdi\n";  // índice lineal → %rdi
  out << "  popq %rcx\n";        // valor → %rcx
  emitLoadVar(m->name);          // puntero base → %rax
  out << "  movq %rcx, (%rax, %rdi, 8)\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(AddrOfExp) — &var → load address of variable into %rax
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(AddrOfExp *a) {
  if (memoriaGlobal.count(a->var))
    out << "  leaq " << a->var << "(%rip), %rax\n";
  else
    out << "  leaq " << memoria[a->var] << "(%rbp), %rax\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(DerefExp) — *ptr → load 64-bit value at address held in ptr
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(DerefExp *d) {
  d->ptr->accept(this);             // %rax = pointer value (an address)
  out << "  movq (%rax), %rax\n";  // load value at *ptr
  return 0;
}

// -----------------------------------------------------------------------------
// computeAddress(DerefExp) — *ptr = value → store value at address held in ptr
// %rax contains the value to store (from AssignStm)
// -----------------------------------------------------------------------------

int GenCodeVisitor::computeAddress(DerefExp *d) {
  out << "  pushq %rax\n";          // save value to store
  d->ptr->accept(this);             // %rax = pointer value (target address)
  out << "  movq %rax, %rcx\n";    // %rcx = target address
  out << "  popq %rax\n";           // restore value
  out << "  movq %rax, (%rcx)\n";  // store value at *ptr
  return 0;
}
