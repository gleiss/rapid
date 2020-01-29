#include "Term.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace logic {

    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector<std::shared_ptr<const logic::Term>>& t){ostr << "not implemented"; return ostr;}
    std::ostream& operator<<(std::ostream& ostr, const std::vector<std::shared_ptr<const logic::LVariable>>& v){ostr << "not implemented"; return ostr;}

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

    
# pragma mark - Terms
    
    std::shared_ptr<const LVariable> Terms::var(std::shared_ptr<const Symbol> symbol)
    {
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
    
    std::shared_ptr<const FuncTerm> Terms::func(std::shared_ptr<const Symbol> symbol, std::vector<std::shared_ptr<const Term>> subterms)
    {
        return std::shared_ptr<const FuncTerm>(new FuncTerm(symbol, subterms));
    }
}

