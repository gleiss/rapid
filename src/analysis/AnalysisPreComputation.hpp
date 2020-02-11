#ifndef __AnalysisPreComputation__
#define __AnalysisPreComputation__

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Term.hpp"
#include "Formula.hpp"

#include "Program.hpp"
#include "Variable.hpp"
#include "Statements.hpp"

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

        static std::unordered_set<std::shared_ptr<const program::Variable>> computeAssignedVars(const program::Statement* statement);

    private:
        static void addEndTimePointForStatement(const program::Statement* statement,
                                                const std::shared_ptr<const logic::Term> nextTimepoint,
                                                EndTimePointMap& endTimePointMap);
        static void addEndTimePointForIfElseStatement(const program::IfElse* ifElse,
                                                      const std::shared_ptr<const logic::Term> nextTimepoint,
                                                      EndTimePointMap& endTimePointMap);
        static void addEndTimePointForWhileStatement(const program::WhileStatement* whileStatement,
                                                     const std::shared_ptr<const logic::Term> nextTimepoint,
                                                     EndTimePointMap& endTimePointMap);
    };
}
#endif
