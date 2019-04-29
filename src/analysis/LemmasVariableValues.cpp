#include "LemmasVariableValues.hpp"

#include "Formula.hpp"
#include "Theory.hpp"
#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

namespace analysis {
    
    void ValueEvolutionLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        // generate lemmas
        generateLemmas(statement, items, InductionKind::Equal);
        //        generateValueEvolutionLemma(statement, items, InductionKind::LessEqual);
        //        generateValueEvolutionLemma(statement, items, InductionKind::GreaterEqual);
        
        generateLemmasBoundedEq(statement, items);
    }
    
    void ValueEvolutionLemmas::generateLemmas(const program::WhileStatement* whileStatement,
                                                                  std::vector<std::shared_ptr<const logic::ProblemItem>>& items,
                                                                  const InductionKind kind)
    {
        auto it = iteratorTermForLoop(whileStatement);
        auto n = lastIterationTermForLoop(whileStatement, twoTraces);
        
        auto lStartIt = timepointForLoopStatement(whileStatement, it);
        auto lStartSuccOfIt = timepointForLoopStatement(whileStatement, logic::Theory::natSucc(it));
        auto lStartZero = timepointForLoopStatement(whileStatement, logic::Theory::natZero());
        auto lStartN = timepointForLoopStatement(whileStatement, n);
        
        auto posSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
        auto pos = logic::Terms::var(posSymbol);
        
        // add lemma for each intVar and each intArrayVar
        for (const auto& v : locationToActiveVars.at(locationSymbolForStatement(whileStatement)->name))
        {
            if (!v->isConstant)
            {
                // Premise: forall it (it<=n => v(l(it1,...,itk,it)    ) C v(l(it1,...,itk,s(it))    )), where C in {=,<=,>=} or
                //          forall it (it<=n => v(l(it1,...,itk,it),pos) C v(l(it1,...,itk,s(it)),pos)), where C in {=,<=,>=}
                auto lhs1 = v->isArray ? toTerm(v,lStartIt, pos) : toTerm(v,lStartIt);
                auto rhs1 = v->isArray ? toTerm(v,lStartSuccOfIt, pos) : toTerm(v,lStartSuccOfIt);
                std::shared_ptr<const logic::Formula> formula1;
                switch (kind)
                {
                    case InductionKind::Equal:
                        formula1 = logic::Formulas::equality(lhs1, rhs1);
                        break;
                    case InductionKind::LessEqual:
                        formula1 = logic::Theory::intLessEqual(lhs1, rhs1);
                        break;
                    case InductionKind::GreaterEqual:
                        formula1 = logic::Theory::intGreaterEqual(lhs1, rhs1);
                        break;
                }

                auto premise =
                    logic::Formulas::universal({it->symbol},
                        logic::Formulas::implication(
                            logic::Theory::natSub(it, n),
                            formula1
                        )
                    );
                
                // Conclusion: v(l(it1,...,itk,0)    ) C v(l(it1,...,itk,n)    ), where C in {=,<=,>=} or
                //             v(l(it1,...,itk,0),pos) C v(l(it1,...,itk,n),pos), where C in {=,<=,>=}
                auto lhs2 = v->isArray ? toTerm(v, lStartZero, pos) : toTerm(v,lStartZero);
                auto rhs2 = v->isArray ? toTerm(v, lStartN, pos) : toTerm(v,lStartN);
                std::shared_ptr<const logic::Formula> conclusion;
                switch (kind)
                {
                    case InductionKind::Equal:
                        conclusion = logic::Formulas::equality(lhs2, rhs2);
                        break;
                    case InductionKind::LessEqual:
                        conclusion = logic::Theory::intLessEqual(lhs2, rhs2);
                        break;
                    case InductionKind::GreaterEqual:
                        conclusion = logic::Theory::intGreaterEqual(lhs2, rhs2);
                        break;
                }
                // forall enclosingIterators: (Premise => Conclusion) or
                // forall enclosingIterators: forall pos. (Premise => Conclusion)
                auto outerImp = logic::Formulas::implication(premise, conclusion);
                auto universal = v->isArray ? logic::Formulas::universal({posSymbol}, outerImp) : outerImp;
                auto bareLemma = logic::Formulas::universal(enclosingIteratorsSymbols(whileStatement), universal);
                
                if (twoTraces)
                {
                    auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                    bareLemma = logic::Formulas::universal({tr}, bareLemma);
                }
                
                std::string connective;
                switch (kind)
                {
                    case InductionKind::Equal:
                        connective = "eq";
                        break;
                    case InductionKind::LessEqual:
                        connective = "leq";
                        break;
                    case InductionKind::GreaterEqual:
                        connective = "geq";
                        break;
                }
                auto name = "value-evolution-" + connective + "-" + v->name + "-" + whileStatement->location;
                items.push_back(std::make_shared<logic::Lemma>(bareLemma, name));
            }
        }
    }
    
    void ValueEvolutionLemmas::generateLemmasBoundedEq(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        auto itSymbol = iteratorSymbol(statement);
        auto it = iteratorTermForLoop(statement);
        auto it2Symbol = logic::Signature::varSymbol("it", logic::Sorts::natSort());
        auto it2 = logic::Terms::var(it2Symbol);
        auto n = lastIterationTermForLoop(statement, twoTraces);
        
        auto lStartIt = timepointForLoopStatement(statement, it);
        auto lStartSuccOfIt = timepointForLoopStatement(statement, logic::Theory::natSucc(it));
        auto lStartSuccOfIt2 = timepointForLoopStatement(statement, logic::Theory::natSucc(it2));
        auto lStartN = timepointForLoopStatement(statement, n);
        
        auto posSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
        auto pos = logic::Terms::var(posSymbol);
        
        // add lemma for each intVar
        //  forall (it2 : Nat)
        //     =>
        //        and
        //           it2 < n
        //           forall (it : Nat)
        //              =>
        //                 it2 < it
        //                 v(l(s(it))) = v(l(it))
        //        v(l(n)) = v(l(s(it2))
        for (const auto& v : locationToActiveVars.at(locationSymbolForStatement(statement)->name))
        {
            if (!v->isConstant)
            {
                // Premise: it2 < n and forall it. (it2<it => v(l(s(it))) = v(l(it)))
                auto premise =
                    logic::Formulas::conjunction({
                        logic::Theory::natSub(it2,n),
                        logic::Formulas::universal({itSymbol},
                            logic::Formulas::implication(
                                logic::Theory::natSub(it2,it),
                                logic::Formulas::equality(
                                    v->isArray ? toTerm(v,lStartSuccOfIt,pos) : toTerm(v,lStartSuccOfIt),
                                    v->isArray ? toTerm(v,lStartIt,pos) : toTerm(v,lStartIt)
                                )
                            )
                        )
                    });

                // Conclusion: (v(l(n)) = v(l(s(it2)))
                auto conclusion =
                    logic::Formulas::equality(
                        v->isArray ? toTerm(v,lStartN,pos) : toTerm(v,lStartN),
                        v->isArray ? toTerm(v,lStartSuccOfIt2,pos) : toTerm(v,lStartSuccOfIt2)
                    );
                
                // forall enclosingIterators. forall it2. (premise => conclusion)
                auto bareLemma =
                    logic::Formulas::universal(enclosingIteratorsSymbols(statement),
                        logic::Formulas::universal({it2Symbol, posSymbol},
                            logic::Formulas::implication(premise,conclusion))
                    );
                
                if (twoTraces)
                {
                    auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                    bareLemma = logic::Formulas::universal({tr}, bareLemma);
                }
                
                auto name = "value-evolution-bounded-" + v->name + "-" + statement->location;
                items.push_back(std::make_shared<logic::Lemma>(bareLemma, name));
            }
        }
    }
    
    void StaticAnalysisLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        auto itSymbol = iteratorSymbol(statement);
        auto it = iteratorTermForLoop(statement);
        
        auto lStartIt = timepointForLoopStatement(statement, it);
        auto lStartZero = timepointForLoopStatement(statement, logic::Theory::natZero());
        
        auto posSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
        auto pos = logic::Terms::var(posSymbol);
        
        auto activeVars = locationToActiveVars.at(statement->location);
        auto assignedVars = computeAssignedVars(statement);
        
        // for each active var, which is not constant but not assigned to in any statement of the loop,
        // add a lemma asserting that var is the same in each iteration as in the first iteration.
        for (const auto& activeVar : activeVars)
        {
            if (!activeVar->isConstant && assignedVars.count(activeVar) == 0)
            {
                // forall enclosing iterators. forall it.     v(l(it)    ) = v(l(zero)    ) or
                // forall enclosing iterators. forall it,pos. v(l(it),pos) = v(l(zero),pos) or
                auto eq =
                    logic::Formulas::equality(
                        activeVar->isArray ? toTerm(activeVar,lStartIt,pos) : toTerm(activeVar,lStartIt),
                        activeVar->isArray ? toTerm(activeVar,lStartZero,pos) : toTerm(activeVar,lStartZero)
                    );
            
                auto universal =
                    activeVar->isArray ?
                        logic::Formulas::universal({itSymbol, posSymbol}, eq) :
                        logic::Formulas::universal({itSymbol}, eq);
                
                auto bareLemma = logic::Formulas::universal(enclosingIteratorsSymbols(statement),universal);

                if (twoTraces)
                {
                    auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                    bareLemma = logic::Formulas::universal({tr}, bareLemma);
                }
                
                auto name = "values-static-" + activeVar->name + "-" + statement->location;
                items.push_back(std::make_shared<logic::Lemma>(bareLemma, name));
            }
        }
    }
    
    std::unordered_set<std::shared_ptr<const program::Variable>> StaticAnalysisLemmas::computeAssignedVars(const program::Statement* statement)
    {
        std::unordered_set<std::shared_ptr<const program::Variable>> assignedVars;
        
        switch (statement->type())
        {
            case program::Statement::Type::IntAssignment:
            {
                auto castedStatement = static_cast<const program::IntAssignment*>(statement);
                // add variable on lhs to assignedVars, independently from whether those vars are simple ones or arrays.
                if (castedStatement->lhs->type() == program::IntExpression::Type::IntVariableAccess)
                {
                    auto access = static_cast<const program::IntVariableAccess*>(castedStatement->lhs.get());
                    assignedVars.insert(access->var);
                }
                else
                {
                    assert(castedStatement->lhs->type() == program::IntExpression::Type::IntArrayApplication);
                    auto arrayAccess = static_cast<const program::IntArrayApplication*>(castedStatement->lhs.get());
                    assignedVars.insert(arrayAccess->array);
                }
                break;
            }
            case program::Statement::Type::IfElse:
            {
                auto castedStatement = static_cast<const program::IfElse*>(statement);
                // collect assignedVars from both branches
                for (const auto& statement : castedStatement->ifStatements)
                {
                    auto res = computeAssignedVars(statement.get());
                    assignedVars.insert(res.begin(), res.end());
                }
                for (const auto& statement : castedStatement->elseStatements)
                {
                    auto res = computeAssignedVars(statement.get());
                    assignedVars.insert(res.begin(), res.end());
                }
                break;
            }
            case program::Statement::Type::WhileStatement:
            {
                auto castedStatement = static_cast<const program::WhileStatement*>(statement);
                // collect assignedVars from body
                for (const auto& statement : castedStatement->bodyStatements)
                {
                    auto res = computeAssignedVars(statement.get());
                    assignedVars.insert(res.begin(), res.end());
                }
                break;
            }
            case program::Statement::Type::SkipStatement:
            {
                break;
            }
        }
        return assignedVars;
    }
}
