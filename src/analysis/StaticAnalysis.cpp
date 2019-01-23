#include "StaticAnalysis.hpp"

#include <cassert>

namespace analysis
{
    std::vector<std::shared_ptr<const logic::Formula>> StaticAnalysis::generateStaticAnalysisLemmas()
    {
        std::vector<std::shared_ptr<const logic::Formula>> lemmas;

        for(const auto& function : program.functions)
        {
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsFunction;
            
            for (const auto& statement : function->statements)
            {
                generateStaticAnalysisLemmas(statement.get(), lemmas);
            }
        }
        
        return lemmas;
    }
    
    void StaticAnalysis::generateStaticAnalysisLemmas(const program::Statement* statement,
                                                      std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            // recurse on both branches
            for (const auto& statement : castedStatement->ifStatements)
            {
                generateStaticAnalysisLemmas(statement.get(), lemmas);
            }
            for (const auto& statement : castedStatement->elseStatements)
            {
                generateStaticAnalysisLemmas(statement.get(), lemmas);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            // generate lemmas
            generateStaticAnalysisLemmas(castedStatement, lemmas);
            
            // recurse on body
            for (const auto& statement : castedStatement->bodyStatements)
            {
                generateStaticAnalysisLemmas(statement.get(), lemmas);
            }
        }
    }
    
    void StaticAnalysis::generateStaticAnalysisLemmas(const program::WhileStatement* whileStatement,
                                                      std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {
        auto activeVars = locationToActiveVars.at(whileStatement->location);
        auto assignedVars = computeAssignedVars(whileStatement);
        
        std::cout << "\nActive vars " << whileStatement->location << ": ";
        for (const auto& var : activeVars)
        {
            std::cout << var->name << ", ";
        }
        std::cout << "\nAssigned vars " << whileStatement->location << ": ";
        for (const auto& var : assignedVars)
        {
            std::cout << var->name << ", ";
        }
        std::cout << "\n";
        // for each active var, which is not constant but not assigned to in any statement of the loop,
        // add a lemma asserting that var is the same in each iteration as in the first iteration.
        for (const auto& activeVar : activeVars)
        {
            if (!activeVar->isConstant && assignedVars.count(activeVar) == 0)
            {
                std::cout << "need lemma for variable " << activeVar->name << " and loop " << whileStatement->location << "\n";
            }
        }
    }
    
    std::unordered_set<std::shared_ptr<const program::Variable>> StaticAnalysis::computeAssignedVars(const program::Statement* statement)
    {
        std::unordered_set<std::shared_ptr<const program::Variable>> assignedVars;
        
        switch (statement->type())
        {
            case program::Statement::Type::IntAssignment:
            {
                auto castedStatement = static_cast<const program::IntAssignment*>(statement);
                // add variable on lhs to assignedVars, independently from whether those vars are simple ones or arrays.
                if (castedStatement->lhs->type() == program::IntExpression::Type::IntVariableAccess)
                {
                    auto access = static_cast<const program::IntVariableAccess*>(castedStatement->lhs.get());
                    assignedVars.insert(access->var);
                }
                else
                {
                    assert(castedStatement->lhs->type() == program::IntExpression::Type::IntArrayApplication);
                    auto arrayAccess = static_cast<const program::IntArrayApplication*>(castedStatement->lhs.get());
                    assignedVars.insert(arrayAccess->array);
                }
                break;
            }
            case program::Statement::Type::IfElse:
            {
                auto castedStatement = static_cast<const program::IfElse*>(statement);
                // collect assignedVars from both branches
                for (const auto& statement : castedStatement->ifStatements)
                {
                    auto res = computeAssignedVars(statement.get());
                    assignedVars.insert(res.begin(), res.end());
                }
                for (const auto& statement : castedStatement->elseStatements)
                {
                    auto res = computeAssignedVars(statement.get());
                    assignedVars.insert(res.begin(), res.end());
                }
                break;
            }
            case program::Statement::Type::WhileStatement:
            {
                auto castedStatement = static_cast<const program::WhileStatement*>(statement);
                // collect assignedVars from body
                for (const auto& statement : castedStatement->bodyStatements)
                {
                    auto res = computeAssignedVars(statement.get());
                    assignedVars.insert(res.begin(), res.end());
                }
                break;
            }
            case program::Statement::Type::SkipStatement:
            {
                break;
            }
        }
        return assignedVars;
    }
}

