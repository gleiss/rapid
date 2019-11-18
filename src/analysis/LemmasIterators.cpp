#include "LemmasIterators.hpp"

#include "Formula.hpp"
#include "Theory.hpp"
#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

namespace analysis {
    
    void IntermediateValueLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        auto itSymbol = iteratorSymbol(statement);
        auto it = iteratorTermForLoop(statement);
        auto it2Symbol = logic::Signature::varSymbol("it", logic::Sorts::natSort());
        auto it2 = logic::Terms::var(it2Symbol);
        auto n = lastIterationTermForLoop(statement, twoTraces);
        
        auto lStartIt = timepointForLoopStatement(statement, it);
        auto lStartIt2 = timepointForLoopStatement(statement, it2);
        auto lStartSuccOfIt = timepointForLoopStatement(statement, logic::Theory::natSucc(it));
        auto lStartZero = timepointForLoopStatement(statement, logic::Theory::natZero());
        auto lStartN = timepointForLoopStatement(statement, n);
        
        auto posSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
        auto pos = logic::Terms::var(posSymbol);
        auto xSymbol = logic::Signature::varSymbol("xInt", logic::Sorts::intSort());
        auto x = logic::Terms::var(xSymbol);
        
        // add lemma for each intVar and each intArrayVar
        for (const auto& v : locationToActiveVars.at(locationSymbolForStatement(statement)->name))
        {
            if (!v->isConstant)
            {
                // PART 1: Add induction-axiom
                auto inductionAxiomName = "iterator-intermediateValue-axiom-" + v->name + "-" + statement->location;
                auto inductionAxiomNameShort = "IndIntermed-" + v->name + "-" + statement->location;

                // IH(it): v(l(it1,...,itk,it)    ) <= x or
                //         v(l(it1,...,itk,it),pos) <= x
                auto inductionHypothesis = [&](std::shared_ptr<const logic::Term> arg)
                {
                    auto lStartArg = timepointForLoopStatement(statement, arg);
                    return logic::Theory::intLessEqual(
                        v->isArray ? toTerm(v, lStartArg, pos) : toTerm(v,lStartArg),
                        x
                    );
                };

                std::vector<std::shared_ptr<const logic::Symbol>> freeVars = {xSymbol};
                if (v->isArray)
                {
                    freeVars.push_back(posSymbol);
                }
                if (twoTraces)
                {
                    auto trSymbol = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                    freeVars.push_back(trSymbol);
                }

                logic::addInductionAxiom1(inductionAxiomName, inductionAxiomNameShort, inductionHypothesis, freeVars, items);

                // PART 2: Add trace lemma
                // Premise: v(l(zero))<=x     & x<v(l(n))     & forall it. (it<n => v(l(s(it)))=v(l(it))+1)         , or
                //          v(l(zero),pos)<=x & x<v(l(n),pos) & forall it. (it<n => v(l(s(it)),pos)=v(l(it),pos)+1)
                auto premise =
                    logic::Formulas::conjunction({
                        logic::Theory::intLessEqual(
                            v->isArray ? toTerm(v,lStartZero,pos) : toTerm(v,lStartZero),
                            x
                        ),
                        logic::Theory::intLess(
                            x,
                            v->isArray ? toTerm(v,lStartN,pos) : toTerm(v,lStartN)
                        ),
                        logic::Formulas::universal({itSymbol},
                            logic::Formulas::implication(
                                logic::Theory::natSub(it, n),
                                logic::Formulas::equality(
                                    v->isArray ? toTerm(v,lStartSuccOfIt,pos) : toTerm(v,lStartSuccOfIt),
                                    logic::Theory::intAddition(
                                        v->isArray ?  toTerm(v,lStartIt,pos) : toTerm(v,lStartIt),
                                        logic::Theory::intConstant(1)
                                    )
                                )
                            )
                        )
                    });

                // Conclusion: exists it2. (v(l(it2))=x     & it2<n) or
                //             exists it2. (v(l(it2),pos)=x & it2<n)
                auto conclusion =
                    logic::Formulas::existential({it2Symbol},
                        logic::Formulas::conjunction({
                            logic::Formulas::equality(
                                v->isArray ? toTerm(v,lStartIt2,pos) : toTerm(v,lStartIt2),
                                x
                            ),
                            logic::Theory::natSub(it2,n)
                        })
                    );
                
                // forall enclosingIterators. forall x. (premise => conclusion) or
                // forall enclosingIterators. forall x,pos. (premise => conclusion)
                auto implication = logic::Formulas::implication(premise,conclusion);
                auto universal =
                    v->isArray ?
                        logic::Formulas::universal({xSymbol,posSymbol},implication) :
                        logic::Formulas::universal({xSymbol},implication);
                auto bareLemma = logic::Formulas::universal(enclosingIteratorsSymbols(statement), universal);
                
                if (twoTraces)
                {
                    auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                    bareLemma = logic::Formulas::universal({tr}, bareLemma);
                }
                
                auto name = "iterator-intermediateValue-" + v->name + "-" + statement->location;
                items.push_back(std::make_shared<logic::Lemma>(bareLemma, name));
            }
        }
    }
    
    void IterationInjectivityLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>> &items)
    {
        auto itSymbol = iteratorSymbol(statement);
        auto it = iteratorTermForLoop(statement);
        auto it1Symbol = logic::Signature::varSymbol("it1", logic::Sorts::natSort());
        auto it1 = logic::Terms::var(it1Symbol);
        auto it2Symbol = logic::Signature::varSymbol("it2", logic::Sorts::natSort());
        auto it2 = logic::Terms::var(it2Symbol);
        auto n = lastIterationTermForLoop(statement, twoTraces);
        
        auto lStartIt = timepointForLoopStatement(statement, it);
        auto lStartSuccOfIt = timepointForLoopStatement(statement, logic::Theory::natSucc(it));
        auto lStartIt1 = timepointForLoopStatement(statement, it1);
        auto lStartIt2 = timepointForLoopStatement(statement, it2);
        
        // add lemma for each intVar
        for (const auto& v : locationToActiveVars.at(locationSymbolForStatement(statement)->name))
        {
            if (!v->isConstant)
            {
                if (!v->isArray) // We assume that loop counters are not array elements and therefore only add these lemmas for non-array-vars
                {
                    // PART 1: Add induction-axiom
                    auto inductionAxiomName1 = "iterator-injectivity-axiom1-" + v->name + "-" + statement->location;
                    auto inductionAxiomName2 = "iterator-injectivity-axiom2-" + v->name + "-" + statement->location;
                    auto inductionAxiomNameShort1 = "Ind1Injec-" + v->name + "-" + statement->location;
                    auto inductionAxiomNameShort2 = "Ind2Injec-" + v->name + "-" + statement->location;

                    // IH1(arg): v(l(it1)) < v(l(arg))
                    auto inductionHypothesis1 = [&](std::shared_ptr<const logic::Term> arg)
                    {
                        auto lStartArg = timepointForLoopStatement(statement, arg);
                        return logic::Theory::intLess(toTerm(v,lStartIt1), toTerm(v,lStartArg));
                    };
                    // IH2(arg): v(l(it2)) < v(l(arg))
                    auto inductionHypothesis2 = [&](std::shared_ptr<const logic::Term> arg)
                    {
                        auto lStartArg = timepointForLoopStatement(statement, arg);
                        return logic::Theory::intLess(toTerm(v,lStartIt2), toTerm(v,lStartArg));
                    };

                    std::vector<std::shared_ptr<const logic::Symbol>> freeVars1 = {it1Symbol};
                    std::vector<std::shared_ptr<const logic::Symbol>> freeVars2 = {it2Symbol};
                    if (twoTraces)
                    {
                        auto trSymbol = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        freeVars1.push_back(trSymbol);
                        freeVars2.push_back(trSymbol);
                    }

                    logic::addInductionAxiom1(inductionAxiomName1, inductionAxiomNameShort1, inductionHypothesis1, freeVars1, items);
                    logic::addInductionAxiom1(inductionAxiomName2, inductionAxiomNameShort2, inductionHypothesis2, freeVars2, items);

                    // PART 2: Add trace lemma
                    /* Premise:
                     * and
                     *    forall it.
                     *       =>
                     *          it<n
                     *          v(l(s(it)))=v(l(it))+1
                     *    it1<n
                     *    it2<n
                     *    it1!=it2
                     */
                    auto premise =
                        logic::Formulas::conjunction({
                            logic::Formulas::universal({itSymbol},
                                logic::Formulas::implication(
                                    logic::Theory::natSub(it,n),
                                    logic::Formulas::equality(
                                        toTerm(v,lStartSuccOfIt),
                                        logic::Theory::intAddition(
                                            toTerm(v,lStartIt),
                                            logic::Theory::intConstant(1)
                                        )
                                    )
                                )
                            ),
                            logic::Theory::natSub(it1,n),
                            logic::Theory::natSub(it2,n),
                            logic::Formulas::disequality(it1,it2)
                        });
                    
                    // Conclusion: v(l(it1))!=v(l(it2))
                    auto conclusion =
                        logic::Formulas::disequality(
                            toTerm(v,lStartIt1),
                            toTerm(v,lStartIt2)
                        );
                    
                    // forall enclosingIterators. forall it1,it2. (premise => conclusion)
                    auto bareLemma =
                        logic::Formulas::universal(enclosingIteratorsSymbols(statement),
                            logic::Formulas::universal({it1Symbol,it2Symbol},
                                logic::Formulas::implication(premise, conclusion)
                            )
                        );

                    if (twoTraces)
                    {
                        auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        bareLemma = logic::Formulas::universal({tr}, bareLemma);
                    }
                    
                    auto name = "iterator-injectivity-" + v->name + "-" + statement->location;
                    items.push_back(std::make_shared<logic::Lemma>(bareLemma, name));
                }
            }
        }
    }
}
