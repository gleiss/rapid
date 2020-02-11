#ifndef __Semantics__
#define __Semantics__

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Formula.hpp"
#include "Program.hpp"
#include "Statements.hpp"
#include "Problem.hpp"
#include "AnalysisPreComputation.hpp"
#include "SemanticsInliner.hpp"

namespace analysis {
    
    class Semantics
    {
    public:
        Semantics(const program::Program& program,
                  std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                  std::vector<std::shared_ptr<const logic::ProblemItem>>& problemItems,
                  bool twoTraces) :
        program(program),
        endTimePointMap(AnalysisPreComputation::computeEndTimePointMap(program)),
        locationToActiveVars(locationToActiveVars),
        problemItems(problemItems),
        twoTraces(twoTraces) {}
        
        std::vector<std::shared_ptr<const logic::Axiom>> generateSemantics();
        
    private:

        const program::Program& program;
        const EndTimePointMap endTimePointMap;
        const std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars;
        std::vector<std::shared_ptr<const logic::ProblemItem>>& problemItems;
        const bool twoTraces;

        std::shared_ptr<const logic::Formula> generateSemantics(const program::Statement* statement, SemanticsInliner& inliner);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::IntAssignment* intAssignment, SemanticsInliner& inliner);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::IfElse* ifElse, SemanticsInliner& inliner);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::WhileStatement* whileStatement, SemanticsInliner& inliner);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::SkipStatement* skipStatement, SemanticsInliner& inliner);
    };
}
#endif


