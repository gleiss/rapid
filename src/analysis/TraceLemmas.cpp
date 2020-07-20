#include "TraceLemmas.hpp"

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
        unsigned numberOfTraces,
        std::vector<std::shared_ptr<const logic::Axiom>> programSemantics,
        InlinedVariableValues& inlinedVarValues)
    {
        std::vector<std::shared_ptr<const logic::ProblemItem>> items;

        // Lemmas to keep track of the values of variables at given timepoints
        ValueEvolutionLemmas valueEvolutionLemmas(program, locationToActiveVars, numberOfTraces);
        valueEvolutionLemmas.generate(items);

        if (!util::Configuration::instance().inlineSemantics())
        {
            StaticAnalysisLemmas staticAnalysisLemmas(program, locationToActiveVars, numberOfTraces, programSemantics);
            staticAnalysisLemmas.generate(items);
        }

        // Lemmas for iterators
        IntermediateValueLemmas intermediateValueLemmas(program, locationToActiveVars, numberOfTraces);
        intermediateValueLemmas.generate(items);

        IterationInjectivityLemmas iterationInjectivityLemmas(program, locationToActiveVars, numberOfTraces);
        iterationInjectivityLemmas.generate(items);

        // Other lemmas
        AtLeastOneIterationLemmas atLeastOneIterationLemmas(program, locationToActiveVars, numberOfTraces, programSemantics, inlinedVarValues);
        atLeastOneIterationLemmas.generate(items);

        //            OrderingSynchronizationLemmas orderingSynchronizationLemmas(program, locationToActiveVars, numberOfTraces);
        //            orderingSynchronizationLemmas.generate(items);

        // Trace lemmas
        if (numberOfTraces > 1)
        {
            EqualityPreservationTracesLemmas equalityPreservationTracesLemmas(program, locationToActiveVars, numberOfTraces);
            equalityPreservationTracesLemmas.generate(items);

            NEqualLemmas nEqualLemmas(program, locationToActiveVars, numberOfTraces, programSemantics, inlinedVarValues);
            nEqualLemmas.generate(items);
        }

        return items;
    }
}


