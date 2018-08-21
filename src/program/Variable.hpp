/**
 *
 * @file Variable.hpp
 *
 * Program variables (and variables coming from assertion quantifiers)
 */

#ifndef __ProgramVariable__
#define __ProgramVariable__

#include <string>
#include <ostream>

#include "Expression.hpp"

#include "Term.hpp"
#include "Formula.hpp"
#include "Signature.hpp"

namespace program {
    
    class IntVariable : public IntExpression
    {
    public:
        IntVariable(std::string& name) : name(name) {}

        const std::string name;
        
        bool operator==(const IntVariable& rhs) const { return (name == rhs.name); }
        bool operator!=(const IntVariable& rhs) const { return !operator==(rhs); }
        
        std::string toString() const override;
        std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const logic::Term> index) const override;
    };
    
    
    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector< std::shared_ptr<const program::IntVariable>>& e);

    class BoolVariable : public BoolExpression
    {
    public:
        BoolVariable(std::string& name) : name(name) {}
        
        const std::string name;
        
        bool operator==(const BoolVariable& rhs) const { return (name == rhs.name); }
        bool operator!=(const BoolVariable& rhs) const { return !operator==(rhs); }
        
        std::string toString() const override;
        std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const logic::Term> index) const override;
    };
    
    class IntArrayVariable
    {
    public:
        IntArrayVariable(std::string& name) : name(name) {}
        
        const std::string name;
        
        bool operator==(const IntVariable& rhs) const { return (name == rhs.name); }
        bool operator!=(const IntVariable& rhs) const { return !operator==(rhs); }
        
        std::string toString() const;
    };
    
    class IntArrayApplication : public IntExpression
    {
    public:
        IntArrayApplication(std::shared_ptr<const IntArrayVariable> array, std::shared_ptr<const IntExpression> index) : array(std::move(array)), index(std::move(index))
        {
            assert(array != nullptr);
            assert(index != nullptr);
        }
        
        const std::shared_ptr<const IntArrayVariable> array;
        const std::shared_ptr<const IntExpression> index;
        
        std::string toString() const override;
        std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const logic::Term> i) const override;
    };
}

#endif // __ProgramVariable__
