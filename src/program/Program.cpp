#include "Program.hpp"

#include <iostream>

namespace program
{
    std::ostream& operator<<(std::ostream& ostr, const Function& f)
    {
        ostr << f.name << "()\n";
        ostr << "{\n";
        for (const auto& statement : f.statements)
        {
            ostr << statement->toString(3) << "\n";
        }
        ostr << "}\n";
        return ostr;
    }
    std::ostream& operator<<(std::ostream& ostr, const Program& p)
    {
        for (const auto& function : p.functions)
        {
            ostr << (*function) << "\n";
        }
        return ostr;
    }
}
