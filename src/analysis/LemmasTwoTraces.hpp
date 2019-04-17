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
     * and if that equality is preserved in an interval, then it also holds at the end of the interval.
     *
     * Variations:
     *  The most general version of this lemma allows arbitrary intervals.
     *  More specific versions fix the left bound to 0 and/or the right bound to the last iteration of one trace (wlog. trace 1).
     *  The current implementation supports all 4 combinations, but doesn't add the most general version.
     *
     * TODO: the cleanest solution would be to use only the most general version. Can we identify Vampire-options, where doing so
     * doesn't slow down the proof search?
     *
     * Why is this lemma useful
     *  This lemma is central for reasoning about relational properties.
     */
    class EqualityPreservationTracesLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
        
        void generateLemmas(const program::WhileStatement* whileStatement,
                                     bool itLeftZero,
                                     bool itRightN,
                                     std::vector<std::shared_ptr<const logic::Lemma>>& lemmas);
        void generateLemmasZeroToRight(const program::WhileStatement* whileStatement,
                                                                 std::vector<std::shared_ptr<const logic::Lemma>>& lemmas);
        void generateLemmasLeftToEnd(const program::WhileStatement* whileStatement,
                                                               std::vector<std::shared_ptr<const logic::Lemma>>& lemmas);
        void generateLemmasLeftToRight(const program::WhileStatement* whileStatement,
                                                                 std::vector<std::shared_ptr<const logic::Lemma>>& lemmas);
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
    class NEqualLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };
    

    
}

#endif

