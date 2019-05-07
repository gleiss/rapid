#ifndef __Theory__
#define __Theory__

#include <memory>
#include <string>
#include <functional>

#include "Term.hpp"
#include "Formula.hpp"

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
     * Generates an induction axiom from an induction hypothesis 'inductionHypothesis' (modelled as function which maps each timepoint to a formula)
     * The induction axiom has the following form:
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
     */
    std::shared_ptr<const Formula> inductionAxiom1(std::function<std::shared_ptr<const Formula> (std::shared_ptr<const Term>)> inductionHypothesis);

    /*
     * Generates the following variant of inductionAxiom1
     * First, instantiate boundR to min(boundR1,boundR2) to get
     * forall boundL,boundR1,boundR2.
     *    =>
     *       and
     *          P(boundL)
     *          forall it.
     *             =>
     *                and
     *                   boundL<=it
     *                   it<min(boundR1,boundR2)
     *                   P(it)
     *                P(s(it))
     *       forall it.
     *          =>
     *             boundL<=it
     *             it<=min(boundR1,boundR2)
     *             P(it)
     *
     * Then eliminate min from the axiom by rewriting it using the following two defining axioms:
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
     * As result we get inductionAxiom2:
     * forall boundL,boundR1,boundR2.
     *    =>
     *       and
     *          P(boundL)
     *          forall it.
     *             =>
     *                and
     *                   boundL<=it
     *                   it<boundR1
     *                   it<boundR2
     *                   P(it)
     *                P(s(it))
     *       forall it.
     *          =>
     *             boundL<=it
     *             it<=boundR1
     *             it<=boundR2
     *             P(it)
     */
    std::shared_ptr<const Formula> inductionAxiom2(std::function<std::shared_ptr<const Formula> (std::shared_ptr<const Term>)> inductionHypothesis);
    
}

#endif

