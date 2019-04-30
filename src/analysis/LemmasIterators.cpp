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

                // PART 1: Add induction-axiom
                auto inductionAxiom = 
                    logic::Formulas::universal({xSymbol},
                        logic::inductionAxiom1(inductionHypothesis)
                    );
                if (v->isArray)
                {
                    inductionAxiom = logic::Formulas::universal({posSymbol}, inductionAxiom);
                }
                if (twoTraces)
                {
                    auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                    inductionAxiom = logic::Formulas::universal({tr}, inductionAxiom);
                }
                auto axiomName = "iterator-intermediateValue-axiom-" + v->name + "-" + statement->location;
                items.push_back(std::make_shared<logic::Axiom>(inductionAxiom, axiomName));

                // PART 2: Add trace lemma
                // Premise: v(l(zero))<=x     & x<v(l(n))     & forall it. v(l(s(it)))=v(l(it))+1         or
                //          v(l(zero),pos)<=x & x<v(l(n),pos) & forall it. v(l(s(it)),pos)=v(l(it),pos)+1
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
                            logic::Formulas::equality(
                                v->isArray ? toTerm(v,lStartSuccOfIt,pos) : toTerm(v,lStartSuccOfIt),
                                logic::Theory::intAddition(
                                    v->isArray ?  toTerm(v,lStartIt,pos) : toTerm(v,lStartIt),
                                    logic::Theory::intConstant(1)
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
        
        // add lemma for each intVar and each intArrayVar
        for (const auto& v : locationToActiveVars.at(locationSymbolForStatement(statement)->name))
        {
            if (!v->isConstant)
            {
                if (!v->isArray) // We assume that loop counters are not array elements and therefore only add these lemmas for non-array-vars
                {
                    // Premise: (forall it. v(l(s(it)))=v(l(it))+1)) & v(l(it1))=v(l(it2))
                    auto premise =
                        logic::Formulas::conjunction({
                            logic::Formulas::universal({itSymbol},
                                logic::Formulas::equality(
                                    toTerm(v,lStartSuccOfIt),
                                    logic::Theory::intAddition(
                                        toTerm(v,lStartIt),
                                        logic::Theory::intConstant(1)
                                    )
                                )
                            ),
                            logic::Formulas::equality(
                                toTerm(v,lStartIt1),
                                toTerm(v,lStartIt2)
                            )
                        });
                    
                    // Conclusion: it1 = it2
                    auto conclusion = logic::Formulas::equality(it1,it2);
                    
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
