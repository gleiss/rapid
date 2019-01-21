#include "TraceLemmas.hpp"

#include <memory>
#include <string>
#include <vector>
#include <cassert>

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
        
        // generate standard induction lemmas for all IntermediateValues, all variables and the predicates =,<,>,<=,>=.
        generateStandardInductionLemmas(lemmas);
        generateLoopLemmas(lemmas);
        generateIntermediateValueLemmas(lemmas);
        
        if (twoTraces)
        {
            // generate for each active variable at each loop an induction lemma for equality of the variable on both traces
            generateTwoTracesLemmas(lemmas);
            generateNEqualLemmas(lemmas);
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
        
        auto enclosingIteratorsSymbols = std::vector<std::shared_ptr<const logic::Symbol>>();

        auto enclosingIteratorsAndIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndSuccOfIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndZero = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndN = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingLoop : *whileStatement->enclosingLoops)
        {
            auto enclosingIteratorSymbol = iteratorSymbol(enclosingLoop);
            enclosingIteratorsSymbols.push_back(enclosingIteratorSymbol);

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
                    // Part1: forall it (it<=n => v(l(it1,...,itk,it)) C v(l(it1,...,itk,s(it)))), where C in {=,<,>,<=,>=}
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
                    auto part1 = logic::Formulas::universal({it->symbol}, imp);
                    
                    // Part2: v(l(it1,...,itk,0)) C v(l(it1,...,itk,n)), where C in {=,<,>,<=,>=}
                    auto lhs2 = toTerm(v,lStartZero);
                    auto rhs2 = toTerm(v,lStartN);
                    std::shared_ptr<const Formula> part2;
                    switch (kind)
                    {
                        case InductionKind::Equal:
                            part2 = logic::Formulas::equality(lhs2, rhs2);
                            break;
                        case InductionKind::Less:
                            part2 = logic::Theory::intLess(lhs2, rhs2);
                            break;
                        case InductionKind::Greater:
                            part2 = logic::Theory::intGreater(lhs2, rhs2);
                            break;
                        case InductionKind::LessEqual:
                            part2 = logic::Theory::intLessEqual(lhs2, rhs2);
                            break;
                        case InductionKind::GreaterEqual:
                            part2 = logic::Theory::intGreaterEqual(lhs2, rhs2);
                            break;
                    }
                    // forall enclosingIterators: (Part1 => Part2)
                    auto outermostImp = logic::Formulas::implication(part1, part2);
                    auto label = "Lemma: Induction on " + connective + " for var " + v->name + " and location " + whileStatement->location;

                    if (twoTraces)
                    {
                        auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, outermostImp);
                        auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        lemmas.push_back(logic::Formulas::universal({tr}, lemma, label));
                    }
                    else
                    {
                        auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, outermostImp, label);
                        lemmas.push_back(lemma);
                    }
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
                    auto part1 = logic::Formulas::universal({it->symbol}, imp);
                    
                    // Part2: v(l(it1,...,itk,0), p) C v(l(it1,...,itk,n), p), where C in {=,<,>,<=,>=}
                    auto lhs2 = toTerm(v, lStartZero, p);
                    auto rhs2 = toTerm(v, lStartN, p);
                    std::shared_ptr<const Formula> part2;
                    switch (kind)
                    {
                        case InductionKind::Equal:
                            part2 = logic::Formulas::equality(lhs2, rhs2);
                            break;
                        case InductionKind::Less:
                            part2 = logic::Theory::intLess(lhs2, rhs2);
                            break;
                        case InductionKind::Greater:
                            part2 = logic::Theory::intGreater(lhs2, rhs2);
                            break;
                        case InductionKind::LessEqual:
                            part2 = logic::Theory::intLessEqual(lhs2, rhs2);
                            break;
                        case InductionKind::GreaterEqual:
                            part2 = logic::Theory::intGreaterEqual(lhs2, rhs2);
                            break;
                    }
                    // forall p. (Part1 => Part2)
                    auto outerImp = logic::Formulas::implication(part1, part2);
                    auto label = "Lemma: Induction on " + connective + " for array var " + v->name + " and location " + whileStatement->location;
                    auto universal = logic::Formulas::universal({pSymbol}, outerImp, label);
                    auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, universal);

                    if (twoTraces)
                    {
                        auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        lemmas.push_back(logic::Formulas::universal({tr}, lemma));
                    }
                    else
                    {
                        lemmas.push_back(lemma);
                    }
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

        auto enclosingIteratorsSymbols = std::vector<std::shared_ptr<const logic::Symbol>>();

        auto enclosingIteratorsAndIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndSuccOfIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndZero = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingLoop : *whileStatement->enclosingLoops)
        {
            auto enclosingIteratorSymbol = iteratorSymbol(enclosingLoop);
            enclosingIteratorsSymbols.push_back(enclosingIteratorSymbol);

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
                    auto outerImplication = logic::Formulas::implication(premise, conclusion, label);
                    auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, outerImplication);
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
                    auto universal = logic::Formulas::universal({pSymbol}, outerImp, label);
                    auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, universal);
                    lemmas.push_back(lemma);

                }
            }
        }
    }
    
    void TraceLemmas::generateNEqualLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {
        for(const auto& function : program.functions)
        {
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsFunction;
            
            for (const auto& statement : function->statements)
            {
                generateNEqualLemmas(statement.get(), lemmas);
            }
        }
    }
    
    void TraceLemmas::generateNEqualLemmas(const program::Statement* statement,
                                              std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            // recurse on both branches
            for (const auto& statement : castedStatement->ifStatements)
            {
                generateNEqualLemmas(statement.get(), lemmas);
            }
            for (const auto& statement : castedStatement->elseStatements)
            {
                generateNEqualLemmas(statement.get(), lemmas);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            // generate lemmas
            generateNEqualLemmas(castedStatement, lemmas);
            
            // recurse on body
            for (const auto& statement : castedStatement->bodyStatements)
            {
                generateNEqualLemmas(statement.get(), lemmas);
            }
        }
    }
    

    void TraceLemmas::generateNEqualLemmas(const program::WhileStatement* whileStatement,
                                              std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {
        assert(twoTraces);

        auto t1 = trace1Term();
        auto t2 = trace2Term();
        
        auto it = iteratorTermForLoop(whileStatement);
        auto nT1 = lastIterationTermForLoop(whileStatement, t1, true);
        auto nT2 = lastIterationTermForLoop(whileStatement, t2, true);
        
        auto iteratorsItTerms = std::vector<std::shared_ptr<const logic::Term>>();
        auto iteratorsNT2Terms = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsSymbols = std::vector<std::shared_ptr<const logic::Symbol>>();

        for (const auto& enclosingLoop : *whileStatement->enclosingLoops)
        {
            auto enclosingIteratorSymbol = iteratorSymbol(enclosingLoop);
            enclosingIteratorsSymbols.push_back(enclosingIteratorSymbol);

            auto enclosingIteratorTerm = iteratorTermForLoop(enclosingLoop);
            iteratorsItTerms.push_back(enclosingIteratorTerm);
            iteratorsNT2Terms.push_back(enclosingIteratorTerm);

        }
        iteratorsItTerms.push_back(it);
        iteratorsNT2Terms.push_back(nT2);

        auto lStartIt = logic::Terms::func(locationSymbolForStatement(whileStatement), iteratorsItTerms);
        auto lStartNT2 = logic::Terms::func(locationSymbolForStatement(whileStatement), iteratorsNT2Terms);

        // Part 1: Loop condition holds at main-loop-location in t1 for all iterations before n(t2)
        auto sub = logic::Theory::natSub(it, nT2);
        auto condition1 = toFormula(whileStatement->condition, lStartIt, t1);
        auto imp = logic::Formulas::implication(sub, condition1);
        auto part1 = logic::Formulas::universal({it->symbol}, imp);
        
        // Part 2: Loop condition doesn't hold at main-loop-location in t1 for iteration n(t2)
        auto condition2 = toFormula(whileStatement->condition, lStartNT2, t1);
        auto part2 = logic::Formulas::negation(condition2);
        
        // (Part1 and Part2) => nT1=nT2
        auto premise = logic::Formulas::conjunction({part1, part2});
        auto conclusion = logic::Formulas::equality(nT2, nT1);
        auto nName = nT1->symbol->name;
        auto label = "Lemma: If " + nName + "(t2) has same properties as " + nName + "(t1), then " + nName + "(t2)=" + nName + "(t1) (for loop at " + whileStatement->location + ")";
        auto lemma = logic::Formulas::implication(premise, conclusion, label);
        auto lemmaEnclosed = logic::Formulas::universal(enclosingIteratorsSymbols, lemma);
        lemmas.push_back(lemmaEnclosed);
    }

    #pragma mark - Loop Lemma
    
    void TraceLemmas::generateLoopLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    
    {
        for(const auto& function : program.functions)
        {
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsFunction;
            
            for (const auto& statement : function->statements)
            {
                generateLoopLemmas(statement.get(), lemmas);
            }
        }
    }

    void TraceLemmas::generateLoopLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            // recurse on both branches
            for (const auto& statement : castedStatement->ifStatements)
            {
                generateLoopLemmas(statement.get(), lemmas);
            }
            for (const auto& statement : castedStatement->elseStatements)
            {
                generateLoopLemmas(statement.get(), lemmas);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            // generate lemmas
            generateLoopLemmas(castedStatement, lemmas);
               
            // recurse on body
            for (const auto& statement : castedStatement->bodyStatements)
            {
                generateLoopLemmas(statement.get(), lemmas);
            }
        }
    }

    void TraceLemmas::generateLoopLemmas(const program::WhileStatement* whileStatement,
                                                      std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {               
        auto it = iteratorTermForLoop(whileStatement);
        auto n = lastIterationTermForLoop(whileStatement, twoTraces);
        auto locationSymbol = locationSymbolForStatement(whileStatement);
        
        auto locationName = locationSymbol->name;
        
        auto enclosingIteratorsSymbols = std::vector<std::shared_ptr<const logic::Symbol>>();

        auto enclosingIteratorsAndIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndSuccOfIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndZero = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndN = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingLoop : *whileStatement->enclosingLoops)
        {
            auto enclosingIteratorSymbol = iteratorSymbol(enclosingLoop);
            enclosingIteratorsSymbols.push_back(enclosingIteratorSymbol);

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
        
        auto lStartZero = logic::Terms::func(locationSymbol, enclosingIteratorsAndZero);
        

        // The lemma says: if the loop condition holds, there should be at least one loop iteration
        // C => exists it. s(it) = n
        // Constuct lhs: loop condition holds at first iteration
        auto c = toFormula(whileStatement->condition,lStartZero);

        // Construct rhs: exists it (s(it) = n)
        auto itSym = logic::Signature::varSymbol("it", logic::Sorts::natSort());
        auto itVar = logic::Terms::var(itSym);
        auto lhs = logic::Formulas::existential({itSym},logic::Formulas::equality(logic::Theory::natSucc(itVar),n));        
        auto label = "Lemma: if the condition of the loop at " + whileStatement->location + " holds initially, there is at least one loop iteration";     
        auto loopLemma = logic::Formulas::implication(c,lhs,label);        
        auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, loopLemma);
                
        if (twoTraces)
        {
            auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
            lemmas.push_back(logic::Formulas::universal({tr}, lemma));
        }
        else
        {
            lemmas.push_back(lemma);
        }

    }

    #pragma mark - Intermediate Value Lemma


    void TraceLemmas::generateIntermediateValueLemmas(std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    
    {
        for(const auto& function : program.functions)
        {
            std::vector<std::shared_ptr<const logic::Formula>> conjunctsFunction;
            
            for (const auto& statement : function->statements)
            {
                generateIntermediateValueLemmas(statement.get(), lemmas);
            }
        }
    }

    void TraceLemmas::generateIntermediateValueLemmas(const program::Statement* statement,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            // recurse on both branches
            for (const auto& statement : castedStatement->ifStatements)
            {
                generateIntermediateValueLemmas(statement.get(), lemmas);
            }
            for (const auto& statement : castedStatement->elseStatements)
            {
                generateIntermediateValueLemmas(statement.get(), lemmas);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            // generate lemmas
            generateIntermediateValueLemmas(castedStatement, lemmas);
               
            // recurse on body
            for (const auto& statement : castedStatement->bodyStatements)
            {
                generateIntermediateValueLemmas(statement.get(), lemmas);
            }
        }
    }

    void TraceLemmas::generateIntermediateValueLemmas(const program::WhileStatement* whileStatement,
                                                      std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
    {                 
        auto iSymbol = iteratorSymbol(whileStatement);
        auto it = iteratorTermForLoop(whileStatement);
        auto n = lastIterationTermForLoop(whileStatement, twoTraces);
        auto locationSymbol = locationSymbolForStatement(whileStatement);
        
        auto locationName = locationSymbol->name;
        
        auto enclosingIteratorsSymbols = std::vector<std::shared_ptr<const logic::Symbol>>();

        auto enclosingIterators = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndSuccOfIt = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndZero = std::vector<std::shared_ptr<const logic::Term>>();
        auto enclosingIteratorsAndN = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingLoop : *whileStatement->enclosingLoops)
        {
            auto enclosingIteratorSymbol = iteratorSymbol(enclosingLoop);
            enclosingIteratorsSymbols.push_back(enclosingIteratorSymbol);

            auto enclosingIterator = iteratorTermForLoop(enclosingLoop);
            enclosingIterators.push_back(enclosingIterator);
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
        // Lemma: forall ((x : Int) (it : Nat)). (v l(zero) <= x & x < v l(n) & v l(s(it)) = (v l(it) +1) 
        //                                        =>  exists (it2 : Nat) v l(it2) = x & it2 < n)
        for (const auto& v : locationToActiveVars.at(locationName))
        {            
            if (!v->isConstant)
            {
                if (!v->isArray)
                {              
                    // Part 1: (v l(zero) <= x & x < v l(n) & v l(s(it)) = (v l(it) +1) 
                    std::vector<std::shared_ptr<const logic::Formula>> conjunctsLHS;      

                    // Part 1.1: v l(zero) <= x
                    auto itSym = logic::Signature::varSymbol("it", logic::Sorts::natSort());
                    auto itVar = logic::Terms::var(itSym);

                    auto xSym = logic::Signature::varSymbol("x", logic::Sorts::intSort());
                    auto x = logic::Terms::var(xSym);
        
                    auto  vzero = toTerm(v,lStartZero);
                    conjunctsLHS.push_back(logic::Theory::intLessEqual(vzero,x));

                    // Part 1.2: x < v l(n)
                    auto  vn = toTerm(v,lStartN);
                    conjunctsLHS.push_back(logic::Theory::intLess(x,vn));

                    // Part 1.3: v l(s(it)) = (v l(it) +1) 
                    auto vsit = toTerm(v,lStartSuccOfIt);
                    auto vit = toTerm(v,lStartIt);                    
                    auto vitpp = logic::Theory::intAddition(vit,logic::Theory::intConstant(1));
                    conjunctsLHS.push_back(logic::Formulas::equality(vsit,vitpp));

                    // Combine 1.1 - 1.3
                    auto lhs = logic::Formulas::conjunction(conjunctsLHS);

                    //Part 2: exists (it2 : Nat) v l(it2) = x & it2 < n
                    std::vector<std::shared_ptr<const logic::Formula>> conjunctsRHS;

                    // Create location vector of enclosing iterators for existentially qualified iterator 'it'                  
                    auto enclosingIteratorsAndIt2 = enclosingIterators;
                    enclosingIteratorsAndIt2.push_back(itVar);
                    auto lStartIt2 = logic::Terms::func(locationSymbol,enclosingIteratorsAndIt2);
                    
                    //Part 2.1: v l(it2) = x 
                    auto vit2 = toTerm(v,lStartIt2);
                    conjunctsRHS.push_back(logic::Formulas::equality(vit2,x));


                    //Part 2.2.: it2 < n
                    conjunctsRHS.push_back(logic::Theory::natSub(itVar,n));

                    // Combine part 2 and quantify
                    auto rhs = logic::Formulas::conjunction(conjunctsRHS);
                    auto qrhs = logic::Formulas::existential({itSym},rhs);

                    // Combine lhs and rhs, then quantify
                    auto combined = logic::Formulas::implication(lhs,qrhs);
                    auto label = "Lemma: Intermediate value for var " + v->name + " at location " + whileStatement->location;
                    auto universal = logic::Formulas::universal({xSym,iSymbol},combined,label);

                    auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, universal);

                    if (twoTraces)
                    {
                        auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        lemmas.push_back(logic::Formulas::universal({tr}, lemma));
                    }
                    else
                    {
                        lemmas.push_back(lemma);
                    }
                }
            }
        }

        // Should this lemma be added for arrays, too?
        
        // add lemma for each intArrayVar
        auto pSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
        auto p = logic::Terms::var(pSymbol);
        for (const auto& v : locationToActiveVars.at(locationName))
        {
            if (!v->isConstant)
            {
                if (v->isArray)
                {
                    // Part 1: (v l(zero) <= x & x < v l(n) & v l(s(it)) = (v l(it) +1) 
                    std::vector<std::shared_ptr<const logic::Formula>> conjunctsLHS;      

                    // Part 1.1: v l(zero) <= x
                    auto itSym = logic::Signature::varSymbol("it", logic::Sorts::natSort());
                    auto itVar = logic::Terms::var(itSym);

                    auto xSym = logic::Signature::varSymbol("x", logic::Sorts::intSort());
                    auto x = logic::Terms::var(xSym);
        
                    auto  vzero = toTerm(v,lStartZero,p);
                    conjunctsLHS.push_back(logic::Theory::intLessEqual(vzero,x));

                    // Part 1.2: x < v l(n)
                    auto  vn = toTerm(v,lStartN,p);
                    conjunctsLHS.push_back(logic::Theory::intLess(x,vn));

                    // Part 1.3: v l(s(it)) = (v l(it) +1) 
                    auto vsit = toTerm(v,lStartSuccOfIt,p);
                    auto vit = toTerm(v,lStartIt,p);                    
                    auto vitpp = logic::Theory::intAddition(vit,logic::Theory::intConstant(1));
                    conjunctsLHS.push_back(logic::Formulas::equality(vsit,vitpp));

                    // Combine 1.1 - 1.3
                    auto lhs = logic::Formulas::conjunction(conjunctsLHS);

                    //Part 2: exists (it2 : Nat) v l(it2) = x & it2 < n
                    std::vector<std::shared_ptr<const logic::Formula>> conjunctsRHS;

                    // Create location vector of enclosing iterators for existentially qualified iterator 'it'                  
                    auto enclosingIteratorsAndIt2 = enclosingIterators;
                    enclosingIteratorsAndIt2.push_back(itVar);
                    auto lStartIt2 = logic::Terms::func(locationSymbol,enclosingIteratorsAndIt2);
                    
                    //Part 2.1: v l(it2) = x 
                    auto vit2 = toTerm(v,lStartIt2,p);
                    conjunctsRHS.push_back(logic::Formulas::equality(vit2,x));


                    //Part 2.2.: it2 < n
                    conjunctsRHS.push_back(logic::Theory::natSub(itVar,n));

                    // Combine part 2 and quantify
                    auto rhs = logic::Formulas::conjunction(conjunctsRHS);
                    auto qrhs = logic::Formulas::existential({itSym},rhs);

                    // Combine lhs and rhs, then quantify
                    auto combined = logic::Formulas::implication(lhs,qrhs);
                    auto label = "Lemma: Intermediate value for array var " + v->name + " at location " + whileStatement->location;
                    auto universal = logic::Formulas::universal({xSym,iSymbol,pSymbol},combined,label);

                    auto lemma = logic::Formulas::universal(enclosingIteratorsSymbols, universal);

                    if (twoTraces)
                    {
                        auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        lemmas.push_back(logic::Formulas::universal({tr}, lemma));
                    }
                    else
                    {
                        lemmas.push_back(lemma);
                    }
                }
            }
        }
    }
}


