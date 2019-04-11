#ifndef __PropertiesTime__
#define __PropertiesTime__

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Formula.hpp"
#include "Sort.hpp"
#include "Term.hpp"
#include "Expression.hpp"
#include "Variable.hpp"
#include "Program.hpp"
#include "ProgramTraverser.hpp"
#include "Problem.hpp"

namespace analysis {
    
    class StandardInductionLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
       
        virtual void generateOutputFor(const program::WhileStatement* statement, std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
        
    private:
        enum class InductionKind { Equal, Less, Greater, LessEqual, GreaterEqual};

        void generateStandardInductionLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Lemma>>& lemmas,
                                             const InductionKind kind);
    };
    
    class TwoTracesLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };

    class NEqualLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };
    
    class AtLeastOneIterationLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };
    
    class IntermediateValueLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };
    
    class ValuePreservationLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };
    
    class EqualityPreservationLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
        void generateEqualityPreservationLemmasZeroToRight(const program::WhileStatement* whileStatement,
                                                           std::vector<std::shared_ptr<const logic::Lemma>>& lemmas);
        void generateEqualityPreservationLemmasLeftToEnd(const program::WhileStatement* whileStatement,
                                                         std::vector<std::shared_ptr<const logic::Lemma>>& lemmas);
        void generateEqualityPreservationLemmasLeftToRight(const program::WhileStatement* whileStatement,
                                                           std::vector<std::shared_ptr<const logic::Lemma>>& lemmas);
    };
    
    class IterationInjectivityLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };
    
    class OrderingSynchronizationLemmas : public ProgramTraverser<std::vector<std::shared_ptr<const logic::Lemma>>>
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateOutputFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Lemma>>& lemmas) override;
    };
    
    std::vector<std::shared_ptr<const logic::Lemma>> generateTraceLemmas(const program::Program& program,
                                                                               std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                                                                               bool twoTraces);
}

#endif

