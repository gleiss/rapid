#include "PropertiesTime.hpp"

#include <cassert>
#include <iostream>

#include "Signature.hpp"
#include "Theory.hpp"
#include "Options.hpp"
#include "Output.hpp"

using namespace logic;

namespace analysis {
    
#pragma mark - High level methods
    
    std::vector<std::shared_ptr<const logic::Formula>> TraceLemmas::generate()
    {
        std::vector<std::shared_ptr<const logic::Formula>> lemmas;
        
        // generate standard induction lemmas for all loops, all variables and the predicates =,<,>,<=,>=.
        generateStandardInductionLemmas(lemmas);
        
        return lemmas;
    }

#pragma mark - Const Var Lemmas
    
    void TraceLemmas::generateStandardInductionLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    
    {
        for(const auto& function : program.functions)
        {
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsFunction;
            
            for (const auto& statement : function->statements)
            {
                generateStandardInductionLemmas(statement.get(), lemmas);
            }
        }
    }

    void TraceLemmas::generateStandardInductionLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
{
    if (statement->type() == program::Statement::Type::IfElse)
    {
        auto castedStatement = static_cast<const program::IfElse*>(statement);
        // recurse on both branches
        for (const auto& statement : castedStatement->ifStatements)
        {
            generateStandardInductionLemmas(statement.get(), lemmas);
        }
        for (const auto& statement : castedStatement->elseStatements)
        {
            generateStandardInductionLemmas(statement.get(), lemmas);
        }
    }
    else if (statement->type() == program::Statement::Type::WhileStatement)
    {
        auto castedStatement = static_cast<const program::WhileStatement*>(statement);
        // generate lemmas
        generateStandardInductionLemmas(castedStatement, lemmas, InductionKind::Equal);
        generateStandardInductionLemmas(castedStatement, lemmas, InductionKind::Less);
        generateStandardInductionLemmas(castedStatement, lemmas, InductionKind::Greater);
        generateStandardInductionLemmas(castedStatement, lemmas, InductionKind::LessEqual);
        generateStandardInductionLemmas(castedStatement, lemmas, InductionKind::GreaterEqual);
        
        // recurse on body
        for (const auto& statement : castedStatement->bodyStatements)
        {
            generateStandardInductionLemmas(statement.get(), lemmas);
        }
    }
}
    
    void TraceLemmas::generateStandardInductionLemmas(const program::WhileStatement* whileStatement,
                                                      std::vector<std::shared_ptr<const logic::Formula>>& lemmas,
                                                      const InductionKind kind)
    {
        std::string connective;
        switch (kind)
        {
            case InductionKind::Equal:
                connective = "=";
                break;
            case InductionKind::Less:
                connective = "<";
                break;
            case InductionKind::Greater:
                connective = ">";
                break;
            case InductionKind::LessEqual:
                connective = "<=";
                break;
            case InductionKind::GreaterEqual:
                connective = ">=";
                break;
        }
        
        auto itSymbol = iteratorMap.at(whileStatement);
        auto it = logic::Terms::var(itSymbol.get());
        auto n = lastIterationMap.at(whileStatement);
        auto locationSymbol = locationSymbolMap.at(whileStatement);
        
        auto locationName = locationSymbol->name;
        
        auto enclosingIteratorsAndIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndSuccOfIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndZero = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndN = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingIteratorSymbol : enclosingIteratorsMap.at(whileStatement))
        {
            enclosingIteratorsAndIt.push_back(logic::Terms::var(enclosingIteratorSymbol.get()));
            enclosingIteratorsAndSuccOfIt.push_back(logic::Terms::var(enclosingIteratorSymbol.get()));
            enclosingIteratorsAndZero.push_back(logic::Terms::var(enclosingIteratorSymbol.get()));
            enclosingIteratorsAndN.push_back(logic::Terms::var(enclosingIteratorSymbol.get()));
        }
        enclosingIteratorsAndIt.push_back(it);
        enclosingIteratorsAndSuccOfIt.push_back(logic::Theory::timeSucc(it));
        enclosingIteratorsAndZero.push_back(logic::Theory::timeZero());
        enclosingIteratorsAndN.push_back(n);
        
        // add lemma for each intVar
        for (const auto& v : locationToActiveVars.at(locationName))
        {
            if (!v->isArray)
            {
                // Part1: it<=n => v(l(it1,...,itk,it)) C v(l(it1,...,itk,s(it))), where C in {=,<,>,<=,>=}
                auto ineq = logic::Theory::timeSub(it, n);
                
                auto lhs1 = v->toTerm(logic::Terms::func(locationSymbol, enclosingIteratorsAndIt));
                auto rhs1 = v->toTerm(logic::Terms::func(locationSymbol, enclosingIteratorsAndSuccOfIt));
                std::shared_ptr<const Formula> formula1;
                switch (kind)
                {
                    case InductionKind::Equal:
                        formula1 = logic::Formulas::equality(lhs1, rhs1);
                        break;
                    case InductionKind::Less:
                        formula1 = logic::Theory::intLess(lhs1, rhs1);
                        break;
                    case InductionKind::Greater:
                        formula1 = logic::Theory::intGreater(lhs1, rhs1);
                        break;
                    case InductionKind::LessEqual:
                        formula1 = logic::Theory::intLessEqual(lhs1, rhs1);
                        break;
                    case InductionKind::GreaterEqual:
                        formula1 = logic::Theory::intGreaterEqual(lhs1, rhs1);
                        break;
                }
                auto imp = logic::Formulas::implication(ineq, formula1);
                auto universal = logic::Formulas::universal({itSymbol}, imp);
                
                // Part2: v(l(it1,...,itk,0)) C v(l(it1,...,itk,n)), where C in {=,<,>,<=,>=}
                auto lhs2 = v->toTerm(logic::Terms::func(locationSymbol, enclosingIteratorsAndZero));
                auto rhs2 = v->toTerm(logic::Terms::func(locationSymbol, enclosingIteratorsAndN));
                std::shared_ptr<const Formula> formula2;
                switch (kind)
                {
                    case InductionKind::Equal:
                        formula2 = logic::Formulas::equality(lhs2, rhs2);
                        break;
                    case InductionKind::Less:
                        formula2 = logic::Theory::intLess(lhs2, rhs2);
                        break;
                    case InductionKind::Greater:
                        formula2 = logic::Theory::intGreater(lhs2, rhs2);
                        break;
                    case InductionKind::LessEqual:
                        formula2 = logic::Theory::intLessEqual(lhs2, rhs2);
                        break;
                    case InductionKind::GreaterEqual:
                        formula2 = logic::Theory::intGreaterEqual(lhs2, rhs2);
                        break;
                }
                // Part1 => Part2
                auto label = "Lemma: Induction on " + connective + " for var " + v->name + " and location " + whileStatement->location;
                auto lemma = logic::Formulas::implication(universal, formula2, label);
                lemmas.push_back(lemma);
            }
        }
        
        // add lemma for each intArrayVar
        auto pSymbol = logic::Signature::varSymbol("p", logic::Sorts::intSort());
        auto p = logic::Terms::var(pSymbol.get());
        for (const auto& v : locationToActiveVars.at(locationName))
        {
            if (v->isArray)
            {
                // Part1: it<=n => v(l(it1,...,itk,it),p) C v(l(it1,...,itk,s(it)),p), where C in {=,<,>,<=,>=}
                auto ineq = logic::Theory::timeSub(it, n);
                
                auto lhs1 = v->toTerm(logic::Terms::func(locationSymbol, enclosingIteratorsAndIt), p);
                auto rhs1 = v->toTerm(logic::Terms::func(locationSymbol, enclosingIteratorsAndSuccOfIt), p);
                std::shared_ptr<const Formula> formula1;
                switch (kind)
                {
                    case InductionKind::Equal:
                        formula1 = logic::Formulas::equality(lhs1, rhs1);
                        break;
                    case InductionKind::Less:
                        formula1 = logic::Theory::intLess(lhs1, rhs1);
                        break;
                    case InductionKind::Greater:
                        formula1 = logic::Theory::intGreater(lhs1, rhs1);
                        break;
                    case InductionKind::LessEqual:
                        formula1 = logic::Theory::intLessEqual(lhs1, rhs1);
                        break;
                    case InductionKind::GreaterEqual:
                        formula1 = logic::Theory::intGreaterEqual(lhs1, rhs1);
                        break;
                }
                auto imp = logic::Formulas::implication(ineq, formula1);
                auto universal = logic::Formulas::universal({itSymbol}, imp);
                
                // Part2: v(l(it1,...,itk,0), p) C v(l(it1,...,itk,n), p), where C in {=,<,>,<=,>=}
                auto lhs2 = v->toTerm(logic::Terms::func(locationSymbol, enclosingIteratorsAndZero), p);
                auto rhs2 = v->toTerm(logic::Terms::func(locationSymbol, enclosingIteratorsAndN), p);
                std::shared_ptr<const Formula> formula2;
                switch (kind)
                {
                    case InductionKind::Equal:
                        formula2 = logic::Formulas::equality(lhs2, rhs2);
                        break;
                    case InductionKind::Less:
                        formula2 = logic::Theory::intLess(lhs2, rhs2);
                        break;
                    case InductionKind::Greater:
                        formula2 = logic::Theory::intGreater(lhs2, rhs2);
                        break;
                    case InductionKind::LessEqual:
                        formula2 = logic::Theory::intLessEqual(lhs2, rhs2);
                        break;
                    case InductionKind::GreaterEqual:
                        formula2 = logic::Theory::intGreaterEqual(lhs2, rhs2);
                        break;
                }
                // forall p. (Part1 => Part2)
                auto outerImp = logic::Formulas::implication(universal, formula2);
                auto label = "Lemma: Induction on " + connective + " for array var " + v->name + " and location " + whileStatement->location;
                auto lemma = logic::Formulas::universal({pSymbol}, outerImp, label);
                
                lemmas.push_back(lemma);
            }
        }
    }
}


