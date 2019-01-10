#ifndef __Problem__
#define __Problem__

#include <iostream>
#include <memory>
#include <vector>

#include "Formula.hpp"

namespace logic {
    
    // represents a first-order problem which can be passed to a prover.
    // for now sorts and signatures are not part of Problem, maybe should be refactored.
    class Problem
    {
    public:
        Problem() : axioms(), conjecture(nullptr), lemmas() {}
        
        std::vector<std::shared_ptr<const Formula>> axioms;
        std::shared_ptr<const Formula> conjecture;
        
        std::vector<std::shared_ptr<const Formula>> lemmas;
        
        void outputSMTLIB(std::ostream& ostr);
    };
}
#endif
