#ifndef __LemmasIterators__
#define __LemmasIterators__

#include <vector>
#include <memory>

#include "ProgramTraverser.hpp"
#include "Problem.hpp"
#include "Program.hpp"

namespace analysis {
    
    /*
     * We use the lemmas in this header to cover the inductive reasoning for iterator variables.
     *
     * An iterator variable is a variable which iterates through a datastructure. We currently don't
     * support any datastructure other than arrays.
     *
     * We assume that any non-array variable potentially is an iterator variable.
     * In the future, one could also add an explicit iterator-type (as in C++) to the input-language,
     * and add iterator lemmas only for those variables.
     * This would introduce less useless lemmas, but would burden the programmer with making the iterator-variables explicit.
     */
    
    /*
     * LEMMA 1:
     * An iterator iterates through the whole array. In particular, for each position of the array, there is a timepoint,
     * where the iterator is at that position.
     * One can see this lemma as a discrete version of the Intermediate Value Theorem for continuous functions.
     *
     * Why is this lemma useful:
     *  This lemma synthesizes interesting timepoints, which can be used to split a loop into intervals.
     *  Often the timepoints in such an interval behave in some uniform way, and if this is the case then
     *  we often can apply other induction lemmas to the interval.
     */
    class IntermediateValueLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };
    
    /*
     * LEMMA 2:
     * Iterators often have the property that they visit each element of the datastructure exactly once.
     * This lemmas states that an array-iterator, which increase by 1 each iteration, visits each array-position only once.
     *
     * Why is this lemma useful:
     *  If each array-position is visited only once, we know that its value is not changed after the first visit,
     *  and in particular the value at the end is the value after the first visit.
     */
    class IterationInjectivityLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };
}

#endif
