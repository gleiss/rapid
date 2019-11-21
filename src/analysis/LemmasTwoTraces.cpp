#include "LemmasTwoTraces.hpp"

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
        
        auto it = iteratorTermForLoop(statement);
        auto nT1 = lastIterationTermForLoop(statement, t1, true);
        auto nT2 = lastIterationTermForLoop(statement, t2, true);
        
        auto lStartIt = timepointForLoopStatement(statement, it);
        auto lStartNT2 = timepointForLoopStatement(statement, nT2);
        
        // Premise: Loop condition holds at main-loop-location in t1 for all iterations before n(t2) and doesn't hold at main-loop-location in t1 for iteration n(t2)
        auto premise =
            logic::Formulas::conjunction({
                logic::Formulas::universal({it->symbol},
                    logic::Formulas::implication(
                        logic::Theory::natSub(it, nT2),
                        toFormula(statement->condition, lStartIt, t1)
                    )
                ),
                logic::Formulas::negation(
                    toFormula(statement->condition, lStartNT2, t1)
                )
            });

        // Conclusion: nT2 = nT1
        auto conclusion = logic::Formulas::equality(nT2, nT1);
        
        // forall enclosingIterators. (premise => conclusion)
        auto bareLemma =
            logic::Formulas::universal(enclosingIteratorsSymbols(statement),
                logic::Formulas::implication(premise, conclusion)
            );

        auto name = "traces-nEqual-" + nT1->symbol->name + "-" + statement->location;
        items.push_back(std::make_shared<logic::Lemma>(bareLemma, name));
    }
}

