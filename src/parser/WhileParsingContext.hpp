#ifndef __WhileParsingContext__
#define __WhileParsingContext__

#include <memory>
#include <vector>
#include "Formula.hpp"
#include "Program.hpp"
#include "Signature.hpp"

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
        WhileParsingContext() : inputFile(""), errorFlag(false), program(nullptr){}
        
        // input
        std::string inputFile;
        bool errorFlag;
        
        // output
        std::unique_ptr<const program::Program> program;
        std::unique_ptr<const program::ProgramGlobalProperties> programGlobalProperties;
        std::shared_ptr<const logic::Formula> conjecture;
        
    private:
        // context-informatino
        std::unordered_map<std::string, const logic::Symbol*> quantifiedVarsDeclarations;
        std::vector<std::vector<std::string>> quantifiedVarsStack;
    public:
        bool pushQuantifiedVars(std::vector<const logic::Symbol*> quantifiedVars);
        void popQuantifiedVars();
        
        // fetch symbol with given name from quantVarDecls or Signature.
        const logic::Symbol* fetch(std::string name);

    };
}

#endif
