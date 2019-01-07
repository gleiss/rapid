#ifndef __Assignment__
#define __Assignment__

#include "Expression.hpp"
#include "Variable.hpp"

#include <iostream>
#include <vector>
#include <memory>

namespace program
{
    class WhileStatement;
    class Statement
    {
    public:
        Statement(unsigned lineNumber) : location("l" + std::to_string(lineNumber)), enclosingLoops(std::make_unique<std::vector<const WhileStatement*>>()) {}
        
        const std::string location;
        /*
         * enclosingLoops needs to be modified in the parser-post-computation (but must not change afterwards)
         * we therefore need our programs to be constant up to the enclosingLoop-fields.
         * we achieve this by using indirection: constness is not transitive in c++,
         * so we get a (constant) pointer to a (not necessarily constant) vector enclosingLoops,
         * which we can fill up in the parser-post-computation.
         */
        std::unique_ptr<std::vector<const WhileStatement*>> enclosingLoops;
        
        enum class Type{ IntAssignment, IfElse, WhileStatement, SkipStatement };
        virtual Type type() const = 0;
        
        virtual std::string toString(int indentation) const = 0;
    };
    
    struct StatementSharedPtrHash
    {
        size_t operator()(const std::shared_ptr<const Statement>& ptr) const {return std::hash<std::string>()(ptr->location);}
    };
    
    std::ostream& operator<<(std::ostream& ostr, const Statement& v);

    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::Statement>>& e);
    
    class IntAssignment : public Statement
    {
    public:
        
        IntAssignment(unsigned lineNumber, std::shared_ptr<const IntExpression> lhs, std::shared_ptr<const IntExpression> rhs) : Statement(lineNumber), lhs(std::move(lhs)), rhs(std::move(rhs)){}
        
        const std::shared_ptr<const IntExpression> lhs;
        const std::shared_ptr<const IntExpression> rhs;
        
        Type type() const override {return Type::IntAssignment;}
        std::string toString(int indentation) const override;
    };
    
    class IfElse : public Statement
    {
    public:
        
        IfElse(unsigned lineNumber, std::shared_ptr<const BoolExpression> condition, std::vector<std::shared_ptr<const Statement>> ifStatements, std::vector<std::shared_ptr<const Statement>> elseStatements) : Statement(lineNumber), condition(std::move(condition)), ifStatements(std::move(ifStatements)), elseStatements(std::move(elseStatements))
        {
            // TODO: add a skip-statement instead, maybe already during parsing (challenge: unique numbering)
            assert(this->ifStatements.size() > 0);
            assert(this->elseStatements.size() > 0);
        }
        
        const std::shared_ptr<const BoolExpression> condition;
        const std::vector<std::shared_ptr<const Statement>> ifStatements;
        const std::vector<std::shared_ptr<const Statement>> elseStatements;
        
        Type type() const override {return Type::IfElse;}
        std::string toString(int indentation) const override;
    };
    
    class WhileStatement : public Statement
    {
    public:
        
        WhileStatement(unsigned lineNumber, std::shared_ptr<const BoolExpression> condition, std::vector<std::shared_ptr<const Statement>> bodyStatements) : Statement(lineNumber), condition(std::move(condition)), bodyStatements(std::move(bodyStatements))
        {
            // TODO: add a skip-statement instead, maybe already during parsing (challenge: unique numbering)
            assert(this->bodyStatements.size() > 0);
        }

        const std::shared_ptr<const BoolExpression> condition;
        const std::vector<std::shared_ptr<const Statement>> bodyStatements;
        
        Type type() const override {return Type::WhileStatement;}
        std::string toString(int indentation) const override;
    };
    
    class SkipStatement : public Statement
    {
    public:
        SkipStatement(unsigned lineNumber) : Statement(lineNumber) {};
        
        Type type() const override {return Type::SkipStatement;}
        std::string toString(int indentation) const override;
    };
}



#endif
