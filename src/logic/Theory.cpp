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
    
    std::shared_ptr<const FuncTerm> Theory::natZero()
    {
        return Terms::func("nat_zero", {}, Sorts::natSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::natSucc(std::shared_ptr<const Term> term)
    {
        return Terms::func("nat_succ", {term}, Sorts::natSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::natPre(std::shared_ptr<const Term> term)
    {
        return Terms::func("nat_pre", {term}, Sorts::natSort(), true);
    }
    
    std::shared_ptr<const Formula> Theory::natSub(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate("nat_sub", {t1,t2}, label, true);
    }
    
}

