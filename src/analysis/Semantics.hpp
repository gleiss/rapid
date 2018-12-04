#ifndef __Semantics__
#define __Semantics__

#include "Formula.hpp"
#include "Program.hpp"
#include "Assignment.hpp"
#include <unordered_map>

namespace analysis {
    
    class Semantics
    {
    public:
        Semantics(const program::Program& program,
                  const program::ProgramGlobalProperties& maps) :
        program(program),
        iteratorMap(maps.iteratorMap),
        lastIterationMap(maps.lastIterationMap),
        enclosingIteratorsMap(maps.enclosingIteratorsMap),
        locationSymbolMap(maps.locationSymbolMap),
        startTimePointMap(maps.startTimePointMap),
        endTimePointMap(maps.endTimePointMap){}
        
        std::vector<std::shared_ptr<const logic::Formula>> generateSemantics();
        
    private:
        const program::Program& program;
        const program::IteratorMap& iteratorMap;
        const program::LastIterationMap& lastIterationMap;
        const program::EnclosingIteratorsMap& enclosingIteratorsMap;
        const program::LocationSymbolMap& locationSymbolMap;
        const program::StartTimePointMap& startTimePointMap;
        const program::EndTimePointMap& endTimePointMap;
        
        std::shared_ptr<const logic::Formula> generateSemantics(const program::Statement* statement,
                                                                const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::IntAssignment* intAssignment,
                                                                const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::IfElse* ifElse,
                                                                const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::WhileStatement* whileStatement,
                                                                const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars);
        std::shared_ptr<const logic::Formula> generateSemantics(const program::SkipStatement* skipStatement);
    };
}
#endif


