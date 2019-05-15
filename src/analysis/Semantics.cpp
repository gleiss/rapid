#include "Semantics.hpp"

#include <algorithm>
#include <memory>
#include <vector>
#include <cassert>

#include "Sort.hpp"
#include "Term.hpp"
#include "Formula.hpp"
#include "Theory.hpp"

#include "SemanticsHelper.hpp"
#include "SymbolDeclarations.hpp"

namespace analysis {
    
    std::vector<std::shared_ptr<const program::Variable>> intersection(std::vector<std::shared_ptr<const program::Variable>> v1,
                                                                       std::vector<std::shared_ptr<const program::Variable>> v2)
    {
        std::vector<std::shared_ptr<const program::Variable>> v3;
        
        std::sort(v1.begin(), v1.end());
        std::sort(v2.begin(), v2.end());
        
        std::set_intersection(v1.begin(),v1.end(),
                              v2.begin(),v2.end(),
                              back_inserter(v3));
        return v3;
    }

    std::vector<std::shared_ptr<const logic::Axiom>> Semantics::generateSemantics()
    {
        // generate semantics compositionally
        std::vector<std::shared_ptr<const logic::Axiom>> axioms;
        for(const auto& function : program.functions)
        {
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsFunction;

            for (const auto& statement : function->statements)
            {
                auto semantics = generateSemantics(statement.get());
                conjunctsFunction.push_back(semantics);
            }
            auto axiomFormula = logic::Formulas::conjunction(conjunctsFunction);
            if(twoTraces)
            {
                auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                axiomFormula = logic::Formulas::universal({tr}, axiomFormula);
            }
            axioms.push_back(std::make_shared<logic::Axiom>(axiomFormula, "Semantics of function " + function->name));
        }
        
        return axioms;
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::Statement* statement)
    {
        if (statement->type() == program::Statement::Type::IntAssignment)
        {
            auto castedStatement = static_cast<const program::IntAssignment*>(statement);
            return generateSemantics(castedStatement);
        }
        else if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            return generateSemantics(castedStatement);
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            return generateSemantics(castedStatement);
        }
        else
        {
            assert(statement->type() == program::Statement::Type::SkipStatement);
            auto castedStatement = static_cast<const program::SkipStatement*>(statement);
            return generateSemantics(castedStatement);
        }
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::IntAssignment* intAssignment)
    {
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;
        
        auto l1 = startTimepointForStatement(intAssignment);
        auto l2 = endTimePointMap.at(intAssignment);
        auto l1Name = l1->symbol->name;
        auto l2Name = l2->symbol->name;
        auto activeVars = intersection(locationToActiveVars.at(l1Name), locationToActiveVars.at(l2Name));

        // case 1: assignment to int var
        if (intAssignment->lhs->type() == program::IntExpression::Type::IntVariableAccess)
        {
            auto castedLhs = std::static_pointer_cast<const program::IntVariableAccess>(intAssignment->lhs);
            
            // lhs(l2) = rhs(l1)
            auto eq = logic::Formulas::equality(toTerm(castedLhs,l2), toTerm(intAssignment->rhs,l1));
            conjuncts.push_back(eq);
            
            for (const auto& var : activeVars)
            {
                if(!var->isConstant)
                {
                    if (!var->isArray)
                    {
                        // forall other active non-const int-variables: v(l2) = v(l1)
                        if (*var != *castedLhs->var)
                        {
                            auto eq = logic::Formulas::equality(toTerm(var,l2), toTerm(var,l1));
                            conjuncts.push_back(eq);
                        }
                    }
                    else
                    {
                        // forall active non-const int-array-variables: forall p. v(l2,p) = v(l1,p)
                        auto pSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
                        auto p = logic::Terms::var(pSymbol);
                        auto conjunct = logic::Formulas::universal({pSymbol}, logic::Formulas::equality(toTerm(var,l2,p), toTerm(var,l1,p)));
                        conjuncts.push_back(conjunct);
                    }
                }
            }

            return logic::Formulas::conjunction(conjuncts, "Update variable " + castedLhs->var->name + " at location " + intAssignment->location);
        }
        // case 2: assignment to int-array var
        else
        {
            assert(intAssignment->lhs->type() == program::IntExpression::Type::IntArrayApplication);
            auto application = std::static_pointer_cast<const program::IntArrayApplication>(intAssignment->lhs);
            
            // a(l2, e(l1)) = rhs(l1)
            auto eq1Lhs = toTerm(application->array, l2, toTerm(application->index,l1));
            auto eq1Rhs = toTerm(intAssignment->rhs,l1);
            auto eq1 = logic::Formulas::equality(eq1Lhs, eq1Rhs);
            conjuncts.push_back(eq1);

            // forall positions p. (p!=e(l1) => a(l2,p) = a(l1,p))
            auto pSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
            auto p = logic::Terms::var(pSymbol);
            
            auto premise = logic::Formulas::disequality(p, toTerm(application->index,l1));
            auto eq2 = logic::Formulas::equality(toTerm(application->array, l2, p), toTerm(application->array, l1, p));
            auto conjunct = logic::Formulas::universal({pSymbol}, logic::Formulas::implication(premise, eq2));
            conjuncts.push_back(conjunct);
            
            for (const auto& var : activeVars)
            {
                if(!var->isConstant)
                {
                    if (!var->isArray)
                    {
                        // forall active non-const int-variables: v(l2) = v(l1)
                        auto eq = logic::Formulas::equality(toTerm(var,l2), toTerm(var,l1));
                        conjuncts.push_back(eq);
                    }
                    else
                    {
                        // forall other active non-const int-array-variables: forall p. v(l2,p) = v(l1,p)
                        if (*var != *application->array)
                        {
                            auto pSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
                            auto p = logic::Terms::var(pSymbol);
                            auto conjunct = logic::Formulas::universal({pSymbol}, logic::Formulas::equality(toTerm(var, l2, p), toTerm(var, l1, p)));
                            conjuncts.push_back(conjunct);
                        }
                    }
                }
            }
            return logic::Formulas::conjunction(conjuncts, "Update array variable " + application->array->name + " at location " + intAssignment->location);
        }
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::IfElse* ifElse)
    {
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;

        auto lStart = startTimepointForStatement(ifElse);
        auto lLeftStart = startTimepointForStatement(ifElse->ifStatements.front().get());
        auto lRightStart = startTimepointForStatement(ifElse->elseStatements.front().get());
        auto lEnd = endTimePointMap.at(ifElse);
        
        auto condition = toFormula(ifElse->condition, lStart);
        auto negatedCondition = logic::Formulas::negation(condition);

        // Part 1: values at the beginning of any branch are the same as at the beginning of the ifElse-statement
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts1;
        
         // don't need to take the intersection with active vars at lLeftStart/lRightStart, since the active vars at lStart are always a subset of those at lLeftStart/lRightStart
        auto activeVars = locationToActiveVars.at(lStart->symbol->name);
        
        auto implicationIfBranch = logic::Formulas::implication(condition, allVarEqual(activeVars,lLeftStart,lStart), "Jumping into the left branch doesn't change the variable values");
        auto implicationElseBranch = logic::Formulas::implication(negatedCondition, allVarEqual(activeVars,lRightStart,lStart), "Jumping into the right branch doesn't change the variable values");

        conjuncts.push_back(implicationIfBranch);
        conjuncts.push_back(implicationElseBranch);
        
        // Part 2: collect all formulas describing semantics of branches and assert them conditionally
        for (const auto& statement : ifElse->ifStatements)
        {
            auto semanticsOfStatement = generateSemantics(statement.get());
            auto implication = logic::Formulas::implication(condition, semanticsOfStatement);
            conjuncts.push_back(implication);
        }
        for (const auto& statement : ifElse->elseStatements)
        {
            auto semanticsOfStatement = generateSemantics(statement.get());
            auto implication = logic::Formulas::implication(negatedCondition, semanticsOfStatement);
            conjuncts.push_back(implication);
        }
        
        return logic::Formulas::conjunction(conjuncts, "Semantics of IfElse at location " + ifElse->location);
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::WhileStatement* whileStatement)
    {
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;

        auto itSymbol = iteratorSymbol(whileStatement);
        auto it = logic::Terms::var(itSymbol);
        auto n = lastIterationTermForLoop(whileStatement, twoTraces);

        auto lStart0 = timepointForLoopStatement(whileStatement, logic::Theory::natZero());
        auto lStartIt = timepointForLoopStatement(whileStatement, it);
        auto lStartN = timepointForLoopStatement(whileStatement, n);
        auto lBodyStartIt = startTimepointForStatement(whileStatement->bodyStatements.front().get());
        auto lEnd = endTimePointMap.at(whileStatement);
        
        auto activeVars = locationToActiveVars.at(lStart0->symbol->name);

        // Part 1: values at the beginning of body are the same as at the beginning of the while-statement
        auto part1 = 
            logic::Formulas::universal({itSymbol}, 
                logic::Formulas::implication(
                    logic::Theory::natSub(it,n), 
                    allVarEqual(activeVars,lBodyStartIt,lStartIt)
                ),
                "Jumping into the loop body doesn't change the variable values"
            );
        conjuncts.push_back(part1);
        
        // Part 2: collect all formulas describing semantics of body
        std::vector<std::shared_ptr<const logic::Formula>> conjunctsBody;
        for (const auto& statement : whileStatement->bodyStatements)
        {
            auto conjunct = generateSemantics(statement.get());
            conjunctsBody.push_back(conjunct);
        }
        auto bodySemantics = 
            logic::Formulas::universal({itSymbol}, 
                logic::Formulas::implication(
                    logic::Theory::natSub(it,n),
                    logic::Formulas::conjunction(conjunctsBody)
                ),
                "Semantics of the body"
            );
        conjuncts.push_back(bodySemantics);
        
        // Part 3: Define last iteration
        // Loop condition holds at main-loop-location for all iterations before n
        auto universal = 
            logic::Formulas::universal({itSymbol}, 
                logic::Formulas::implication(
                    logic::Theory::natSub(it, n), 
                    toFormula(whileStatement->condition, lStartIt)), 
                "The loop-condition holds always before the last iteration"
            );
        conjuncts.push_back(universal);
        
        // loop condition doesn't hold at n
        auto negConditionAtN = logic::Formulas::negation(toFormula(whileStatement->condition, lStartN), "The loop-condition doesn't hold in the last iteration");
        conjuncts.push_back(negConditionAtN);
        
        // Part 4: The values after the while-loop are the values from the timepoint with location lStart and iteration n
        auto part4 = allVarEqual(activeVars,lEnd,lStartN, "The values after the while-loop are the values from the last iteration");
        conjuncts.push_back(part4);
        
        return logic::Formulas::conjunction(conjuncts, "Loop at location " + whileStatement->location);
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::SkipStatement* skipStatement)
    {        
        auto l1 = startTimepointForStatement(skipStatement);
        auto l2 = endTimePointMap.at(skipStatement);

        // identify startTimePoint and endTimePoint
        auto eq = logic::Formulas::equality(l1, l2, "Ignore any skip statement");
        return eq;
    }
}