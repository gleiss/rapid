#include "Expression.hpp"

#include "Sort.hpp"
#include "Theory.hpp"
#include "Options.hpp"

namespace program {
    
//    bool ArithmeticExpression::evalToCstInt(int &value)
//    {
//        int a, b;
//        switch (_kind) {
//            case ArithExprKind::EXP_CST_INTEGER:
//                value = _constInfo;
//                return true;
//            case ArithExprKind::EXP_ADD:
//                if (_children[0]->evalToCstInt(a) && _children[1]->evalToCstInt(b)) {
//                    value = a + b;
//                    return true;
//                }
//                return false;
//            case ArithExprKind::EXP_SUB:
//                if (_children[0]->evalToCstInt(a) && _children[1]->evalToCstInt(b)) {
//                    value = a - b;
//                    return true;
//                }
//                return false;
//            case ArithExprKind::EXP_MUL:
//                if (_children[0]->evalToCstInt(a) && _children[1]->evalToCstInt(b)) {
//                    value = a * b;
//                    return true;
//                }
//                return false;
//            case ArithExprKind::EXP_DIV:
//                if (_children[0]->evalToCstInt(a) && _children[1]->evalToCstInt(b)) {
//                    value = a / b;
//                    return true;
//                }
//                return false;
//            case ArithExprKind::EXP_MINUS:
//                if (_children[0]->evalToCstInt(a)) {
//                    value = -a;
//                    return true;
//                }
//                return false;
//        }
//        return false;
//    }
    
//    bool ArithmeticExpression::equivToVPlusX(const PVariable *v, int &incr) const
//    {
//        Expression *e1, *e2;
//        int a, b;
//        switch (_kind) {
//            case ArithExprKind::EXP_ADD:
//                e1 = _children[0];
//                e2 = _children[1];
//                if (e1->equivToVPlusX(v, a) && e2->evalToCstInt(b)) {
//                    incr = a + b;
//                    return true;
//                } else if (e1->evalToCstInt(a) && e2->equivToVPlusX(v, b)) {
//                    incr = a + b;
//                    return true;
//                }
//                return false;
//            case ArithExprKind::EXP_SUB:
//                e1 = _children[0];
//                e2 = _children[1];
//                if (e1->equivToVPlusX(v, a) && e2->evalToCstInt(b)) {
//                    incr = a - b;
//                    return true;
//                }
//                return false;
//            default:
//                return false;
//        }
//    }
    
//    bool LocationExpression::equivToVPlusX(const PVariable *v, int &incr) const
//    {
//        if (_kind == LocationExprKind::EXP_VAR_LOC && _var == v) {
//            incr = 0;
//            return true;
//        }
//        return false;
//    }
    
    std::shared_ptr<const logic::Term> ArithmeticConstant::toTerm(std::shared_ptr<const logic::Term> index) const
    {
        return logic::Theory::intConstant(value);
    }
    std::shared_ptr<const logic::Term> Addition::toTerm(std::shared_ptr<const logic::Term> index) const
    {
        return logic::Theory::intAddition(summand1->toTerm(index), summand2->toTerm(index));
    }
    std::shared_ptr<const logic::Term> Subtraction::toTerm(std::shared_ptr<const logic::Term> index) const
    {
        return logic::Theory::intSubtraction(child1->toTerm(index), child2->toTerm(index));
    }
    std::shared_ptr<const logic::Term> Multiplication::toTerm(std::shared_ptr<const logic::Term> index) const
    {
        return logic::Theory::intMultiplication(factor1->toTerm(index), factor2->toTerm(index));
    }
    std::shared_ptr<const logic::Term> UnaryMinus::toTerm(std::shared_ptr<const logic::Term> index) const
    {
        return logic::Theory::intUnaryMinus(child->toTerm(index));
    }
    
    std::shared_ptr<const logic::Formula> BooleanConstant::toFormula(std::shared_ptr<const logic::Term> index) const
    {
        if (value)
        {
            return logic::Theory::boolTrue();
        }
        else
        {
            return logic::Theory::boolFalse();
        }
    }
    std::shared_ptr<const logic::Formula> BooleanAnd::toFormula(std::shared_ptr<const logic::Term> index) const
    {
        return logic::Formulas::conjunctionFormula({child1->toFormula(index), child2->toFormula(index)});
    }
    std::shared_ptr<const logic::Formula> BooleanOr::toFormula(std::shared_ptr<const logic::Term> index) const
    {
        return logic::Formulas::disjunctionFormula({child1->toFormula(index), child2->toFormula(index)});
    }
    std::shared_ptr<const logic::Formula> BooleanNot::toFormula(std::shared_ptr<const logic::Term> index) const
    {
        return logic::Formulas::negationFormula(child->toFormula(index));
    }
    std::shared_ptr<const logic::Formula> ArithmeticComparison::toFormula(std::shared_ptr<const logic::Term> index) const
    {
        if (kind == ArithmeticComparison::Kind::GT)
        {
            return logic::Theory::intGreater(child1->toTerm(index), child2->toTerm(index));
        }
        else if (kind == ArithmeticComparison::Kind::GE)
        {
            return logic::Theory::intGreaterEqual(child1->toTerm(index), child2->toTerm(index));
        }
        else if (kind == ArithmeticComparison::Kind::LT)
        {
            return logic::Theory::intLess(child1->toTerm(index), child2->toTerm(index));
        }
        else if (kind == ArithmeticComparison::Kind::LE)
        {
            return logic::Theory::intLessEqual(child1->toTerm(index), child2->toTerm(index));
        }
        else
        {
            assert(kind == ArithmeticComparison::Kind::EQ);
            return logic::Formulas::equalityFormula(true, child1->toTerm(index), child2->toTerm(index));
        }
    }
    
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
    std::string Multiplication::toString() const
    {
        return "(" + factor1->toString() + ") * (" + factor2->toString() + ")";
    }
    std::string UnaryMinus::toString() const
    {
        return "-(" + child->toString() + ")";
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

