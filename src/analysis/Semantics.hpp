#ifndef __Semantics__
#define __Semantics__

#include "Formula.hpp"
#include "Program.hpp"
#include "Statements.hpp"
#include <unordered_map>

#include "AnalysisPreComputation.hpp"

namespace analysis {
    
    class Semantics
    {
    public:
        Semantics(const program::Program& program,
                  std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                  bool twoTraces) :
        program(program),
        startTimePointMap(AnalysisPreComputation::computeStartTimePointMap(program)),
        endTimePointMap(AnalysisPreComputation::computeEndTimePointMap(program, startTimePointMap)),
        locationToActiveVars(locationToActiveVars),
        twoTraces(twoTraces) {}
        
        std::vector<std::shared_ptr<const logic::Formula>> generateSemantics();
        
    private:
        const program::Program& program;
        const StartTimePointMap startTimePointMap;
        const EndTimePointMap endTimePointMap;
        const std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars;
        
        const bool twoTraces;
        
        std::shared_ptr<const logic::Formula> generateSemantics(const program::Statement* statement);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::IntAssignment* intAssignment);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::IfElse* ifElse);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::WhileStatement* whileStatement);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::SkipStatement* skipStatement);
    };
}
#endif


