#ifndef __Problem__
#define __Problem__

#include <iostream>
#include <memory>
#include <vector>

#include "Formula.hpp"

namespace logic {
    
    // represents a first-order reasoning task which can be passed to a prover.
    // for now sorts and signatures are not part of Problem, maybe should be refactored.
    class ReasoningTask
    {
    public:
        ReasoningTask(std::vector<std::shared_ptr<const Formula>> axioms, std::shared_ptr<const Formula> conjecture) : axioms(axioms), conjecture(conjecture) {}
        
        std::vector<std::shared_ptr<const Formula>> axioms;
        std::shared_ptr<const Formula> conjecture;
        
        void outputSMTLIB(std::ostream& ostr) const;
    };
    
    /*
     * abstract class for wrapping a formula and tagging it as either axiom, lemma or conjecture
     */
    class ProblemItem
    {
    public:
        enum class Type { Program, Axiom, Lemma, Conjecture};
        
        ProblemItem(Type type, std::shared_ptr<const logic::Formula> formula) : type(type), formula(formula) {}
        
        const Type type;
        const std::shared_ptr<const logic::Formula> formula;
    };
    
    class Axiom : public ProblemItem
    {
    public:
        Axiom(std::shared_ptr<const logic::Formula> axiom) : ProblemItem(ProblemItem::Type::Axiom, axiom){}
    };
    
    class Lemma : public ProblemItem
    {
    public:
        Lemma(std::shared_ptr<const logic::Formula> lemma) : ProblemItem(ProblemItem::Type::Lemma, lemma){}
    };
    
    class Conjecture : public ProblemItem
    {
    public:
        Conjecture(std::shared_ptr<const logic::Formula> conjecture) : ProblemItem(ProblemItem::Type::Conjecture, conjecture){}
    };
    
    /* represents a list of axioms, lemmas and conjectures,
     * where each lemma and each conjecture needs to be proven
     * from the preceding axioms and lemmas.
     * This is done by generating a ReasoningTask for each such
     * lemma or conjecture, which can then be passed to a prover
     */
    class Problem
    {
    public:
        Problem(std::vector<std::shared_ptr<const ProblemItem>> items) : items(std::move(items)){}
        
        std::vector<std::shared_ptr<const ProblemItem>> items;
        
        std::vector<const ReasoningTask> generateReasoningTasks() const;
    };
}
#endif
