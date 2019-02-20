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

namespace analysis {
    
    class TraceLemmas
    {
    public:
        TraceLemmas(const program::Program& program,
                    std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                    bool twoTraces) :
        program(program),
        locationToActiveVars(locationToActiveVars),
        twoTraces(twoTraces) {}
        
        std::vector<std::shared_ptr<const logic::Formula>> generate();
        
    private:
        const program::Program& program;
        const std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars;
        const bool twoTraces;
        
        enum class InductionKind { Equal, Less, Greater, LessEqual, GreaterEqual};
        
        void generateStandardInductionLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateStandardInductionLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateStandardInductionLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas,
                                             const InductionKind kind);
        
        void generateTwoTracesLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateTwoTracesLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateTwoTracesLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        
        void generateNEqualLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateNEqualLemmas(const program::Statement* statement,
                                     std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateNEqualLemmas(const program::WhileStatement* whileStatement,
                                     std::vector<std::shared_ptr<const logic::Formula>>& lemmas);

        void generateAtLeastOneIterationLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateAtLeastOneIterationLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateAtLeastOneIterationLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);

        void generateIntermediateValueLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateIntermediateValueLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateIntermediateValueLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);

        void generateValuePreservationLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateValuePreservationLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateValuePreservationLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);

        void generateEqualityPreservationLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateEqualityPreservationLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateEqualityPreservationLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);


        void generateEqualityPreservationLemmasZeroToRight(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateEqualityPreservationLemmasZeroToRight(const program::Statement* statement,
                                              std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateEqualityPreservationLemmasZeroToRight(const program::WhileStatement* whileStatement,
                                                                                  std::vector<std::shared_ptr<const logic::Formula>>& lemmas);

        void generateEqualityPreservationLemmasLeftToEnd(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateEqualityPreservationLemmasLeftToEnd(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateEqualityPreservationLemmasLeftToEnd(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);

        void generateEqualityPreservationLemmasLeftToRight(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateEqualityPreservationLemmasLeftToRight(const program::Statement* statement,
                                            std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateEqualityPreservationLemmasLeftToRight(const program::WhileStatement* whileStatement,
                                            std::vector<std::shared_ptr<const logic::Formula>>& lemmas);

        void generateIterationInjectivityLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateIterationInjectivityLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateIterationInjectivityLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);

        void generateOrderingSynchronizationLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateOrderingSynchronizationLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);
        void generateOrderingSynchronizationLemmas(const program::WhileStatement* whileStatement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas);

    };
}

#endif

