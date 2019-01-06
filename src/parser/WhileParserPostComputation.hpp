#ifndef __WhileParserPostComputation__
#define __WhileParserPostComputation__

#include <unordered_map>
#include "Program.hpp"
#include "Term.hpp"

namespace parser
{
    /*
     * TODO: can't we just use a context during parsing?
     * we need to know for each statement in which loops it is nested in.
     * we therefore compute this information at the end of parsing.
     * the enclosingLoops are added directly to each statement.
     */
    class WhileParserPostComputation
    {
    public:
        static void addEnclosingLoops(const program::Program& program);

    private:
        static void addEnclosingLoopsForStatement(const program::Statement* statement,
                                                  std::vector<const program::WhileStatement*> enclosingLoops);
    };
}

#endif

