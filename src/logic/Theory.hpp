#ifndef __Theory__
#define __Theory__

#include <memory>
#include <string>
#include <functional>

#include "Term.hpp"
#include "Formula.hpp"
#include "Problem.hpp"

namespace logic {
    
    class Theory {
    public:
        static void declareTheories();
        
        static std::shared_ptr<const FuncTerm> intConstant(int i);
        static std::shared_ptr<const FuncTerm> intAddition(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const FuncTerm> intSubtraction(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const FuncTerm> intModulo(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const FuncTerm> intMultiplication(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
        static std::shared_ptr<const FuncTerm> intAbsolute(std::shared_ptr<const Term> t);

        static std::shared_ptr<const Formula> intLess(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label="");
        static std::shared_ptr<const Formula> intLessEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label="");
        static std::shared_ptr<const Formula> intGreater(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label="");
        static std::shared_ptr<const Formula> intGreaterEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label="");
        
        static std::shared_ptr<const Formula> boolTrue(std::string label="");
        static std::shared_ptr<const Formula> boolFalse(std::string label="");

        static std::shared_ptr<const FuncTerm> natZero();
        static std::shared_ptr<const FuncTerm> natSucc(std::shared_ptr<const Term> term);
        static std::shared_ptr<const FuncTerm> natPre(std::shared_ptr<const Term> term);
        static std::shared_ptr<const Formula> natSub(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label="");
        static std::shared_ptr<const Formula> natSubEq(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label="");
    };
    
    /*
     * Generates inductionAxiom1 from the induction hypothesis 'inductionHypothesis' (short IH, modelled as function which maps each timepoint to a formula), and adds it to 'items'. 'freeVars' must contain exactly all free variables of 'inductionHypothesis' different from the free variable over which we perform induction.
     * The induction axiom then has the following form:
     * forall boundL,boundR.
     *    =>
     *       and
     *          BC(boundL)
     *          IC(boundL, boundR)
     *       Con(boundL, boundR),
     * where:
     * - the base case BC(boundL) is defined as
     *   IH(boundL)
     * - the inductive case IC(boundL,boundR) is defined as
     *   forall it.
     *      =>
     *         and
     *            boundL<=it<boundR
     *            IH(it)
     *         IH(s(it))
     * - the conclusion Con(boundL,boundR) is defined as
     *   forall it.
     *      =>
     *         boundL<=it<=boundR
     *         IH(it)
     */
    void addInductionAxiom1(
        std::string name,
        std::string shortName,
        std::function<std::shared_ptr<const Formula> (std::shared_ptr<const Term>)> inductionHypothesis,
        std::vector<std::shared_ptr<const Symbol>> freeVars,
        std::vector<std::shared_ptr<const logic::ProblemItem>>& items);

    /*
     * Generates inductionAxiom2 from the induction hypothesis 'inductionHypothesis' (short IH, modelled as function which maps each timepoint to a formula).
     * The induction axiom then has the following form:
     * forall boundL,boundR1,boundR2.
     *    =>
     *       and
     *          BC(boundL)
     *          IC(boundL,boundR1,boundR2)
     *       Con(boundL,boundR1,boundR2)
     * where:
     * - the base case BC(boundL) is defined as
     *   IH(boundL)
     * - the inductive case IC(boundL,boundR1,boundR2) is defined as
     *   forall it.
     *      =>
     *         and
     *            boundL<=it
     *            it<boundR1
     *            it<boundR2
     *            IH(it)
     *         IH(s(it))
     * - the conclusion Con(boundL,boundR1,boundR2) is defined as
     *   forall it.
     *      =>
     *         and
     *            boundL<=it
     *            it<=boundR1
     *            it<=boundR2
     *         IH(it)
     * 
     * InductionAxiom2 only differs from inductionAxiom1 in the inductive case and in 
     * the conclusion. It can be obtained from inductionAxiom1 in the following way:
     * 
     * First, instantiate boundR to min(boundR1,boundR2) to get:
     * - IC(boundL,boundR1,boundR2):
     *   forall it.
     *      =>
     *         and
     *            boundL<=it
     *            it<min(boundR1,boundR2)
     *            IH(it)
     *         IH(s(it))
     * - Con(boundL,boundR1,boundR2):
     *   forall it.
     *      =>
     *         boundL<=it<=min(boundR1,boundR2)
     *         IH(it)
     *
     * Then eliminate min from both the inductive case and the conclusion by rewriting it using the following two defining axioms:
     * Axiom1:
     * forall it,it1,it2.
     *    <=>
     *       it < min(it1,it2)
     *       and
     *          it < it1
     *          it < it2
     * Axiom2:
     * forall it,it1,it2.
     *    <=>
     *       it <= min(it1,it2)
     *       and
     *          it <= it1
     *          it <= it2
     * 
     * As result we get inductionAxiom2.
     */
    std::shared_ptr<const Formula> inductionAxiom2(std::function<std::shared_ptr<const Formula> (std::shared_ptr<const Term>)> inductionHypothesis);
    
}

#endif

