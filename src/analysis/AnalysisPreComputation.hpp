#ifndef __AnalysisPreComputation__
#define __AnalysisPreComputation__

#include "Program.hpp"

#include <memory>
#include <unordered_map>

#include "Term.hpp"
#include "Formula.hpp"

namespace analysis
{
    typedef std::unordered_map<const program::Statement*, std::shared_ptr<const logic::Term>> EndTimePointMap;
        
    class AnalysisPreComputation
    {
    public:
        /*
         * for each statement, the end-timePoint is a term refering to the first location after the statement
         */
        static EndTimePointMap computeEndTimePointMap(const program::Program& program);
    private:
        static void addEndTimePointForStatement(const program::Statement* statement,
                                                EndTimePointMap& endTimePointMap);
        static void addEndTimePointForIfElseStatement(const program::IfElse* ifElse,
                                                      EndTimePointMap& endTimePointMap);
        static void addEndTimePointForWhileStatement(const program::WhileStatement* whileStatement,
                                                     EndTimePointMap& endTimePointMap);
    };
}
#endif
