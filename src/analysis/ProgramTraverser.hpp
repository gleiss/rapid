#ifndef __ProgramTraverser__
#define __ProgramTraverser__

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include "Formula.hpp"

#include "Program.hpp"
#include "Variable.hpp"


namespace analysis
{
    // abstract traversal of programs for generating formulas
    // the main method generateFormulas() traverses the program and calls the
    // corresponding version of generateFormulasFor() for each statement of the program
    // the intended usage is to subclass this class and override
    // those generateFormulasFor() methods for the relevant statements.
    class ProgramTraverser
    {
    public:
        ProgramTraverser(const program::Program& program,
                         std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                         bool twoTraces) :
        program(program),
        locationToActiveVars(locationToActiveVars),
        twoTraces(twoTraces) {}
        
        void generateFormulas(std::vector<std::shared_ptr<const logic::Formula>>& formulas);
        
    protected:
        const program::Program& program;
        const std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars;
        const bool twoTraces;
        
    private:
        void visitStatement(const program::Statement* statement, std::vector<std::shared_ptr<const logic::Formula>>& formulas);
        
        virtual void generateFormulasFor(const program::IntAssignment* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas);
        virtual void generateFormulasFor(const program::IfElse* statement,         std::vector<std::shared_ptr<const logic::Formula>>& formulas);
        virtual void generateFormulasFor(const program::WhileStatement* statement, std::vector<std::shared_ptr<const logic::Formula>>& formulas);
        virtual void generateFormulasFor(const program::SkipStatement* statement,  std::vector<std::shared_ptr<const logic::Formula>>& formulas);
    };
}
#endif
