#ifndef __Program__
#define __Program__

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cassert>

#include "Expression.hpp"
#include "Statements.hpp"

namespace program
{
    class Function
    {
    public:
        Function(std::string name,
                 std::vector<std::shared_ptr<const Statement>> statements)
        : name(name), statements(std::move(statements))
        {
            // TODO: add a skip-statement instead, maybe already during parsing (challenge: unique numbering)
            assert(this->statements.size() > 0);
        }
        
        const std::string name;
        const std::vector<std::shared_ptr<const Statement>> statements;
    };
    std::ostream& operator<<(std::ostream& ostr, const Function& p);

    class Program
    {
    public:
        Program(std::vector< std::shared_ptr<const Function>> functions) : functions(std::move(functions))
        {
            // TODO: enforce that one of the functions is called main
//                assert(this->functions.size() > 0);
        }
        
        const std::vector< std::shared_ptr<const Function>> functions;
    };
    std::ostream& operator<<(std::ostream& ostr, const Program& p);
}

#endif
