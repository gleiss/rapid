#include "TheoryAxioms.hpp"

#include <memory>
#include <string>
#include <vector>
#include <cassert>

#include "Signature.hpp"
#include "Theory.hpp"
#include "Options.hpp"
#include "Output.hpp"

#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

using namespace logic;

namespace analysis {
    
#pragma mark - High level methods
    
    std::vector<std::shared_ptr<const logic::Axiom>> TheoryAxioms::generate()
    {
        std::vector<std::shared_ptr<const logic::Axiom>> axioms;

        // generateTheoryAxiom1(axioms);
        // generateTheoryAxiom2(axioms);
        generateTheoryAxiom3(axioms);
        return axioms;
    }

    void TheoryAxioms::generateTheoryAxiom1(std::vector<std::shared_ptr<const logic::Axiom>>& axioms)
    {
        auto xSym = logic::Signature::varSymbol("xInt", logic::Sorts::intSort());
        auto ySym = logic::Signature::varSymbol("yInt", logic::Sorts::intSort());
        auto zSym = logic::Signature::varSymbol("zInt", logic::Sorts::intSort());
        auto x = logic::Terms::var(xSym);
        auto y = logic::Terms::var(ySym);
        auto z = logic::Terms::var(zSym);

        // forall x y z. (x + z) - (y + z) = (x - y)
        auto axiom =
            logic::Formulas::universal({xSym,ySym,zSym},
                logic::Formulas::equality(
                    logic::Theory::intSubtraction(
                        logic::Theory::intAddition(x,z),
                        logic::Theory::intAddition(y,z)),
                    logic::Theory::intSubtraction(x,y)
                )
            );
        axioms.push_back(std::make_shared<const logic::Axiom>(axiom, "Theory axiom: forall x y z. (x + z) - (y + z) = x - y"));
    }

    void TheoryAxioms::generateTheoryAxiom2(std::vector<std::shared_ptr<const logic::Axiom>>& axioms)
    {
        auto xSym = logic::Signature::varSymbol("xInt", logic::Sorts::intSort());
        auto ySym = logic::Signature::varSymbol("yInt", logic::Sorts::intSort());
        auto zSym = logic::Signature::varSymbol("zInt", logic::Sorts::intSort());
        auto x = logic::Terms::var(xSym);
        auto y = logic::Terms::var(ySym);
        auto z = logic::Terms::var(zSym);

        // forall x y z. (x + y) + z = (x + z) + y
        auto axiom =
            logic::Formulas::universal({xSym,ySym,zSym},
                logic::Formulas::equality(
                    logic::Theory::intAddition(
                        logic::Theory::intAddition(x,y),
                        z
                    ),
                    logic::Theory::intAddition(
                        logic::Theory::intAddition(x,z),
                        y
                    )
                )
            );

        axioms.push_back(std::make_shared<const logic::Axiom>(axiom, "Theory axiom: forall x y z. (x + y) + z) = (x + z) + y"));
    }

    void TheoryAxioms::generateTheoryAxiom3(std::vector<std::shared_ptr<const logic::Axiom>>& axioms)
    {
        auto xSym = logic::Signature::varSymbol("xInt", logic::Sorts::intSort());
        auto ySym = logic::Signature::varSymbol("yInt", logic::Sorts::intSort());
        auto x = logic::Terms::var(xSym);
        auto y = logic::Terms::var(ySym);

        // forall x,y. (x<y => x+1<=y)
        auto axiom =
            logic::Formulas::universal({xSym,ySym},
                logic::Formulas::implication(
                    logic::Theory::intLess(x,y),
                    logic::Theory::intLessEqual(
                        logic::Theory::intAddition(
                            x,
                            logic::Theory::intConstant(1)
                        ),
                        y
                    )
                )
            );

        axioms.push_back(std::make_shared<const logic::Axiom>(axiom, "Theory axiom: forall x,y. (x<y => x+1<=y)"));
    }
}   
