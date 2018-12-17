#include "Semantics.hpp"

#include "Sort.hpp"
#include "Term.hpp"
#include "Formula.hpp"
#include "Theory.hpp"

namespace analysis {
    
    std::vector<std::shared_ptr<const logic::Formula>> Semantics::generateSemantics()
    {
        // generate semantics compositionally
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;
        for(const auto& function : program.functions)
        {
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsFunction;

            for (const auto& statement : function->statements)
            {
                auto semantics = generateSemantics(statement.get(), function->intVariables, function->intArrayVariables);
                conjunctsFunction.push_back(semantics);
            }
            conjuncts.push_back(logic::Formulas::conjunction(conjunctsFunction, "Semantics of function " + function->name));
        }
        
        return conjuncts;
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::Statement* statement,
                                                                       const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                       const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars)
    {
        if (statement->type() == program::Statement::Type::IntAssignment)
        {
            auto castedStatement = static_cast<const program::IntAssignment*>(statement);
            return generateSemantics(castedStatement, intVars, intArrayVars);
        }
        else if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            return generateSemantics(castedStatement, intVars, intArrayVars);
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            return generateSemantics(castedStatement, intVars, intArrayVars);
        }
        else
        {
            assert(statement->type() == program::Statement::Type::SkipStatement);
            auto castedStatement = static_cast<const program::SkipStatement*>(statement);
            return generateSemantics(castedStatement);
        }
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::IntAssignment* intAssignment,
                                                                       const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                       const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars)
    {
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;
        
        auto l1 = startTimePointMap.at(intAssignment);
        auto l2 = endTimePointMap.at(intAssignment);

        // case 1: assignment to int var
        if (intAssignment->lhs->type() == program::IntExpression::Type::IntVariable)
        {
            auto castedLhs = std::static_pointer_cast<const program::IntVariable>(intAssignment->lhs);
            
            // lhs(l2) = rhs(l1)
            auto eq = logic::Formulas::equality(castedLhs->toTerm(l2), intAssignment->rhs->toTerm(l1));
            conjuncts.push_back(eq);
            
            // forall other int-variables: v(l2) = v(l1)
            for (const auto& var : intVars)
            {
                if (*var != *castedLhs)
                {
                    auto eq = logic::Formulas::equality(var->toTerm(l2), var->toTerm(l1));
                    conjuncts.push_back(eq);
                }
            }
            // forall int-array-variables: forall p. v(l2,p) = v(l1,p)
            for (const auto& var : intArrayVars)
            {
                auto pSymbol = logic::Signature::varSymbol("p", logic::Sorts::intSort());
                auto p = logic::Terms::var(pSymbol.get());
                auto conjunct = logic::Formulas::universal({pSymbol}, logic::Formulas::equality(var->toTerm(l2, p), var->toTerm(l1, p)));
                conjuncts.push_back(conjunct);
            }
            return logic::Formulas::conjunction(conjuncts, "Update variable " + castedLhs->name + " at location " + intAssignment->location);
        }
        // case 2: assignment to int-array var
        else
        {
            assert(intAssignment->lhs->type() == program::IntExpression::Type::IntArrayApplication);
            auto application = std::static_pointer_cast<const program::IntArrayApplication>(intAssignment->lhs);
            
            // a(l2, e(l1)) = rhs(l1)
            auto eq1Lhs = application->array->toTerm(l2, application->index->toTerm(l1));
            auto eq1Rhs = intAssignment->rhs->toTerm(l1);
            auto eq1 = logic::Formulas::equality(eq1Lhs, eq1Rhs);
            conjuncts.push_back(eq1);

            // forall positions p. (p!=e(l1) => a(l2,p) = a(l1,p))
            auto pSymbol = logic::Signature::varSymbol("p", logic::Sorts::intSort());
            auto p = logic::Terms::var(pSymbol.get());
            
            auto premise = logic::Formulas::disequality(p, application->index->toTerm(l1));
            auto eq2 = logic::Formulas::equality(application->array->toTerm(l2, p), application->array->toTerm(l1, p));
            auto conjunct = logic::Formulas::universal({pSymbol}, logic::Formulas::implication(premise, eq2));
            conjuncts.push_back(conjunct);
            
            // forall int-variables: v(l2) = v(l1)
            for (const auto& var : intVars)
            {
                auto eq = logic::Formulas::equality(var->toTerm(l2), var->toTerm(l1));
                conjuncts.push_back(eq);
            }
            
            // forall other int-array-variables: forall p. v(l2,p) = v(l1,p)
            for (const auto& var : intArrayVars)
            {
                if (*var != *application->array)
                {
                    auto pSymbol = logic::Signature::varSymbol("p", logic::Sorts::intSort());
                    auto p = logic::Terms::var(pSymbol.get());
                    auto conjunct = logic::Formulas::universal({pSymbol}, logic::Formulas::equality(var->toTerm(l2, p), var->toTerm(l1, p)));
                    conjuncts.push_back(conjunct);
                }
            }
            return logic::Formulas::conjunction(conjuncts, "Update array variable " + application->array->name + " at location " + intAssignment->location);
        }
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::IfElse* ifElse,
                                                                       const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                       const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars)
    {
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;

        auto lStart = startTimePointMap.at(ifElse);
        auto lLeftStart = startTimePointMap.at(ifElse->ifStatements.front().get());
        auto lRightStart = startTimePointMap.at(ifElse->elseStatements.front().get());

        auto lEnd = endTimePointMap.at(ifElse);
        auto lLeftEnd = endTimePointMap.at(ifElse->ifStatements.back().get());
        auto lRightEnd = endTimePointMap.at(ifElse->elseStatements.back().get());
        
        // Part 1: values at the beginning of any branch are the same as at the beginning of the ifElse-statement
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts1;

        // TODO: sideconditions
        for (const auto& var : intVars)
        {
            // v(lLeftStart) = v(lStart)
            auto eq = logic::Formulas::equality(var->toTerm(lLeftStart), var->toTerm(lStart));
            conjuncts1.push_back(eq);
        }
        for (const auto& var : intArrayVars)
        {
            // forall p. v(lLeftStart,p) = v(lStart,p)
            auto pSymbol = logic::Signature::varSymbol("p", logic::Sorts::intSort());
            auto p = logic::Terms::var(pSymbol.get());
            auto conjunct = logic::Formulas::universal({pSymbol}, logic::Formulas::equality(var->toTerm(lLeftStart, p), var->toTerm(lStart, p)));
            conjuncts1.push_back(conjunct);
        }
        for (const auto& var : intVars)
        {
            // v(lRightStart) = v(lStart)
            auto eq = logic::Formulas::equality(var->toTerm(lRightStart), var->toTerm(lStart));
            conjuncts1.push_back(eq);
        }
        for (const auto& var : intArrayVars)
        {
            // forall p. v(lRightStart,p) = v(lStart,p)
            auto pSymbol = logic::Signature::varSymbol("p", logic::Sorts::intSort());
            auto p = logic::Terms::var(pSymbol.get());
            auto conjunct = logic::Formulas::universal({pSymbol}, logic::Formulas::equality(var->toTerm(lRightStart, p), var->toTerm(lStart, p)));
            conjuncts1.push_back(conjunct);
        }
        conjuncts.push_back(logic::Formulas::conjunction(conjuncts1, "Jumping into any branch doesn't change the variable values"));
        
        // Part 2: values at the end of the ifElse-statement are either values at the end of the left branch or the right branch,
        // depending on the branching condition
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts2;
        
        auto c = ifElse->condition->toFormula(lStart);
        for (const auto& var : intVars)
        {
            // condition(lStart) => v(lEnd)=v(lLeftEnd)
            auto eq1 = logic::Formulas::equality(var->toTerm(lEnd), var->toTerm(lLeftEnd));
            conjuncts2.push_back(logic::Formulas::implication(c, eq1));
        }
        for (const auto& var : intArrayVars)
        {
            auto pSymbol = logic::Signature::varSymbol("p", logic::Sorts::intSort());
            auto p = logic::Terms::var(pSymbol.get());

            // condition(lStart) => forall p. v(lEnd,p) = v(lLeftEnd,p)
            auto conclusion1 = logic::Formulas::universal({pSymbol}, logic::Formulas::equality(var->toTerm(lEnd, p), var->toTerm(lLeftEnd, p)));
            conjuncts2.push_back(logic::Formulas::implication(c, conclusion1));
        }
        for (const auto& var : intVars)
        {
            // not condition(lStart) => v(lEnd)=v(lRightEnd)
            auto eq2 = logic::Formulas::equality(var->toTerm(lEnd), var->toTerm(lRightEnd));
            conjuncts2.push_back(logic::Formulas::implication(logic::Formulas::negation(c), eq2));
        }
        for (const auto& var : intArrayVars)
        {
            auto pSymbol = logic::Signature::varSymbol("p", logic::Sorts::intSort());
            auto p = logic::Terms::var(pSymbol.get());

            // not condition(lStart) => forall p. v(lEnd,p) = v(lLeftEnd,p)
            auto conclusion2 = logic::Formulas::universal({pSymbol}, logic::Formulas::equality(var->toTerm(lEnd, p), var->toTerm(lRightEnd, p)));
            conjuncts2.push_back(logic::Formulas::implication(logic::Formulas::negation(c), conclusion2));
        }
        conjuncts.push_back(logic::Formulas::conjunction(conjuncts2, "The branching-condition determines which values to use after the if-statement"));
        
        // Part 3: collect all formulas describing semantics of branches
        for (const auto& statement : ifElse->ifStatements)
        {
            auto conjunct = generateSemantics(statement.get(), intVars, intArrayVars);
            conjuncts.push_back(conjunct);
        }
        for (const auto& statement : ifElse->elseStatements)
        {
            auto conjunct = generateSemantics(statement.get(), intVars, intArrayVars);
            conjuncts.push_back(conjunct);
        }
        
        return logic::Formulas::conjunction(conjuncts, "Semantics of IfElse at location " + ifElse->location);
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::WhileStatement* whileStatement,
                                                                       const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                       const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars)
    {
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;

        auto iSymbol = iteratorMap.at(whileStatement);
        auto i = logic::Terms::var(iSymbol.get());
        auto n = lastIterationMap.at(whileStatement);

        auto lStart0 = startTimePointMap.at(whileStatement);

        auto iteratorsItTerms = std::vector<std::shared_ptr<const logic::Term>>();
        auto iteratorsNTerms = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& iteratorIt : enclosingIteratorsMap.at(whileStatement))
        {
            iteratorsItTerms.push_back(logic::Terms::var(iteratorIt.get()));
            iteratorsNTerms.push_back(logic::Terms::var(iteratorIt.get()));
        }
        iteratorsItTerms.push_back(i);
        iteratorsNTerms.push_back(n);

        auto lStartIt = logic::Terms::func(locationSymbolMap.at(whileStatement), iteratorsItTerms);
        auto lStartN = logic::Terms::func(locationSymbolMap.at(whileStatement), iteratorsNTerms);
        
        auto lBodyStartIt = startTimePointMap.at(whileStatement->bodyStatements.front().get());
        
        auto lEnd = endTimePointMap.at(whileStatement);
        
        // Part 1: values at the beginning of body are the same as at the beginning of the while-statement
        // TODO: sideconditions
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts1;
        for (const auto& var : intVars)
        {
            // v(lBodyStartIt) = v(lStartIt)
            auto eq = logic::Formulas::equality(var->toTerm(lBodyStartIt), var->toTerm(lStartIt));
            conjuncts1.push_back(eq);
        }
        for (const auto& var : intArrayVars)
        {
            // forall p. v(lBodyStartIt,p) = v(lStartIt,p)
            auto pSymbol = logic::Signature::varSymbol("p", logic::Sorts::intSort());
            auto p = logic::Terms::var(pSymbol.get());
            auto conjunct = logic::Formulas::universal({pSymbol}, logic::Formulas::equality(var->toTerm(lBodyStartIt, p), var->toTerm(lStartIt, p)));
            conjuncts1.push_back(conjunct);
        }
        conjuncts.push_back(logic::Formulas::universal({iSymbol}, logic::Formulas::conjunction(conjuncts1), "Jumping into the body doesn't change the variable values"));
        
        // Part 2: collect all formulas describing semantics of body
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts2;
        for (const auto& statement : whileStatement->bodyStatements)
        {
            auto conjunct = generateSemantics(statement.get(), intVars, intArrayVars);
            conjuncts2.push_back(conjunct);
        }
        conjuncts.push_back(logic::Formulas::universal({iSymbol}, logic::Formulas::conjunction(conjuncts2), "Semantics of the body"));
        
        // Part 3: Define last iteration
        // Loop condition holds always before n
        auto iLessN = logic::Theory::timeSub(i, n);
        auto conditionAtI = whileStatement->condition->toFormula(i);
        auto imp = logic::Formulas::implication(iLessN, conditionAtI);
        conjuncts.push_back(logic::Formulas::universal({iSymbol}, imp, "The loop-condition holds always before the last iteration"));
        
        // loop condition doesn't hold at n
        auto negConditionAtN = logic::Formulas::negation(whileStatement->condition->toFormula(n), "The loop-condition doesn't hold in the last iteration");
        conjuncts.push_back(negConditionAtN);
        
        // Part 4: The end-timepoint of the while-statement is the timepoint with location lStart and iteration n

        auto eq = logic::Formulas::equality(lEnd, lStartN, "The values after the while-loop are the values from the last iteration");
        conjuncts.push_back(eq);

        return logic::Formulas::conjunction(conjuncts, "Loop at location " + whileStatement->location);
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::SkipStatement* skipStatement)
    {        
        auto l1 = startTimePointMap.at(skipStatement);
        auto l2 = endTimePointMap.at(skipStatement);

        // identify startTimePoint and endTimePoint
        auto eq = logic::Formulas::equality(l1, l2, "Ignore any skip statement");
        return eq;
    }
}
