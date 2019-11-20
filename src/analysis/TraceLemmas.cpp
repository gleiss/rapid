#include "TraceLemmas.hpp"

#include <memory>
#include <string>
#include <vector>
#include <cassert>

#include "Signature.hpp"
#include "Theory.hpp"
#include "Options.hpp"
#include "Output.hpp"

#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

#include "LemmasVariableValues.hpp"
#include "LemmasIterators.hpp"
#include "LemmasOther.hpp"
#include "LemmasTwoTraces.hpp"

using namespace logic;

namespace analysis {

#pragma mark - High level methods
    
    std::vector<std::shared_ptr<const logic::ProblemItem>> generateTraceLemmas(
        const program::Program& program,
        std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
        bool twoTraces,
        std::vector<std::shared_ptr<const logic::Axiom>> programSemantics)
    {
        std::vector<std::shared_ptr<const logic::ProblemItem>> items;
        
        // Lemmas to keep track of the values of variables at given timepoints
        ValueEvolutionLemmas valueEvolutionLemmas(program, locationToActiveVars, twoTraces);
        valueEvolutionLemmas.generate(items);
        
        StaticAnalysisLemmas staticAnalysisLemmas(program, locationToActiveVars, twoTraces, programSemantics);
        staticAnalysisLemmas.generate(items);
        
        // Lemmas for iterators
        IntermediateValueLemmas intermediateValueLemmas(program, locationToActiveVars, twoTraces);
        intermediateValueLemmas.generate(items);
        
        IterationInjectivityLemmas iterationInjectivityLemmas(program, locationToActiveVars, twoTraces);
        iterationInjectivityLemmas.generate(items);
        
        // Other lemmas
        AtLeastOneIterationLemmas atLeastOneIterationLemmas(program, locationToActiveVars, twoTraces);
        atLeastOneIterationLemmas.generate(items);
        
        //            OrderingSynchronizationLemmas orderingSynchronizationLemmas(program, locationToActiveVars, twoTraces);
        //            orderingSynchronizationLemmas.generate(items);

        // Trace lemmas
        if (twoTraces)
        {            
            EqualityPreservationTracesLemmas equalityPreservationTracesLemmas(program, locationToActiveVars, twoTraces);
            equalityPreservationTracesLemmas.generate(items);
            
            NEqualLemmas nEqualLemmas(program, locationToActiveVars, twoTraces);
            nEqualLemmas.generate(items);
        }
        
        return items;
    }
    
    
}


