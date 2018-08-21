#ifndef __Formula__
#define __Formula__

#include <initializer_list>
#include <ostream>
#include "Term.hpp"

namespace logic {
    
    class Formula {
    public:
        std::string declareSMTLIB(std::string decl, bool conjecture = false) const;
        
        virtual std::string toSMTLIB(unsigned indentation = 0) const = 0;
        virtual std::string prettyString(unsigned indentation = 0) const = 0;
    };
    
    class PredicateFormula : public Formula
    {
        friend class Formulas;
        
    public:
        PredicateFormula(const Symbol* symbol, std::initializer_list<std::shared_ptr<const Term>> subterms) : symbol(symbol), subterms(subterms)
        {
            assert(symbol->argSorts.size() == subterms.size());
            for (int i=0; i < symbol->argSorts.size(); ++i)
            {
                assert(symbol->argSorts[i] == this->subterms[i]->symbol->rngSort);
            }
        }

        const Symbol* symbol;
        const std::vector<std::shared_ptr<const Term>> subterms;

        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class EqualityFormula : public Formula
    {
        friend class Formulas;
        
    public:
        EqualityFormula(bool polarity, std::shared_ptr<const Term> left, std::shared_ptr<const Term> right) :
        polarity(polarity),
        left(left),
        right(right)
        {}

        const bool polarity;
        const std::shared_ptr<const Term> left;
        const std::shared_ptr<const Term> right;
        
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class ConjunctionFormula : public Formula
    {
        friend class Formulas;
        
    public:
        ConjunctionFormula(std::vector<std::shared_ptr<const Formula>> conj) : conj(conj){}
        ConjunctionFormula(std::initializer_list<std::shared_ptr<const Formula>> conj) : conj(conj){}
        
        const std::vector<std::shared_ptr<const Formula>> conj;

        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class DisjunctionFormula : public Formula
    {
        friend class Formulas;
        
    public:
        DisjunctionFormula(std::vector<std::shared_ptr<const Formula>> disj) : disj(disj){}
        DisjunctionFormula(std::initializer_list<std::shared_ptr<const Formula>> disj) : disj(disj){}
        
        const std::vector<std::shared_ptr<const Formula>> disj;

        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class NegationFormula : public Formula
    {
        friend class Formulas;
        
    public:
        NegationFormula(std::shared_ptr<const Formula> f) : f(f) {}
        
        const std::shared_ptr<const Formula> f;

        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
        
    };
    
    class ExistentialFormula : public Formula
    {
        friend class Formulas;
        
    public:
        ExistentialFormula(std::vector<std::shared_ptr<const LVariable>> vars, std::shared_ptr<const Formula> f) : vars(vars), f(f){}
        ExistentialFormula(std::initializer_list<std::shared_ptr<const LVariable>> vars, std::shared_ptr<const Formula> f) : vars(vars), f(f){}
        
        const std::vector<std::shared_ptr<const LVariable>> vars;
        const std::shared_ptr<const Formula> f;
        
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class UniversalFormula : public Formula
    {
        friend class Formulas;
        
    public:
        UniversalFormula(std::vector<std::shared_ptr<const LVariable>> vars, std::shared_ptr<const Formula> f) : vars(vars), f(f){}
        UniversalFormula(std::initializer_list<std::shared_ptr<const LVariable>> vars, std::shared_ptr<const Formula> f) : vars(vars), f(f){}

        const std::vector<std::shared_ptr<const LVariable>> vars;
        const std::shared_ptr<const Formula> f;
        
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class ImplicationFormula : public Formula
    {
        friend class Formulas;
        
    public:
        ImplicationFormula(std::shared_ptr<const Formula> f1, std::shared_ptr<const Formula> f2) : f1(f1), f2(f2) {}
        
        const std::shared_ptr<const Formula> f1;
        const std::shared_ptr<const Formula> f2;
        
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    inline std::ostream& operator<<(std::ostream& ostr, const Formula& e) { ostr << e.toSMTLIB(); return ostr; }
    
# pragma mark - Formulas
    
    // We use Formulas as a manager-class for Formula-instances
    class Formulas
    {
    public:
        
        // construct new terms
        static std::shared_ptr<const PredicateFormula> predicate(std::string name, std::initializer_list<std::shared_ptr<const Term>> subterms, bool noDeclaration=false);
        
        static std::shared_ptr<const EqualityFormula> equality(std::shared_ptr<const Term> left, std::shared_ptr<const Term> right);
        static std::shared_ptr<const NegationFormula> disequality(std::shared_ptr<const Term> left, std::shared_ptr<const Term> right);

        static std::shared_ptr<const NegationFormula>  negation(std::shared_ptr<const Formula> f);

        static std::shared_ptr<const ConjunctionFormula> conjunction(std::vector<std::shared_ptr<const Formula>> conj);
        static std::shared_ptr<const ConjunctionFormula> conjunction(std::initializer_list<std::shared_ptr<const Formula>> conj);
        static std::shared_ptr<const DisjunctionFormula> disjunction(std::vector<std::shared_ptr<const Formula>> disj);
        static std::shared_ptr<const DisjunctionFormula> disjunction(std::initializer_list<std::shared_ptr<const Formula>> disj);
        
        static std::shared_ptr<const ImplicationFormula> implication(std::shared_ptr<const Formula> f1, std::shared_ptr<const Formula> f2);
        
        static std::shared_ptr<const ExistentialFormula> existential(std::vector<std::shared_ptr<const LVariable>> vars, std::shared_ptr<const Formula> f);
        static std::shared_ptr<const UniversalFormula> universal(std::vector<std::shared_ptr<const LVariable>> vars, std::shared_ptr<const Formula> f);
    };
}

#endif

