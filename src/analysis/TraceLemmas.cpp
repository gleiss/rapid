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
    
    std::vector<std::shared_ptr<const logic::Lemma>> generateTraceLemmas(const program::Program& program,
                                                                           std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                                                                           bool twoTraces)
    {
        std::vector<std::shared_ptr<const logic::Lemma>> lemmas;
        
        // Lemmas to keep track of the values of variables at given timepoints
        ValueEvolutionLemmas valueEvolutionLemmas(program, locationToActiveVars, twoTraces);
        valueEvolutionLemmas.generate(lemmas);
        
        StaticAnalysisLemmas staticAnalysisLemmas(program, locationToActiveVars, twoTraces);
        staticAnalysisLemmas.generate(lemmas);
        
        // Lemmas for iterators
        IntermediateValueLemmas intermediateValueLemmas(program, locationToActiveVars, twoTraces);
        intermediateValueLemmas.generate(lemmas);
        
        IterationInjectivityLemmas iterationInjectivityLemmas(program, locationToActiveVars, twoTraces);
        iterationInjectivityLemmas.generate(lemmas);
        
        // Other lemmas
        AtLeastOneIterationLemmas atLeastOneIterationLemmas(program, locationToActiveVars, twoTraces);
        atLeastOneIterationLemmas.generate(lemmas);
        
        //            OrderingSynchronizationLemmas orderingSynchronizationLemmas(program, locationToActiveVars, twoTraces);
        //            orderingSynchronizationLemmas.generate(lemmas);

        // Trace lemmas
        if (twoTraces)
        {            
            EqualityPreservationTracesLemmas equalityPreservationTracesLemmas(program, locationToActiveVars, twoTraces);
            equalityPreservationTracesLemmas.generate(lemmas);
            
            NEqualLemmas nEqualLemmas(program, locationToActiveVars, twoTraces);
            nEqualLemmas.generate(lemmas);
        }
        
        return lemmas;
    }
    
    
}


