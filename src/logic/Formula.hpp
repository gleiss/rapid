#ifndef __Formula__
#define __Formula__

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cassert>

#include "Term.hpp"

namespace logic {
    
    class Formula
    {
    public:
        Formula(std::string label) : label(label) {}
        
        const std::string label;
        
        std::string declareSMTLIB(std::string decl, bool conjecture = false) const;
        
        virtual std::string toSMTLIB(unsigned indentation = 0) const = 0;
        virtual std::string prettyString(unsigned indentation = 0) const = 0;
        
    protected:
        std::string stringForLabel(unsigned indentation) const;
    };
    
    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector<std::shared_ptr<const logic::Formula>>& f);
    
    class PredicateFormula : public Formula
    {
        friend class Formulas;
        
    public:
        PredicateFormula(std::shared_ptr<const Symbol> symbol, std::vector<std::shared_ptr<const Term>> subterms, std::string label = "") : Formula(label), symbol(symbol), subterms(subterms)
        {
            assert(symbol->argSorts.size() == subterms.size());
            for (int i=0; i < symbol->argSorts.size(); ++i)
            {
                assert(symbol->argSorts[i] == this->subterms[i]->symbol->rngSort);
            }
        }

        std::shared_ptr<const Symbol> symbol;
        const std::vector<std::shared_ptr<const Term>> subterms;

        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class EqualityFormula : public Formula
    {
        friend class Formulas;
        
    public:
        // TODO: refactor polarity into explicit negation everywhere
        EqualityFormula(bool polarity, std::shared_ptr<const Term> left, std::shared_ptr<const Term> right, std::string label = "")
        : Formula(label), polarity(polarity), left(left), right(right) {}

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
        ConjunctionFormula(std::vector<std::shared_ptr<const Formula>> conj, std::string label = "") : Formula(label), conj(conj) {}
        
        const std::vector<std::shared_ptr<const Formula>> conj;

        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class DisjunctionFormula : public Formula
    {
        friend class Formulas;
        
    public:
        DisjunctionFormula(std::vector<std::shared_ptr<const Formula>> disj, std::string label = "") : Formula(label), disj(disj){}
        
        const std::vector<std::shared_ptr<const Formula>> disj;

        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class NegationFormula : public Formula
    {
        friend class Formulas;
        
    public:
        NegationFormula(std::shared_ptr<const Formula> f, std::string label = "") : Formula(label), f(f) {}
        
        const std::shared_ptr<const Formula> f;

        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
        
    };
    
    class ExistentialFormula : public Formula
    {
        friend class Formulas;
        
    public:
        ExistentialFormula(std::vector<std::shared_ptr<const Symbol>> vars, std::shared_ptr<const Formula> f, std::string label = "")
        : Formula(label), vars(std::move(vars)), f(f)
        {
            for (const auto& var : vars)
            {
                assert(var->argSorts.empty());
            }
        }
        
        const std::vector<std::shared_ptr<const Symbol>> vars;
        const std::shared_ptr<const Formula> f;
        
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class UniversalFormula : public Formula
    {
        friend class Formulas;
        
    public:
        UniversalFormula(std::vector<std::shared_ptr<const Symbol>> vars, std::shared_ptr<const Formula> f, std::string label = "")
        : Formula(label), vars(std::move(vars)), f(f)
        {
            for (const auto& var : vars)
            {
                assert(var->argSorts.empty());
            }
        }

        const std::vector<std::shared_ptr<const Symbol>> vars;
        const std::shared_ptr<const Formula> f;
        
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class ImplicationFormula : public Formula
    {
        friend class Formulas;
        
    public:
        ImplicationFormula(std::shared_ptr<const Formula> f1, std::shared_ptr<const Formula> f2, std::string label = "")
        : Formula(label), f1(f1), f2(f2) {}
        
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
        static std::shared_ptr<const PredicateFormula> predicate(std::string name, std::vector<std::shared_ptr<const Term>> subterms, std::string label = "", bool noDeclaration=false);
        
        static std::shared_ptr<const EqualityFormula> equality(std::shared_ptr<const Term> left, std::shared_ptr<const Term> right, std::string label = "");
        static std::shared_ptr<const NegationFormula> disequality(std::shared_ptr<const Term> left, std::shared_ptr<const Term> right, std::string label = "");

        static std::shared_ptr<const NegationFormula>  negation(std::shared_ptr<const Formula> f, std::string label = "");

        static std::shared_ptr<const ConjunctionFormula> conjunction(std::vector<std::shared_ptr<const Formula>> conj, std::string label = "");
        static std::shared_ptr<const DisjunctionFormula> disjunction(std::vector<std::shared_ptr<const Formula>> disj, std::string label = "");
        
        static std::shared_ptr<const ImplicationFormula> implication(std::shared_ptr<const Formula> f1, std::shared_ptr<const Formula> f2, std::string label = "");
        
        static std::shared_ptr<const Formula> existential(std::vector<std::shared_ptr<const Symbol>> vars, std::shared_ptr<const Formula> f, std::string label = "");
        static std::shared_ptr<const Formula> universal(std::vector<std::shared_ptr<const Symbol>> vars, std::shared_ptr<const Formula> f, std::string label = "");
    };
}

#endif

