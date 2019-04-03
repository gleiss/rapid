#include "StaticAnalysis.hpp"

#include <cassert>

#include "Theory.hpp"
#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

namespace analysis
{
    void StaticAnalysisLemmas::generateFormulasFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::Formula> > &formulas)
    {
        auto activeVars = locationToActiveVars.at(statement->location);
        auto assignedVars = computeAssignedVars(statement);

        auto iSymbol = iteratorSymbol(statement);
        auto it = iteratorTermForLoop(statement);
        auto locationSymbol = locationSymbolForStatement(statement);

        auto enclosingIteratorsSymbols = std::vector<std::shared_ptr<const logic::Symbol>>();
        for (const auto& enclosingLoop : *statement->enclosingLoops)
        {
            enclosingIteratorsSymbols.push_back(iteratorSymbol(enclosingLoop));
        }

        auto lStartIt = timepointForLoopStatement(statement, it);
        auto lStartZero = timepointForLoopStatement(statement, logic::Theory::natZero());
                
        // for each active var, which is not constant but not assigned to in any statement of the loop,
        // add a lemma asserting that var is the same in each iteration as in the first iteration.        
        for (const auto& activeVar : activeVars)
        {
            if (!activeVar->isConstant && assignedVars.count(activeVar) == 0)
            {
                if (!activeVar->isArray)
                {
                    // forall (it : Nat) (v(l(it)) = v(l(zero)))
                    auto vit = toTerm(activeVar,lStartIt);
                    auto vzero = toTerm(activeVar,lStartZero);
                    auto eq = logic::Formulas::equality(vit,vzero);
                    auto label = "Static analysis lemma for var " + activeVar->name + " at location " + statement->location;
                    auto bareLemma = logic::Formulas::universal({iSymbol},eq,label);
                    auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, bareLemma);
                    
                    if (twoTraces)
                    {
                        auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        formulas.push_back(logic::Formulas::universal({tr}, lemma));
                    }
                    else
                    {
                        formulas.push_back(lemma);
                    }
                }
                else
                {
                    auto pSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
                    auto p = logic::Terms::var(pSymbol);
                    
                    // forall (it : Nat, pos: Int) (v(l(it),pos) = v(l(zero), pos))
                    auto vit = toTerm(activeVar,lStartIt,p);
                    auto vzero = toTerm(activeVar,lStartZero,p);
                    auto eq = logic::Formulas::equality(vit,vzero);
                    auto label = "Static analysis lemma for array var " + activeVar->name + " at location " + statement->location;
                    auto bareLemma = logic::Formulas::universal({iSymbol,pSymbol},eq,label);
                    auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, bareLemma);
                    
                    if (twoTraces)
                    {
                        auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        formulas.push_back(logic::Formulas::universal({tr}, lemma));
                    }
                    else
                    {
                        formulas.push_back(lemma);
                    }
                }
            }    

        }
    }
    
    std::unordered_set<std::shared_ptr<const program::Variable>> StaticAnalysisLemmas::computeAssignedVars(const program::Statement* statement)
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

