#ifndef __LemmasVariableValues__
#define __LemmasVariableValues__

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ProgramTraverser.hpp"
#include "Problem.hpp"
#include "Program.hpp"

namespace analysis {
    
    /*
     * We use the lemmas in this header to cover the inductive reasoning about how values of program variables change in given intervals
     */
    
    /*
     * LEMMA 1
     * Induction lemma to reason about how a single value of a variable (-position) evolves over an interval, for reflexive transitive connectives =,<=,>=.
     * (added for both non-array- and array variables. we ignore array positions and nested iterators in this description)
     * forall boundL,boundR.
     *    =>
     *       Prem(boundL, boundR)
     *       =>
     *          boundL<=boundR
     *          v(l(boundL)) C v(l(boundR))
     * where
     * - C is either =, <= or >=
     * - Prem(boundL, boundR) :=
     *   forall it.
     *      =>
     *         and
     *            boundL<=it<boundR
     *            v(l(boundL)) C (l(it))
     *         v(l(boundL)) C (l(s(it)))
     * Soundness: This lemma is an instantiation of the following lemma 1A (by inlining the definitions, and instantiating it in the conclusion to boundR. Note that the first precondition holds for reflexive C):
     * forall boundL,boundR.
     *    =>
     *       and
     *          v(l(boundL)) C (l(boundL))
     *          forall it.
     *             =>
     *                and
     *                   boundL<=it<boundR
     *                   v(l(boundL)) C (l(it))
     *                v(l(boundL)) C (l(s(it)))
     *       forall it.
     *          =>
     *             boundL<=it<=boundR
     *             v(l(boundL)) C v(l(it))
     *
     * Lemma 1A is an instance of the general induction scheme
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
     * where P(it) := v(l(boundL)) C (l(it))
     *
     * Discussion on possible variations:
     * The following variations apply to induction in general and are not specific to the used induction hypothesis.
     * The most general induction lemma states:
     *      if the truth of predicate P is preserved in the interval [boundL,boundR], then P(boundL) implies P(it) for all it in [boundL,boundR].
     * The most important bounds are 0 and the last iteration n. It could speed up Vampire to replace the most general version by more refined ones (with the downside that we are less complete)
     *      if the truth of predicate P is preserved in the interval [0,boundR], then P(0) implies P(it) for all it in [0,boundR].
     *      if the truth of predicate P is preserved in the interval [boundL,n], then P(boundL) implies P(it) for all it in [boundL,n].
     *      if the truth of predicate P is preserved in the interval [0,n], then P(0) implies P(it) for all it in [0,n].
     * TODO:
     *  the most elegant set of lemmas would be to only use the most general version of these lemmas, and include one such lemma for each predicate symbol =,<=,>=.
     *  Can we use this set of lemmas?
     *
     * Why are these lemmas useful:
     *  The most useful instance of these lemmas is the one with equality as connective.
     *  It is used to conclude that a variable value doesn't change in an interval.
     *  The other connectives are not very important. One use case of <= and >= is a loop which computes the maximal element of an array.
     *
     * TODO: we probably want lemmas which cover similar inductive reasoning for the (non-reflexive!) predicates < and >.
     */
    class ValueEvolutionLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::ProblemItem>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
        virtual void generateOutputFor(const program::WhileStatement* statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items) override;
    };
    
    /* LEMMA 2
     * check whether we can syntactically conclude (from looking at the program code) that a variable v is not changed in a loop l. In such a case, we add the lemma
     * forall it.( it<n => v(l(it)) = v(l(zero)) ).
     * (added for both non-array- and array variables. we ignore array positions and enclosing iterators in this description)
     *
     * Soundness: We first statically check that forall it.( it<n => v(l(s(it)))=v(l(it)) ) holds.
     * The added lemma is then the conclusion of the following lemma 2A:
     *    =>
     *       and
     *          v(l(zero)) = v(l(zero))
     *          forall it.
     *             =>
     *                and
     *                   it<n
     *                   v(l(s(it)))=v(l(it))
     *       forall it.
     *          =>
     *             it<=n
     *             P(it)
     *
     * Lemma 2A is an instance of the general induction scheme
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
     * by
     * - substituting boundL->zero, boundR->n
     * - defining P(it) := v(l(zero)) = v(l(it))
     * - applying simplifications which use the reflexivity and transitivity of equality and the fact that 0 is the smallest natural number.
     *
     * Discussion on possible Variations:
     *  Could also add a similar lemma for other reflexive transitive predicates, i.e. for <= and >=. But this doesn't help to much probably,
     *  since those lemmas are much less useful, and anyway also covered logically by the value-evolution-lemma.
     *
     * Why is this lemma useful:
     *  Vampire could discover this consequence semantically, but it takes much longer to do so.
     *  TODO: get a better understanding why this lemma helps.
     */
    class StaticAnalysisLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::ProblemItem>>>
    {
    public:
        StaticAnalysisLemmas(
            const program::Program& program,
            std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
            bool twoTraces,
            std::vector<std::shared_ptr<const logic::Axiom>> programSemantics) : 
            ProgramTraverser<std::vector<std::shared_ptr<const logic::ProblemItem>>>(program, locationToActiveVars, twoTraces), programSemantics(programSemantics) {}
        
    private:
        std::vector<std::shared_ptr<const logic::Axiom>> programSemantics;

        virtual void generateOutputFor(const program::WhileStatement* statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items) override;
        
        std::unordered_set<std::shared_ptr<const program::Variable>> computeAssignedVars(const program::Statement* statement);
    };
}



#endif
