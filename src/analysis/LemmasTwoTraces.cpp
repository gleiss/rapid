#include "LemmasTwoTraces.hpp"

#include "Formula.hpp"
#include "Theory.hpp"
#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

namespace analysis {
    
    void EqualityPreservationTracesLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::Lemma>> &lemmas)
    {
        generateLemmas(statement, true, true, lemmas);
        generateLemmas(statement, true, false, lemmas);
        generateLemmas(statement, false, true, lemmas);
        generateLemmas(statement, false, false, lemmas);
    }

    void EqualityPreservationTracesLemmas::generateLemmas(const program::WhileStatement *statement,
                                                                   bool itLeftZero,
                                                                   bool itRightN,
                                                                   std::vector<std::shared_ptr<const logic::Lemma>> &lemmas)
    {
        auto t1 = trace1Term();
        auto t2 = trace2Term();
        
        std::shared_ptr<const logic::Term> itLeft = logic::Terms::var(logic::Signature::varSymbol("itL", logic::Sorts::natSort()));
        if (itLeftZero)
        {
            itLeft = logic::Theory::natZero(); // use concrete bound 0 instead
        }
        std::shared_ptr<const logic::Term> itRight = logic::Terms::var(logic::Signature::varSymbol("itR", logic::Sorts::natSort()));
        if (itRightN)
        {
            itRight = lastIterationTermForLoop(statement, t1, true); // use concrete bound n(t1) instead
        }
        
        auto itSymbol = iteratorSymbol(statement);
        auto it = logic::Terms::var(itSymbol);
        auto succOfIt = logic::Theory::natSucc(it);
        
        auto lStartIt = timepointForLoopStatement(statement, it);
        auto lStartSuccOfIt = timepointForLoopStatement(statement, succOfIt);
        auto lStartLeft = timepointForLoopStatement(statement, itLeft);
        auto lStartRight = timepointForLoopStatement(statement, itRight);

        auto posSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
        auto pos = logic::Terms::var(posSymbol);
        
        // add lemma for each intVar and each intArrayVar
        for (const auto& v : locationToActiveVars.at(locationSymbolForStatement(statement)->name))
        {
            if (!v->isConstant)
            {
                // indHyp(iter): v(l(iter),    t1) = v(l(iter),    t2) or
                //               v(l(iter),pos,t1) = v(l(iter),pos,t2)
                auto indHypLeft =
                    logic::Formulas::equality(
                        v->isArray ? toTermFull(v, lStartLeft, pos, t1) : toTermFull(v, lStartLeft, t1),
                        v->isArray ? toTermFull(v, lStartLeft, pos, t2) : toTermFull(v, lStartLeft, t2)
                    );
                auto indHypIt =
                    logic::Formulas::equality(
                        v->isArray ? toTermFull(v, lStartIt, pos, t1) : toTermFull(v, lStartIt, t1),
                        v->isArray ? toTermFull(v, lStartIt, pos, t2) : toTermFull(v, lStartIt, t2)
                    );
                auto indHypSuccOfIt =
                    logic::Formulas::equality(
                        v->isArray ? toTermFull(v, lStartSuccOfIt, pos, t1) : toTermFull(v, lStartSuccOfIt, t1),
                        v->isArray ? toTermFull(v, lStartSuccOfIt, pos, t2) : toTermFull(v, lStartSuccOfIt, t2)
                    );
                
                // BaseCase: indHyp(l(itLeft))
                auto baseCase = indHypLeft;
                
                // InductiveCase: forall it. (( itLeft<=it<itRight & indHyp(it) ) => indHyp(s(it)) )
                auto inInterval1 =
                    itLeftZero ?
                        logic::Theory::natSub(it, itRight) :
                        logic::Formulas::conjunction({
                            logic::Theory::natSubEq(itLeft, it),
                            logic::Theory::natSub(it, itRight)
                        });
                auto inductiveCase =
                    logic::Formulas::universal({itSymbol},
                        logic::Formulas::implication(
                            logic::Formulas::conjunction({inInterval1, indHypIt}),
                            indHypSuccOfIt
                        )
                    );
                
                // premise: baseCase & inductiveCase
                auto premise = logic::Formulas::conjunction({baseCase, inductiveCase});
                
                // conclusion: forall it. (( itLeft<=it<=itRight ) => indHyp(it) )
                auto inInterval2 =
                    itLeftZero ?
                        logic::Theory::natSubEq(it, itRight) :
                        logic::Formulas::conjunction({
                                logic::Theory::natSubEq(itLeft, it),
                                logic::Theory::natSubEq(it, itRight)
                        });
                auto conclusion =
                    logic::Formulas::universal({itSymbol},
                        logic::Formulas::implication(
                            inInterval2,
                            indHypIt
                        )
                    );
                
                // forall enclosingIterators.forall {itLeft}{itRight}{pos} (premise -> conclusion) or
                auto outerImplication = logic::Formulas::implication(premise, conclusion);
                std::vector<std::shared_ptr<const logic::Symbol>> quantifierSymbols;
                if(!itLeftZero)
                {
                    quantifierSymbols.push_back(itLeft->symbol);
                }
                if(!itRightN)
                {
                    quantifierSymbols.push_back(itRight->symbol);
                }
                if(v->isArray)
                {
                    quantifierSymbols.push_back(posSymbol);
                }
                auto universal = logic::Formulas::universal(quantifierSymbols, outerImplication);
                auto bareLemma = logic::Formulas::universal(enclosingIteratorsSymbols(statement), universal);
                
                auto name = "traces-eq-preservation-" + std::string(itLeftZero ? "0" : "l") + std::string(itRightN ? "n" : "r") + "-" + v->name + "-" + statement->location;
                lemmas.push_back(std::make_shared<logic::Lemma>(bareLemma, name));
            }
        }
    }
    
    void NEqualLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::Lemma>> &lemmas)
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
        lemmas.push_back(std::make_shared<logic::Lemma>(bareLemma, name));
    }
}

