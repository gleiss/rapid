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
        return logic::Terms::func(logic::Sorts::intSort(), name, { i });
    }
    
    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::IntVariable>>& e){ostr << "not implemented"; return ostr;}
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::IntArrayVariable>>& e){ostr << "not implemented"; return ostr;}
    std::ostream& operator<<(std::ostream& ostr, const std::pair<std::vector<std::shared_ptr<const program::IntVariable>>, std::vector<std::shared_ptr<const program::IntArrayVariable>>>& e){ostr << "not implemented"; return ostr;}

    std::shared_ptr<const logic::Formula> BoolVariable::toFormula(std::shared_ptr<const logic::Term> i) const
    {
        assert(i != nullptr);
        return logic::Formulas::predicate(name, { i });
    }

    std::shared_ptr<const logic::Term> IntArrayVariable::toTerm(std::shared_ptr<const logic::Term> index, std::shared_ptr<const logic::Term> position) const
    {
        return logic::Terms::func(logic::Sorts::intSort(), name, {index, position});
    }

    std::shared_ptr<const logic::Term> IntArrayApplication::toTerm(std::shared_ptr<const logic::Term> i) const
    {
        assert(i != nullptr);
        return array->toTerm(i, index->toTerm(i));
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

