#ifndef __WhileParsingContext__
#define __WhileParsingContext__

#include <memory>

#include "Program.hpp"

namespace parser
{
    /*
     * this class is used to communicate with the bison-parser, in particular to
     * get back the parsed program.
     */
    class WhileParsingContext
    {
    public:
        WhileParsingContext() : inputFile(""), errorFlag(false), program(nullptr){}
        
        std::string inputFile;
        bool errorFlag;
        std::shared_ptr<const program::Program> program;
    };
}

#endif
