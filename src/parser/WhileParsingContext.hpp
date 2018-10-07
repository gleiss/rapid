#ifndef __WhileParsingContext__
#define __WhileParsingContext__

#include <memory>
#include <vector>
#include "Formula.hpp"
#include "Program.hpp"

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
        
        std::string inputFile;
        bool errorFlag;
        std::unique_ptr<const program::Program> program;
        std::unique_ptr<const program::ProgramGlobalProperties> programGlobalProperties;
        std::shared_ptr<const logic::Formula> conjecture;
    };
}

#endif
