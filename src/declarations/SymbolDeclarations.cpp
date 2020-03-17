#include "SymbolDeclarations.hpp"

#include <memory>
#include <string>
#include <vector>

std::shared_ptr<const logic::Symbol> locationSymbol(std::string location, unsigned numberOfLoops)
{
    auto enclosingIteratorTypes = std::vector<const logic::Sort*>();
    for(int i=0; i < numberOfLoops; ++i)
    {
        enclosingIteratorTypes.push_back(logic::Sorts::natSort());
    }
    return logic::Signature::fetchOrAdd(location, enclosingIteratorTypes, logic::Sorts::timeSort());

}

std::shared_ptr<const logic::Symbol> locationSymbolForStatement(const program::Statement* statement)
{
    if (statement->type() == program::Statement::Type::WhileStatement)
    {
        return locationSymbol(statement->location, statement->enclosingLoops->size() + 1);
    }
    else
    {
        return locationSymbol(statement->location, statement->enclosingLoops->size());
    }
}

std::shared_ptr<const logic::Symbol> locationSymbolEndLocation(const program::Function* function)
{
    return locationSymbol(function->name + "_end", 0);
}

std::shared_ptr<const logic::Symbol> lastIterationSymbol(const program::WhileStatement* statement, unsigned numberOfTraces)
{
    std::vector<const logic::Sort*> argumentSorts;
    for (unsigned i=0; i < statement->enclosingLoops->size(); ++i)
    {
        argumentSorts.push_back(logic::Sorts::natSort());
    }
    if (numberOfTraces > 1)
    {
        argumentSorts.push_back(logic::Sorts::traceSort());
    }
    return logic::Signature::fetchOrAdd("n" + statement->location, argumentSorts, logic::Sorts::natSort());
}

std::shared_ptr<const logic::Symbol> iteratorSymbol(const program::WhileStatement* whileStatement)
{
    return logic::Signature::varSymbol("It" + whileStatement->location, logic::Sorts::natSort());
}

std::shared_ptr<const logic::Symbol> posVarSymbol()
{
    return logic::Signature::varSymbol("pos", logic::Sorts::intSort());
}

std::shared_ptr<const logic::Symbol> traceSymbol(unsigned traceNumber)
{
    std::string traceName = "t" + std::to_string(traceNumber);
    return logic::Signature::fetchOrAdd(traceName, {}, logic::Sorts::traceSort());
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
    if(var->numberOfTraces > 1)
    {
        argSorts.push_back(logic::Sorts::traceSort());
    }
    
    logic::Signature::add(var->name, argSorts, logic::Sorts::intSort());
}

void declareSymbolsForTraces(unsigned numberOfTraces)
{
    // declare trace symbols
    // note: we number traces starting from 1
    for (unsigned i = 1; i < numberOfTraces + 1; i++)
    {
        traceSymbol(i);
    }
}

// symbols get declared by constructing them once
void declareSymbolsForFunction(const program::Function* function, unsigned numberOfTraces)
{
    // recurse on statements
    for (const auto& statement : function->statements)
    {
        declareSymbolsForStatements(statement.get(), numberOfTraces);
    }
    
    // declare end-location of function
    locationSymbolEndLocation(function);
}

void declareSymbolsForStatements(const program::Statement* statement, unsigned numberOfTraces)
{
    // declare main location symbol
    locationSymbolForStatement(statement);
    
    if (statement->type() == program::Statement::Type::IfElse)
    {
        auto castedStatement = static_cast<const program::IfElse*>(statement);
        
        // recurse
        for (const auto& statementInBranch : castedStatement->ifStatements)
        {
            declareSymbolsForStatements(statementInBranch.get(), numberOfTraces);
        }
        for (const auto& statementInBranch : castedStatement->elseStatements)
        {
            declareSymbolsForStatements(statementInBranch.get(), numberOfTraces);
        }
    }
    else if (statement->type() == program::Statement::Type::WhileStatement)
    {
        auto castedStatement = static_cast<const program::WhileStatement*>(statement);
        
        // declare last iteration-symbol
        lastIterationSymbol(castedStatement, numberOfTraces);
        
        // recurse
        for (const auto& bodyStatement : castedStatement->bodyStatements)
        {
            declareSymbolsForStatements(bodyStatement.get(), numberOfTraces);
        }
    }
}


