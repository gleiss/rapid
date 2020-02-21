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

    bool operator==(const Term& t1, const Term& t2)
    {
        if (t1.type() != t2.type())
        {
            return false;
        }
        if (t1.type() == Term::Type::Variable)
        {
            return *t1.symbol == *t2.symbol;
        }
        else
        {
            assert(t1.type() == Term::Type::FuncTerm);
            auto f1 = static_cast<const FuncTerm&>(t1);
            auto f2 = static_cast<const FuncTerm&>(t2);
            if (*f1.symbol != *f2.symbol || f1.subterms.size() != f2.subterms.size())
            {
                return false;
            }
            for (unsigned i = 0; i < f1.subterms.size(); i++)
            {
                if (*f1.subterms[i] != *f2.subterms[i])
                {
                    return false;
                }
            }
            return true;
        }
    }
    bool operator!=(const Term& t1, const Term& t2)
    {
        return !(t1 == t2);
    }

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
        auto symbol = Signature::fetchOrAdd(name, subtermSorts, sort, false, noDeclaration);
        return std::shared_ptr<const FuncTerm>(new FuncTerm(symbol, subterms));
    }
    
    std::shared_ptr<const FuncTerm> Terms::func(std::shared_ptr<const Symbol> symbol, std::vector<std::shared_ptr<const Term>> subterms)
    {
        return std::shared_ptr<const FuncTerm>(new FuncTerm(symbol, subterms));
    }
}

