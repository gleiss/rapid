#ifndef __PropertiesTime__
#define __PropertiesTime__

#include <vector>
#include <map>
#include <utility>

#include "Formula.hpp"
#include "Sort.hpp"
#include "Term.hpp"
#include "Expression.hpp"
#include "Variable.hpp"
#include "Program.hpp"

namespace analysis {
    
    class TraceLemmas
    {
    public:
        TraceLemmas(const program::Program& program,
                    const program::ProgramGlobalProperties& maps,
                    std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars) :
        program(program),
        iteratorMap(maps.iteratorMap),
        lastIterationMap(maps.lastIterationMap),
        enclosingIteratorsMap(maps.enclosingIteratorsMap),
        locationSymbolMap(maps.locationSymbolMap),
        locationToActiveVars(locationToActiveVars) {}
        
        std::vector<std::shared_ptr<const logic::Formula>> generate();
        
    private:
        const program::Program& program;
        const program::IteratorMap& iteratorMap;
        const program::LastIterationMap& lastIterationMap;
        const program::EnclosingIteratorsMap& enclosingIteratorsMap;
        const program::LocationSymbolMap& locationSymbolMap;
        const std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars;

        enum class InductionKind { Equal, Less, Greater, LessEqual, GreaterEqual};
        
        void generateStandardInductionLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateStandardInductionLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateStandardInductionLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas,
                                             const InductionKind kind);
    };
}

#endif

