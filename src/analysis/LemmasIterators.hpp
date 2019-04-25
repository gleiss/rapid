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
     * If the (iterator-) variable v is dense, and the value x is between the value of v at the beginning and at the end of the loop, then there is a timepoint in the loop,
     * where v has value x. The value x usually denotes some position in an array.
     * One can see this lemma as a discrete version of the Intermediate Value Theorem for continuous functions.
     * (added for both non-array- and array variables. we ignore array positions and enclosing iterators in this description) // TODO: only add it for iterator variables.
     *
     * forall x.
     *    =>
     *       and
     *          v(l(zero))<=x
     *          x<v(l(n))
     *          forall it.
     *             v(l(s(it)))=v(l(it))+1
     *    exists it2.
     *       and
     *          it2<n
     *          v(l(it2))=x
     *
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
