#include "StaticAnalysis.hpp"

#include <cassert>

#include "Theory.hpp"
#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

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
            generateStaticAnalysisLemmasUnassignedVars(castedStatement, lemmas);
            //generateStaticAnalysisLemmasAssignedVars(castedStatement, lemmas);
            
            // recurse on body
            for (const auto& statement : castedStatement->bodyStatements)
            {
                generateStaticAnalysisLemmas(statement.get(), lemmas);
            }
        }
    }
    
    void StaticAnalysis::generateStaticAnalysisLemmasUnassignedVars(const program::WhileStatement* whileStatement,
                                                      std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {
        auto activeVars = locationToActiveVars.at(whileStatement->location);
        auto assignedVars = computeAssignedVars(whileStatement);

        auto iSymbol = iteratorSymbol(whileStatement);
        auto it = iteratorTermForLoop(whileStatement);
        auto locationSymbol = locationSymbolForStatement(whileStatement);
        auto locationName = locationSymbol->name;

        auto enclosingIteratorsSymbols = std::vector<std::shared_ptr<const logic::Symbol>>();
        auto enclosingIteratorsAndIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndZero = std::vector<std::shared_ptr<const logic::Term>>();

        for (const auto& enclosingLoop : *whileStatement->enclosingLoops)
        {
            auto enclosingIteratorSymbol = iteratorSymbol(enclosingLoop);
            enclosingIteratorsSymbols.push_back(enclosingIteratorSymbol);

            auto enclosingIterator = iteratorTermForLoop(enclosingLoop);
            enclosingIteratorsAndIt.push_back(enclosingIterator);            
            enclosingIteratorsAndZero.push_back(enclosingIterator);            
        }
        enclosingIteratorsAndIt.push_back(it);
        enclosingIteratorsAndZero.push_back(logic::Theory::natZero());
                
        auto lStartIt = logic::Terms::func(locationSymbol, enclosingIteratorsAndIt);        
        auto lStartZero = logic::Terms::func(locationSymbol, enclosingIteratorsAndZero);
                
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
                    auto label = "Static analysis lemma for var " + activeVar->name + " at location " + whileStatement->location;
                    auto bareLemma = logic::Formulas::universal({iSymbol},eq,label);
                    auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, bareLemma);
                    
                    if (twoTraces)
                    {
                        auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        lemmas.push_back(logic::Formulas::universal({tr}, lemma));
                    }
                    else
                    {
                        lemmas.push_back(lemma);
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
                    auto label = "Static analysis lemma for array var " + activeVar->name + " at location " + whileStatement->location;
                    auto bareLemma = logic::Formulas::universal({iSymbol,pSymbol},eq,label);
                    auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, bareLemma);
                    
                    if (twoTraces)
                    {
                        auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        lemmas.push_back(logic::Formulas::universal({tr}, lemma));
                    }
                    else
                    {
                        lemmas.push_back(lemma);
                    }
                }
            }    

        }
    }

    // void StaticAnalysis::generateStaticAnalysisLemmasAssignedVars(const program::WhileStatement* whileStatement,
    //                                                   std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    // {

    //     // For every assignmend a = e, in which a is assigned a value exactly once in the loop and all vars in e are not changed during the iteration
    //     // add a lemma saying that a (s (it)) = e(it)       

    //     auto assignedVars = computeAssignedVars(whileStatement);
    //     auto castedStatement = static_cast<const program::WhileStatement*>(statement);

    //     for (const auto& statement : castedStatement->bodyStatements)
    //     {
    //         if (statement->type() == program::Statement::Type::IntAssignment)
    //         {
    //             auto castedAssignment = static_cast<const program::IntAssignment*>(statement);  
    //             // get lhs var
    //             auto lhs = static_cast<const program::IntVariableAccess*>(castedAssignment->lhs.get());
    //             // extract all vars from rhs
    //             auto rhs = ?

    //             // if lhs  = assigned just once
    //             // and everything in rhs is assigned 0 times OR is equal to LHS
    //             // a (s (it)) = e(it)       
    //         }
    //     }
    // }
    
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

