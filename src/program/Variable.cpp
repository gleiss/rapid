/**
 * @file Variable.cpp
 *
 */

#include "Variable.hpp"

#include "Sort.hpp"
#include "Theory.hpp"
#include "Options.hpp"

namespace program {
    
    std::shared_ptr<const logic::Term> IntVariable::toTerm(std::shared_ptr<const logic::Term> i) const
    {
        assert(i != nullptr);
        return logic::Terms::funcTerm(logic::Sorts::intSort(), name, { i });
    }
    
    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::IntVariable>>& e){ostr << "not implemented"; return ostr;}
    
    std::shared_ptr<const logic::Formula> BoolVariable::toFormula(std::shared_ptr<const logic::Term> i) const
    {
        assert(i != nullptr);
        return logic::Formulas::predicateFormula(logic::Terms::predTerm(name, { i }));
    }
    
    std::shared_ptr<const logic::Term> IntArrayApplication::toTerm(std::shared_ptr<const logic::Term> i) const
    {
        assert(i != nullptr);
        return logic::Terms::funcTerm(logic::Sorts::intSort(), array->name, { i, index->toTerm(i)});
    }
    
    std::string IntVariable::toString() const
    {
        return name;
    }
    std::string BoolVariable::toString() const
    {
        return name;
    }
    std::string IntArrayVariable::toString() const
    {
        return name;
    }
    std::string IntArrayApplication::toString() const
    {
        return array->toString() + "[" + index->toString() + "]";
    }
}

