/**
 * @file Variable.cpp
 *
 */

#include "Variable.hpp"

#include "Options.hpp"

#include <iostream>
namespace program {
    
//    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::Variable>>& e){ostr << "not implemented"; return ostr;}

    std::string IntVariableAccess::toString() const
    {
        return var->name;
    }

    std::string IntArrayApplication::toString() const
    {
        return array->name + "[" + index->toString() + "]";
    }
}

