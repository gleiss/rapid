#include "Theory.hpp"

#include <cassert>

namespace logic {

    // declare each function-/predicate-symbol by constructing it
    // has additional sideeffect of declaring the involved sorts
    void Theory::declareTheories()
    {
        boolTrue();
        boolFalse();
        
        auto intConst = intConstant(0);
        intAddition(intConst,intConst);
        intSubtraction(intConst,intConst);
        intMultiplication(intConst,intConst);
        intLess(intConst, intConst);
        intLessEqual(intConst, intConst);
        intGreater(intConst, intConst);
        intGreaterEqual(intConst, intConst);
        
        auto zero = natZero();
        natSucc(zero);
        natPre(zero);
        natSub(zero, zero);
    }

    std::shared_ptr<const FuncTerm> Theory::intConstant(int i)
    {
        return Terms::func(std::to_string(i), {}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::intAddition(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        return Terms::func("+", {t1,t2}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::intSubtraction(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        return Terms::func("-", {t1,t2}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::intMultiplication(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        return Terms::func("*", {t1,t2}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const Formula> Theory::intLess(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate("<", {t1,t2}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::intLessEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate("<=", {t1,t2}, label, true);
    }

    std::shared_ptr<const Formula> Theory::intGreater(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2,  std::string label)
    {
        return Formulas::predicate(">", {t1,t2}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::intGreaterEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate(">=", {t1,t2}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::boolTrue(std::string label)
    {
        return Formulas::predicate("true", {}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::boolFalse(std::string label)
    {
        return Formulas::predicate("false", {}, label, true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::natZero()
    {
        return Terms::func("zero", {}, Sorts::natSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::natSucc(std::shared_ptr<const Term> term)
    {
        return Terms::func("s", {term}, Sorts::natSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::natPre(std::shared_ptr<const Term> term)
    {
        return Terms::func("p", {term}, Sorts::natSort(), true);
    }
    
    std::shared_ptr<const Formula> Theory::natSub(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate("Sub", {t1,t2}, label, true);
    }
    
}

