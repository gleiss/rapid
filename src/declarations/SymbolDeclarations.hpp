#ifndef __SymbolDeclarations__
#define __SymbolDeclarations__

#include "Signature.hpp"
#include "Statements.hpp"

std::shared_ptr<const logic::Symbol> iteratorForLoop(const program::WhileStatement* whileStatement);
std::shared_ptr<const logic::Symbol> lastIterationForLoop(const program::WhileStatement* whileStatement, bool twoTraces);

std::shared_ptr<const logic::Symbol> locationSymbolForStatement(const program::Statement* statement);

void declareSymbolForProgramVar(const program::Variable* var);

#endif
