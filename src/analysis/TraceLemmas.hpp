#ifndef __PropertiesTime__
#define __PropertiesTime__

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
    
    class TraceLemmas
    {
    public:
        TraceLemmas(const program::Program& program,
                    std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                    bool twoTraces) :
        program(program),
        locationToActiveVars(locationToActiveVars),
        twoTraces(twoTraces) {}
        
        std::vector<std::shared_ptr<const logic::Formula>> generate();
        
    private:
        const program::Program& program;
        const std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars;
        const bool twoTraces;
        
        enum class InductionKind { Equal, Less, Greater, LessEqual, GreaterEqual};
        
        void generateStandardInductionLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateStandardInductionLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateStandardInductionLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas,
                                             const InductionKind kind);
        
        void generateTwoTracesLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateTwoTracesLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateTwoTracesLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        
        void generateNEqualLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateNEqualLemmas(const program::Statement* statement,
                                     std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateNEqualLemmas(const program::WhileStatement* whileStatement,
                                     std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        
    };
}

#endif

