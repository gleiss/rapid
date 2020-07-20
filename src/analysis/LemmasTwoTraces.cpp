#include "LemmasTwoTraces.hpp"

#include <cassert>
#include <iostream>

#include "Formula.hpp"
#include "Theory.hpp"
#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"
#include "AnalysisPreComputation.hpp"

namespace analysis {

    void EqualityPreservationTracesLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        auto posSymbol = posVarSymbol();
        auto pos = posVar();

        auto assignedVars = AnalysisPreComputation::computeAssignedVars(statement);

        // add lemma for each intVar and each intArrayVar
        for (const auto& v : locationToActiveVars.at(locationSymbolForStatement(statement)->name))
        {
            if (!v->isConstant && assignedVars.find(v) != assignedVars.end())
            {
                for (unsigned traceNumber1 = 1; traceNumber1 < numberOfTraces+1; traceNumber1++)
                {
                    for (unsigned traceNumber2 = traceNumber1+1; traceNumber2 < numberOfTraces+1; traceNumber2++)
                    {
                        auto t1 = traceTerm(traceNumber1);
                        auto t2 = traceTerm(traceNumber2);

                        // Note: We use the induction axiom directly as lemma, so the lemma trivially holds and we don't need to prove it.
                        auto nameSuffix = "-" + v->name + "-" + statement->location + "-" + std::to_string(traceNumber1) + std::to_string(traceNumber2);
                        auto inductionAxiomName = "traces-eq-preservation" + nameSuffix;
                        auto inductionAxiomNameShort = "EqPres" + nameSuffix;

                        // IH(arg): v(l(arg),    t1) = v(l(arg),    t2) or
                        //          v(l(arg),pos,t1) = v(l(arg),pos,t2)
                        auto inductionHypothesis = [&](std::shared_ptr<const logic::Term> arg)
                            {
                                auto lStartArg = timepointForLoopStatement(statement, arg);
                                return
                                    logic::Formulas::equality(
                                        v->isArray ? toTerm(v, lStartArg, pos, t1) : toTerm(v, lStartArg, t1),
                                        v->isArray ? toTerm(v, lStartArg, pos, t2) : toTerm(v, lStartArg, t2)
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
        }
    }

    void NEqualLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        assert(numberOfTraces > 1);

        auto itSymbol = iteratorSymbol(statement);
        auto it = iteratorTermForLoop(statement);
        auto lStartIt = timepointForLoopStatement(statement, it);

        auto assignedVars = AnalysisPreComputation::computeAssignedVars(statement);
        std::unordered_set<std::shared_ptr<const program::Variable>> loopConditionVars;
        AnalysisPreComputation::computeVariablesContainedInLoopCondition(statement->condition, loopConditionVars);

        for (unsigned traceNumber1 = 1; traceNumber1 < numberOfTraces+1; traceNumber1++)
        {
            for (unsigned traceNumber2 = traceNumber1+1; traceNumber2 < numberOfTraces+1; traceNumber2++)
            {
                auto t1 = traceTerm(traceNumber1);
                auto t2 = traceTerm(traceNumber2);
                auto nT1 = lastIterationTermForLoop(statement, numberOfTraces, t1);
                auto nT2 = lastIterationTermForLoop(statement, numberOfTraces, t2);
                auto lStartNT1 = timepointForLoopStatement(statement, nT1);
                auto lStartNT2 = timepointForLoopStatement(statement, nT2);


                auto nameSuffix = "-" + statement->location + "-" + std::to_string(traceNumber1) + std::to_string(traceNumber2);
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
                        if (assignedVars.find(v) != assignedVars.end())
                        {
                            if (v->isArray)
                            {
                                auto posSymbol = posVarSymbol();
                                auto pos = posVar();
                                conjuncts.push_back(
                                    logic::Formulas::universal(
                                        {posSymbol},
                                        logic::Formulas::equality(
                                            toTerm(v, lStartArg, pos, t1),
                                            toTerm(v, lStartArg, pos, t2)
                                        )
                                    )
                                );
                            }
                            else
                            {
                                conjuncts.push_back(
                                        logic::Formulas::equality(
                                        toTerm(v, lStartArg, t1),
                                        toTerm(v, lStartArg, t2)
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

                // PART 2A: Add EqVC to premise, for (i) constant vars and (ii) non-constant but non-assigned vars
                for (const auto& v : loopConditionVars)
                {
                    if (assignedVars.find(v) == assignedVars.end())
                    {
                        if (v->isArray)
                        {
                            auto posSymbol = posVarSymbol();
                            auto pos = posVar();
                            premiseConjuncts.push_back(
                                logic::Formulas::universal(
                                    {posSymbol},
                                    logic::Formulas::equality(
                                        inlinedVarValues.toInlinedTerm(statement, v, pos, t1),
                                        inlinedVarValues.toInlinedTerm(statement, v, pos, t2)
                                    )
                                )
                            );
                        }
                        else
                        {
                            premiseConjuncts.push_back(
                                    logic::Formulas::equality(
                                        inlinedVarValues.toInlinedTerm(statement, v, t1),
                                        inlinedVarValues.toInlinedTerm(statement, v, t2)
                                )
                            );
                        }
                    }
                }

                // PART 2B: Add IH(zero) to premise, ensuring that non-constant vars are the same at the beginning of the loop execution
                premiseConjuncts.push_back(
                    inductionHypothesis(logic::Theory::natZero())
                );

                // PART 2C: Add definition for IC and add IC to premise
                std::vector<std::shared_ptr<const logic::Term>> freeVars = {};
                for (const auto& symbol : freeVarSymbols)
                {
                    freeVars.push_back(logic::Terms::var(symbol));
                }

                auto inductiveCase = logic::Formulas::lemmaPredicate("IC-" + nameShort, freeVars);

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
                std::vector<std::string> fromItems = {inductionAxBCDef->name, inductionAxICDef->name, inductionAxiomConDef->name, inductionAxiom->name, inductiveCaseDef->name};
                for (auto& item : programSemantics)
                {
                    fromItems.push_back(item->name);
                }
                items.push_back(std::make_shared<logic::Lemma>(lemma, name, logic::ProblemItem::Visibility::Implicit, fromItems));
            }
        }
    }
}