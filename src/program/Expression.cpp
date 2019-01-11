#include "Expression.hpp"

#include <iostream>
#include <string>

#include "Options.hpp"

namespace program {
    
    std::string ArithmeticConstant::toString() const
    {
        return std::to_string(value);
    }
    std::string Addition::toString() const
    {
        return "(" + summand1->toString() + ") + (" + summand2->toString() + ")";
    }
    std::string Subtraction::toString() const
    {
        return "(" + child1->toString() + ") - (" + child2->toString() + ")";
    }
    std::string Modulo::toString() const
    {
        return "(" + child1->toString() + ") mod (" + child2->toString() + ")";
    }
    std::string Multiplication::toString() const
    {
        return "(" + factor1->toString() + ") * (" + factor2->toString() + ")";
    }
    std::string BooleanConstant::toString() const
    {
        return value ? "true" : "false";
    }
    std::string BooleanAnd::toString() const
    {
        return "(" + child1->toString() + ") && (" + child2->toString() + ")";
    }
    std::string BooleanOr::toString() const
    {
        return "(" + child1->toString() + ") || (" + child2->toString() + ")";
    }
    std::string BooleanNot::toString() const
    {
        return "!(" + child->toString() + ")";
    }
    std::string ArithmeticComparison::toString() const
    {
        if (kind == ArithmeticComparison::Kind::GT)
        {
            return child1->toString() + " > " + child2->toString();
        }
        else if (kind == ArithmeticComparison::Kind::GE)
        {
            return child1->toString() + " >= " + child2->toString();
        }
        else if (kind == ArithmeticComparison::Kind::LT)
        {
            return child1->toString() + " < " + child2->toString();
        }
        else if (kind == ArithmeticComparison::Kind::LE)
        {
            return child1->toString() + " <= " + child2->toString();
        }
        else
        {
            assert(kind == ArithmeticComparison::Kind::EQ);
            return child1->toString() + " == " + child2->toString();
        }
    }
    
    std::ostream& operator<<(std::ostream& ostr, const IntExpression& e)
    {
        ostr << e.toString();
        return ostr;
    }
    std::ostream& operator<<(std::ostream& ostr, const BoolExpression& e)
    {
        ostr << e.toString();
        return ostr;
    }

}

