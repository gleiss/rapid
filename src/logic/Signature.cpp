#include "Signature.hpp"

#include <algorithm>

namespace logic {
    
#pragma mark - Symbol

    std::string Symbol::declareSymbolSMTLIB() const
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
        if (interpreted)
        {
            return ""; // don't  need to declare symbols, which are already known to TPTP-solvers.
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
            assert(!interpreted);
            return name;
        }
    }
    
#pragma mark - Signature
    
    std::unordered_set<std::unique_ptr<Symbol>, SymbolPtrHash, SymbolPtrEquality> Signature::_signature;

    Symbol* Signature::fetchOrDeclare(std::string name, Sort* rngSort, bool interpreted, bool colored)
    {
        auto pair = _signature.insert(std::unique_ptr<Symbol>(new Symbol(name, rngSort, interpreted, colored)));
        return pair.first->get();
    }
    
    Symbol* Signature::fetchOrDeclare(std::string name, std::initializer_list<Sort*> argSorts, Sort* rngSort, bool interpreted, bool colored)
    {
        auto pair = _signature.insert(std::unique_ptr<Symbol>(new Symbol(name, argSorts, rngSort, interpreted, colored)));
        return pair.first->get();
    }

}
