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
                auto nameSuffix = "-" + v->name + "-" + statement->location;
                auto name = "iterator-intermediateValue" + nameSuffix;
                auto nameShort = "Intermed" + nameSuffix;
                auto inductionAxiomName = "induction-axiom-" + name;
                auto inductionAxiomNameShort = "Ax-" + nameShort;

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

                auto freeVarSymbols1 = enclosingIteratorsSymbols(statement);
                if (v->isArray)
                {
                    freeVarSymbols1.push_back(posSymbol);
                }
                if (twoTraces)
                {
                    auto trSymbol = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                    freeVarSymbols1.push_back(trSymbol);
                }
                auto freeVarSymbols2 = freeVarSymbols1;
                freeVarSymbols2.push_back(xSymbol);

                auto [inductionAxBCDef, inductionAxICDef,inductionAxiomConDef, inductionAxiom] = logic::inductionAxiom1(inductionAxiomName, inductionAxiomNameShort, inductionHypothesis, freeVarSymbols2);
                items.push_back(inductionAxBCDef);
                items.push_back(inductionAxICDef);
                items.push_back(inductionAxiomConDef);
                items.push_back(inductionAxiom);

                // PART 2: Add trace lemma
                std::vector<std::shared_ptr<const logic::Term>> freeVars1 = {};
                std::vector<std::shared_ptr<const logic::Term>> freeVars2 = {};
                for (const auto& symbol : freeVarSymbols1)
                {
                    freeVars1.push_back(logic::Terms::var(symbol));
                }
                for (const auto& symbol : freeVarSymbols2)
                {
                    freeVars2.push_back(logic::Terms::var(symbol));
                }

                // PART 2A: Add definition for dense
                auto dense = logic::Formulas::predicate("Dense-" + nameShort, freeVars1);
                // Dense_v: forall it. (it<n => v(l(s(it)))=v(l(it))+1)         , or
                //          forall it. (it<n => v(l(s(it)),pos)=v(l(it),pos)+1)
                auto denseFormula =
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
                    );
                auto denseDef = 
                    std::make_shared<logic::Definition>(
                        logic::Formulas::universal(freeVarSymbols1,
                            logic::Formulas::equivalence(
                                dense,
                                denseFormula
                            )
                        ), 
                        "Dense for " + name, 
                        logic::ProblemItem::Visibility::Implicit
                    );

                items.push_back(denseDef);

                // PART 2B: Add definition for premise
                auto premise = logic::Formulas::predicate("Prem-" + nameShort, freeVars2);
                // Premise: v(l(zero))<=x     & x<v(l(n))     & Dense_v         , or
                //          v(l(zero),pos)<=x & x<v(l(n),pos) & Dense_v
                auto premiseFormula =
                    logic::Formulas::conjunction({
                        logic::Theory::intLessEqual(
                            v->isArray ? toTerm(v,lStartZero,pos) : toTerm(v,lStartZero),
                            x
                        ),
                        logic::Theory::intLess(
                            x,
                            v->isArray ? toTerm(v,lStartN,pos) : toTerm(v,lStartN)
                        ),
                        dense
                    });
                auto premiseDef =
                    std::make_shared<logic::Definition>(
                        logic::Formulas::universal(freeVarSymbols2,
                            logic::Formulas::equivalence(
                                premise,
                                premiseFormula
                            )
                        ),
                        "Premise for " + name,
                        logic::ProblemItem::Visibility::Implicit
                    );

                items.push_back(premiseDef);

                // PART 2C: Add lemma
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
                auto lemma =
                    logic::Formulas::universal(freeVarSymbols2,
                        logic::Formulas::implication(premise,conclusion)
                    );
                std::vector<std::shared_ptr<const logic::ProblemItem>> fromItems = {inductionAxBCDef, inductionAxICDef, inductionAxiomConDef, inductionAxiom, denseDef, premiseDef};
                items.push_back(std::make_shared<logic::Lemma>(lemma, name, logic::ProblemItem::Visibility::Implicit, fromItems));
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
                    auto nameSuffix = "-" + v->name + "-" + statement->location;
                    auto name = "iterator-injectivity" + nameSuffix;
                    auto nameShort = "Injec" + nameSuffix;
                    auto inductionAxiomName = "induction-axiom-" + name;
                    auto inductionAxiomNameShort = "Ax-" + nameShort;

                    auto freeVarSymbols = enclosingIteratorsSymbols(statement);
                    if (twoTraces)
                    {
                        auto trSymbol = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        freeVarSymbols.push_back(trSymbol);
                    }

                    auto freeVarSymbolsInd = freeVarSymbols;
                    freeVarSymbolsInd.push_back(it1Symbol);

                    // PART 1: Add induction-axiom
                    // IH(arg): v(l(it1)) < v(l(arg))
                    auto inductionHypothesis = [&](std::shared_ptr<const logic::Term> arg)
                    {
                        auto lStartArg = timepointForLoopStatement(statement, arg);
                        return logic::Theory::intLess(toTerm(v,lStartIt1), toTerm(v,lStartArg));
                    };

                    auto [inductionAxBCDef, inductionAxICDef,inductionAxiomConDef, inductionAxiom] = logic::inductionAxiom1(inductionAxiomName, inductionAxiomNameShort, inductionHypothesis, freeVarSymbolsInd);
                    items.push_back(inductionAxBCDef);
                    items.push_back(inductionAxICDef);
                    items.push_back(inductionAxiomConDef);
                    items.push_back(inductionAxiom);

                    // PART 2: Add trace lemma

                    // PART 2A: Add definition for dense
                    // TODO: share definition dense between IntermediateValueTheorem and this lemma.
                    std::vector<std::shared_ptr<const logic::Term>> freeVars = {};
                    for (const auto& symbol : freeVarSymbols)
                    {
                        freeVars.push_back(logic::Terms::var(symbol));
                    }
                    auto dense = logic::Formulas::predicate("Dense-" + nameShort, freeVars);

                    // Dense_v: forall it. (it<n => v(l(s(it)))=v(l(it))+1)
                    auto denseFormula =
                        logic::Formulas::universal({itSymbol},
                            logic::Formulas::implication(
                                logic::Theory::natSub(it, n),
                                logic::Formulas::equality(
                                    toTerm(v,lStartSuccOfIt),
                                    logic::Theory::intAddition(
                                        toTerm(v,lStartIt),
                                        logic::Theory::intConstant(1)
                                    )
                                )
                            )
                        );
                    auto denseDef =
                        std::make_shared<logic::Definition>(
                            logic::Formulas::universal(freeVarSymbols,
                                logic::Formulas::equivalence(
                                    dense,
                                    denseFormula
                                )
                            ),
                            "Dense for " + name,
                            logic::ProblemItem::Visibility::Implicit
                        );

                    items.push_back(denseDef);

                    /* Premise:
                     *    and
                     *       Dense_v
                     *       it1<it2
                     *       it2<=n
                     */
                    auto premise =
                        logic::Formulas::conjunction({
                            dense,
                            logic::Theory::natSub(it1,it2),
                            logic::Theory::natSubEq(it2,n),
                        });
                    
                    // Conclusion: v(l(it1))!=v(l(it2))
                    auto conclusion =
                        logic::Formulas::disequality(
                            toTerm(v,lStartIt1),
                            toTerm(v,lStartIt2)
                        );
                    
                    // forall enclosingIterators. forall it1,it2. (premise => conclusion)
                    auto lemma =
                        logic::Formulas::universal(freeVarSymbols,
                            logic::Formulas::universal({it1Symbol,it2Symbol},
                                logic::Formulas::implication(premise, conclusion)
                            )
                        );

                    std::vector<std::shared_ptr<const logic::ProblemItem>> fromItems = {inductionAxBCDef, inductionAxICDef, inductionAxiomConDef, inductionAxiom, denseDef};
                    items.push_back(std::make_shared<logic::Lemma>(lemma, name, logic::ProblemItem::Visibility::Implicit, fromItems));
                }
            }
        }
    }
}
