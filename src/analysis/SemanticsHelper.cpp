#include "SemanticsHelper.hpp"

#include <cassert>

#include "Variable.hpp"
#include "Term.hpp"
#include "Theory.hpp"

#include "SymbolDeclarations.hpp"

namespace analysis {
    
# pragma mark - Methods for generating most used trace terms
    
    std::shared_ptr<const logic::Term> trace1Term()
    {
        return logic::Terms::func(trace1Symbol(), {});
    }
    std::shared_ptr<const logic::Term> trace2Term()
    {
        return logic::Terms::func(trace2Symbol(), {});
    }

# pragma mark - Methods for generating most used timepoint terms and symbols
    
    std::shared_ptr<const logic::LVariable> iteratorTermForLoop(const program::WhileStatement* whileStatement)
    {
        assert(whileStatement != nullptr);
        
        return logic::Terms::var(iteratorSymbol(whileStatement));
    }
    
    std::shared_ptr<const logic::Term> lastIterationTermForLoop(const program::WhileStatement* whileStatement, std::shared_ptr<const logic::Term> trace, bool twoTraces)
    {
        assert(whileStatement != nullptr);
        assert(trace != nullptr);

        auto symbol = lastIterationSymbol(whileStatement, twoTraces);
        std::vector<std::shared_ptr<const logic::Term>> subterms;
        for (const auto& loop : *whileStatement->enclosingLoops)
        {
            subterms.push_back(iteratorTermForLoop(loop));
        }
        if (twoTraces)
        {
            subterms.push_back(trace);
        }
        return logic::Terms::func(symbol, subterms);
    }
    
    std::shared_ptr<const logic::Term> timepointForNonLoopStatement(const program::Statement* statement)
    {
        assert(statement != nullptr);
        assert(statement->type() != program::Statement::Type::WhileStatement);
        
        auto enclosingLoops = *statement->enclosingLoops;
        auto enclosingIteratorTerms = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingLoop : enclosingLoops)
        {
            auto enclosingIteratorSymbol = iteratorSymbol(enclosingLoop);
            enclosingIteratorTerms.push_back(logic::Terms::var(enclosingIteratorSymbol));
        }

        return logic::Terms::func(locationSymbolForStatement(statement), enclosingIteratorTerms);
    }
    
    std::shared_ptr<const logic::Term> timepointForLoopStatement(const program::WhileStatement* whileStatement, std::shared_ptr<const logic::Term> innerIteration)
    {
        assert(whileStatement != nullptr);
        assert(innerIteration != nullptr);
        
        auto enclosingLoops = *whileStatement->enclosingLoops;
        auto enclosingIteratorTerms = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingLoop : enclosingLoops)
        {
            auto enclosingIteratorSymbol = iteratorSymbol(enclosingLoop);
            enclosingIteratorTerms.push_back(logic::Terms::var(enclosingIteratorSymbol));
        }
        enclosingIteratorTerms.push_back(innerIteration);
        return logic::Terms::func(locationSymbolForStatement(whileStatement), enclosingIteratorTerms);
    }

    std::shared_ptr<const logic::Term> startTimepointForStatement(const program::Statement* statement)
    {
        if (statement->type() != program::Statement::Type::WhileStatement)
        {
            return timepointForNonLoopStatement(statement);
        }
        else
        {
            auto whileStatement = static_cast<const program::WhileStatement*>(statement);
            return timepointForLoopStatement(whileStatement, logic::Theory::natZero());
        }
    }
    
    std::vector<std::shared_ptr<const logic::Symbol>> enclosingIteratorsSymbols(const program::Statement* statement)
    {
        auto enclosingIteratorsSymbols = std::vector<std::shared_ptr<const logic::Symbol>>();
        for (const auto& enclosingLoop : *statement->enclosingLoops)
        {
            enclosingIteratorsSymbols.push_back(iteratorSymbol(enclosingLoop));
        }
        return enclosingIteratorsSymbols;
    }

    
# pragma mark - Methods for generating most used terms/predicates denoting program-expressions
    std::shared_ptr<const logic::Term> toTermFull(std::shared_ptr<const program::Variable> var, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> trace)
    {
        assert(var != nullptr);
        assert(trace != nullptr);
        
        assert(!var->isArray);
        
        std::vector<std::shared_ptr<const logic::Term>> arguments;
        
        if (!var->isConstant)
        {
            assert(timePoint != nullptr);
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
        assert(position != nullptr);
        assert(trace != nullptr);
        
        assert(var->isArray);
        
        std::vector<std::shared_ptr<const logic::Term>> arguments;
        
        if (!var->isConstant)
        {
            assert(timePoint != nullptr);
            arguments.push_back(timePoint);
        }
        
        arguments.push_back(position);
        
        if (var->twoTraces)
        {
            arguments.push_back(trace);
        }
        
        return logic::Terms::func(var->name, arguments, logic::Sorts::intSort());
    }
    
    std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const program::IntExpression> expr, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> trace)
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
                return logic::Theory::intAddition(toTerm(castedExpr->summand1, timePoint, trace), toTerm(castedExpr->summand2, timePoint, trace));
            }
            case program::IntExpression::Type::Subtraction:
            {
                auto castedExpr = std::static_pointer_cast<const program::Subtraction>(expr);
                return logic::Theory::intSubtraction(toTerm(castedExpr->child1, timePoint, trace), toTerm(castedExpr->child2, timePoint, trace));
            }
            case program::IntExpression::Type::Modulo:
            {
                auto castedExpr = std::static_pointer_cast<const program::Modulo>(expr);
                return logic::Theory::intModulo(toTerm(castedExpr->child1, timePoint, trace), toTerm(castedExpr->child2, timePoint, trace));
            }
            case program::IntExpression::Type::Multiplication:
            {
                auto castedExpr = std::static_pointer_cast<const program::Multiplication>(expr);
                return logic::Theory::intMultiplication(toTerm(castedExpr->factor1, timePoint, trace), toTerm(castedExpr->factor2, timePoint, trace));
            }
            case program::IntExpression::Type::IntVariableAccess:
            {
                auto castedExpr = std::static_pointer_cast<const program::IntVariableAccess>(expr);
                return toTermFull(castedExpr->var, timePoint, trace);
            }
            case program::IntExpression::Type::IntArrayApplication:
            {
                auto castedExpr = std::static_pointer_cast<const program::IntArrayApplication>(expr);
                return toTermFull(castedExpr->array, timePoint, toTerm(castedExpr->index, timePoint, trace), trace);
            }
        }
    }

    std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const program::BoolExpression> expr, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> trace)
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
                return logic::Formulas::conjunction({toFormula(castedExpr->child1, timePoint, trace), toFormula(castedExpr->child2, timePoint, trace)});
            }
            case program::BoolExpression::Type::BooleanOr:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanOr>(expr);
                return logic::Formulas::disjunction({toFormula(castedExpr->child1, timePoint, trace), toFormula(castedExpr->child2, timePoint, trace)});
            }
            case program::BoolExpression::Type::BooleanNot:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanNot>(expr);
                return logic::Formulas::negation(toFormula(castedExpr->child, timePoint, trace));
            }
            case program::BoolExpression::Type::ArithmeticComparison:
            {
                auto castedExpr = std::static_pointer_cast<const program::ArithmeticComparison>(expr);
                switch (castedExpr->kind)
                {
                    case program::ArithmeticComparison::Kind::GT:
                        return logic::Theory::intGreater(toTerm(castedExpr->child1, timePoint, trace), toTerm(castedExpr->child2, timePoint, trace));
                    case program::ArithmeticComparison::Kind::GE:
                        return logic::Theory::intGreaterEqual(toTerm(castedExpr->child1, timePoint, trace), toTerm(castedExpr->child2, timePoint, trace));
                    case program::ArithmeticComparison::Kind::LT:
                        return logic::Theory::intLess(toTerm(castedExpr->child1, timePoint, trace), toTerm(castedExpr->child2, timePoint, trace));
                    case program::ArithmeticComparison::Kind::LE:
                        return logic::Theory::intLessEqual(toTerm(castedExpr->child1, timePoint, trace), toTerm(castedExpr->child2, timePoint, trace));
                    case program::ArithmeticComparison::Kind::EQ:
                        return logic::Formulas::equality(toTerm(castedExpr->child1, timePoint, trace), toTerm(castedExpr->child2, timePoint, trace));
                }
            }
        }
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
        auto trSymbol = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
        auto tr = logic::Terms::var(trSymbol);
        return toTerm(expr, timePoint, tr);
    }
    std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const program::BoolExpression> expr, std::shared_ptr<const logic::Term> timePoint)
    {
        auto trSymbol = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
        auto tr = logic::Terms::var(trSymbol);
        return toFormula(expr, timePoint, tr);
    }
    
    std::shared_ptr<const logic::Term> lastIterationTermForLoop(const program::WhileStatement* whileStatement, bool twoTraces)
    {
        auto trSymbol = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
        auto tr = logic::Terms::var(trSymbol);
        return lastIterationTermForLoop(whileStatement, tr, twoTraces);
    }

    std::shared_ptr<const logic::Formula> varEqual(std::shared_ptr<const program::Variable> v, std::shared_ptr<const logic::Term> timePoint1, std::shared_ptr<const logic::Term> timePoint2)
    {
        if(!v->isArray)
        {
            return
                logic::Formulas::equality(
                    toTerm(v,timePoint1),
                    toTerm(v,timePoint2)
                );
        }
        else
        {
            auto posSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
            auto pos = logic::Terms::var(posSymbol);
            return
                logic::Formulas::universal({posSymbol},
                    logic::Formulas::equality(
                        toTerm(v,timePoint1,pos),
                        toTerm(v,timePoint2,pos)
                    )
                );
        }
    }

    std::shared_ptr<const logic::Formula> allVarEqual(const std::vector<std::shared_ptr<const program::Variable>>& activeVars, std::shared_ptr<const logic::Term> timePoint1, std::shared_ptr<const logic::Term> timePoint2, std::string label)
    {
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;
        for (const auto& var : activeVars)
        {
            if(!var->isConstant)
            {
                conjuncts.push_back(varEqual(var, timePoint1, timePoint2));
            }
        }
        return logic::Formulas::conjunction(conjuncts, label);
    }
}
