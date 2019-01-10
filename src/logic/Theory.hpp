#ifndef __Theory__
#define __Theory__

#include <memory>
#include <string>

#include "Term.hpp"
#include "Formula.hpp"

namespace logic {
    
    class Theory {
    public:
        static void declareTheories();
        
        static std::shared_ptr<const FuncTerm> intConstant(int i);
        static std::shared_ptr<const FuncTerm> intAddition(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const FuncTerm> intSubtraction(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const FuncTerm> intMultiplication(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const FuncTerm> intAbsolute(std::shared_ptr<const Term> t);

        static std::shared_ptr<const Formula> intLess(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label="");
        static std::shared_ptr<const Formula> intLessEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label="");
        static std::shared_ptr<const Formula> intGreater(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label="");
        static std::shared_ptr<const Formula> intGreaterEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label="");
        
        static std::shared_ptr<const Formula> boolTrue(std::string label="");
        static std::shared_ptr<const Formula> boolFalse(std::string label="");

        static std::shared_ptr<const FuncTerm> natZero();
        static std::shared_ptr<const FuncTerm> natSucc(std::shared_ptr<const Term> term);
        static std::shared_ptr<const FuncTerm> natPre(std::shared_ptr<const Term> term);
        static std::shared_ptr<const Formula> natSub(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label="");
    };
    
}

#endif

