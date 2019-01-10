#ifndef __WhileParsingContext__
#define __WhileParsingContext__

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Formula.hpp"
#include "Program.hpp"
#include "Signature.hpp"
#include "Variable.hpp"

namespace parser
{
    /*
     * this class is used to communicate with the bison-parser, in particular to
     * get back the parsed program.
     * furthermore it contains the context-information, which Bison's LALR parser doesn't support
     */
    class WhileParsingContext
    {
    public:
        WhileParsingContext() : inputFile(""), errorFlag(false), program(nullptr), conjecture(nullptr), locationToActiveVars(), twoTraces(false){}
        
        // input
        std::string inputFile;
        bool errorFlag;
        
        // output
        std::unique_ptr<const program::Program> program;
        std::shared_ptr<const logic::Formula> conjecture;
        std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars;
        bool twoTraces;
        
    private:
        // context-information
        std::unordered_map<std::string, std::shared_ptr<const logic::Symbol>> quantifiedVarsDeclarations;
        std::vector<std::vector<std::string>> quantifiedVarsStack;
        
        std::unordered_map<std::string, std::shared_ptr<const program::Variable>> programVarsDeclarations;
        std::vector<std::vector<std::string>> programVarsStack;
                
    public:
        // methods which are called by bison to interact with the context
        bool pushQuantifiedVars(std::vector<std::shared_ptr<const logic::Symbol>> quantifiedVars);
        void popQuantifiedVars();
        bool isDeclared(std::string name);
        std::shared_ptr<const logic::Symbol> fetch(std::string name); // fetch symbol with given name from quantVarDecls or Signature.
        
        void pushProgramVars();
        void popProgramVars();
        bool addProgramVar(std::shared_ptr<const program::Variable> programVar);
        std::shared_ptr<const program::Variable> getProgramVar(std::string name);
        std::vector<std::shared_ptr<const program::Variable>> getActiveProgramVars();
        
        /*
         * we need to know for each statement in which loops it is nested in.
         * we compute this information at the end of parsing each function.
         * the enclosingLoops are added directly to each statement.
         * note that we can't add the enclosing loops on the fly, since the
         * enclosing loop is only constructed after the statement is constructed.
         */
    public:
        void addEnclosingLoops(const program::Function& function);

    private:
        static void addEnclosingLoopsForStatement(const program::Statement* statement,
                                                  std::vector<const program::WhileStatement*> enclosingLoops);
    };
}

#endif
