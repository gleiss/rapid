#include "TraceLemmas.hpp"

#include <cassert>
#include <iostream>

#include "Signature.hpp"
#include "Theory.hpp"
#include "Options.hpp"
#include "Output.hpp"

#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

using namespace logic;

namespace analysis {
    
#pragma mark - High level methods
    
    std::vector<std::shared_ptr<const logic::Formula>> TraceLemmas::generate()
    {
        std::vector<std::shared_ptr<const logic::Formula>> lemmas;
        
        // generate standard induction lemmas for all loops, all variables and the predicates =,<,>,<=,>=.
        generateStandardInductionLemmas(lemmas);
        
        if (twoTraces)
        {
            // generate for each active variable at each loop an induction lemma for equality of the variable on both traces
            generateTwoTracesLemmas(lemmas);
        }
        return lemmas;
    }

#pragma mark - Standard Induction Lemmas
    
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
        
        auto it = iteratorTermForLoop(whileStatement);
        auto n = lastIterationTermForLoop(whileStatement, twoTraces);
        auto locationSymbol = locationSymbolForStatement(whileStatement);
        
        auto locationName = locationSymbol->name;
        
        auto enclosingIteratorsAndIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndSuccOfIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndZero = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndN = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingLoop : *whileStatement->enclosingLoops)
        {
            auto enclosingIterator = iteratorTermForLoop(enclosingLoop);
            enclosingIteratorsAndIt.push_back(enclosingIterator);
            enclosingIteratorsAndSuccOfIt.push_back(enclosingIterator);
            enclosingIteratorsAndZero.push_back(enclosingIterator);
            enclosingIteratorsAndN.push_back(enclosingIterator);
        }
        enclosingIteratorsAndIt.push_back(it);
        enclosingIteratorsAndSuccOfIt.push_back(logic::Theory::natSucc(it));
        enclosingIteratorsAndZero.push_back(logic::Theory::natZero());
        enclosingIteratorsAndN.push_back(n);
        
        auto lStartIt = logic::Terms::func(locationSymbol, enclosingIteratorsAndIt);
        auto lStartSuccOfIt = logic::Terms::func(locationSymbol, enclosingIteratorsAndSuccOfIt);
        auto lStartZero = logic::Terms::func(locationSymbol, enclosingIteratorsAndZero);
        auto lStartN = logic::Terms::func(locationSymbol, enclosingIteratorsAndN);
        
        // add lemma for each intVar
        for (const auto& v : locationToActiveVars.at(locationName))
        {
            if (!v->isConstant)
            {
                if (!v->isArray)
                {
                    // Part1: it<=n => v(l(it1,...,itk,it)) C v(l(it1,...,itk,s(it))), where C in {=,<,>,<=,>=}
                    auto ineq = logic::Theory::natSub(it, n);
                    
                    auto lhs1 = toTerm(v,lStartIt);
                    auto rhs1 = toTerm(v,lStartSuccOfIt);
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
                    auto universal = logic::Formulas::universal({it->symbol}, imp);
                    
                    // Part2: v(l(it1,...,itk,0)) C v(l(it1,...,itk,n)), where C in {=,<,>,<=,>=}
                    auto lhs2 = toTerm(v,lStartZero);
                    auto rhs2 = toTerm(v,lStartN);
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
        }
        
        // add lemma for each intArrayVar
        auto pSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
        auto p = logic::Terms::var(pSymbol);
        for (const auto& v : locationToActiveVars.at(locationName))
        {
            if (!v->isConstant)
            {
                if (v->isArray)
                {
                    // Part1: it<=n => v(l(it1,...,itk,it),p) C v(l(it1,...,itk,s(it)),p), where C in {=,<,>,<=,>=}
                    auto ineq = logic::Theory::natSub(it, n);
                    
                    auto lhs1 = toTerm(v,lStartIt, p);
                    auto rhs1 = toTerm(v,lStartSuccOfIt, p);
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
                    auto universal = logic::Formulas::universal({it->symbol}, imp);
                    
                    // Part2: v(l(it1,...,itk,0), p) C v(l(it1,...,itk,n), p), where C in {=,<,>,<=,>=}
                    auto lhs2 = toTerm(v, lStartZero, p);
                    auto rhs2 = toTerm(v, lStartN, p);
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
    
#pragma mark - Lemmas for two traces
    void TraceLemmas::generateTwoTracesLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {
        for(const auto& function : program.functions)
        {
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsFunction;
            
            for (const auto& statement : function->statements)
            {
                generateTwoTracesLemmas(statement.get(), lemmas);
            }
        }
    }
    
    void TraceLemmas::generateTwoTracesLemmas(const program::Statement* statement,
                                                      std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            // recurse on both branches
            for (const auto& statement : castedStatement->ifStatements)
            {
                generateTwoTracesLemmas(statement.get(), lemmas);
            }
            for (const auto& statement : castedStatement->elseStatements)
            {
                generateTwoTracesLemmas(statement.get(), lemmas);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            // generate lemmas
            generateTwoTracesLemmas(castedStatement, lemmas);
            
            // recurse on body
            for (const auto& statement : castedStatement->bodyStatements)
            {
                generateTwoTracesLemmas(statement.get(), lemmas);
            }
        }
    }
    
    void TraceLemmas::generateTwoTracesLemmas(const program::WhileStatement* whileStatement,
                                              std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {
        auto t1 = trace1Term();
        auto t2 = trace2Term();
        
        auto itSymbol = iteratorSymbol(whileStatement);
        auto it = logic::Terms::var(itSymbol);
        auto locationSymbol = locationSymbolForStatement(whileStatement);
        
        auto locationName = locationSymbol->name;
        
        auto enclosingIteratorsAndIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndSuccOfIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndZero = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingLoop : *whileStatement->enclosingLoops)
        {
            auto enclosingIterator = iteratorTermForLoop(enclosingLoop);
            enclosingIteratorsAndIt.push_back(enclosingIterator);
            enclosingIteratorsAndSuccOfIt.push_back(enclosingIterator);
            enclosingIteratorsAndZero.push_back(enclosingIterator);
        }
        enclosingIteratorsAndIt.push_back(it);
        enclosingIteratorsAndSuccOfIt.push_back(logic::Theory::natSucc(it));
        enclosingIteratorsAndZero.push_back(logic::Theory::natZero());
        
        auto lStartIt = logic::Terms::func(locationSymbol, enclosingIteratorsAndIt);
        auto lStartSuccOfIt = logic::Terms::func(locationSymbol, enclosingIteratorsAndSuccOfIt);
        auto lStartZero = logic::Terms::func(locationSymbol, enclosingIteratorsAndZero);

        // add lemma for each intVar
        for (const auto& v : locationToActiveVars.at(locationName))
        {
            if (!v->isConstant)
            {
                if (!v->isArray)
                {
                    // Part1a: v(l(0),t1) = v(l(0),t2)
                    auto lhs1 = toTermFull(v, lStartZero, t1);
                    auto rhs1 = toTermFull(v, lStartZero, t2);
                    auto baseCase = logic::Formulas::equality(lhs1, rhs1);
                    
                    // Part1b: v(l(it),t1) = v(l(it), t2) => v(l(s(it)),t1) = v(l(s(it)),t2)
                    auto lhs2 = toTermFull(v, lStartIt, t1);
                    auto rhs2 = toTermFull(v, lStartIt, t2);
                    auto eq2 = logic::Formulas::equality(lhs2, rhs2);
                    
                    auto lhs3 = toTermFull(v, lStartSuccOfIt, t1);
                    auto rhs3 = toTermFull(v, lStartSuccOfIt, t2);
                    auto eq3 = logic::Formulas::equality(lhs3, rhs3);
                    
                    auto implication = logic::Formulas::implication(eq2, eq3);
                    auto inductiveCase = logic::Formulas::universal({itSymbol}, implication);
                    
                    // Part2: forall it. v(l(it),t1) = v(l(it), t2)
                    auto lhs4 = toTermFull(v, lStartIt, t1);
                    auto rhs4 = toTermFull(v, lStartIt, t2);
                    auto eq4 = logic::Formulas::equality(lhs4, rhs4);
                    
                    auto conclusion = logic::Formulas::universal({itSymbol}, eq4);
                    
                    // (Part1a and Part1b) => Part2
                    auto premise = logic::Formulas::conjunction({baseCase, inductiveCase});
                    
                    auto label = "Lemma: Var " + v->name + " at loop " + whileStatement->location + " has same values on both traces";
                    auto lemma = logic::Formulas::implication(premise, conclusion, label);
                    lemmas.push_back(lemma);
                }
            }
        }

        // add lemma for each arrayVar
        auto pSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
        auto p = logic::Terms::var(pSymbol);
        for (const auto& v : locationToActiveVars.at(locationName))
        {
            if (!v->isConstant)
            {
                if (v->isArray)
                {
                    // Part1a: v(l(0),p,t1) = v(l(0),p,t2)
                    auto lhs1 = toTermFull(v, lStartZero, p, t1);
                    auto rhs1 = toTermFull(v, lStartZero, p, t2);
                    auto baseCase = logic::Formulas::equality(lhs1, rhs1);
                    
                    // Part1b: v(l(it),p,t1) = v(l(it),p,t2) => v(l(s(it)),p,t1) = v(l(s(it)),p,t2)
                    auto lhs2 = toTermFull(v, lStartIt, p, t1);
                    auto rhs2 = toTermFull(v, lStartIt, p, t2);
                    auto eq2 = logic::Formulas::equality(lhs2, rhs2);
                    
                    auto lhs3 = toTermFull(v, lStartSuccOfIt, p, t1);
                    auto rhs3 = toTermFull(v, lStartSuccOfIt, p, t2);
                    auto eq3 = logic::Formulas::equality(lhs3, rhs3);
                    
                    auto implication = logic::Formulas::implication(eq2, eq3);
                    auto inductiveCase = logic::Formulas::universal({itSymbol}, implication);
                    
                    // Part2: forall it. v(l(it),p,t1) = v(l(it),p,t2)
                    auto lhs4 = toTermFull(v, lStartIt, p, t1);
                    auto rhs4 = toTermFull(v, lStartIt, p, t2);
                    auto eq4 = logic::Formulas::equality(lhs4, rhs4);
                    
                    auto conclusion = logic::Formulas::universal({itSymbol}, eq4);
                    
                    // forall p. ((Part1a and Part1b) => Part2)
                    auto premise = logic::Formulas::conjunction({baseCase, inductiveCase});

                    auto outerImp = logic::Formulas::implication(premise, conclusion);
                    auto label = "Lemma: Array var " + v->name + " at loop " + whileStatement->location + " has same values on both traces";
                    auto lemma = logic::Formulas::universal({pSymbol}, outerImp, label);

                    lemmas.push_back(lemma);

                }
            }
        }
    }
}


