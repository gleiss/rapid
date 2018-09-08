#ifndef __Term__
#define __Term__

#include <cassert>
#include <list>
#include <ostream>
#include <string>
#include <vector>
#include "Signature.hpp"
#include "Sort.hpp"

namespace logic {
    
    class Term
    {
    public:
        Term(const Symbol* symbol) : symbol(symbol) {}
        
        const Symbol* symbol;
        
        virtual std::string toSMTLIB() const = 0;
        virtual std::string prettyString() const = 0;
    };
    
    class LVariable : public Term
    {
        friend class Terms;
        
        LVariable(const Symbol* symbol) : Term(symbol), id(freshId++){}

    public:
        const unsigned id;

        std::string toSMTLIB() const override;
        virtual std::string prettyString() const override;
        
        static unsigned freshId;
    };
    
    bool compareLVarPointers(const LVariable* p1, const LVariable* p2);
    bool eqLVarPointers(const LVariable* p1, const LVariable* p2);
    
    class FuncTerm : public Term
    {
        friend class Terms;
        FuncTerm(const Symbol* symbol, std::vector<std::shared_ptr<const Term>> subterms) : Term(symbol), subterms(std::move(subterms))
        {
            assert(this->symbol->argSorts.size() == this->subterms.size());
            for (int i=0; i < this->symbol->argSorts.size(); ++i)
            {
                assert(this->symbol->argSorts[i] == this->subterms[i]->symbol->rngSort);
            }
        }
        
    public:
        const std::vector<std::shared_ptr<const Term>> subterms;
        
        std::string toSMTLIB() const override;
        virtual std::string prettyString() const override;
    };
    
    inline std::ostream& operator<<(std::ostream& ostr, const Term& e) { ostr << e.toSMTLIB(); return ostr; }

    
# pragma mark - Terms
    
    // We use Terms as a manager-class for Term-instances
    class Terms
    {
    public:

        // construct new terms
        static std::shared_ptr<const LVariable> var(const std::string name, const Sort* s);
        static std::shared_ptr<const FuncTerm> func(std::string name, std::vector<std::shared_ptr<const Term>> subterms, const Sort* sort, bool noDeclaration=false);
    };
}
#endif

