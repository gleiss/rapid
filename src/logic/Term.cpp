#include <algorithm>

#include "Term.hpp"

namespace logic {

  unsigned LVariable::freshId = 0;

    std::string LVariable::toSMTLIB() const
    {
        return symbol->name;
    }
    
    std::string LVariable::prettyString() const
    {
        return symbol->name;
    }

    std::string FuncTerm::toSMTLIB() const
    {
        if (subterms.size() == 0)
        {
            return symbol->toSMTLIB();
        }
        else
        {
            std::string str = "(" + symbol->toSMTLIB() + " ";
            for (unsigned i = 0; i < subterms.size(); i++)
            {
                str += subterms[i]->toSMTLIB();
                str += (i == subterms.size() - 1) ? ")" : " ";
            }
            return str;
        }
    }
    
    std::string FuncTerm::prettyString() const
    {
        if (subterms.size() == 0)
        {
            return symbol->toSMTLIB();
        }
        else
        {
            std::string str = symbol->toSMTLIB() + "(";
            for (unsigned i = 0; i < subterms.size(); i++) {
                str += subterms[i]->toSMTLIB();
                str += (i == subterms.size() - 1) ? ")" : ",";
            }
            return str;
        }
    }

    bool compareLVarPointers(const LVariable* p1, const LVariable* p2)
    {
        return p1->id < p2->id;
    }
    
    bool eqLVarPointers(const LVariable* p1, const LVariable* p2) {
        return p1->id == p2->id;
    }

//    std::vector<std::shared_ptr<const LVariable>> LVariable::freeVariables() const
//    {
//        return { std::make_shared<const LVariable>(this) };
//    }
//
//    std::vector<std::shared_ptr<const LVariable>> FuncTerm::freeVariables() const {
//        std::vector<std::shared_ptr<const LVariable>> freeVars;
//        // collect free vars from all subterms
//        for (const auto& subterm : subterms)
//        {
//            auto freeVarsSubterm = subterm->freeVariables();
//            freeVars.insert(freeVars.end(), freeVarsSubterm.begin(), freeVarsSubterm.end());
//        }
//        // sort and remove duplicates
//        std::sort(freeVars.begin(), freeVars.end(), compareLVarPointers);
//        freeVars.erase( unique(freeVars.begin(), freeVars.end(), eqLVarPointers), freeVars.end());
//
//        return freeVars;
//    }
//
//    std::vector<std::shared_ptr<const LVariable>> PredTerm::freeVariables() const
//    {
//        std::vector<std::shared_ptr<const LVariable>> freeVars;
//        // collect free vars from all subterms
//        for (const auto& subterm : subterms)
//        {
//            auto freeVarsSubterm = subterm->freeVariables();
//            freeVars.insert(freeVars.end(), freeVarsSubterm.begin(), freeVarsSubterm.end());
//        }
//        // sort and remove duplicates
//        std::sort(freeVars.begin(), freeVars.end(), compareLVarPointers);
//        freeVars.erase( unique(freeVars.begin(), freeVars.end(), eqLVarPointers), freeVars.end());
//
//        return freeVars;
//    }
    
# pragma mark - Terms
    
    // use this functions for quantified variables, which don't need to be declared (since they will be declared by the quantifiers)
    std::shared_ptr<const LVariable> Terms::var(std::string name, const Sort* sort)
    {
        auto symbol = Signature::fetchOrAdd(name, {}, sort, true);
        return std::shared_ptr<const LVariable>(new LVariable(symbol));
    }

    std::shared_ptr<const FuncTerm> Terms::func(std::string name, std::vector<std::shared_ptr<const Term>> subterms, const Sort* sort, bool noDeclaration)
    {
        std::vector<const Sort*> subtermSorts;
        for (const auto& subterm : subterms)
        {
            subtermSorts.push_back(subterm->symbol->rngSort);
        }
        auto symbol = Signature::fetchOrAdd(name, subtermSorts, sort, noDeclaration);
        return std::shared_ptr<const FuncTerm>(new FuncTerm(symbol, subterms));
    }
}

