#ifndef __StaticAnalysis__
#define __StaticAnalysis__

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Program.hpp"
#include "Variable.hpp"
#include "Formula.hpp"

#include "ProgramTraverser.hpp"

namespace analysis
{
    class StaticAnalysisLemmas : public ProgramTraverser
    {
    public:
        using ProgramTraverser::ProgramTraverser; // inherit initializer, note: doesn't allow additional members in subclass!
        
    private:
        virtual void generateFormulasFor(const program::WhileStatement* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas) override;
        
        std::unordered_set<std::shared_ptr<const program::Variable>> computeAssignedVars(const program::Statement* statement);
    };
}
#endif
