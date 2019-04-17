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
     *
     * TODO:    It is unclear why this lemma is useful at all. It doesn't cover any inductive reasoning we couldn't conclude from other axioms,
     *          and the covered reasoning steps should be straightforward to conclude.
     * TODO:    It would probably improve the lemma if we replace the conclusion with "then n is different from 0" (from which we can also conclude the current conclusion)
     */
    class AtLeastOneIterationLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };
    
    /*
     * LEMMA 2
     * TODO: not used currently, since implementation unsound
     * TODO: check, generalize, extend, recategorize, test.
     */
    class OrderingSynchronizationLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };
}

#endif
