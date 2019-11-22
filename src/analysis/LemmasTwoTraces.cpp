#include "LemmasTwoTraces.hpp"

#include <memory>

#include "Formula.hpp"
#include "Theory.hpp"
#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

namespace analysis {
    
    void EqualityPreservationTracesLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        auto t1 = trace1Term();
        auto t2 = trace2Term();

        auto posSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
        auto pos = logic::Terms::var(posSymbol);
        
        // add lemma for each intVar and each intArrayVar
        for (const auto& v : locationToActiveVars.at(locationSymbolForStatement(statement)->name))
        {
            if (!v->isConstant)
            {
                // Note: We use the induction axiom directly as lemma, so the lemma trivially holds and we don't need to prove it.
                auto inductionAxiomName = "traces-eq-preservation-" + v->name + "-" + statement->location;
                auto inductionAxiomNameShort = "IndEqPres" + v->name + "-" + statement->location;

                // IH(arg): v(l(arg),    t1) = v(l(arg),    t2) or
                //          v(l(arg),pos,t1) = v(l(arg),pos,t2)
                auto inductionHypothesis = [&](std::shared_ptr<const logic::Term> arg)
                    {
                        auto lStartArg = timepointForLoopStatement(statement, arg);
                        return
                            logic::Formulas::equality(
                                v->isArray ? toTermFull(v, lStartArg, pos, t1) : toTermFull(v, lStartArg, t1),
                                v->isArray ? toTermFull(v, lStartArg, pos, t2) : toTermFull(v, lStartArg, t2)
                            );
                    };

                std::vector<std::shared_ptr<const logic::Symbol>> freeVars = enclosingIteratorsSymbols(statement);
                if(v->isArray)
                {
                    freeVars.push_back(posSymbol);
                }

                auto [inductionAxBCDef, inductionAxICDef,inductionAxiomConDef, inductionAxiom] = inductionAxiom1(inductionAxiomName, inductionAxiomNameShort, inductionHypothesis, freeVars, logic::ProblemItem::Visibility::Implicit);
                items.push_back(inductionAxBCDef);
                items.push_back(inductionAxICDef);
                items.push_back(inductionAxiomConDef);
                items.push_back(inductionAxiom);
            }
        }
    }

    void NEqualLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        assert(twoTraces);
        

        auto t1 = trace1Term();
        auto t2 = trace2Term();

        auto itSymbol = iteratorSymbol(statement);
        auto it = iteratorTermForLoop(statement);
        auto nT1 = lastIterationTermForLoop(statement, t1, true);
        auto nT2 = lastIterationTermForLoop(statement, t2, true);
        
        auto lStartIt = timepointForLoopStatement(statement, it);
        auto lStartNT1 = timepointForLoopStatement(statement, nT1);
        auto lStartNT2 = timepointForLoopStatement(statement, nT2);

        std::unordered_set<std::shared_ptr<const program::Variable>> loopConditionVars;
        std::cout << statement->condition->toString() << std::endl;
        computeVariablesContainedInLoopCondition(statement->condition, loopConditionVars);

        auto nameSuffix = "-" + statement->location;
        auto name = "last-iteration-equal" + nameSuffix;
        auto nameShort = "lastItEqual" + nameSuffix;
        auto inductionAxiomName = "induction-axiom-" + name;
        auto inductionAxiomNameShort = "Ax-" + nameShort;

        // PART 1: Add induction-axiom
        // IH(it) :=
        // "forall" non-const variables v
        //    v(l(it),t1) = v(l(it),t2)
        auto inductionHypothesis = [&](std::shared_ptr<const logic::Term> arg)
        {
            auto lStartArg = timepointForLoopStatement(statement, arg);

            std::vector<std::shared_ptr<const logic::Formula>> conjuncts;
            for (const auto& v : loopConditionVars)
            {
                if (!v->isConstant)
                {
                    if (v->isArray)
                    {
                        auto posSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
                        auto pos = logic::Terms::var(posSymbol);
                        conjuncts.push_back(
                            logic::Formulas::universal(
                                {posSymbol},
                                logic::Formulas::equality(
                                    toTermFull(v, lStartArg, pos, t1),
                                    toTermFull(v, lStartArg, pos, t2)
                                )
                            )
                        );
                    }
                    else
                    {
                        conjuncts.push_back(
                                logic::Formulas::equality(
                                toTermFull(v, lStartArg, t1),
                                toTermFull(v, lStartArg, t2)
                            )
                        );
                    }
                }
            }

            return logic::Formulas::conjunction(conjuncts);
        };

        auto freeVarSymbols = enclosingIteratorsSymbols(statement);

        auto [inductionAxBCDef, inductionAxICDef,inductionAxiomConDef, inductionAxiom] = logic::inductionAxiom2(inductionAxiomName, inductionAxiomNameShort, inductionHypothesis, nT1, nT2, freeVarSymbols);
        items.push_back(inductionAxBCDef);
        items.push_back(inductionAxICDef);
        items.push_back(inductionAxiomConDef);
        items.push_back(inductionAxiom);

        // PART 2: Add trace lemma
        std::vector<std::shared_ptr<const logic::Formula>> premiseConjuncts;

        // PART 2A: Add EqVC to premise
        for (const auto& v : loopConditionVars)
        {
            if (v->isConstant)
            {
                if (v->isArray)
                {
                    auto posSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
                    auto pos = logic::Terms::var(posSymbol);
                    premiseConjuncts.push_back(
                        logic::Formulas::universal(
                            {posSymbol},
                            logic::Formulas::equality(
                                toTermFull(v, nullptr, pos, t1),
                                toTermFull(v, nullptr, pos, t2)
                            )
                        )
                    );
                }
                else
                {
                    premiseConjuncts.push_back(
                            logic::Formulas::equality(
                            toTermFull(v, nullptr, t1),
                            toTermFull(v, nullptr, t2)
                        )
                    );
                }
            }
        }

        // PART 2B: Add IH(zero) to premise
        premiseConjuncts.push_back(
            inductionHypothesis(logic::Theory::natZero())
        );

        // PART 2C: Add definition for IC and add IC to premise
        std::vector<std::shared_ptr<const logic::Term>> freeVars = {};
        for (const auto& symbol : freeVarSymbols)
        {
            freeVars.push_back(logic::Terms::var(symbol));
        }

        auto inductiveCase = logic::Formulas::predicate("IC-" + nameShort, freeVars);

        // forall it.
        //    =>
        //       and
        //          it<n(t1)
        //          it<n(t2)
        //          IH(it)
        //       IH(s(it))
        auto inductiveCaseFormula =
            logic::Formulas::universal({itSymbol},
                logic::Formulas::implication(
                    logic::Formulas::conjunction({
                        logic::Theory::natSub(it, nT1),
                        logic::Theory::natSub(it, nT2),
                        inductionHypothesis(it)
                    }),
                    inductionHypothesis(logic::Theory::natSucc(it))
                )
            );
        auto inductiveCaseDef =
            std::make_shared<logic::Definition>(
                logic::Formulas::universal(freeVarSymbols,
                    logic::Formulas::equivalence(
                        inductiveCase,
                        inductiveCaseFormula
                    )
                ),
                "IC for " + name,
                logic::ProblemItem::Visibility::Implicit
            );

        items.push_back(inductiveCaseDef);
        
        premiseConjuncts.push_back(inductiveCase);

        // Part 2D: Add lemma
        // forall enclosingIterators.
        //    =>
        //       and
        //          EqVC
        //          IH(0)
        //          IC
        //       n(t1)=n(t2)
        auto lemma =
            logic::Formulas::universal(freeVarSymbols,
                logic::Formulas::implication(
                    logic::Formulas::conjunction(premiseConjuncts),
                    logic::Formulas::equality(nT1,nT2)
                )
            );
        std::vector<std::shared_ptr<const logic::ProblemItem>> fromItems = {inductionAxBCDef, inductionAxICDef, inductionAxiomConDef, inductionAxiom, inductiveCaseDef};
        for (auto& item : programSemantics)
        {
            fromItems.push_back(item);
        }
        items.push_back(std::make_shared<logic::Lemma>(lemma, name, logic::ProblemItem::Visibility::Implicit, fromItems));
    }

    void NEqualLemmas::computeVariablesContainedInLoopCondition(std::shared_ptr<const program::BoolExpression> expr, std::unordered_set<std::shared_ptr<const program::Variable>>& variables) 
    {
        assert(expr != nullptr);
        switch (expr->type())
        {
            case program::BoolExpression::Type::BooleanAnd:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanAnd>(expr);
                computeVariablesContainedInLoopCondition(castedExpr->child1, variables);
                computeVariablesContainedInLoopCondition(castedExpr->child2, variables);
                break;
            }
            case program::BoolExpression::Type::BooleanOr:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanOr>(expr);
                computeVariablesContainedInLoopCondition(castedExpr->child1, variables);
                computeVariablesContainedInLoopCondition(castedExpr->child2, variables);
                break;
            }
            case program::BoolExpression::Type::BooleanNot:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanNot>(expr);
                computeVariablesContainedInLoopCondition(castedExpr->child, variables);
                break;
            }
            case program::BoolExpression::Type::ArithmeticComparison:
            {
                auto castedExpr = std::static_pointer_cast<const program::ArithmeticComparison>(expr);
                computeVariablesContainedInLoopCondition(castedExpr->child1, variables);
                computeVariablesContainedInLoopCondition(castedExpr->child2, variables);
                break;
            }
            case program::BoolExpression::Type::BooleanConstant:
            {
                // do nothing
                break;
            }
        }
    }

    void NEqualLemmas::computeVariablesContainedInLoopCondition(std::shared_ptr<const program::IntExpression> expr, std::unordered_set<std::shared_ptr<const program::Variable>>& variables)
    {
        assert(expr != nullptr);
        switch (expr->type())
        {
            case program::IntExpression::Type::Addition:
            {
                auto castedExpr = std::static_pointer_cast<const program::Addition>(expr);
                computeVariablesContainedInLoopCondition(castedExpr->summand1, variables);
                computeVariablesContainedInLoopCondition(castedExpr->summand2, variables);
                break;
            }
            case program::IntExpression::Type::Subtraction:
            {
                auto castedExpr = std::static_pointer_cast<const program::Subtraction>(expr);
                computeVariablesContainedInLoopCondition(castedExpr->child1, variables);
                computeVariablesContainedInLoopCondition(castedExpr->child2, variables);
                break;
            }
            case program::IntExpression::Type::Multiplication:
            {
                auto castedExpr = std::static_pointer_cast<const program::Multiplication>(expr);
                computeVariablesContainedInLoopCondition(castedExpr->factor1, variables);
                computeVariablesContainedInLoopCondition(castedExpr->factor2, variables);
                break;
            }
            case program::IntExpression::Type::Modulo:
            {
                auto castedExpr = std::static_pointer_cast<const program::Modulo>(expr);
                computeVariablesContainedInLoopCondition(castedExpr->child1, variables);
                computeVariablesContainedInLoopCondition(castedExpr->child2, variables);
                break;
            }
            case program::IntExpression::Type::IntVariableAccess:
            {
                auto castedExpr = std::static_pointer_cast<const program::IntVariableAccess>(expr);
                variables.insert(castedExpr->var);
                break;
            }
            case program::IntExpression::Type::IntArrayApplication:
            {
                auto castedExpr = std::static_pointer_cast<const program::IntArrayApplication>(expr);
                variables.insert(castedExpr->array);
                computeVariablesContainedInLoopCondition(castedExpr->index, variables);
                break;
            }
            case program::IntExpression::Type::ArithmeticConstant:
            {
                // do nothing
                break;
            }
        }
    }
}

