#ifndef __SymbolDeclarations__
#define __SymbolDeclarations__

#include "Signature.hpp"
#include "Statements.hpp"
#include "Program.hpp"

std::shared_ptr<const logic::Symbol> locationSymbolForStatement(const program::Statement* statement);

std::shared_ptr<const logic::Symbol> locationSymbolLeftBranch(const program::IfElse* ifElse);
std::shared_ptr<const logic::Symbol> locationSymbolRightBranch(const program::IfElse* ifElse);
std::shared_ptr<const logic::Symbol> locationSymbolEndLocation(const program::Function* function);

std::shared_ptr<const logic::Symbol> iteratorSymbol(const program::WhileStatement* whileStatement);
std::shared_ptr<const logic::Symbol> lastIterationSymbol(const program::WhileStatement* whileStatement, bool twoTraces);

/*
 * The parser needs to declare itself the symbols corresponding to variables, locations and last-loop-iterations,
 * since later parts of the parsing (i.e. the smtlib-formula-parsing) require those declarations to exist already.
 * program variables are declared on the fly during parsing (at least for now),
 * all other symbols are declared per function whenever the function-parsing is finished.
 */
void declareSymbolForProgramVar(const program::Variable* var);
void declareSymbolsForFunction(const program::Function* function, bool twoTraces);
// helper method for declareSymbolsForFunction
void declareSymbolsForStatements(const program::Statement* statement, bool twoTraces);


#endif
