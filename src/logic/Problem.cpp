#include "Problem.hpp"

#include <iostream>
#include <cassert>

#include "Output.hpp"

namespace logic {
    
    void Problem::outputSMTLIB(std::ostream& ostr)
    {        
        // output sort declarations
        for(const auto& pair : Sorts::nameToSort())
        {
            ostr << declareSortSMTLIB(*pair.second);
        }
        
        // output symbol definitions
        for (const auto& pairStringSymbol : Signature::signature())
        {
            ostr << pairStringSymbol.second->declareSymbolSMTLIB();
        }
        
        // output each axiom
        for (const auto& axiom : axioms)
        {
            ostr << "\n(assert\n" << axiom->toSMTLIB(3) + "\n)\n";
        }

        // output each lemma
        for (const auto& lemma : lemmas)
        {
            // TODO: improve handling for lemmas:
            // custom smtlib-extension
            ostr << "\n(assert\n" << lemma->toSMTLIB(3) + "\n)\n";
        }
        
        // output conjecture
        assert(conjecture != nullptr);
        ostr << "\n(assert-not\n" << conjecture->toSMTLIB(3) + "\n)\n";
    }
}
