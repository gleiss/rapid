#include "ProgramTraverser.hpp"

namespace analysis {
    
    void ProgramTraverser::generateFormulas(std::vector<std::shared_ptr<const logic::Formula>>& formulas)
    {
        for(const auto& function : program.functions)
        {
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsFunction;
            
            for (const auto& statement : function->statements)
            {
                visitStatement(statement.get(), formulas);
            }
        }
    }
    
    void ProgramTraverser::visitStatement(const program::Statement* statement, std::vector<std::shared_ptr<const logic::Formula>>& formulas)
    {
        switch (statement->type())
        {
            case program::Statement::Type::IntAssignment:
            {
                auto castedAssignment = static_cast<const program::IntAssignment*>(statement);
                // generate formulas
                generateFormulasFor(castedAssignment, formulas);
                break;
            }
                
            case program::Statement::Type::IfElse:
            {
                auto castedIfElse = static_cast<const program::IfElse*>(statement);
                
                // generate formulas
                generateFormulasFor(castedIfElse, formulas);
                
                // recurse on both branches
                for (const auto& statement : castedIfElse->ifStatements)
                {
                    visitStatement(statement.get(), formulas);
                }
                for (const auto& statement : castedIfElse->elseStatements)
                {
                    visitStatement(statement.get(), formulas);
                }
                break;
            }
                
            case program::Statement::Type::WhileStatement:
            {
                auto castedWhile = static_cast<const program::WhileStatement*>(statement);
                
                // generate formulas
                generateFormulasFor(castedWhile, formulas);
                
                // recurse on body
                for (const auto& statement : castedWhile->bodyStatements)
                {
                    visitStatement(statement.get(), formulas);
                }
                break;
            }
                
            case program::Statement::Type::SkipStatement:
            {
                auto castedSkip = static_cast<const program::SkipStatement*>(statement);
                // generate formulas
                generateFormulasFor(castedSkip, formulas);
                break;
            }
                
            default:
            {
                assert(false);
                break;
            }
        }
    }
    
    void ProgramTraverser::generateFormulasFor(const program::IntAssignment* statement, std::vector<std::shared_ptr<const logic::Formula>>& formulas){}
    void ProgramTraverser::generateFormulasFor(const program::IfElse* statement, std::vector<std::shared_ptr<const logic::Formula>>& formulas){}
    void ProgramTraverser::generateFormulasFor(const program::WhileStatement* statement, std::vector<std::shared_ptr<const logic::Formula>>& formulas){}
    void ProgramTraverser::generateFormulasFor(const program::SkipStatement* statement, std::vector<std::shared_ptr<const logic::Formula>>& formulas){}
    
}

