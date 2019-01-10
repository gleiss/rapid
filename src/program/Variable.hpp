/**
 *
 * @file Variable.hpp
 *
 * Program variables (and variables coming from assertion quantifiers)
 */

#ifndef __ProgramVariable__
#define __ProgramVariable__

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Expression.hpp"

namespace program {
    
    class Variable
    {
    public:
        Variable(std::string name, bool isConstant, bool isArray, bool twoTraces) : name(name), isConstant(isConstant), isArray(isArray), twoTraces(twoTraces) {}

        const std::string name;
        const bool isConstant;
        const bool isArray;
        const bool twoTraces;
                
        bool operator==(const Variable& rhs) const { return (name == rhs.name); }
        bool operator!=(const Variable& rhs) const { return !operator==(rhs); }
    };

    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::Variable>>& e);
    
    class IntVariableAccess : public IntExpression
    {
    public:
        IntVariableAccess(std::shared_ptr<const Variable> var) : IntExpression(), var(var) {}
        
        const std::shared_ptr<const Variable> var;

        IntExpression::Type type() const override {return IntExpression::Type::IntVariableAccess;}
        
        std::string toString() const override;
    };
    
    class IntArrayApplication : public IntExpression
    {
    public:
        IntArrayApplication(std::shared_ptr<const Variable> array, std::shared_ptr<const IntExpression> index) : array(std::move(array)), index(std::move(index))
        {
            assert(this->array != nullptr);
            assert(this->index != nullptr);
        }
        
        const std::shared_ptr<const Variable> array;
        const std::shared_ptr<const IntExpression> index;
        
        IntExpression::Type type() const override {return IntExpression::Type::IntArrayApplication;}

        std::string toString() const override;
    };
}

#endif // __ProgramVariable__
