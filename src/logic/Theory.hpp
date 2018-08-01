#ifndef __Theory__
#define __Theory__

#include <string>
#include "Term.hpp"
#include "Formula.hpp"

namespace logic {
    
    class Theory {
    public:
                
        static std::shared_ptr<const FuncTerm> intConstant(int i);
        static std::shared_ptr<const FuncTerm> intAddition(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const FuncTerm> intSubtraction(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const FuncTerm> intMultiplication(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const FuncTerm> intUnaryMinus(std::shared_ptr<const Term> t);

        static std::shared_ptr<const Formula> intLess(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const Formula> intLessEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const Formula> intGreater(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const Formula> intGreaterEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        
        static std::shared_ptr<const Formula> boolTrue();
        static std::shared_ptr<const Formula> boolFalse();

        static std::shared_ptr<const FuncTerm> timeZero();
        static std::shared_ptr<const FuncTerm> timeSucc(std::shared_ptr<const Term> term);
        static std::shared_ptr<const FuncTerm> timePre(std::shared_ptr<const Term> term);
        static std::shared_ptr<const PredTerm> timeSub(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
    };
    
}

#endif

