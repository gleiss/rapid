/**
 * @file Variable.cpp
 *
 */

#include "Variable.hpp"

#include "Sort.hpp"
#include "Signature.hpp"
#include "Theory.hpp"
#include "Options.hpp"

#include <iostream>
namespace program {
    
//    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::Variable>>& e){ostr << "not implemented"; return ostr;}

    void Variable::addSymbolToSignature() const
    {
        std::vector<const logic::Sort*> argSorts;
        if (!isConstant)
        {
            argSorts.push_back(logic::Sorts::timeSort());
        }
        if (isArray)
        {
            argSorts.push_back(logic::Sorts::intSort());
        }
        if(twoTraces)
        {
            argSorts.push_back(logic::Sorts::traceSort());
        }
        
        logic::Signature::add(name, argSorts, logic::Sorts::intSort());
    }

    std::string IntVariableAccess::toString() const
    {
        return var->name;
    }

    std::string IntArrayApplication::toString() const
    {
        return array->name + "[" + index->toString() + "]";
    }
}

