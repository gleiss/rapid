#include "Problem.hpp"
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
        for (const auto& symbol : Signature::signature())
        {
            ostr << symbol->declareSymbolSMTLIB();
        }
        
        // output each axiom
        for (const auto& axiom : axioms)
        {
            ostr << axiom.second->declareSMTLIB(axiom.first) << std::endl;
        }

        // output each lemma
        for (const auto& lemma : lemmas)
        {
            ostr << lemma.second->declareSMTLIB(lemma.first) << std::endl;
        }
        
        // output conjecture
        assert(conjecture != nullptr);
        ostr << conjecture->declareSMTLIB("conjecture", true) << std::endl;
    }

}
