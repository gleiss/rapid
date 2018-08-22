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
        std::shared_ptr<const logic::Formula> generateSemantics(const program::Program& program);
        
        typedef std::unordered_map<const std::shared_ptr<const program::Statement>, std::shared_ptr<const logic::Term>, program::StatementSharedPtrHash> EndLocationMap;

        std::shared_ptr<const logic::Formula> generateSemantics(const std::shared_ptr<const program::Statement> statement,
                                                                const EndLocationMap& map,
                                                                const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars,
                                                                std::vector<std::shared_ptr<const logic::Term>> iterators);
        std::shared_ptr<const logic::Formula> generateSemantics(const std::shared_ptr<const program::IntAssignment> intAssignment,
                                                                const EndLocationMap& map,
                                                                const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars,
                                                                std::vector<std::shared_ptr<const logic::Term>> iterators);
        std::shared_ptr<const logic::Formula> generateSemantics(const std::shared_ptr<const program::IfElse> ifElse,
                                                                const EndLocationMap& map,
                                                                const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars,
                                                                std::vector<std::shared_ptr<const logic::Term>> iterators);
        std::shared_ptr<const logic::Formula> generateSemantics(const std::shared_ptr<const program::WhileStatement> whileStatement,
                                                                const EndLocationMap& map,
                                                                const std::vector<std::shared_ptr<const program::IntVariable>>& intVars,
                                                                const std::vector<std::shared_ptr<const program::IntArrayVariable>>& intArrayVars,
                                                                std::vector<std::shared_ptr<const logic::Term>> iterators);
        std::shared_ptr<const logic::Formula> generateSemantics(const std::shared_ptr<const program::SkipStatement> skipStatement,
                                                                const EndLocationMap& map,
                                                                std::vector<std::shared_ptr<const logic::Term>> iterators);

        std::shared_ptr<const logic::Term> startTimePoint(std::shared_ptr<const program::Statement> statement, std::vector<std::shared_ptr<const logic::Term>> iterators);

        EndLocationMap computeEndLocations(const program::Program& program);
        void computeEndLocations(const std::shared_ptr<const program::Statement> statement, EndLocationMap& map, std::vector<std::shared_ptr<const logic::Term>> iterators);
        void computeEndLocations(const std::shared_ptr<const program::IfElse> statement, EndLocationMap& map, std::vector<std::shared_ptr<const logic::Term>> iterators);
        void computeEndLocations(const std::shared_ptr<const program::WhileStatement> statement, EndLocationMap& map, std::vector<std::shared_ptr<const logic::Term>> iterators);

        std::shared_ptr<const logic::LVariable> loopIterator(std::shared_ptr<const program::WhileStatement> whileStatement);
        std::shared_ptr<const logic::FuncTerm> lastIteration(std::shared_ptr<const program::WhileStatement> whileStatement);

    };
}
#endif


