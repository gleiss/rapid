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
        if (isArray)
        {
            if (isConstant)
            {
                logic::Signature::add(name, {logic::Sorts::intSort()}, logic::Sorts::intSort());
            }
            else
            {
                logic::Signature::add(name, {logic::Sorts::timeSort(), logic::Sorts::intSort()}, logic::Sorts::intSort());
            }
        }
        else
        {
            if (isConstant)
            {
                logic::Signature::add(name, {}, logic::Sorts::intSort());
            }
            else
            {
                logic::Signature::add(name, {logic::Sorts::timeSort()}, logic::Sorts::intSort());
            }
        }
    }

    std::shared_ptr<const logic::Term> Variable::toTerm(std::shared_ptr<const logic::Term> i) const
    {
        assert(i != nullptr);
        assert(!isArray);
        
        if (isConstant)
        {
            return logic::Terms::func(name, {}, logic::Sorts::intSort());
        }
        else
        {
            return logic::Terms::func(name, { i }, logic::Sorts::intSort());
        }
    }

    std::shared_ptr<const logic::Term> Variable::toTerm(std::shared_ptr<const logic::Term> index, std::shared_ptr<const logic::Term> position) const
    {
        assert(index != nullptr);
        assert(position != nullptr);
        assert(isArray);
        
        if (isConstant)
        {
            return logic::Terms::func(name, {position}, logic::Sorts::intSort());
        }
        else
        {
            return logic::Terms::func(name, {index, position}, logic::Sorts::intSort());
        }
    }
    
    std::shared_ptr<const logic::Term> IntVariableAccess::toTerm(std::shared_ptr<const logic::Term> i) const
    {
        assert(i != nullptr);
        return var->toTerm(i);
    }
    
    std::shared_ptr<const logic::Term> IntArrayApplication::toTerm(std::shared_ptr<const logic::Term> index, std::shared_ptr<const logic::Term> position) const
    {
        assert(index != nullptr);
        assert(position != nullptr);
        return array->toTerm(index, position);
    }

    std::shared_ptr<const logic::Term> IntArrayApplication::toTerm(std::shared_ptr<const logic::Term> i) const
    {
        assert(i != nullptr);
        return toTerm(i, index->toTerm(i));
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

