#ifndef __LemmasVariableValues__
#define __LemmasVariableValues__

#include <vector>
#include <memory>
#include <unordered_set>

#include "ProgramTraverser.hpp"
#include "Problem.hpp"
#include "Program.hpp"

namespace analysis {
    
    /*
     * We use the lemmas in this header to cover the inductive reasoning about how values of program variables change in given intervals
     */
    
    /*
     * LEMMA 1
     * Induction lemma to reason about how a single value of a variable (-position) evolves over an interval.
     *
     * For any predicate symbols C in =,<,>,<=,>=, this lemma is equivalent to bounded induction with induction hypothesis v(l(it)) C v(l(boundL)).
     * Instead of encoding the inductive step as v(l(it)) C v(l(boundL)) => v(l(s(it))) C v(l(boundL)) we use the equivalent v(l(s(it))) C v(l(it)).
     *
     * Discussion on possible variations:
     * The following variations apply to induction in general and are not specific to the used induction hypothesis.
     * The most general induction lemma states:
     *      if the truth of predicate P is preserved in the interval [boundL,boundR], then P(boundL) implies P(it) for all it in [boundL,boundR].
     * The most important bounds are 0 and the last iteration n. It could speed up Vampire to replace the most general version by more refined ones (with the downside that we are less complete)
     *      if the truth of predicate P is preserved in the interval [0,boundR], then P(0) implies P(it) for all it in [0,boundR].
     *      if the truth of predicate P is preserved in the interval [boundL,n], then P(boundL) implies P(it) for all it in [boundL,n].
     *      if the truth of predicate P is preserved in the interval [0,n], then P(0) implies P(it) for all it in [0,n].
     *
     * TODO:
     *  the most elegant set of lemmas would be to only use the most general version of these lemmas, and include one such lemma for each predicate symbol =,<,>,<=,>=.
     *  Can we use this set of lemmas?
     *
     * Why are these lemmas useful:
     *  The most useful instance of these lemmas is the one with equality as connective.
     *  It is used to conclude that a variable value doesn't change in an interval.
     *  All other connectives are not very important.
     */
    class ValueEvolutionLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
        virtual void generateOutputFor(const program::WhileStatement* statement, std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
        
    private:
        enum class InductionKind { Equal, Less, Greater, LessEqual, GreaterEqual};
        
        void generateLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Lemma>>& lemmas,
                                             const InductionKind kind);
        void generateLemmasBoundedEq(const program::WhileStatement* whileStatement,
                                         std::vector<std::shared_ptr<const logic::Lemma>>& lemmas);
    };
    
    /* LEMMA 2
     * check whether we can syntactically conclude (from looking at the program code) that a variable v is not changed in a loop l. In such a case, we add the lemma
     * forall it, pos. v(l(it),pos) = v(l(zero), pos).
     *
     * Discussion on possible Variations:
     *  No other ideas for now.
     *
     * Why is this lemma useful:
     *  Vampire could discover this consequence semantically, but it takes much longer to do so.
     *  TODO: get a better understanding why this lemma helps.
     */
    class StaticAnalysisLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement, std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
        
        std::unordered_set<std::shared_ptr<const program::Variable>> computeAssignedVars(const program::Statement* statement);
    };
}



#endif
