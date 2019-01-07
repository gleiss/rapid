#include "Sort.hpp"

#include <cassert>
#include "Options.hpp"

namespace logic {
    
#pragma mark - Sort
    
    std::string Sort::toSMTLIB() const
    {
        if (name == "Int")
        {
            return "Int";
        }
        else if (name == "Bool")
        {
            return "Bool";
        }
        else
        {
            return name;
        }
    }
    
    std::string declareSortSMTLIB(const Sort& s)
    {
        if (s.toSMTLIB() == "Int" || s.toSMTLIB() == "Bool")
        {
            // SMTLIB already knows Int and Bool.
            return "";
        }
        else if (s.toSMTLIB() == "Nat")
        {
            return "(declare-datatypes ((Nat 0)) (( (zero) (s (p Nat)) )) )\n";
        }
        else if (s.toSMTLIB() == "Trace")
        {
            return "(declare-datatypes ((Trace 0)) (( (t1) (t2) )) )\n";
        }
        else
        {
            return "(declare-sort " + s.toSMTLIB() + " 0)\n";
        }
    }
    
    bool Sort::operator==(Sort& o)
    {
        return name == o.name;
    }
    
    std::ostream& operator<<(std::ostream& ostr, const Sort& s) {
        ostr << s.name;
        return ostr;
    }
    
#pragma mark - Sorts

    std::map<std::string, std::unique_ptr<Sort>> Sorts::_sorts;

    Sort* Sorts::fetchOrDeclare(std::string name)
    {
        auto it = _sorts.find(name);
        
        if (it == _sorts.end())
        {
            auto ret = _sorts.insert(std::make_pair(name, std::unique_ptr<Sort>(new Sort(name))));
            return ret.first->second.get();
        }
        else
        {
            return (*it).second.get();
        }
    }
    
}

