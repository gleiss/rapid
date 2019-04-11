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
    // abstract traversal of programs for generating output, e.g. for generating formulas.
    // the main method generate() traverses the program and calls the
    // corresponding version of generateOutputFor() for each statement of the program
    // the intended usage is to subclass this class and override
    // those generateOutputFor() methods for the relevant statements.
    template <class OutputType>
    class ProgramTraverser
    {
    public:
        ProgramTraverser(const program::Program& program,
                         std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                         bool twoTraces) :
        program(program),
        locationToActiveVars(locationToActiveVars),
        twoTraces(twoTraces) {}
        
        void generate(OutputType& output);
        
    protected:
        const program::Program& program;
        const std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars;
        const bool twoTraces;
        
    private:
        void visitStatement(const program::Statement* statement, OutputType& output);
        
        virtual void generateOutputFor(const program::IntAssignment* statement,  OutputType& output);
        virtual void generateOutputFor(const program::IfElse* statement,         OutputType& output);
        virtual void generateOutputFor(const program::WhileStatement* statement, OutputType& output);
        virtual void generateOutputFor(const program::SkipStatement* statement,  OutputType& output);
    };
    
    
    template <class OutputType>
    void ProgramTraverser<OutputType>::generate(OutputType& output)
    {
        for(const auto& function : program.functions)
        {
            for (const auto& statement : function->statements)
            {
                visitStatement(statement.get(), output);
            }
        }
    }
    
    template <class OutputType>
    void ProgramTraverser<OutputType>::visitStatement(const program::Statement* statement, OutputType& output)
    {
        switch (statement->type())
        {
            case program::Statement::Type::IntAssignment:
            {
                auto castedAssignment = static_cast<const program::IntAssignment*>(statement);
                // generate output
                generateOutputFor(castedAssignment, output);
                break;
            }
                
            case program::Statement::Type::IfElse:
            {
                auto castedIfElse = static_cast<const program::IfElse*>(statement);
                
                // generate output
                generateOutputFor(castedIfElse, output);
                
                // recurse on both branches
                for (const auto& statement : castedIfElse->ifStatements)
                {
                    visitStatement(statement.get(), output);
                }
                for (const auto& statement : castedIfElse->elseStatements)
                {
                    visitStatement(statement.get(), output);
                }
                break;
            }
                
            case program::Statement::Type::WhileStatement:
            {
                auto castedWhile = static_cast<const program::WhileStatement*>(statement);
                
                // generate output
                generateOutputFor(castedWhile, output);
                
                // recurse on body
                for (const auto& statement : castedWhile->bodyStatements)
                {
                    visitStatement(statement.get(), output);
                }
                break;
            }
                
            case program::Statement::Type::SkipStatement:
            {
                auto castedSkip = static_cast<const program::SkipStatement*>(statement);
                // generate output
                generateOutputFor(castedSkip, output);
                break;
            }
                
            default:
            {
                assert(false);
                break;
            }
        }
    }
    
    template <class OutputType>
    void ProgramTraverser<OutputType>::generateOutputFor(const program::IntAssignment* statement, OutputType& output){}
    template <class OutputType>
    void ProgramTraverser<OutputType>::generateOutputFor(const program::IfElse* statement, OutputType& output){}
    template <class OutputType>
    void ProgramTraverser<OutputType>::generateOutputFor(const program::WhileStatement* statement, OutputType& output){}
    template <class OutputType>
    void ProgramTraverser<OutputType>::generateOutputFor(const program::SkipStatement* statement, OutputType& output){}
}
#endif
