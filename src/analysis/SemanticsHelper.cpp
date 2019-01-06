#include "SemanticsHelper.hpp"

#include "Variable.hpp"
#include "Term.hpp"
#include "Theory.hpp"

#include "SymbolDeclarations.hpp"

namespace analysis {
    
    std::shared_ptr<const logic::Term> toTermFull(std::shared_ptr<const program::Variable> var, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> trace)
    {
        assert(var != nullptr);
        assert(timePoint != nullptr);
        assert(trace != nullptr);
        
        assert(!var->isArray);
        
        std::vector<std::shared_ptr<const logic::Term>> arguments;
        
        if (!var->isConstant)
        {
            arguments.push_back(timePoint);
        }
        if (var->twoTraces)
        {
            arguments.push_back(trace);
        }
        
        return logic::Terms::func(var->name, arguments, logic::Sorts::intSort());
    }
    
    std::shared_ptr<const logic::Term> toTermFull(std::shared_ptr<const program::Variable> var, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> position, std::shared_ptr<const logic::Term> trace)
    {
        assert(var != nullptr);
        assert(timePoint != nullptr);
        assert(position != nullptr);
        assert(trace != nullptr);
        
        assert(var->isArray);
        
        std::vector<std::shared_ptr<const logic::Term>> arguments;
        
        if (!var->isConstant)
        {
            arguments.push_back(timePoint);
        }
        
        arguments.push_back(position);
        
        if (var->twoTraces)
        {
            auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
            arguments.push_back(logic::Terms::var(tr));
        }
        
        return logic::Terms::func(var->name, arguments, logic::Sorts::intSort());
    }
    
    std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const program::Variable> var, std::shared_ptr<const logic::Term> timePoint)
    {
        assert(var != nullptr);
        assert(timePoint != nullptr);
        
        assert(!var->isArray);
        
        auto trSymbol = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
        auto tr = logic::Terms::var(trSymbol);
        return toTermFull(var, timePoint, tr);
    }
    
    std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const program::Variable> var, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> position)
    {
        assert(var != nullptr);
        assert(timePoint != nullptr);
        assert(position != nullptr);
        
        assert(var->isArray);
        
        auto trSymbol = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
        auto tr = logic::Terms::var(trSymbol);
        return toTermFull(var, timePoint, position, tr);
    }
    
    std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const program::IntExpression> expr, std::shared_ptr<const logic::Term> timePoint)
    {
        assert(expr != nullptr);
        assert(timePoint != nullptr);
        
        switch (expr->type())
        {
            case program::IntExpression::Type::ArithmeticConstant:
            {
                auto castedExpr = std::static_pointer_cast<const program::ArithmeticConstant>(expr);
                return logic::Theory::intConstant(castedExpr->value);
            }
            case program::IntExpression::Type::Addition:
            {
                auto castedExpr = std::static_pointer_cast<const program::Addition>(expr);
                return logic::Theory::intAddition(toTerm(castedExpr->summand1, timePoint), toTerm(castedExpr->summand2, timePoint));
            }
            case program::IntExpression::Type::Subtraction:
            {
                auto castedExpr = std::static_pointer_cast<const program::Subtraction>(expr);
                return logic::Theory::intSubtraction(toTerm(castedExpr->child1, timePoint), toTerm(castedExpr->child2, timePoint));
            }
            case program::IntExpression::Type::Multiplication:
            {
                auto castedExpr = std::static_pointer_cast<const program::Multiplication>(expr);
                return logic::Theory::intMultiplication(toTerm(castedExpr->factor1, timePoint), toTerm(castedExpr->factor2, timePoint));
            }
            case program::IntExpression::Type::UnaryMinus:
            {
                auto castedExpr = std::static_pointer_cast<const program::UnaryMinus>(expr);
                return logic::Theory::intUnaryMinus(toTerm(castedExpr->child, timePoint));
            }
            case program::IntExpression::Type::IntVariableAccess:
            {
                auto castedExpr = std::static_pointer_cast<const program::IntVariableAccess>(expr);
                return toTerm(castedExpr->var, timePoint);
            }
            case program::IntExpression::Type::IntArrayApplication:
            {
                auto castedExpr = std::static_pointer_cast<const program::IntArrayApplication>(expr);
                return toTerm(castedExpr->array, timePoint, toTerm(castedExpr->index, timePoint));
            }
        }
    }
    
    std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const program::BoolExpression> expr, std::shared_ptr<const logic::Term> timePoint)
    {
        assert(expr != nullptr);
        assert(timePoint != nullptr);
        
        switch (expr->type())
        {
            case program::BoolExpression::Type::BooleanConstant:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanConstant>(expr);
                return castedExpr->value ? logic::Theory::boolTrue() : logic::Theory::boolFalse();
            }
            case program::BoolExpression::Type::BooleanAnd:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanAnd>(expr);
                return logic::Formulas::conjunction({toFormula(castedExpr->child1, timePoint), toFormula(castedExpr->child2, timePoint)});
            }
            case program::BoolExpression::Type::BooleanOr:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanOr>(expr);
                return logic::Formulas::disjunction({toFormula(castedExpr->child1, timePoint), toFormula(castedExpr->child2, timePoint)});
            }
            case program::BoolExpression::Type::BooleanNot:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanNot>(expr);
                return logic::Formulas::negation(toFormula(castedExpr->child, timePoint));
            }
            case program::BoolExpression::Type::ArithmeticComparison:
            {
                auto castedExpr = std::static_pointer_cast<const program::ArithmeticComparison>(expr);
                switch (castedExpr->kind)
                {
                    case program::ArithmeticComparison::Kind::GT:
                        return logic::Theory::intGreater(toTerm(castedExpr->child1, timePoint), toTerm(castedExpr->child2, timePoint));
                    case program::ArithmeticComparison::Kind::GE:
                        return logic::Theory::intGreaterEqual(toTerm(castedExpr->child1, timePoint), toTerm(castedExpr->child2, timePoint));
                    case program::ArithmeticComparison::Kind::LT:
                        return logic::Theory::intLess(toTerm(castedExpr->child1, timePoint), toTerm(castedExpr->child2, timePoint));
                    case program::ArithmeticComparison::Kind::LE:
                        return logic::Theory::intLessEqual(toTerm(castedExpr->child1, timePoint), toTerm(castedExpr->child2, timePoint));
                    case program::ArithmeticComparison::Kind::EQ:
                        return logic::Formulas::equality(toTerm(castedExpr->child1, timePoint), toTerm(castedExpr->child2, timePoint));
                }
            }
        }
    }
    
    std::shared_ptr<const logic::LVariable> iteratorTermForLoop(const program::WhileStatement* whileStatement)
    {
        return logic::Terms::var(iteratorForLoop(whileStatement));
    }
    
    std::shared_ptr<const logic::Term> lastIterationTermForLoop(const program::WhileStatement* whileStatement, bool twoTraces)
    {
        auto symbol = lastIterationForLoop(whileStatement, twoTraces);
        std::vector<std::shared_ptr<const logic::Term>> subterms;
        if (twoTraces)
        {
            auto trSymbol = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
            auto tr = logic::Terms::var(trSymbol);
            subterms.push_back(tr);
        }
        return logic::Terms::func(symbol, subterms);
    }
    
    
}
