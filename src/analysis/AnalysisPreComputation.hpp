#ifndef __AnalysisPreComputation__
#define __AnalysisPreComputation__

#include "Program.hpp"

namespace analysis
{
    typedef std::unordered_map<const program::Statement*, std::shared_ptr<const logic::Term>> StartTimePointMap;
    typedef std::unordered_map<const program::Statement*, std::shared_ptr<const logic::Term>> EndTimePointMap;
    
    class AnalysisPreComputation
    {
    public:
        /*
         * for each statement, the start-timePoint is a term refering to the first location of the statement
         */
        static StartTimePointMap computeStartTimePointMap(const program::Program& program);
        /*
         * for each statement, the end-timePoint is a term refering to the first location after the statement
         */
        static EndTimePointMap computeEndTimePointMap(const program::Program& program,
                                                               const StartTimePointMap& startTimePointMap);
    private:
        static void addStartTimePointForStatement(const program::Statement* statement,
                                                  StartTimePointMap& startTimePointMap);
        
        static void addEndTimePointForStatement(const program::Statement* statement,
                                                const StartTimePointMap& startTimePointMap,
                                                EndTimePointMap& endTimePointMap);
        static void addEndTimePointForIfElseStatement(const program::IfElse* ifElse,
                                                      const StartTimePointMap& startTimePointMap,
                                                      EndTimePointMap& endTimePointMap);
        static void addEndTimePointForWhileStatement(const program::WhileStatement* whileStatement,
                                                     const StartTimePointMap& startTimePointMap,
                                                     EndTimePointMap& endTimePointMap);
    };
}
#endif
