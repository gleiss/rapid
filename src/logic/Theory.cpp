#include "Theory.hpp"

#include <cassert>

namespace logic {
    
    std::shared_ptr<const FuncTerm> Theory::intConstant(int i)
    {
        return Terms::func(std::to_string(i), {}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::intAddition(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        return Terms::func("int_plus", {t1,t2}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::intSubtraction(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        return Terms::func("int_minus", {t1,t2}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::intMultiplication(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        return Terms::func("int_multiply", {t1,t2}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::intUnaryMinus(std::shared_ptr<const Term> t)
    {
        return Terms::func("int_unary_minus", {t}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const Formula> Theory::intLess(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate("int_less", {t1,t2}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::intLessEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate("int_less_eq", {t1,t2}, label, true);
    }

    std::shared_ptr<const Formula> Theory::intGreater(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2,  std::string label)
    {
        return Formulas::predicate("int_greater", {t1,t2}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::intGreaterEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate("int_greater_eq", {t1,t2}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::boolTrue(std::string label)
    {
        return Formulas::predicate("bool_true", {}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::boolFalse(std::string label)
    {
        return Formulas::predicate("bool_false", {}, label, true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::timeZero()
    {
        return Terms::func("time_zero", {}, Sorts::timeSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::timeSucc(std::shared_ptr<const Term> term)
    {
        return Terms::func("time_succ", {term}, Sorts::timeSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::timePre(std::shared_ptr<const Term> term)
    {
        return Terms::func("time_pre", {term}, Sorts::timeSort(), true);
    }
    
    std::shared_ptr<const Formula> Theory::timeSub(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate("time_sub", {t1,t2}, label, true);
    }
    
}

