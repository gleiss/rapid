#include "Program.hpp"

namespace program
{
    std::ostream& operator<<(std::ostream& ostr, const Function& f)
    {
        ostr << f.name << "()\n";
        ostr << "{\n";
        for (const auto& var : f.vars)
        {
            if (var->isArray)
            {
                ostr << std::string(3, ' ') << "int[] " << var->name << "\n";
            }
            else
            {
                ostr << std::string(3, ' ') << "int " << var->name << "\n";
            }
        }
        ostr << "\n";
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
