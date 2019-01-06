#include "SymbolDeclarations.hpp"

namespace analysis
{
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
}
