#ifndef __Theory__
#define __Theory__

#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

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
     * Generates inductionAxiom1 from the induction hypothesis 'inductionHypothesis' (short IH, modelled as function which maps each timepoint to a formula), and adds it to 'items'. 'freeVarSymbols' must contain exactly all free variables of 'inductionHypothesis' different from the free variable over which we perform induction. If two invocations of this function differ in 'inductionHypothesis', they also need to differ in the shortName.
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
    std::tuple<
        std::shared_ptr<logic::Definition>,
        std::shared_ptr<logic::Definition>,
        std::shared_ptr<logic::Definition>,
        std::shared_ptr<logic::Axiom>
    > inductionAxiom1(
        std::string name,
        std::string shortName,
        std::function<std::shared_ptr<const Formula> (std::shared_ptr<const Term>)> inductionHypothesis,
        std::vector<std::shared_ptr<const Symbol>> freeVarSymbols,
        ProblemItem::Visibility visibility = ProblemItem::Visibility::None);

    /*
     * Generates inductionAxiom2 from the induction hypothesis 'inductionHypothesis' (short IH, modelled as function which maps each timepoint to a formula).
     * The induction axiom then has the following form:
     * =>
     *    and
     *       BC
     *       IC
     *    Con
     * where:
     * - the base case BC is defined as
     *   IH(0)
     * - the inductive case IC is defined as
     *   forall it.
     *      =>
     *         and
     *            it<n(t1)
     *            it<n(t2)
     *            IH(it)
     *         IH(s(it))
     * - the conclusion Con is defined as
     *   forall it.
     *      =>
     *         and
     *            it<=n(t1)
     *            it<=n(t2)
     *         IH(it)
     * 
     * InductionAxiom2 differs from inductionAxiom1 in the base case, the inductive case and in
     * the conclusion. It can be obtained from inductionAxiom1 in the following way:
     * 
     * First, instantiate boundL to 0 and boundR to min(n(t1),n(t2)) and simplify using the theory axiom forall it. 0<=it to get:
     * - BC :=
     *   IH(0)
     * - IC :=
     *   forall it.
     *      =>
     *         and
     *            it<min(n(t1),n(t2))
     *            IH(it)
     *         IH(s(it))
     * - Con :=
     *   forall it.
     *      =>
     *         it<=min(n(t1),n(t2))
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
    std::tuple<
        std::shared_ptr<logic::Definition>,
        std::shared_ptr<logic::Definition>,
        std::shared_ptr<logic::Definition>,
        std::shared_ptr<logic::Axiom>
    > inductionAxiom2(
        std::string name,
        std::string shortName,
        std::function<std::shared_ptr<const Formula> (std::shared_ptr<const Term>)> inductionHypothesis,
        std::shared_ptr<const logic::Term> nt1,
        std::shared_ptr<const logic::Term> nt2,
        std::vector<std::shared_ptr<const Symbol>> freeVarSymbols,
        ProblemItem::Visibility visibility = ProblemItem::Visibility::None);
}

#endif

