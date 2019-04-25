#ifndef __Problem__
#define __Problem__

#include <iostream>
#include <memory>
#include <vector>

#include "Formula.hpp"
#include "Problem.hpp"

namespace logic {
    
    /*
     * abstract class for wrapping a formula and tagging it as either axiom, lemma or conjecture
     */
    class ProblemItem
    {
    public:
        enum class Type { Program, Axiom, Lemma, Conjecture};
        
        ProblemItem(Type type, std::shared_ptr<const logic::Formula> formula, std::string name) : type(type), formula(formula), name(name) {}
        virtual ~ProblemItem() = default;
        
        const Type type;
        const std::shared_ptr<const logic::Formula> formula;
        const std::string name;
    };
    
    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector<std::shared_ptr<const ProblemItem>>& f);
    
    class Axiom : public ProblemItem
    {
    public:
        Axiom(std::shared_ptr<const logic::Formula> axiom, std::string name = "") : ProblemItem(ProblemItem::Type::Axiom, axiom, name){}
    };
    
    class Lemma : public ProblemItem
    {
    public:
        Lemma(std::shared_ptr<const logic::Formula> lemma, std::string name = "") : ProblemItem(ProblemItem::Type::Lemma, lemma, name){}
    };
    
    class Conjecture : public ProblemItem
    {
    public:
        Conjecture(std::shared_ptr<const logic::Formula> conjecture, std::string name = "") : ProblemItem(ProblemItem::Type::Conjecture, conjecture, name){}
    };
    
    // represents a first-order reasoning task which can be passed to a prover.
    // for now sorts and signatures are not part of Problem, maybe should be refactored.
    class ReasoningTask
    {
    public:
        ReasoningTask(std::vector<std::shared_ptr<const Axiom>> axioms, std::shared_ptr<const Conjecture> conjecture) : axioms(axioms), conjecture(conjecture) {}
        
        const std::vector<std::shared_ptr<const Axiom>> axioms;
        const std::shared_ptr<const Conjecture> conjecture;

        /*
         * generate a new file in the directory 'dirPath' and output the reasoning task in SMTLIB syntax.
         * the preamble string is added at the beginning of the file.
         */
        void outputSMTLIBToDir(std::string dirPath, std::string preamble) const;
        
    private:
        void outputSMTLIB(std::ostream& ostr) const;
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
