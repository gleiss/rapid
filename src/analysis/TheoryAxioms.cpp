#include "TheoryAxioms.hpp"

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

        if (!util::Configuration::instance().nativeNat())
        {
            // add an axiomatization of Sub for natural numbers.
            // NOTE: this axiomatization should be the same as the one added by Vampire internally for Sub, so that fair comparisons can be made.
            addZeroSmallestElementAxiom(axioms);
            addDefineSubEqAxiom(axioms);
            addMonotonicityAxiom(axioms);
            addTransitivityAxioms(axioms);
            addTotalityAxiom(axioms);
            addDisjointnessAxioms(axioms);
        }

        return axioms;
    }

    // forall x. 0 < s(x)
    void TheoryAxioms::addZeroSmallestElementAxiom(std::vector<std::shared_ptr<const logic::Axiom>>& axioms)
    {
        auto xSym = logic::Signature::varSymbol("xNat", logic::Sorts::natSort());
        auto x = logic::Terms::var(xSym);

        auto axiom =
            logic::Formulas::universal({xSym},
                logic::Theory::natSub(
                    logic::Theory::natZero(),
                    logic::Theory::natSucc(x)
                )
            );

        axioms.push_back(std::make_shared<const logic::Axiom>(axiom, "Theory axiom (zero smallest element Nat): forall x. 0 < s(x)"));
    }

    // forall x,y. x<s(y) <=> (x=y or x<y)
    void TheoryAxioms::addDefineSubEqAxiom(std::vector<std::shared_ptr<const logic::Axiom>>& axioms)
    {
        auto xSym = logic::Signature::varSymbol("xNat", logic::Sorts::natSort());
        auto ySym = logic::Signature::varSymbol("yNat", logic::Sorts::natSort());
        auto x = logic::Terms::var(xSym);
        auto y = logic::Terms::var(ySym);

        // clause 1: x!<s(y) or x=y or x<y
        auto clause1 =
            logic::Formulas::universal({xSym,ySym},
                logic::Formulas::disjunction({
                    logic::Formulas::negation(
                        logic::Theory::natSub(
                            x,
                            logic::Theory::natSucc(y)
                        )
                    ),
                    logic::Formulas::equality(x,y),
                    logic::Theory::natSub(x,y)
                })
            );
        axioms.push_back(std::make_shared<const logic::Axiom>(clause1, "Theory axiom (subeq Nat): x!<s(y) or x=y or x<y"));

        // clause 2: x!=y or x<s(y), simplified to x<s(x)
        auto clause2 =
            logic::Formulas::universal({xSym},
                logic::Theory::natSub(
                    x,
                    logic::Theory::natSucc(x)
                )
            );
        axioms.push_back(std::make_shared<const logic::Axiom>(clause2, "Theory axiom (subeq Nat): x<s(x)"));

        // clause 3: x!<y or x<s(y)
        auto clause3 =
            logic::Formulas::universal({xSym,ySym},
                logic::Formulas::disjunction({
                    logic::Formulas::negation(
                        logic::Theory::natSub(x,y)
                    ),
                    logic::Theory::natSub(
                        x,
                        logic::Theory::natSucc(y)
                    )
                })
            );
        axioms.push_back(std::make_shared<const logic::Axiom>(clause3, "Theory axiom (subeq Nat): x!<y or x<s(y)"));
    }

    // forall x,y. x<y <=> s(x)<s(y)
    void TheoryAxioms::addMonotonicityAxiom(std::vector<std::shared_ptr<const logic::Axiom>>& axioms)
    {
        auto xSym = logic::Signature::varSymbol("xNat", logic::Sorts::natSort());
        auto ySym = logic::Signature::varSymbol("yNat", logic::Sorts::natSort());
        auto x = logic::Terms::var(xSym);
        auto y = logic::Terms::var(ySym);
        auto sx = logic::Theory::natSucc(x);
        auto sy = logic::Theory::natSucc(y);

        // clause 1: x!<y or s(x)<s(y)
        auto clause1 =
            logic::Formulas::universal({xSym,ySym},
                logic::Formulas::disjunction({
                    logic::Formulas::negation(
                        logic::Theory::natSub(x,y)
                    ),
                    logic::Theory::natSub(sx,sy)
                })
            );
        axioms.push_back(std::make_shared<const logic::Axiom>(clause1, "Theory axiom (monotonicity Nat): x!<y or s(x)<s(y)"));

        // clause 2: s(x)!<s(y) or x<y
        auto clause2 =
            logic::Formulas::universal({xSym,ySym},
                logic::Formulas::disjunction({
                    logic::Formulas::negation(
                        logic::Theory::natSub(sx,sy)
                    ),
                    logic::Theory::natSub(x,y)
                })
            );
        axioms.push_back(std::make_shared<const logic::Axiom>(clause2, "Theory axiom (monotonicity Nat): s(x)!<s(y) or x<y"));
    }

    void TheoryAxioms::addTransitivityAxioms(std::vector<std::shared_ptr<const logic::Axiom>>& axioms)
    {
        auto xSym = logic::Signature::varSymbol("xNat", logic::Sorts::natSort());
        auto ySym = logic::Signature::varSymbol("yNat", logic::Sorts::natSort());
        auto zSym = logic::Signature::varSymbol("zNat", logic::Sorts::natSort());
        auto x = logic::Terms::var(xSym);
        auto y = logic::Terms::var(ySym);
        auto z = logic::Terms::var(zSym);

        auto strict1 = logic::Formulas::negation(logic::Theory::natSub(x,y));
        auto strict2 = logic::Formulas::negation(logic::Theory::natSub(y,z));
        auto nonStrict1 = logic::Formulas::negation(logic::Theory::natSubEq(x,y));
        auto nonStrict2 = logic::Formulas::negation(logic::Theory::natSubEq(y,z));
        auto strict3 = logic::Theory::natSub(x,z);
        auto nonStrict3 = logic::Theory::natSubEq(x,z);

        // variant 1: forall x,y,z. x!<y or y!<z or x<z
        auto clause1 =
            logic::Formulas::universal({xSym,ySym,zSym},
                logic::Formulas::disjunction({
                    strict1,
                    strict2,
                    strict3
                })
            );
        axioms.push_back(std::make_shared<const logic::Axiom>(clause1, "Theory axiom (transitivity1 Nat): forall x,y,z. x!<y or y!<z or x<z"));

        // variant 2: forall x,y,z. x!<s(y) or y!<z or x<z
        auto clause2 =
            logic::Formulas::universal({xSym,ySym,zSym},
                logic::Formulas::disjunction({
                    nonStrict1,
                    strict2,
                    strict3
                })
            );
        axioms.push_back(std::make_shared<const logic::Axiom>(clause2, "Theory axiom (transitivity2 Nat): forall x,y,z. x!<s(y) or y!<z or x<z"));

        // variant 3: forall x,y,z. x!<y or y!<s(z) or x<z
        auto clause3 =
            logic::Formulas::universal({xSym,ySym,zSym},
                logic::Formulas::disjunction({
                    strict1,
                    nonStrict2,
                    strict3
                })
            );
        axioms.push_back(std::make_shared<const logic::Axiom>(clause3, "Theory axiom (transitivity3 Nat): forall x,y,z. x!<y or y!<s(z) or x<z"));

        // variant 4: forall x,y,z. x!<s(y) or y!<s(z) or x<s(z)
        auto clause4 =
            logic::Formulas::universal({xSym,ySym,zSym},
                logic::Formulas::disjunction({
                    nonStrict1,
                    nonStrict2,
                    nonStrict3
                })
            );
        axioms.push_back(std::make_shared<const logic::Axiom>(clause4, "Theory axiom (transitivity4 Nat): forall x,y,z. x!<s(y) or y!<s(z) or x<s(z)"));
    }

    void TheoryAxioms::addTotalityAxiom(std::vector<std::shared_ptr<const logic::Axiom>>& axioms)
    {
        auto xSym = logic::Signature::varSymbol("xNat", logic::Sorts::natSort());
        auto ySym = logic::Signature::varSymbol("yNat", logic::Sorts::natSort());
        auto x = logic::Terms::var(xSym);
        auto y = logic::Terms::var(ySym);

        // forall x,y. (x<y or x=y or x>y)
        auto axiom =
            logic::Formulas::universal({xSym,ySym},
                logic::Formulas::disjunction({
                    logic::Theory::natSub(x,y),
                    logic::Formulas::equality(x,y),
                    logic::Theory::natSub(y,x)
                })
            );

        axioms.push_back(std::make_shared<const logic::Axiom>(axiom, "Theory axiom (totality Nat): forall x,y. (x<y or x=y or x>y)"));
    }

    void TheoryAxioms::addDisjointnessAxioms(std::vector<std::shared_ptr<const logic::Axiom>>& axioms)
    {
        auto xSym = logic::Signature::varSymbol("xNat", logic::Sorts::natSort());
        auto x = logic::Terms::var(xSym);

        // Clause 1: x!<y or x!=y, simplified to x!<x
        auto axiom =
            logic::Formulas::universal({xSym},
                logic::Formulas::negation(
                    logic::Theory::natSub(x,x)
                )
            );
        axioms.push_back(std::make_shared<const logic::Axiom>(axiom, "Theory axiom (disjointness Nat): forall x. x!<x"));

        // NOTE: all other clauses for this axiom are already subsumed by other axioms
    }
}
