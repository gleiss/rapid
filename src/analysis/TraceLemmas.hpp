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

namespace analysis {
    
    class StandardInductionLemmas : public ProgramTraverser
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
       
        virtual void generateFormulasFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas) override;
        
    private:
        enum class InductionKind { Equal, Less, Greater, LessEqual, GreaterEqual};

        void generateStandardInductionLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas,
                                             const InductionKind kind);
    };
    
    class TwoTracesLemmas : public ProgramTraverser
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateFormulasFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas) override;
    };

    class NEqualLemmas : public ProgramTraverser
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateFormulasFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas) override;
    };
    
    class AtLeastOneIterationLemmas : public ProgramTraverser
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateFormulasFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas) override;
    };
    
    class IntermediateValueLemmas : public ProgramTraverser
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateFormulasFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas) override;
    };
    
    class ValuePreservationLemmas : public ProgramTraverser
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateFormulasFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas) override;
    };
    
    class EqualityPreservationLemmas : public ProgramTraverser
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateFormulasFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas) override;
        void generateEqualityPreservationLemmasZeroToRight(const program::WhileStatement* whileStatement,
                                                           std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateEqualityPreservationLemmasLeftToEnd(const program::WhileStatement* whileStatement,
                                                         std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateEqualityPreservationLemmasLeftToRight(const program::WhileStatement* whileStatement,
                                                           std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
    };
    
    class IterationInjectivityLemmas : public ProgramTraverser
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateFormulasFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas) override;
    };
    
    class OrderingSynchronizationLemmas : public ProgramTraverser
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateFormulasFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas) override;
    };
    
    std::vector<std::shared_ptr<const logic::Formula>> generateTraceLemmas(const program::Program& program,
                                                                               std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                                                                               bool twoTraces);
}

#endif

