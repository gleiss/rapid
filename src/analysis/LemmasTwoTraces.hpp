#ifndef __LemmasTwoTraces__
#define __LemmasTwoTraces__

#include <vector>
#include <memory>
#include <unordered_set>

#include "ProgramTraverser.hpp"
#include "Problem.hpp"
#include "Program.hpp"

namespace analysis {
    
    /*
     * We use the lemmas in this header to cover the inductive reasoning which relates values and timepoints of two execution-traces.
     */
    
    /*
     * LEMMA 1
     * If a variable has the same value (or if an array has at one position the same value) in both traces at some iteration,
     * and if that equality is preserved in an interval, then the equality holds during any point of the interval (including the end of the interval).
     *
     * Soundness: This lemma is exactly the instantiation of the following standard induction axiom scheme with P(it) := v(l(it),t1)=v(l(it),t2)
     * forall boundL,boundR.
     *    =>
     *       and
     *          P(boundL)
     *          forall it.
     *             =>
     *                and
     *                   boundL<=it<boundR
     *                   P(it)
     *                P(s(it))
     *       forall it.
     *          =>
     *             boundL<=it<=boundR
     *             P(it)
     *
     * Variations:
     *  The currently used version is the most general one, and allows arbitrary intervals.
     *  More specific versions could fix the left bound to 0 and/or the right bound to the last iteration of one trace (wlog. trace 1).
     *
     * Why is this lemma useful
     *  This lemma is central for reasoning about relational properties.
     */
    class EqualityPreservationTracesLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::ProblemItem>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::ProblemItem>>& items) override;
    };
    
    /*
     * LEMMA 2
     * If the last iteration of trace 2 fulfils the defining properties of the last iteration of trace 1, then those last iterations are the same.
     * Holds, since for the natural numbers the smallest element with property P is unique.
     *
     * Discussion on Variations:
     *  This lemma seems to be the most general lemma to conclude that the loop iterations are the same.
     *  One could also add more specific lemmas, e.g.:
     *      if all variables used in the loop iteration are the same in each iteration,
     *      then the loops terminate after the same number of steps
     *
     * Why is this lemma useful?
     * Most relational properties only hold, if the number of iterations of the involved loops is the same in both traces.
     */
    class NEqualLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::ProblemItem>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::ProblemItem>>& items) override;
    };
    

    
}

#endif

