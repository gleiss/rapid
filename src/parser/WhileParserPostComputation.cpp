#include "WhileParserPostComputation.hpp"

#include "Theory.hpp"

using namespace program;

namespace parser
{
# pragma mark - compute

    
# pragma mark enclosingIteratorsMap

    void WhileParserPostComputation::addEnclosingLoops(const program::Program& program)
    {
        for(const auto& function : program.functions)
        {
            for (const auto& statement : function->statements)
            {
                addEnclosingLoopsForStatement(statement.get(), {});
            }
        }
    }

    void WhileParserPostComputation::addEnclosingLoopsForStatement(const program::Statement* statement, std::vector<const program::WhileStatement*> enclosingLoops)
    {
        *statement->enclosingLoops = enclosingLoops;
        
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            for (const auto& statementInBranch : castedStatement->ifStatements)
            {
                addEnclosingLoopsForStatement(statementInBranch.get(), enclosingLoops);
            }
            for (const auto& statementInBranch : castedStatement->elseStatements)
            {
                addEnclosingLoopsForStatement(statementInBranch.get(), enclosingLoops);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);

            auto enclosingLoopsCopy = enclosingLoops;
            enclosingLoopsCopy.push_back(castedStatement);
            for (const auto& bodyStatement : castedStatement->bodyStatements)
            {
                addEnclosingLoopsForStatement(bodyStatement.get(), enclosingLoopsCopy);
            }
        }
    }
    





}

