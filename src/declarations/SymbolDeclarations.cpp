#include "SymbolDeclarations.hpp"

std::shared_ptr<const logic::Symbol> iteratorForLoop(const program::WhileStatement* whileStatement)
{
    return logic::Signature::varSymbol("It" + whileStatement->location, logic::Sorts::timeSort());
}

std::shared_ptr<const logic::Symbol> lastIterationForLoop(const program::WhileStatement* statement, bool twoTraces)
{
    std::vector<const logic::Sort*> argumentSorts;
    if (twoTraces)
    {
        argumentSorts.push_back(logic::Sorts::traceSort());
    }
    return logic::Signature::fetchOrAdd("n" + statement->location, argumentSorts, logic::Sorts::timeSort());
}

std::shared_ptr<const logic::Symbol> locationSymbolForStatement(const program::Statement* statement)
{
    auto enclosingIteratorTypes = std::vector<const logic::Sort*>();
    for (const auto& enclosingLoop : *statement->enclosingLoops)
    {
        auto enclosingIteratorSymbol = iteratorForLoop(enclosingLoop);
        enclosingIteratorTypes.push_back(enclosingIteratorSymbol->rngSort);
    }
    if (statement->type() == program::Statement::Type::WhileStatement)
    {
        enclosingIteratorTypes.push_back(logic::Sorts::timeSort());
    }
    return logic::Signature::fetchOrAdd(statement->location, enclosingIteratorTypes, logic::Sorts::timeSort());
}

void declareSymbolForProgramVar(const program::Variable* var)
{
    std::vector<const logic::Sort*> argSorts;
    if (!var->isConstant)
    {
        argSorts.push_back(logic::Sorts::timeSort());
    }
    if (var->isArray)
    {
        argSorts.push_back(logic::Sorts::intSort());
    }
    if(var->twoTraces)
    {
        argSorts.push_back(logic::Sorts::traceSort());
    }
    
    logic::Signature::add(var->name, argSorts, logic::Sorts::intSort());
}

