#include "LemmasOther.hpp"

#include <cassert>

#include "Formula.hpp"
#include "Theory.hpp"
#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

namespace analysis {

    void AtLeastOneIterationLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        auto itSymbol = iteratorSymbol(statement);
        auto it = iteratorTermForLoop(statement);

        auto lStartZero = timepointForLoopStatement(statement, logic::Theory::natZero());

        for (unsigned traceNumber = 1; traceNumber < numberOfTraces+1; traceNumber++)
        {
            auto trace = traceTerm(traceNumber);
            auto n = lastIterationTermForLoop(statement, numberOfTraces, trace);

            auto name = "atLeastOneIteration-" + statement->location + (numberOfTraces > 1 ? ("-" + trace->symbol->name) : "");

            // forall enclosingIterators. (Cond(l(0)) => exists it. s(it)=n)
            auto bareLemma =
                logic::Formulas::universal(enclosingIteratorsSymbols(statement),
                    logic::Formulas::implication(
                        inlinedVarValues.toInlinedFormula(statement, statement->condition, lStartZero, trace),
                        logic::Formulas::existential({itSymbol},
                            logic::Formulas::equality(logic::Theory::natSucc(it),n)
                        )
                    )
                );

            std::vector<std::string> fromItems;
            for (auto& item : programSemantics)
            {
                fromItems.push_back(item->name);
            }
            items.push_back(std::make_shared<logic::Lemma>(bareLemma, name, logic::ProblemItem::Visibility::Implicit, fromItems));
        }
    }

    void OrderingSynchronizationLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        // assert(numberOfTraces > 1);

        // auto t1 = traceTerm(1);
        // auto t2 = traceTerm(2);

        // auto iSymbol = iteratorSymbol(statement);
        // auto it = iteratorTermForLoop(statement);
        // auto it1Symbol = logic::Signature::varSymbol("it1", logic::Sorts::natSort());
        // auto it1 = logic::Terms::var(it1Symbol);
        // auto it2Symbol = logic::Signature::varSymbol("it2", logic::Sorts::natSort());
        // auto it2 = logic::Terms::var(it2Symbol);

        // auto lStartIt = timepointForLoopStatement(statement, it);
        // auto lStartZero = timepointForLoopStatement(statement, logic::Theory::natZero());
        // auto lStartSuccOfIt = timepointForLoopStatement(statement, logic::Theory::natSucc(it));
        // auto lStartIt1 = timepointForLoopStatement(statement, it1);
        // auto lStartIt2 = timepointForLoopStatement(statement, it2);

        // auto trSymbol = traceVarSymbol();
        // auto tr = traceVar();
        // auto tr3Symbol = logic::Signature::varSymbol("tr3", logic::Sorts::traceSort());
        // auto tr3 = logic::Terms::var(tr3Symbol);

        // // add lemma for each intVar
        // // Lemma:
        // // forall ((tr : Trace) (it1 : Nat) (it2 : Nat))
        // //    =>
        // //       and
        // //          it1 < it2
        // //          v(l(zero),t1) = v(l(zero),t2)
        // //          forall ((it : Nat))
        // //             v(l(s(it))) = v(l(it)) + 1
        // //       i(l(it1),tr) < i(l(it2),tr)
        // for (const auto& v : locationToActiveVars.at(locationSymbolForStatement(statement)->name))
        // {
        //     if (!v->isConstant)
        //     {
        //         if (!v->isArray)
        //         {
        //             // Premise: it1<it2 & v(l(zero),t1)=v(l(zero),t2) & forall it. v(l(s(it)))=v(l(it))+1
        //             auto premise =
        //                 logic::Formulas::conjunction({
        //                     logic::Theory::natSub(it1,it2),
        //                     logic::Formulas::equality(
        //                         toTerm(v,lStartZero,t1),
        //                         toTerm(v,lStartZero,t2)
        //                     ),
        //                     logic::Formulas::universal({tr3Symbol,iSymbol},
        //                         logic::Formulas::equality(
        //                             toTerm(v,lStartSuccOfIt,tr3),
        //                             logic::Theory::intAddition(
        //                                 toTerm(v,lStartIt,tr3),
        //                                 logic::Theory::intConstant(1)
        //                             )
        //                         )
        //                     )
        //                 });

        //             // Conclusion: i(l(it1),tr) < i(l(it2),tr)
        //             auto conclusion =
        //                 logic::Theory::intLess(
        //                     toTerm(v,lStartIt1,tr),
        //                     toTerm(v,lStartIt2,tr)
        //                 );

        //             // forall enclosingIterators. forall tr,it1,it2. (premise => conclusion)
        //             auto bareLemma =
        //                 logic::Formulas::universal(enclosingIteratorsSymbols(statement),
        //                     logic::Formulas::universal({trSymbol,it1Symbol,it2Symbol},
        //                         logic::Formulas::implication(premise,conclusion)
        //                     )
        //                 );

        //             auto name = "synchronization-orderings-" + v->name + "-" + statement->location;
        //             items.push_back(std::make_shared<logic::Lemma>(bareLemma, name));
        //         }
        //     }
        // }
    }
}
