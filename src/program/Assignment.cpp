#include "Assignment.hpp"

namespace program
{
    std::ostream& operator<<(std::ostream& ostr, const Statement& v){ostr << v.toString(0); return ostr;};

    std::string IntAssignment::toString(int indentation) const
    {
        return std::string(indentation, ' ') + lhs->toString() + " = " + rhs->toString();
    }

    std::string IfElse::toString(int indentation) const
    {
        auto s = std::string(indentation, ' ') + "if (" + condition->toString() + ")\n";
        s += std::string(indentation, ' ') + "{\n";
        for (const auto& statement : ifStatements)
        {
            s += statement->toString(indentation + 3) + "\n";
        }
        s += std::string(indentation, ' ') + "}\n";
        s += std::string(indentation, ' ') + "else\n";
        s += std::string(indentation, ' ') + "{\n";
        for (const auto& statement : elseStatements)
        {
            s += statement->toString(indentation + 3) + "\n";
        }
        s += std::string(indentation, ' ') + "}";
        return s;
    }
    
    std::string WhileStatement::toString(int indentation) const
    {
        auto s = std::string(indentation, ' ') + "while (" + condition->toString() + ")\n";
        s += std::string(indentation, ' ') + "{\n";
        for (const auto& statement : bodyStatements)
        {
            s += statement->toString(indentation + 3) + "\n";
        }
        s += std::string(indentation, ' ') + "}";
        return s;
    }
    
    std::string SkipStatement::toString(int indentation) const
    {
        return std::string(indentation, ' ') + "skip";
    }
    
    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::Statement>>& e){ostr << "not implemented"; return ostr;}
}
