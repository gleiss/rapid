#ifndef __Semantics__
#define __Semantics__

#include "Formula.hpp"
#include "Program.hpp"
#include "Statements.hpp"
#include <unordered_map>

namespace analysis {
    
    class Semantics
    {
    public:
        Semantics(const program::Program& program,
                  const program::ProgramGlobalProperties& maps,
                  std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                  bool twoTraces = true) :
        program(program),
        iteratorMap(maps.iteratorMap),
        lastIterationMap(maps.lastIterationMap),
        enclosingIteratorsMap(maps.enclosingIteratorsMap),
        locationSymbolMap(maps.locationSymbolMap),
        startTimePointMap(maps.startTimePointMap),
        endTimePointMap(maps.endTimePointMap),
        locationToActiveVars(locationToActiveVars),
        twoTraces(twoTraces) {}
        
        std::vector<std::shared_ptr<const logic::Formula>> generateSemantics();
        
    private:
        const program::Program& program;
        const program::IteratorMap& iteratorMap;
        const program::LastIterationMap& lastIterationMap;
        const program::EnclosingIteratorsMap& enclosingIteratorsMap;
        const program::LocationSymbolMap& locationSymbolMap;
        const program::StartTimePointMap& startTimePointMap;
        const program::EndTimePointMap& endTimePointMap;
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


