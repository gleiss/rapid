#include "Semantics.hpp"

#include <algorithm>
#include <memory>
#include <vector>
#include <cassert>
#include <unordered_set>

#include "Sort.hpp"
#include "Term.hpp"
#include "Formula.hpp"
#include "Theory.hpp"
#include "Options.hpp"

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
            SemanticsInliner inliner(problemItems);
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsFunction;
            for (const auto& statement : function->statements)
            {
                auto semantics = generateSemantics(statement.get(), inliner);
                conjunctsFunction.push_back(semantics);
            }
            if (util::Configuration::instance().inlineSemantics())
            {
                // handle persistence of last statement of the function
                auto lEnd = endTimePointMap.at(function->statements.back().get());
                inliner.currTimepoint = lEnd;
                auto f = inliner.handlePersistence(lEnd, locationToActiveVars.at(lEnd->symbol->name), "Define referenced terms denoting variable values at " + lEnd->symbol->name);
                conjunctsFunction.push_back(f);
            }

            auto axiomFormula = logic::Formulas::conjunctionSimp(conjunctsFunction);
            if(numberOfTraces > 1)
            {
                axiomFormula = logic::Formulas::universal({traceVarSymbol()}, axiomFormula);
            }
            axioms.push_back(std::make_shared<logic::Axiom>(axiomFormula, "Semantics of function " + function->name, logic::ProblemItem::Visibility::Implicit));
        }

        return axioms;
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::Statement* statement, SemanticsInliner& inliner)
    {
        if (statement->type() == program::Statement::Type::IntAssignment)
        {
            auto castedStatement = static_cast<const program::IntAssignment*>(statement);
            return generateSemantics(castedStatement, inliner);
        }
        else if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            return generateSemantics(castedStatement, inliner);
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            return generateSemantics(castedStatement, inliner);
        }
        else
        {
            assert(statement->type() == program::Statement::Type::SkipStatement);
            auto castedStatement = static_cast<const program::SkipStatement*>(statement);
            return generateSemantics(castedStatement, inliner);
        }
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::IntAssignment* intAssignment, SemanticsInliner& inliner)
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
            
            if (util::Configuration::instance().inlineSemantics())
            {
                inliner.currTimepoint = l1;
                auto f1 = inliner.handlePersistence(l1, locationToActiveVars.at(l1Name));
                conjuncts.push_back(f1);

                auto f2 = inliner.setIntVarValue(castedLhs->var, inliner.toCachedTerm(intAssignment->rhs));
                conjuncts.push_back(f2);

                return logic::Formulas::conjunctionSimp(conjuncts, "Update variable " + castedLhs->var->name + " at location " + intAssignment->location);
            }
            else
            {
                // lhs(l2) = rhs(l1)
                auto eq = logic::Formulas::equality(toTerm(castedLhs,l2,traceVar()), toTerm(intAssignment->rhs,l1,traceVar()));
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
                                auto eq =
                                    logic::Formulas::equality(
                                        toTerm(var,l2,traceVar()),
                                        toTerm(var,l1,traceVar())
                                    );
                                conjuncts.push_back(eq);
                            }
                        }
                        else
                        {
                            // forall active non-const int-array-variables: forall p. v(l2,p) = v(l1,p)
                            auto posSymbol = posVarSymbol();
                            auto pos = posVar();
                            auto conjunct =
                                logic::Formulas::universal({posSymbol},
                                    logic::Formulas::equality(
                                        toTerm(var,l2,pos,traceVar()),
                                        toTerm(var,l1,pos,traceVar())
                                    )
                                );
                            conjuncts.push_back(conjunct);
                        }
                    }
                }

                return logic::Formulas::conjunction(conjuncts, "Update variable " + castedLhs->var->name + " at location " + intAssignment->location);
            }
        }
        // case 2: assignment to int-array var
        else
        {
            assert(intAssignment->lhs->type() == program::IntExpression::Type::IntArrayApplication);
            auto application = std::static_pointer_cast<const program::IntArrayApplication>(intAssignment->lhs);

            if (util::Configuration::instance().inlineSemantics())
            {
                inliner.currTimepoint = l1;
                auto f1 = inliner.handlePersistence(l1, locationToActiveVars.at(l1Name));
                conjuncts.push_back(f1);

                // a(l2, cached(e)) = cached(rhs)
                auto eq1Lhs = toTerm(application->array,l2,inliner.toCachedTerm(application->index),traceVar());
                auto eq1Rhs = inliner.toCachedTerm(intAssignment->rhs);
                auto eq1 = logic::Formulas::equality(eq1Lhs, eq1Rhs);
                conjuncts.push_back(eq1);

                // forall positions pos. (pos!=cached(e) => a(l2,pos) = cached(a,pos))
                auto posSymbol = posVarSymbol();
                auto pos = posVar();

                auto premise = logic::Formulas::disequality(pos, inliner.toCachedTerm(application->index));
                auto eq2 = logic::Formulas::equality(toTerm(application->array,l2,pos,traceVar()), inliner.toCachedTermFull(application->array,pos));
                auto conjunct = logic::Formulas::universal({posSymbol}, logic::Formulas::implication(premise, eq2));
                conjuncts.push_back(conjunct);

                // set last assignment of a to l2
                inliner.setArrayVarTimepoint(application->array, l2);

                return logic::Formulas::conjunctionSimp(conjuncts, "Update array variable " + application->array->name + " at location " + intAssignment->location);
            }
            else
            {
                // a(l2, e(l1)) = rhs(l1)
                auto eq1Lhs = toTerm(application->array,l2,toTerm(application->index,l1,traceVar()),traceVar());
                auto eq1Rhs = toTerm(intAssignment->rhs,l1,traceVar());
                auto eq1 = logic::Formulas::equality(eq1Lhs, eq1Rhs);
                conjuncts.push_back(eq1);

                // forall positions pos. (pos!=e(l1) => a(l2,pos) = a(l1,pos))
                auto posSymbol = posVarSymbol();
                auto pos = posVar();

                auto premise = logic::Formulas::disequality(pos, toTerm(application->index,l1,traceVar()));
                auto eq2 = logic::Formulas::equality(toTerm(application->array, l2, pos,traceVar()), toTerm(application->array,l1,pos,traceVar()));
                auto conjunct = logic::Formulas::universal({posSymbol}, logic::Formulas::implication(premise, eq2));
                conjuncts.push_back(conjunct);

                for (const auto& var : activeVars)
                {
                    if(!var->isConstant)
                    {
                        if (!var->isArray)
                        {
                            // forall active non-const int-variables: v(l2) = v(l1)
                            auto eq =
                                logic::Formulas::equality(
                                    toTerm(var,l2,traceVar()),
                                    toTerm(var,l1,traceVar())
                                );
                            conjuncts.push_back(eq);
                        }
                        else
                        {
                            // forall other active non-const int-array-variables: forall pos. v(l2,pos) = v(l1,pos)
                            if (*var != *application->array)
                            {
                                auto posSymbol = posVarSymbol();
                                auto pos = posVar();
                                auto conjunct =
                                    logic::Formulas::universal({posSymbol},
                                        logic::Formulas::equality(
                                            toTerm(var,l2,pos,traceVar()),
                                            toTerm(var,l1,pos,traceVar())
                                        )
                                    );
                                conjuncts.push_back(conjunct);
                            }
                        }
                    }
                }
                return logic::Formulas::conjunction(conjuncts, "Update array variable " + application->array->name + " at location " + intAssignment->location);
            }
        }
    }
    
    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::IfElse* ifElse, SemanticsInliner& inliner)
    {
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;

        auto lStart = startTimepointForStatement(ifElse);
        auto lEnd = endTimePointMap.at(ifElse);
        auto lLeftStart = startTimepointForStatement(ifElse->ifStatements.front().get());
        auto lRightStart = startTimepointForStatement(ifElse->elseStatements.front().get());

        if (util::Configuration::instance().inlineSemantics())
        {
            // Part 1: visit start-location
            inliner.currTimepoint = lStart;
            auto f1 = inliner.handlePersistence(lStart, locationToActiveVars.at(lStart->symbol->name), "Define referenced terms denoting variable values at " + ifElse->location);
            conjuncts.push_back(f1);

            // Part 2: go through branches: collect all formulas describing semantics of branches and assert them conditionally
            // note: we need to generate condition and negatedCondition here, since they depend on the state of the inliner.
            auto condition = inliner.toCachedFormula(ifElse->condition);
            auto negatedCondition = logic::Formulas::negation(condition);

            std::vector<std::shared_ptr<const logic::Formula>> conjunctsLeft;
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsRight;
            SemanticsInliner inlinerLeft(inliner);
            SemanticsInliner inlinerRight(inliner);

            for (const auto& statement : ifElse->ifStatements)
            {
                auto semanticsOfStatement = generateSemantics(statement.get(), inlinerLeft);
                conjunctsLeft.push_back(semanticsOfStatement);
            }
            for (const auto& statement : ifElse->elseStatements)
            {
                auto semanticsOfStatement = generateSemantics(statement.get(), inlinerRight);
                conjunctsRight.push_back(semanticsOfStatement);
            }

            // Part 3: define variable values at the merge of branches
            auto cachedIntVarValues = inliner.getCachedIntVarValues();
            auto cachedArrayVarTimepoints = inliner.getCachedArrayVarTimepoints();
            auto cachedIntVarValuesLeft = inlinerLeft.getCachedIntVarValues();
            auto cachedArrayVarTimepointsLeft = inlinerLeft.getCachedArrayVarTimepoints();
            auto cachedIntVarValuesRight = inlinerRight.getCachedIntVarValues();
            auto cachedArrayVarTimepointsRight = inlinerRight.getCachedArrayVarTimepoints();

            std::unordered_set<std::shared_ptr<const program::Variable>> mergeVars;
            for (const auto& pair : cachedIntVarValuesLeft)
            {
                auto var = pair.first;
                if (cachedIntVarValues.find(var) == cachedIntVarValues.end() || *cachedIntVarValues[var] != *pair.second)
                {
                    mergeVars.insert(var);
                }
            }
            for (const auto& pair : cachedIntVarValuesRight)
            {
                auto var = pair.first;
                if (cachedIntVarValues.find(var) == cachedIntVarValues.end() || *cachedIntVarValues[var] != *pair.second)
                {
                    mergeVars.insert(var);
                }
            }
            for (const auto& pair : cachedArrayVarTimepointsLeft)
            {
                auto var = pair.first;
                if (cachedArrayVarTimepoints.find(var) == cachedArrayVarTimepoints.end() || *cachedArrayVarTimepoints[var] != *pair.second)
                {
                    mergeVars.insert(var);
                }
            }
            for (const auto& pair : cachedArrayVarTimepointsRight)
            {
                auto var = pair.first;
                if (cachedArrayVarTimepoints.find(var) == cachedArrayVarTimepoints.end() || *cachedArrayVarTimepoints[var] != *pair.second)
                {
                    mergeVars.insert(var);
                }
            }

            for (const auto& var : mergeVars)
            {
                if (!var->isArray)
                {
                    auto varLEnd = toTerm(var,lEnd,traceVar());

                    // define the value of var at lEnd as the merge of values at the end of the two branches
                    conjunctsLeft.push_back(
                        logic::Formulas::equalitySimp(
                            varLEnd,
                            inlinerLeft.toCachedTermFull(var)
                        )
                    );
                    conjunctsRight.push_back(
                        logic::Formulas::equalitySimp(
                            varLEnd,
                            inlinerRight.toCachedTermFull(var)
                        )
                    );

                    // remember that lEnd is the last timepoint where var was set
                    assert(!var->isConstant);
                    auto result = inliner.setIntVarValue(var, varLEnd);
                }
                else
                {
                    assert(cachedArrayVarTimepointsLeft.find(var) != cachedArrayVarTimepointsLeft.end());
                    assert(cachedArrayVarTimepointsRight.find(var) != cachedArrayVarTimepointsRight.end());

                    auto posSymbol = posVarSymbol();
                    auto pos = posVar();
                    auto varLEnd = toTerm(var,lEnd,pos,traceVar());

                    // define the value of var at lEnd as the merge of values at the end of the two branches
                    conjunctsLeft.push_back(
                        logic::Formulas::universalSimp({posSymbol},
                            logic::Formulas::equalitySimp(
                                varLEnd,
                                inlinerLeft.toCachedTermFull(var,pos)
                            )
                        )
                    );
                    conjunctsRight.push_back(
                        logic::Formulas::universalSimp({posSymbol},
                            logic::Formulas::equalitySimp(
                                varLEnd,
                                inlinerRight.toCachedTermFull(var,pos)
                            )
                        )
                    );

                    // cache the fact that lEnd is the last timepoint where var was set
                    assert(!var->isConstant);
                    inliner.setArrayVarTimepoint(var, lEnd);
                }
            }

            conjuncts.push_back(
                logic::Formulas::implicationSimp(
                    condition,
                    logic::Formulas::conjunctionSimp(conjunctsLeft),
                    "Semantics of left branch"
                )
            );
            conjuncts.push_back(
                logic::Formulas::implicationSimp(
                     negatedCondition,
                     logic::Formulas::conjunctionSimp(conjunctsRight),
                     "Semantics of right branch"
                )
            );

            return logic::Formulas::conjunctionSimp(conjuncts, "Semantics of IfElse at location " + ifElse->location);
        }
        else
        {
            auto condition = toFormula(ifElse->condition, lStart, traceVar());
            auto negatedCondition = logic::Formulas::negation(condition);

            // Part 1: values at the beginning of any branch are the same as at the beginning of the ifElse-statement
            // don't need to take the intersection with active vars at lLeftStart/lRightStart, since the active vars at lStart are always a subset of those at lLeftStart/lRightStart
            auto activeVars = locationToActiveVars.at(lStart->symbol->name);

            auto implicationIfBranch = logic::Formulas::implication(condition, allVarEqual(activeVars,lLeftStart,lStart, traceVar()), "Jumping into the left branch doesn't change the variable values");
            auto implicationElseBranch = logic::Formulas::implication(negatedCondition, allVarEqual(activeVars,lRightStart,lStart, traceVar()), "Jumping into the right branch doesn't change the variable values");

            conjuncts.push_back(implicationIfBranch);
            conjuncts.push_back(implicationElseBranch);

            // Part 2: collect all formulas describing semantics of branches and assert them conditionally
            for (const auto& statement : ifElse->ifStatements)
            {
                auto semanticsOfStatement = generateSemantics(statement.get(), inliner);
                auto implication = logic::Formulas::implication(condition, semanticsOfStatement, "Semantics of left branch");
                conjuncts.push_back(implication);
            }
            for (const auto& statement : ifElse->elseStatements)
            {
                auto semanticsOfStatement = generateSemantics(statement.get(), inliner);
                auto implication = logic::Formulas::implication(negatedCondition, semanticsOfStatement,  "Semantics of right branch");
                conjuncts.push_back(implication);
            }

            return logic::Formulas::conjunction(conjuncts, "Semantics of IfElse at location " + ifElse->location);
        }
    }

    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::WhileStatement* whileStatement, SemanticsInliner& inliner)
    {
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;

        auto itSymbol = iteratorSymbol(whileStatement);
        auto it = logic::Terms::var(itSymbol);
        auto n = lastIterationTermForLoop(whileStatement, numberOfTraces, traceVar());

        auto lStart0 = timepointForLoopStatement(whileStatement, logic::Theory::natZero());
        auto lStartIt = timepointForLoopStatement(whileStatement, it);
        auto lStartSuccOfIt = timepointForLoopStatement(whileStatement, logic::Theory::natSucc(it));
        auto lStartN = timepointForLoopStatement(whileStatement, n);
        auto lBodyStartIt = startTimepointForStatement(whileStatement->bodyStatements.front().get());
        auto lEnd = endTimePointMap.at(whileStatement);

        auto posSymbol = posVarSymbol();
        auto pos = posVar();

        auto activeVars = locationToActiveVars.at(lStart0->symbol->name);

        if (util::Configuration::instance().inlineSemantics())
        {
            auto assignedVars = AnalysisPreComputation::computeAssignedVars(whileStatement);
            // Part 0: custom persistence handling: handle all vars which are 1) active, 2) keep the same value throughout the loop, 3) non-const, and 4) persistent at the loop condition check
            // note: condition 3) is a requirement since otherwise the defining formulas could be unsound. Condition 4) does not lead to incompleteness of the formalization, since the value of variables, which change their value in the loop, will be defined afterwards anyway.
            std::vector<std::shared_ptr<const program::Variable>> vars;
            for (const auto& var : activeVars)
            {
                if (assignedVars.find(var) == assignedVars.end() && !var->isConstant)
                {
                    vars.push_back(var);
                }
            }
            auto f1 = inliner.handlePersistenceOfLoop(lStart0, lStartIt, vars);
            conjuncts.push_back(
                logic::Formulas::universalSimp({itSymbol},
                    logic::Formulas::implicationSimp(
                        logic::Theory::natSubEq(it,n),
                        f1
                    ),
                    "Define referenced terms denoting variable values at " + whileStatement->location
                )
            );

            // Part 1: define values of assignedVars at iteration zero
            std::vector<std::shared_ptr<const logic::Formula>> conjPart1;

            inliner.currTimepoint = lStart0;
            for (const auto& var : assignedVars)
            {
                if (!var->isArray)
                {
                    conjPart1.push_back(
                        logic::Formulas::equalitySimp(
                            toTerm(var, lStart0,traceVar()),
                            inliner.toCachedTermFull(var)
                        )
                    );
                }
                else
                {
                    conjPart1.push_back(
                        logic::Formulas::universalSimp({posSymbol},
                            logic::Formulas::equalitySimp(
                                toTerm(var,lStart0,pos,traceVar()),
                                inliner.toCachedTermFull(var, pos)
                            )
                        )
                    );
                }
            }

            conjuncts.push_back(
                logic::Formulas::conjunctionSimp(
                    conjPart1,
                    "Define variable values at beginning of loop"
                )
            );

            // Part 2a: Loop condition holds at main-loop-location for all iterations before n
            inliner.currTimepoint = lStartIt;
            for (const auto& var : assignedVars)
            {
                if (!var->isArray)
                {
                    assert(!var->isConstant);
                    auto result = inliner.setIntVarValue(var,toTerm(var,lStartIt,traceVar()));
                }
                else
                {
                    inliner.setArrayVarTimepoint(var,lStartIt);
                }
            }

            conjuncts.push_back(
                logic::Formulas::universal({itSymbol},
                    logic::Formulas::implication(
                        logic::Theory::natSub(it, n),
                        inliner.toCachedFormula(whileStatement->condition)
                    ),
                    "The loop-condition holds always before the last iteration"
                )
            );

            // Part 3: collect all formulas describing semantics of the body, and define values of assignedVars at all iterations s(it)
            assert(*inliner.currTimepoint == *lStartIt);

            std::vector<std::shared_ptr<const logic::Formula>> conjunctsBody;
            for (const auto& statement : whileStatement->bodyStatements)
            {
                auto semanticsOfStatement = generateSemantics(statement.get(), inliner);
                conjunctsBody.push_back(semanticsOfStatement);
            }

            inliner.currTimepoint = lStartSuccOfIt;
            for (const auto& var : assignedVars)
            {
                if (!var->isArray)
                {
                    conjunctsBody.push_back(
                        logic::Formulas::equalitySimp(
                            toTerm(var,lStartSuccOfIt,traceVar()),
                            inliner.toCachedTermFull(var),
                            "Define value of variable " + var->name + " at beginning of next iteration"
                        )
                    );
                }
                else
                {
                    conjunctsBody.push_back(
                        logic::Formulas::universalSimp({posSymbol},
                            logic::Formulas::equalitySimp(
                                toTerm(var,lStartSuccOfIt,pos,traceVar()),
                                inliner.toCachedTermFull(var, pos)
                            ),
                            "Define value of array variable " + var->name + " at beginning of next iteration"
                        )
                    );
                }
            }

            conjuncts.push_back(
                logic::Formulas::universalSimp({itSymbol},
                    logic::Formulas::implicationSimp(
                        logic::Theory::natSub(it,n),
                        logic::Formulas::conjunctionSimp(conjunctsBody)
                    ),
                    "Semantics of the body"
                )
            );

            // Part 4: define values of assignedVars after the execution of the loop
            inliner.currTimepoint = lStartN;
            for (const auto& var : assignedVars)
            {
                if (!var->isArray)
                {
                    assert(!var->isConstant);
                    auto result = inliner.setIntVarValue(var, toTerm(var,lStartN,traceVar()));
                }
                else
                {
                    inliner.setArrayVarTimepoint(var,lStartN);
                }
            }

            // Part 2b: loop condition doesn't hold at n
            assert(*inliner.currTimepoint == *lStartN);
            conjuncts.push_back(
                logic::Formulas::negation(
                    inliner.toCachedFormula(whileStatement->condition),
                    "The loop-condition doesn't hold in the last iteration"
                )
            );

            return logic::Formulas::conjunctionSimp(conjuncts, "Loop at location " + whileStatement->location);
        }
        else
        {
            // Part 1: values at the beginning of body are the same as at the beginning of the while-statement
            auto part1 =
                logic::Formulas::universal({itSymbol},
                    logic::Formulas::implication(
                        logic::Theory::natSub(it,n),
                        allVarEqual(activeVars,lBodyStartIt,lStartIt, traceVar())
                    ),
                    "Jumping into the loop body doesn't change the variable values"
                );
            conjuncts.push_back(part1);

            // Part 2: collect all formulas describing semantics of body
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsBody;
            for (const auto& statement : whileStatement->bodyStatements)
            {
                auto conjunct = generateSemantics(statement.get(), inliner);
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
                        toFormula(whileStatement->condition, lStartIt, traceVar())),
                    "The loop-condition holds always before the last iteration"
                );
            conjuncts.push_back(universal);

            // loop condition doesn't hold at n
            auto negConditionAtN = logic::Formulas::negation(toFormula(whileStatement->condition, lStartN, traceVar()), "The loop-condition doesn't hold in the last iteration");
            conjuncts.push_back(negConditionAtN);

            // Part 4: The values after the while-loop are the values from the timepoint with location lStart and iteration n
            auto part4 = allVarEqual(activeVars,lEnd,lStartN, traceVar(), "The values after the while-loop are the values from the last iteration");
            conjuncts.push_back(part4);

            return logic::Formulas::conjunction(conjuncts, "Loop at location " + whileStatement->location);
        }
    }

    std::shared_ptr<const logic::Formula> Semantics::generateSemantics(const program::SkipStatement* skipStatement, SemanticsInliner& inliner)
    {
        auto l1 = startTimepointForStatement(skipStatement);

        if (util::Configuration::instance().inlineSemantics())
        {
            inliner.currTimepoint = l1;
            return inliner.handlePersistence(l1, locationToActiveVars.at(l1->symbol->name));
        }
        else
        {
            auto l2 = endTimePointMap.at(skipStatement);

            // identify startTimePoint and endTimePoint
            auto eq = logic::Formulas::equality(l1, l2, "Ignore any skip statement");
            return eq;
        }
    }
}