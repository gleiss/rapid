#include "Statements.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace program
{
    std::ostream& operator<<(std::ostream& ostr, const Statement& v){ostr << v.toString(0); return ostr;};

    std::string IntAssignment::toString(int indentation) const
    {
        return std::string(indentation, ' ') + lhs->toString() + " = " + rhs->toString() + " @" + location;
    }

    std::string IfElse::toString(int indentation) const
    {
        auto s = std::string(indentation, ' ') + "if (" + condition->toString() + ") @" + location + "\n";
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
        auto s = std::string(indentation, ' ') + "while (" + condition->toString() + ") @" + location + "\n";
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
        return std::string(indentation, ' ') + "skip @" + location;
    }
    
    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::Statement>>& e){ostr << "not implemented"; return ostr;}
}
