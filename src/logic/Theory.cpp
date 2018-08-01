#include "Theory.hpp"

#include <cassert>

namespace logic {
    
    std::shared_ptr<const FuncTerm> Theory::intConstant(int i)
    {
        auto s = Signature::fetchOrDeclare(std::to_string(i), Sorts::intSort(), true);
        return Terms::funcTerm(s, {});
    }
    
    std::shared_ptr<const FuncTerm> Theory::intAddition(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        auto s = Signature::fetchOrDeclare("int_plus", { Sorts::intSort(), Sorts::intSort() }, Sorts::intSort(), true);
        return Terms::funcTerm(s, {t1,t2});
    }
    std::shared_ptr<const FuncTerm> Theory::intSubtraction(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        auto s = Signature::fetchOrDeclare("int_minus", { Sorts::intSort(), Sorts::intSort() }, Sorts::intSort(), true);
        return Terms::funcTerm(s, {t1,t2});
    }
    std::shared_ptr<const FuncTerm> Theory::intMultiplication(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        auto s = Signature::fetchOrDeclare("int_multiply", { Sorts::intSort(), Sorts::intSort() }, Sorts::intSort(), true);
        return Terms::funcTerm(s, {t1,t2});
    }
    std::shared_ptr<const FuncTerm> Theory::intUnaryMinus(std::shared_ptr<const Term> t)
    {
        auto s = Signature::fetchOrDeclare("int_unary_minus", { Sorts::intSort() } , Sorts::intSort(), true);
        return Terms::funcTerm(s, {t});
    }
    
    std::shared_ptr<const Formula> Theory::intLess(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        auto s = Signature::fetchOrDeclare("int_less", { Sorts::intSort(), Sorts::intSort() }, Sorts::boolSort(), true);
        return Formulas::predicateFormula(Terms::predTerm(s, {t1,t2}));
    }
    std::shared_ptr<const Formula> Theory::intLessEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        auto s = Signature::fetchOrDeclare("int_less_eq", { Sorts::intSort(), Sorts::intSort() }, Sorts::boolSort(), true);
        return Formulas::predicateFormula(Terms::predTerm(s, {t1,t2}));
    }

    std::shared_ptr<const Formula> Theory::intGreater(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        auto s = Signature::fetchOrDeclare("int_greater", { Sorts::intSort(), Sorts::intSort() }, Sorts::boolSort(), true);
        return Formulas::predicateFormula(Terms::predTerm(s, {t1,t2}));
    }
    std::shared_ptr<const Formula> Theory::intGreaterEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        auto s = Signature::fetchOrDeclare("int_greater_eq", { Sorts::intSort(), Sorts::intSort() }, Sorts::boolSort(), true);
        return Formulas::predicateFormula(Terms::predTerm(s, {t1,t2}));
    }
    
    std::shared_ptr<const Formula> Theory::boolTrue()
    {
        auto s = Signature::fetchOrDeclare("bool_true", Sorts::boolSort(), true);
        return Formulas::predicateFormula(Terms::predTerm(s, {}));
    }
    
    std::shared_ptr<const Formula> Theory::boolFalse()
    {
        auto s = Signature::fetchOrDeclare("bool_false", Sorts::boolSort(), true);
        return Formulas::predicateFormula(Terms::predTerm(s, {}));
    }
    
    std::shared_ptr<const FuncTerm> Theory::timeZero()
    {
        auto s = Signature::fetchOrDeclare("time_zero", Sorts::timeSort(), true, true);
        return Terms::funcTerm(s, {});
    }
    
    std::shared_ptr<const FuncTerm> Theory::timeSucc(std::shared_ptr<const Term> term)
    {
        auto s = Signature::fetchOrDeclare("time_succ", {Sorts::timeSort()}, Sorts::timeSort(), true, true);
        return Terms::funcTerm(s, {term});
    }
    
    std::shared_ptr<const FuncTerm> Theory::timePre(std::shared_ptr<const Term> term)
    {
        auto s = Signature::fetchOrDeclare("time_pre", {Sorts::timeSort()}, Sorts::timeSort(), true, true);
        return Terms::funcTerm(s, {term});
    }
    
    std::shared_ptr<const PredTerm> Theory::timeSub(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        auto s = Signature::fetchOrDeclare("time_sub", {Sorts::timeSort(), Sorts::timeSort()}, Sorts::boolSort(), true, true);
        return Terms::predTerm(s, {t1,t2});
    }
    
}

