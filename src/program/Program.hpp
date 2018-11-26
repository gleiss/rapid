#ifndef __Program__
#define __Program__

#include <memory>
#include <vector>

#include "Expression.hpp"
#include "Assignment.hpp"

namespace program
{
    class Function
    {
    public:
        Function(std::string name,
                 std::vector<std::shared_ptr<const IntVariable>> intVariables,
                 std::vector<std::shared_ptr<const IntArrayVariable>> intArrayVariables,
                 std::vector<std::shared_ptr<const Statement>> statements)
        : name(name), intVariables(std::move(intVariables)), intArrayVariables(std::move(intArrayVariables)), statements(std::move(statements))
        {
            // TODO: add a skip-statement instead, maybe already during parsing (challenge: unique numbering)
            assert(this->statements.size() > 0);
        }
        
        const std::string name;
        const std::vector<std::shared_ptr<const IntVariable>> intVariables;
        const std::vector<std::shared_ptr<const IntArrayVariable>> intArrayVariables;
        const std::vector<std::shared_ptr<const Statement>> statements;
    };
    std::ostream& operator<<(std::ostream& ostr, const Function& p);

    class Program
    {
    public:
        Program(std::vector< std::shared_ptr<const Function>> functions) : functions(std::move(functions))
        {
            // TODO: enforce that one of the functions is called main
//                assert(this->functions.size() > 0);
        }
        
        const std::vector< std::shared_ptr<const Function>> functions;
    };
    std::ostream& operator<<(std::ostream& ostr, const Program& p);
    
    typedef std::unordered_map<const program::WhileStatement*, std::shared_ptr<const logic::Symbol>> IteratorMap;
    typedef std::unordered_map<const program::WhileStatement*, std::shared_ptr<const logic::Term>> LastIterationMap;
    typedef std::unordered_map<const program::Statement*, std::vector<std::shared_ptr<const logic::Symbol>>> EnclosingIteratorsMap;
    typedef std::unordered_map<const program::Statement*, const logic::Symbol*> LocationSymbolMap;
    typedef std::unordered_map<const program::Statement*, std::shared_ptr<const logic::Term>> StartTimePointMap;
    typedef std::unordered_map<const program::Statement*, std::shared_ptr<const logic::Term>> EndTimePointMap;
    
    class ProgramGlobalProperties
    {
    public:
        ProgramGlobalProperties(program::IteratorMap iteratorMap,
                                program::LastIterationMap lastIterationMap,
                                program::EnclosingIteratorsMap enclosingIteratorsMap,
                                program::LocationSymbolMap locationSymbolMap,
                                program::StartTimePointMap startTimePointMap,
                                program::EndTimePointMap endTimePointMap) :
        iteratorMap(iteratorMap),
        lastIterationMap(lastIterationMap),
        enclosingIteratorsMap(enclosingIteratorsMap),
        locationSymbolMap(locationSymbolMap),
        startTimePointMap(startTimePointMap),
        endTimePointMap(endTimePointMap) {}
        
        const program::IteratorMap iteratorMap;
        const program::LastIterationMap lastIterationMap;
        const program::EnclosingIteratorsMap enclosingIteratorsMap;
        const program::LocationSymbolMap locationSymbolMap;
        const program::StartTimePointMap startTimePointMap;
        const program::EndTimePointMap endTimePointMap;
    };
}

#endif
