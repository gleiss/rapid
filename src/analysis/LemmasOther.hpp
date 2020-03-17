#ifndef __LemmasOther__
#define __LemmasOther__

#include <vector>
#include <memory>

#include "ProgramTraverser.hpp"
#include "Problem.hpp"
#include "Program.hpp"

namespace analysis {
    
    /*
     * We use the lemmas in this header to cover additional (inductive) reasoning which doesn't fit in any particular category.
     */
    
    /*
     * LEMMA 1
     * if the loop condition holds at the first iteration, then there exists an iteration whose successor is the last iteration.
     * TODO:    It would probably improve the lemma if we replace the conclusion with "then n is different from 0" (from which we can also conclude the current conclusion)
     *
     * TODO:    It is unclear why this lemma is useful at all. It doesn't cover any inductive reasoning we couldn't conclude from other axioms.
     *          It could be the case that it is useful, since superposition doesn't propergate disequality eagerly, that is, from P(0) and not P(n)
     *          we can't conclude 0!=n. Instead we use the negated disequality 0=n in a clause n=0 \/ C to rewrite not P(n) to not P(0) and combine
     *          the resulting clause with P(0) to derive C.
     */
    class AtLeastOneIterationLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::ProblemItem>>>
    {
    public:
        AtLeastOneIterationLemmas(
            const program::Program& program,
            std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
            unsigned numberOfTraces,
            std::vector<std::shared_ptr<const logic::Axiom>> programSemantics) :
            ProgramTraverser<std::vector<std::shared_ptr<const logic::ProblemItem>>>(program, locationToActiveVars, numberOfTraces), programSemantics(programSemantics) {}
    private:
        std::vector<std::shared_ptr<const logic::Axiom>> programSemantics;

        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::ProblemItem>>& items) override;
    };
    
    /*
     * LEMMA 2
     * TODO: not used currently, since implementation unsound
     * TODO: check, generalize, extend, recategorize, test.
     */
    class OrderingSynchronizationLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::ProblemItem>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::ProblemItem>>& items) override;
    };
}

#endif
