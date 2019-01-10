#include "Formula.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace logic {
    
    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector<std::shared_ptr<const logic::Formula>>& f){ostr << "not implemented"; return ostr;}
    
    std::string Formula::stringForLabel(unsigned indentation) const
    {
        std::string str = "";
        if (!label.empty())
        {
            str += std::string(indentation, ' ') + ";" + label + "\n";
        }
        return str;
    }

    std::string PredicateFormula::toSMTLIB(unsigned indentation) const
    {
        std::string str = stringForLabel(indentation);
        str += std::string(indentation, ' ');
        if (subterms.size() == 0)
        {
            str += symbol->toSMTLIB();
        }
        else
        {
            str += "(" + symbol->toSMTLIB() + " ";
            for (unsigned i = 0; i < subterms.size(); i++)
            {
                str += subterms[i]->toSMTLIB();
                str += (i == subterms.size() - 1) ? ")" : " ";
            }
        }
        return str;
    }

    std::string EqualityFormula::toSMTLIB(unsigned indentation) const
    {
        std::string str = stringForLabel(indentation);
        str += std::string(indentation, ' ');
        if (polarity)
        {
            str += "(= " + left->toSMTLIB() + " " + right->toSMTLIB() + ")";
        }
        else
        {
            str += "(not (= " + left->toSMTLIB() + " " + right->toSMTLIB() + "))";
        }
        return str;
    }

    std::string ConjunctionFormula::toSMTLIB(unsigned indentation) const
    {
        std::string str = stringForLabel(indentation);
        if (conj.size() == 0)
        {
            return str + std::string(indentation, ' ') + "true";
        }
        str += std::string(indentation, ' ') + "(and\n";
        for (unsigned i = 0; i < conj.size(); i++) {
            str += conj[i]->toSMTLIB(indentation + 3) + "\n";
        }
        str += std::string(indentation, ' ') + ")";
        return str;
    }
    
    std::string DisjunctionFormula::toSMTLIB(unsigned indentation) const
    {
        std::string str = stringForLabel(indentation);
        if (disj.size() == 0)
        {
            return str + std::string(indentation, ' ') + "false";
        }
        str = std::string(indentation, ' ') + "(or\n";
        for (unsigned i = 0; i < disj.size(); i++) {
            str += disj[i]->toSMTLIB(indentation + 3) + "\n";
        }
        str += std::string(indentation, ' ') + ")";
        return str;
    }
    
    std::string NegationFormula::toSMTLIB(unsigned indentation) const
    {
        std::string str = stringForLabel(indentation);
        str += std::string(indentation, ' ') + "(not\n";
        str += f->toSMTLIB(indentation + 3) + "\n";
        str += std::string(indentation, ' ') + ")";
        return  str;
    }
    
    std::string ExistentialFormula::toSMTLIB(unsigned indentation) const
    {
        std::string str = stringForLabel(indentation);
        str += std::string(indentation, ' ') + "(exists ";
        
        //list of quantified variables
        str += "(";
        for (const auto& var : vars)
        {
            str += "(" + var->name + " " + var->rngSort->toSMTLIB() + ")";
        }
        str += ")\n";
        
        // formula
        str += f->toSMTLIB(indentation + 3) + "\n";
        
        str += std::string(indentation, ' ') + ")";
        return str;
    }

    std::string UniversalFormula::toSMTLIB(unsigned indentation) const
    {
        std::string str = stringForLabel(indentation);
        str += std::string(indentation, ' ') + "(forall ";
        
        //list of quantified variables
        str += "(";
        for (const auto& var : vars)
        {
            str += "(" + var->name + " " + var->rngSort->toSMTLIB() + ")";
        }
        str += ")\n";
        
        // formula
        str += f->toSMTLIB(indentation + 3) + "\n";
        
        str += std::string(indentation, ' ') + ")";
        return str;
    }
    
    std::string ImplicationFormula::toSMTLIB(unsigned indentation) const
    {
        std::string str = stringForLabel(indentation);

        str += std::string(indentation, ' ') + "(=>\n";
        str += f1->toSMTLIB(indentation + 3) + "\n";
        str += f2->toSMTLIB(indentation + 3) + "\n";
        str += std::string(indentation, ' ') + ")";
        return  str;
    }

    std::string PredicateFormula::prettyString(unsigned indentation) const
    {
        auto str = std::string(indentation, ' ');
        if (subterms.size() == 0)
        {
            str += symbol->toSMTLIB();
        }
        else
        {
            str += symbol->toSMTLIB() + "(";
            for (unsigned i = 0; i < subterms.size(); i++) {
                str += subterms[i]->toSMTLIB();
                str += (i == subterms.size() - 1) ? ")" : ",";
            }
        }
        return str;
    }
    
    std::string EqualityFormula::prettyString(unsigned indentation) const
    {
        if (polarity)
            return std::string(indentation, ' ') + left->prettyString() + " = " + right->prettyString();
        else
            return std::string(indentation, ' ') + left->prettyString() + " != " + right->prettyString();
    }
    
    std::string ConjunctionFormula::prettyString(unsigned indentation) const
    {
        if (conj.size() == 0)
            return std::string(indentation, ' ') + "True";
        
        std::string str = std::string(indentation, ' ') + "AND\n";
        
        for (unsigned i = 0; i < conj.size(); i++)
        {
            str += conj[i]->prettyString(indentation + 3);
            str += (i+1 < conj.size()) ? "\n" : "";
        }
        
        return str;
    }
    
    std::string DisjunctionFormula::prettyString(unsigned indentation) const
    {
        if (disj.size() == 0)
            return std::string(indentation, ' ') + "False";
        
        std::string str = std::string(indentation, ' ') + "OR\n";
        
        for (unsigned i = 0; i < disj.size(); i++)
        {
            str += disj[i]->prettyString(indentation + 3);
            str += (i+1 < disj.size()) ? "\n" : "";
        }
        
        return str;
    }
    
    std::string NegationFormula::prettyString(unsigned indentation) const
    {
        std::string str = std::string(indentation, ' ') + "NOT\n";
        str += f->prettyString(indentation + 3);
        return  str;
    }
    
    std::string ExistentialFormula::prettyString(unsigned indentation) const
    {
        std::string str = std::string(indentation, ' ') + "EXISTS ";
        for (unsigned i = 0; i < vars.size(); i++) {
            str += vars[i]->name + " : " + vars[i]->rngSort->name;
            if (i != vars.size() - 1) { str += ", "; }
        }
        str += ".\n" + f->prettyString(indentation + 3);
        return str;
    }
    
    std::string UniversalFormula::prettyString(unsigned indentation) const
    {
        std::string str = std::string(indentation, ' ') + "FORALL ";
        for (unsigned i = 0; i < vars.size(); i++) {
            str += vars[i]->name + " : " + vars[i]->rngSort->name;
            if (i != vars.size() - 1) { str += ", "; }
        }
        str += ".\n";
        str += f->prettyString(indentation + 3);
        
        return str;
    }
    std::string ImplicationFormula::prettyString(unsigned indentation) const
    {
        std::string str = std::string(indentation, ' ') + "=>\n";
        str += f1->prettyString(indentation + 3) + "\n";
        str += f2->prettyString(indentation + 3);
        return  str;
    }
    
# pragma mark - Formulas
    std::shared_ptr<const PredicateFormula> Formulas::predicate(std::string name, std::vector<std::shared_ptr<const Term>> subterms, std::string label, bool noDeclaration)
    {
        std::vector<const Sort*> subtermSorts;
        for (const auto& subterm : subterms)
        {
            subtermSorts.push_back(subterm->symbol->rngSort);
        }
        auto symbol = Signature::fetchOrAdd(name, subtermSorts, Sorts::boolSort(), noDeclaration);
        return std::shared_ptr<const PredicateFormula>(new PredicateFormula(symbol, subterms, label));
    }

    std::shared_ptr<const EqualityFormula> Formulas::equality(std::shared_ptr<const Term> left, std::shared_ptr<const Term> right, std::string label)
    {
        return std::shared_ptr<const EqualityFormula>(new EqualityFormula(true, left, right, label));
    }
    
    std::shared_ptr<const NegationFormula> Formulas::disequality(std::shared_ptr<const Term> left, std::shared_ptr<const Term> right, std::string label)
    {
        auto eq = std::shared_ptr<const EqualityFormula>(new EqualityFormula(true, left, right));
        return std::shared_ptr<const NegationFormula>(new NegationFormula(eq, label));
    }
    
    std::shared_ptr<const NegationFormula>  Formulas::negation(std::shared_ptr<const Formula> f, std::string label)
    {
        return std::shared_ptr<const NegationFormula>(new NegationFormula(f, label));
    }
    
    std::shared_ptr<const ConjunctionFormula> Formulas::conjunction(std::vector<std::shared_ptr<const Formula>> conj, std::string label)
    {
        return std::shared_ptr<const ConjunctionFormula>(new ConjunctionFormula(conj, label));
    }
    std::shared_ptr<const DisjunctionFormula> Formulas::disjunction(std::vector<std::shared_ptr<const Formula>> disj, std::string label)
    {
        return std::shared_ptr<const DisjunctionFormula>(new DisjunctionFormula(disj, label));
    }
    
    std::shared_ptr<const ImplicationFormula> Formulas::implication(std::shared_ptr<const Formula> f1, std::shared_ptr<const Formula> f2, std::string label)
    {
        return std::shared_ptr<const ImplicationFormula>(new ImplicationFormula(f1, f2, label));
    }
    
    std::shared_ptr<const ExistentialFormula> Formulas::existential(std::vector<std::shared_ptr<const Symbol>> vars, std::shared_ptr<const Formula> f, std::string label)
    {
        return std::shared_ptr<const ExistentialFormula>(new ExistentialFormula(std::move(vars), f, label));
    }
    std::shared_ptr<const UniversalFormula> Formulas::universal(std::vector<std::shared_ptr<const Symbol>> vars, std::shared_ptr<const Formula> f, std::string label)
    {
        return std::shared_ptr<const UniversalFormula>(new UniversalFormula(std::move(vars), f, label));
    }
}

