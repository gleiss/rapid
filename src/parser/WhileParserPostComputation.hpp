#ifndef __WhileParserPostComputation__
#define __WhileParserPostComputation__

#include <unordered_map>
#include "Program.hpp"
#include "Term.hpp"

namespace parser
{
    /*
     * This class finalizes parsing of a program
     * by computing information, which is global and can't
     * be easily computed during bison's parsing process.
     * This includes:
     *    - adds to each while-statement its iterator-variable and its lastIteration-function (as byproduct the lastIteration-symbol gets declared for each while-statement)
     *    - collects for each statement the enclosing iterators, i.e. all the iterators of enclosing while-statements
     *    - declares and adds to each statement its startTimePoint (as byproduct the symbol denoting the statement-location gets declared:
     *      This symbol is of type (Nats x Nat)->timepoint for while-statements and type Nats -> timepoint for everything else (Nats denotes the iterators))
     *    - declares and adds to each statement its endTimePoint
     */
    class WhileParserPostComputation
    {
    public:
        static std::unique_ptr<const program::ProgramGlobalProperties> compute(const program::Program& program, bool twoTraces);

    private:
        static program::IteratorMap computeIteratorMap(const program::Program& program);
        static void addIteratorsForStatement(const program::Statement* statement, program::IteratorMap& iteratorMap);
        
        static program::LastIterationMap computeLastIterationMap(const program::Program& program, bool twoTraces);
        static void addLastIterationsForStatement(const program::Statement* statement, program::LastIterationMap& lastIterationMap, bool twoTraces);

        static program::EnclosingIteratorsMap computeEnclosingIteratorsMap(const program::Program& program, const program::IteratorMap& iteratorMap);
        static void addEnclosingIteratorsForStatement(const program::Statement* statement,
                                                      const program::IteratorMap& iteratorMap,
                                                      std::vector<std::shared_ptr<const logic::Symbol>> enclosingIterators,
                                                      program::EnclosingIteratorsMap& enclosingIteratorsMap);

        static program::LocationSymbolMap computeLocationSymbolMap(const program::Program& program, const program::EnclosingIteratorsMap& enclosingIteratorsMap);
        static void addLocationSymbolForStatement(const program::Statement* statement,
                                                  const program::EnclosingIteratorsMap& enclosingIteratorsMap,
                                                  program::LocationSymbolMap& locationSymbolMap);
        
        static program::StartTimePointMap computeStartTimePointMap(const program::Program& program,
                                                                   const program::EnclosingIteratorsMap& enclosingIteratorsMap,
                                                                   const program::LocationSymbolMap& locationSymbolMap);
        static void addStartTimePointForStatement(const program::Statement* statement,
                                                  const program::EnclosingIteratorsMap& enclosingIteratorsMap,
                                                  const program::LocationSymbolMap& locationSymbolMap,
                                                  program::StartTimePointMap& startTimePointMap);

        static program::EndTimePointMap computeEndTimePointMap(const program::Program& program,
                                                               const program::IteratorMap& iteratorMap,
                                                               const program::EnclosingIteratorsMap& enclosingIteratorsMap,
                                                               const program::StartTimePointMap& startTimePointMap);
        static void addEndTimePointForStatement(const program::Statement* statement,
                                                const program::IteratorMap& iteratorMap,
                                                const program::EnclosingIteratorsMap& enclosingIteratorsMap,
                                                const program::StartTimePointMap& startTimePointMap,
                                                program::EndTimePointMap& endTimePointMap);
        static void addEndTimePointForIfElseStatement(const program::IfElse* ifElse,
                                                      const program::IteratorMap& iteratorMap,
                                                      const program::EnclosingIteratorsMap& enclosingIteratorsMap,
                                                      const program::StartTimePointMap& startTimePointMap,
                                                      program::EndTimePointMap& endTimePointMap);
        static void addEndTimePointForWhileStatement(const program::WhileStatement* whileStatement,
                                                     const program::IteratorMap& iteratorMap,
                                                     const program::EnclosingIteratorsMap& enclosingIteratorsMap,
                                                     const program::StartTimePointMap& startTimePointMap,
                                                     program::EndTimePointMap& endTimePointMap);
        
    };
}

#endif

