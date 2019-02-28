#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Formula.hpp"
#include "Sort.hpp"
#include "Term.hpp"
#include "Expression.hpp"
#include "Variable.hpp"
#include "Program.hpp"

namespace analysis {
    
    class TheoryAxioms
    {
    public:
        TheoryAxioms() {}

         std::vector<std::shared_ptr<const logic::Formula>> generate();
        
    private:
        void generateTheoryAxiom1(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);    
        void generateTheoryAxiom2(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);    
    };
}
