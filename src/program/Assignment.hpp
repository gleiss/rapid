#ifndef __Assignment__
#define __Assignment__

#include "Expression.hpp"
#include "Variable.hpp"

namespace program
{
    class Statement
    {
    public:
        virtual std::string toString(int indentation) const = 0;
    };
    std::ostream& operator<<(std::ostream& ostr, const Statement& v);

    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::Statement>>& e);
    
    class IntAssignment : public Statement
    {
    public:
        
        IntAssignment(std::shared_ptr<const IntExpression> lhs, std::shared_ptr<const IntExpression> rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)){}
        
        const std::shared_ptr<const IntExpression> lhs;
        const std::shared_ptr<const IntExpression> rhs;
        
        std::string toString(int indentation) const override;
    };
    
    class IfElse : public Statement
    {
    public:
        
        IfElse(std::shared_ptr<const BoolExpression> condition, std::vector<std::shared_ptr<const Statement>> ifStatements, std::vector<std::shared_ptr<const Statement>> elseStatements) : condition(std::move(condition)), ifStatements(std::move(ifStatements)), elseStatements(std::move(elseStatements)){}
        
        const std::shared_ptr<const BoolExpression> condition;
        const std::vector<std::shared_ptr<const Statement>> ifStatements;
        const std::vector<std::shared_ptr<const Statement>> elseStatements;
        
        std::string toString(int indentation) const override;
    };
    
    class WhileStatement : public Statement
    {
    public:
        
        WhileStatement(std::shared_ptr<const BoolExpression> condition, std::vector<std::shared_ptr<const Statement>> bodyStatements) : condition(std::move(condition)), bodyStatements(std::move(bodyStatements)){}

        const std::shared_ptr<const BoolExpression> condition;
        const std::vector<std::shared_ptr<const Statement>> bodyStatements;
        
        std::string toString(int indentation) const override;
    };
    
    class SkipStatement : public Statement
    {
    public:
        SkipStatement(){};
        
        std::string toString(int indentation) const override;
    };
}



#endif
