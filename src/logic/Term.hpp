#ifndef __Term__
#define __Term__

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cassert>

#include "Signature.hpp"
#include "Sort.hpp"

namespace logic {
    
    class Term
    {
    public:
        Term(std::shared_ptr<const Symbol> symbol) : symbol(symbol) {}
        virtual ~Term() {}

        std::shared_ptr<const Symbol> symbol;
        
        enum class Type
        {
            Variable,
            FuncTerm,
        };
        virtual Type type() const = 0;

        virtual std::string toSMTLIB() const = 0;
        virtual std::string prettyString() const = 0;
    };

    bool operator==(const Term& t1, const Term& t2);
    bool operator!=(const Term& t1, const Term& t2);
        
    class LVariable : public Term
    {
        friend class Terms;
        
        LVariable(std::shared_ptr<const Symbol> symbol) : Term(symbol), id(freshId++){}

    public:
        const unsigned id;

        Type type() const override { return Term::Type::Variable; }
        std::string toSMTLIB() const override;
        virtual std::string prettyString() const override;
        
        static unsigned freshId;
    };
    
    class FuncTerm : public Term
    {
        friend class Terms;
        FuncTerm(std::shared_ptr<const Symbol> symbol, std::vector<std::shared_ptr<const Term>> subterms) : Term(symbol), subterms(std::move(subterms))
        {
            assert(this->symbol->argSorts.size() == this->subterms.size());
            for (int i=0; i < this->symbol->argSorts.size(); ++i)
            {
                assert(this->symbol->argSorts[i] == this->subterms[i]->symbol->rngSort);
            }
        }
        
    public:
        const std::vector<std::shared_ptr<const Term>> subterms;
        
        Type type() const override { return Term::Type::FuncTerm; }
        std::string toSMTLIB() const override;
        virtual std::string prettyString() const override;
    };
    
    inline std::ostream& operator<<(std::ostream& ostr, const Term& e) { ostr << e.toSMTLIB(); return ostr; }

    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector<std::shared_ptr<const logic::Term>>& t);
    std::ostream& operator<<(std::ostream& ostr, const std::vector<std::shared_ptr<const logic::LVariable>>& v);
}

// custom hash for terms
namespace std
{
    template<> struct hash<const logic::Term>
    {
        using argument_type = const logic::Term;
        using result_type = std::size_t;

        result_type operator ()(argument_type const& t) const
        {
            // start from symbol of term
            size_t result = std::hash<const logic::Symbol>()(*t.symbol);
            // then integrate type into the hash
            result ^= std::hash<logic::Term::Type>()(t.type()) + 0x9e3779b9 + (result << 6) + (result >> 2);
            if (t.type() == logic::Term::Type::Variable)
            {
                // for an LVariable, finally integrate id into the hash
                auto castedTerm = dynamic_cast<const logic::LVariable&>(t);
                result ^= std::hash<unsigned>()(castedTerm.id) + 0x9e3779b9 + (result << 6) + (result >> 2);
            }
            else
            {
                // for a FuncTerm, integrate each subterm into the hash
                auto castedTerm = dynamic_cast<const logic::FuncTerm&>(t);
                for (const auto& subterm : castedTerm.subterms)
                {
                    result ^= std::hash<const logic::Term>()(*subterm) + 0x9e3779b9 + (result << 6) + (result >> 2);
                }
            }
            return result;
        }
    };
}

# pragma mark - Terms
namespace logic {
    // We use Terms as a manager-class for Term-instances
    class Terms
    {
    public:

        // construct new terms
        static std::shared_ptr<const LVariable> var(std::shared_ptr<const Symbol> symbol);
        static std::shared_ptr<const FuncTerm> func(std::string name, std::vector<std::shared_ptr<const Term>> subterms, const Sort* sort, bool noDeclaration=false);
        static std::shared_ptr<const FuncTerm> func(std::shared_ptr<const Symbol> symbol, std::vector<std::shared_ptr<const Term>> subterms);
    };
}
#endif

