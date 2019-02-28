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
    
    std::vector<std::shared_ptr<const logic::Formula>> TheoryAxioms::generate()
    {
        std::vector<std::shared_ptr<const logic::Formula>> lemmas;        

        // generateTheoryAxiom1(lemmas);     
        generateTheoryAxiom2(lemmas);     
        return lemmas;
    }

#pragma mark - Theory Axiom 1
    void TheoryAxioms::generateTheoryAxiom1(std::vector<std::shared_ptr<const logic::Formula>>& lemmas)    
    {
        // forall x y z. (x + z) - (y + z) = (x - y)

        auto xSym = logic::Signature::varSymbol("xInt", logic::Sorts::intSort());
        auto x = logic::Terms::var(xSym);
        auto ySym = logic::Signature::varSymbol("yInt", logic::Sorts::intSort());
        auto y = logic::Terms::var(ySym);
        auto zSym = logic::Signature::varSymbol("zInt", logic::Sorts::intSort());
        auto z = logic::Terms::var(zSym);

        auto s1 = logic::Theory::intAddition(x,z);
        auto s2 = logic::Theory::intAddition(y,z);
        auto lhs = logic::Theory::intSubtraction(s1,s2);
        auto rhs = logic::Theory::intSubtraction(x,y);

        auto label = "Theory axiom: forall x y z. (x + z) - (y + z) = x - y";
        auto bareLemma = logic::Formulas::equality(lhs,rhs);
        auto lemma = logic::Formulas::universal({xSym,ySym,zSym},bareLemma,label);


        lemmas.push_back(lemma);
                
    }

    #pragma mark - Theory Axiom 1
    void TheoryAxioms::generateTheoryAxiom2(std::vector<std::shared_ptr<const logic::Formula>>& lemmas)    
    {
        // forall x y z. (x + y) + z = (x + z) + y

        auto xSym = logic::Signature::varSymbol("xInt", logic::Sorts::intSort());
        auto x = logic::Terms::var(xSym);
        auto ySym = logic::Signature::varSymbol("yInt", logic::Sorts::intSort());
        auto y = logic::Terms::var(ySym);
        auto zSym = logic::Signature::varSymbol("zInt", logic::Sorts::intSort());
        auto z = logic::Terms::var(zSym);

        auto sxy = logic::Theory::intAddition(x,y);
        auto lhs = logic::Theory::intAddition(sxy,z);

        auto sxz = logic::Theory::intAddition(x,z);
        auto rhs = logic::Theory::intAddition(sxz,y);        

        auto label = "Theory axiom: forall x y z. (x + y) + z) = (x + z) + y";
        auto bareLemma = logic::Formulas::equality(lhs,rhs);
        auto lemma = logic::Formulas::universal({xSym,ySym,zSym},bareLemma,label);


        lemmas.push_back(lemma);
                
    }
}   
