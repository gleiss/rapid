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
        
        auto sortToDescribeTime = (util::Configuration::instance().timepoints().getValue()) ? logic::Sorts::timeSort() : logic::Sorts::intSort();
        auto s = logic::Signature::fetchOrDeclare(name, { sortToDescribeTime }, logic::Sorts::intSort(), false, true);
        return logic::Terms::funcTerm(s, { i });
    }
    
    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::IntVariable>>& e){ostr << "not implemented"; return ostr;}
    
    std::shared_ptr<const logic::Formula> BoolVariable::toFormula(std::shared_ptr<const logic::Term> i) const
    {
        assert(i != nullptr);
        
        auto sortToDescribeTime = (util::Configuration::instance().timepoints().getValue()) ? logic::Sorts::timeSort() : logic::Sorts::intSort();
        auto s = logic::Signature::fetchOrDeclare(name, { sortToDescribeTime }, logic::Sorts::intSort(), false, true);
        return logic::Formulas::predicateFormula(logic::Terms::predTerm(s, { i }));
    }
    
    std::shared_ptr<const logic::Term> IntArrayApplication::toTerm(std::shared_ptr<const logic::Term> i) const
    {
        assert(i != nullptr);
        
        auto sortToDescribeTime = (util::Configuration::instance().timepoints().getValue()) ? logic::Sorts::timeSort() : logic::Sorts::intSort();
        auto s = logic::Signature::fetchOrDeclare(array->name, { sortToDescribeTime, logic::Sorts::intSort() }, logic::Sorts::intSort(), false, true);
        
        return logic::Terms::funcTerm(s, { i, index->toTerm(i)});
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

