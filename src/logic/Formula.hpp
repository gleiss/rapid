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
        virtual ~Formula() {}
        const std::string label;

        enum class Type
        {
            Predicate,
            Equality,
            Conjunction,
            Disjunction,
            Negation,
            Existential,
            Universal,
            Implication,
            Equivalence,
            True,
            False
        };
        virtual Type type() const = 0;
        
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

        Type type() const override { return Formula::Type::Predicate; }
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
        
        Type type() const override { return Formula::Type::Equality; }
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class ConjunctionFormula : public Formula
    {
        friend class Formulas;
        
    public:
        ConjunctionFormula(std::vector<std::shared_ptr<const Formula>> conj, std::string label = "") : Formula(label), conj(conj) {}
        
        const std::vector<std::shared_ptr<const Formula>> conj;

        Type type() const override { return Formula::Type::Conjunction; }
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class DisjunctionFormula : public Formula
    {
        friend class Formulas;
        
    public:
        DisjunctionFormula(std::vector<std::shared_ptr<const Formula>> disj, std::string label = "") : Formula(label), disj(disj){}
        
        const std::vector<std::shared_ptr<const Formula>> disj;

        Type type() const override { return Formula::Type::Disjunction; }
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class NegationFormula : public Formula
    {
        friend class Formulas;
        
    public:
        NegationFormula(std::shared_ptr<const Formula> f, std::string label = "") : Formula(label), f(f) {}
        
        const std::shared_ptr<const Formula> f;

        Type type() const override { return Formula::Type::Negation; }
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
        
        Type type() const override { return Formula::Type::Existential; }
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
        
        Type type() const override { return Formula::Type::Universal; }
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
        
        Type type() const override { return Formula::Type::Implication; }
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };

    class EquivalenceFormula : public Formula
    {
        friend class Formulas;

    public:
        EquivalenceFormula(std::shared_ptr<const Formula> f1, std::shared_ptr<const Formula> f2, std::string label = "")
        : Formula(label), f1(f1), f2(f2) {}

        const std::shared_ptr<const Formula> f1;
        const std::shared_ptr<const Formula> f2;

        Type type() const override { return Formula::Type::Equivalence; }
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };

    class TrueFormula : public Formula
    {
        friend class Formulas;
        
    public:
        TrueFormula(std::string label = "") : Formula(label) {}
        
        Type type() const override { return Formula::Type::True; }
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    
    class FalseFormula : public Formula
    {
        friend class Formulas;
        
    public:
        FalseFormula(std::string label = "") : Formula(label) {}
        
        Type type() const override { return Formula::Type::False; }
        std::string toSMTLIB(unsigned indentation = 0) const override;
        std::string prettyString(unsigned indentation = 0) const override;
    };
    inline std::ostream& operator<<(std::ostream& ostr, const Formula& e) { ostr << e.toSMTLIB(); return ostr; }
    
# pragma mark - Formulas
    
    // We use Formulas as a manager-class for Formula-instances
    class Formulas
    {
    public:
        static std::shared_ptr<const Formula> predicate(std::string name, std::vector<std::shared_ptr<const Term>> subterms, std::string label = "", bool noDeclaration=false);
        
        static std::shared_ptr<const Formula> equality(std::shared_ptr<const Term> left, std::shared_ptr<const Term> right, std::string label = "");
        static std::shared_ptr<const Formula> disequality(std::shared_ptr<const Term> left, std::shared_ptr<const Term> right, std::string label = "");

        static std::shared_ptr<const Formula> negation(std::shared_ptr<const Formula> f, std::string label = "");

        static std::shared_ptr<const Formula> conjunction(std::vector<std::shared_ptr<const Formula>> conj, std::string label = "");
        static std::shared_ptr<const Formula> disjunction(std::vector<std::shared_ptr<const Formula>> disj, std::string label = "");
        
        static std::shared_ptr<const Formula> implication(std::shared_ptr<const Formula> f1, std::shared_ptr<const Formula> f2, std::string label = "");
        static std::shared_ptr<const Formula> equivalence(std::shared_ptr<const Formula> f1, std::shared_ptr<const Formula> f2, std::string label = "");
        
        static std::shared_ptr<const Formula> existential(std::vector<std::shared_ptr<const Symbol>> vars, std::shared_ptr<const Formula> f, std::string label = "");
        static std::shared_ptr<const Formula> universal(std::vector<std::shared_ptr<const Symbol>> vars, std::shared_ptr<const Formula> f, std::string label = "");

        static std::shared_ptr<const Formula> trueFormula(std::string label = "");
        static std::shared_ptr<const Formula> falseFormula(std::string label = "");
    };
}

#endif

