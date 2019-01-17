#ifndef __StaticAnalysis__
#define __StaticAnalysis__

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Program.hpp"
#include "Variable.hpp"
#include "Formula.hpp"

namespace analysis
{
    class StaticAnalysis
    {
    public:
        StaticAnalysis(const program::Program& program,
                    std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                    bool twoTraces) :
        program(program),
        locationToActiveVars(locationToActiveVars),
        twoTraces(twoTraces) {}
        
        std::vector<std::shared_ptr<const logic::Formula>> generateStaticAnalysisLemmas();

    private:
        const program::Program& program;
        const std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars;
        const bool twoTraces;
        
        void generateStaticAnalysisLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateStaticAnalysisLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        
        std::unordered_set<std::shared_ptr<const program::Variable>> computeAssignedVars(const program::Statement* statement);
    };
}
#endif
