#include "Signature.hpp"

#include <algorithm>

#include <iostream>

namespace logic {
    
#pragma mark - Symbol

    std::string Symbol::declareSymbolSMTLIB() const
    {
        if (!noDeclaration)
        {
            // hack since Vampire currently doesn't add the sub-predicate itself
            // declare and define the symbol time_sub
            if (name == "time_sub")
            {
                std::string ret = "(declare-fun Sub (Time Time) Bool)\n";
                ret += "(assert (forall ((it Time)) (Sub it (s it))) )\n";
                ret += "(assert (forall ((it1 Time)(it2 Time)) (=> (Sub it1 it2) (Sub it1 (s it2))) ))\n";
                return ret;
            }
            if (argSorts.size() == 0)
            {
                return "(declare-const " + toSMTLIB() + " " + rngSort->toSMTLIB() + ")\n";
            }
            else
            {
                std::string res = "(declare-fun " + toSMTLIB() + " (";
                for (int i=0; i < argSorts.size(); ++i)
                {
                    res += argSorts[i]->toSMTLIB() + (i+1 == argSorts.size() ? "" : " ");
                }
                res += ") " + rngSort->toSMTLIB() + ")\n";
                return res;
            }
        }
        else
        {
            return "";
        }
    }
    
    std::string Symbol::toSMTLIB() const
    {
        if (name == "int_plus")
        {
            return "+";
        }
        else if (name == "int_minus")
        {
            return "-";
        }
        else if (name == "int_multiply")
        {
            return "*";
        }
        else if (name == "int_quotient_e")
        {
            return "div";
        }
        else if (name == "int_unary_minus")
        {
            return "-";
        }
        else if (name == "int_greater")
        {
            return ">";
        }
        else if (name == "int_greater_eq")
        {
            return ">=";
        }
        else if (name == "int_less")
        {
            return "<";
        }
        else if (name == "int_less_eq")
        {
            return "<=";
        }
        else if (name == "array_select")
        {
            assert(false); // TODO: not implemented yet
            return "$select";
        }
        else if (name == "array_store")
        {
            assert(false); // TODO: not implemented yet
            return "$store";
        }
        else if (name == "bool_true")
        {
            return "true";
        }
        else if (name == "bool_false")
        {
            return "false";
        }
        else if (name == "time_zero")
        {
            return "zero";
        }
        else if (name == "time_succ")
        {
            return "s";
        }
        else if (name == "time_pre")
        {
            return "p";
        }
        else if (name == "time_sub")
        {
            return "Sub";
        }
        // if non-negative integer constant
        else if (std::all_of(name.begin(), name.end(), ::isdigit))
        {
            return name;
        }
        // if negative integer constant
        else if (name[0]=='-' && std::all_of(name.begin()+1, name.end(), ::isdigit))
        {
            // need to encode negative integer as unary minus of positive integer
            return  "(- " + name.substr(1,name.size()-1) + ")";
        }
        else
        {
            return name;
        }
    }
    
#pragma mark - Signature
    
    std::unordered_map<std::string, std::unique_ptr<const Symbol>> Signature::_signature;
    
    const Symbol* Signature::fetch(std::string name)
    {
        auto it = _signature.find(name);
        assert(it != _signature.end());
        
        return it->second.get();
    }
    
    const Symbol* Signature::add(std::string name, std::vector<const Sort*> argSorts, const Sort* rngSort, bool noDeclaration)
    {
        // there must be no symbol with name name already added
        assert(_signature.count(name) == 0);
        
        auto pair = _signature.insert(std::make_pair(name,std::unique_ptr<Symbol>(new Symbol(name, argSorts, rngSort, noDeclaration))));
        assert(pair.second); // must succeed since we checked that no such symbols existed before the insertion
        return pair.first->second.get();
    }
    
    const Symbol* Signature::fetchOrAdd(std::string name, std::vector<const Sort*> argSorts, const Sort* rngSort, bool noDeclaration)
    {
        
        auto pair = _signature.insert(std::make_pair(name, std::unique_ptr<Symbol>(new Symbol(name, argSorts, rngSort, noDeclaration))));
        
        auto symbol = pair.first->second.get();
        // if a symbol with the name already exist, make sure it has the same sorts
        if (!pair.second)
        {
            assert(argSorts.size() == symbol->argSorts.size());
            for (int i=0; i < argSorts.size(); ++i)
            {
                assert(argSorts[i] == symbol->argSorts[i]);
            }
            assert(rngSort = symbol->rngSort);
            assert(noDeclaration == symbol->noDeclaration);
        }
        return symbol;
    }
    
    std::shared_ptr<const Symbol> Signature::varSymbol(std::string name, const Sort* rngSort)
    {
        // there must be no symbol with name name already added
        assert(_signature.count(name) == 0);
        
        return std::shared_ptr<Symbol>(new Symbol(name, rngSort, true));
    }

}
