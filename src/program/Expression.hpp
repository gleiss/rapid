/**
 * @file Expression.hpp
 *
 * Defines class Program::Expression, representing expressions in the
 * guarded command language
 *
 */

#ifndef __ProgramExpression__
#define __ProgramExpression__

#include "Sort.hpp"
#include "Term.hpp"
#include "Formula.hpp"

namespace program {
    
    class IntExpression
    {
    public:
        enum class Type{ IntVariableAccess, IntArrayApplication, Other};
        virtual Type type() const {return Type::Other;}
        
        virtual std::string toString() const = 0;
        virtual std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const logic::Term> index) const = 0;
    };
    std::ostream& operator<<(std::ostream& ostr, const IntExpression& e);

    class ArithmeticConstant : public IntExpression
    {
    public:
        ArithmeticConstant(unsigned value) : value(value){}
        
        const int value;
        
        std::string toString() const override;
        std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const logic::Term> index) const override;
    };
    
    class Addition : public IntExpression
    {
    public:
        Addition(std::shared_ptr<const IntExpression> summand1, std::shared_ptr<const IntExpression> summand2)
        : summand1(std::move(summand1)), summand2(std::move(summand2)){}
        
        const std::shared_ptr<const IntExpression> summand1;
        const std::shared_ptr<const IntExpression> summand2;
        
        std::string toString() const override;
        std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const logic::Term> index) const override;
    };

    class Subtraction : public IntExpression
    {
    public:
        Subtraction(std::shared_ptr<const IntExpression> child1, std::shared_ptr<const IntExpression> child2)
        : child1(std::move(child1)), child2(std::move(child2)){}
        
        const std::shared_ptr<const IntExpression> child1;
        const std::shared_ptr<const IntExpression> child2;
        
        std::string toString() const override;
        std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const logic::Term> index) const override;
    };
    
    class Multiplication : public IntExpression
    {
    public:
        Multiplication(std::shared_ptr<const IntExpression> factor1, std::shared_ptr<const IntExpression> factor2)
        : factor1(std::move(factor1)), factor2(std::move(factor2)){}

        const std::shared_ptr<const IntExpression> factor1;
        const std::shared_ptr<const IntExpression> factor2;
        
        std::string toString() const override;
        std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const logic::Term> index) const override;
    };
    
    class UnaryMinus : public IntExpression
    {
    public:
        UnaryMinus(std::shared_ptr<const IntExpression> child) :child(std::move(child)){}

        const std::shared_ptr<const IntExpression> child;
        
        std::string toString() const override;
        std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const logic::Term> index) const override;
    };
    
    class BoolExpression
    {
    public:
        virtual std::string toString() const = 0;
        virtual std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const logic::Term> index) const = 0;
    };
    std::ostream& operator<<(std::ostream& ostr, const BoolExpression& e);

    class BooleanConstant : public BoolExpression
    {
    public:
        BooleanConstant(bool value) : value(value){}
        
        const bool value;
        
        std::string toString() const override;
        std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const logic::Term> index) const override;
    };
    
    class BooleanAnd : public BoolExpression
    {
    public:
        BooleanAnd(std::shared_ptr<const BoolExpression> child1, std::shared_ptr<const BoolExpression> child2)
        : child1(std::move(child1)), child2(std::move(child2)){}

        const std::shared_ptr<const BoolExpression> child1;
        const std::shared_ptr<const BoolExpression> child2;
        
        std::string toString() const override;
        std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const logic::Term> index) const override;
    };
    
    class BooleanOr : public BoolExpression
    {
    public:
        BooleanOr(std::shared_ptr<const BoolExpression> child1, std::shared_ptr<const BoolExpression> child2)
        : child1(std::move(child1)), child2(std::move(child2)){}
        
        const std::shared_ptr<const BoolExpression> child1;
        const std::shared_ptr<const BoolExpression> child2;
        
        std::string toString() const override;
        std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const logic::Term> index) const override;
    };
    
    class BooleanNot : public BoolExpression
    {
    public:
        BooleanNot(std::shared_ptr<const BoolExpression> child)
        : child(std::move(child)){}
        
        const std::shared_ptr<const BoolExpression> child;
        
        std::string toString() const override;
        std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const logic::Term> index) const override;
    };
    
    class ArithmeticComparison : public BoolExpression
    {
    public:
        enum class Kind {
            GE,
            GT,
            LE,
            LT,
            EQ
        };
        
        ArithmeticComparison(Kind kind, std::shared_ptr<const IntExpression> child1, std::shared_ptr<const IntExpression> child2)
        : kind(kind), child1(std::move(child1)), child2(std::move(child2)){}
        
        const Kind kind;
        const std::shared_ptr<const IntExpression> child1;
        const std::shared_ptr<const IntExpression> child2;
        
        std::string toString() const override;
        std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const logic::Term> index) const override;
    };
    

}
#endif
