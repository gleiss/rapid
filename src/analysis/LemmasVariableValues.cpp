#include "LemmasVariableValues.hpp"

#include "Formula.hpp"
#include "Theory.hpp"
#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

namespace analysis {
    
    void ValueEvolutionLemmas::generateOutputFor(const program::WhileStatement* whileStatement,
                                                                  std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        auto boundLSymbol = logic::Signature::varSymbol("boundL", logic::Sorts::natSort());
        auto boundRSymbol = logic::Signature::varSymbol("boundR", logic::Sorts::natSort());
        
        auto boundL = logic::Terms::var(boundLSymbol);
        auto boundR = logic::Terms::var(boundRSymbol);
        auto it = iteratorTermForLoop(whileStatement);

        auto lStartBoundL = timepointForLoopStatement(whileStatement, boundL);
        auto lStartboundR = timepointForLoopStatement(whileStatement, boundR);
        auto lStartIt = timepointForLoopStatement(whileStatement, it);
        auto lStartSuccOfIt = timepointForLoopStatement(whileStatement, logic::Theory::natSucc(it));
        
        auto posSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
        auto pos = logic::Terms::var(posSymbol);
        
        auto predicates = {
            std::make_pair(logic::Formulas::equality, std::string("eq")),
            std::make_pair(logic::Theory::intLessEqual, std::string("leq")),
            std::make_pair(logic::Theory::intGreaterEqual, std::string("geq"))
        };
        
        // add lemma for each intVar and each intArrayVar, for each variant
        for (const auto& v : locationToActiveVars.at(locationSymbolForStatement(whileStatement)->name))
        {
            if (!v->isConstant)
            {
                for (const auto predicate : predicates)
                {
                    auto predicateFunctor = predicate.first;
                    auto predicateString = predicate.second;
                    
                    // IH(it): v(l(it1,...,itk,boundL)    ) C v(l(it1,...,itk,it)    ), where C in {=,<=,>=} or
                    //         v(l(it1,...,itk,boundL),pos) C v(l(it1,...,itk,it),pos), where C in {=,<=,>=}
                    auto inductionHypothesis = [&](std::shared_ptr<const logic::Term> arg)
                    {
                        auto lStartArg = timepointForLoopStatement(whileStatement, arg);
                        return predicateFunctor(
                            v->isArray ? toTerm(v, lStartBoundL, pos) : toTerm(v,lStartBoundL),
                            v->isArray ? toTerm(v, lStartArg, pos) : toTerm(v,lStartArg),
                            ""
                        );
                    };
                    
                    // PART 1: Add induction-axiom
                    auto inductionAxiom = logic::inductionAxiom1(inductionHypothesis);
                    if (v->isArray)
                    {
                        inductionAxiom = logic::Formulas::universal({posSymbol}, inductionAxiom);
                    }
                    if (twoTraces)
                    {
                        auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        inductionAxiom = logic::Formulas::universal({tr}, inductionAxiom);
                    }
                    auto axiomName = "value-evolution-axiom-" + predicateString + "-" + v->name + "-" + whileStatement->location;
                    items.push_back(std::make_shared<logic::Axiom>(inductionAxiom, axiomName));
                    
                    // PART 2: Add trace lemma
                    // Premise: forall it. (boundL<=it<boundR => v(l(it1,...,itk,it)    ) C v(l(it1,...,itk,s(it))    )), where C in {=,<=,>=} or
                    //          forall it. (boundL<=it<boundR => v(l(it1,...,itk,it),pos) C v(l(it1,...,itk,s(it)),pos)), where C in {=,<=,>=}
                    auto premise =
                        logic::Formulas::universal({it->symbol},
                            logic::Formulas::implication(
                                logic::Formulas::conjunction({
                                    logic::Theory::natSubEq(boundL, it),
                                    logic::Theory::natSub(it, boundR)
                                }),
                                predicateFunctor(
                                    v->isArray ? toTerm(v,lStartIt, pos) : toTerm(v,lStartIt),
                                    v->isArray ? toTerm(v,lStartSuccOfIt, pos) : toTerm(v,lStartSuccOfIt),
                                    ""
                                )
                            )
                        );
                    
                    // Conclusion: forall it. (boundL<=it<=boundR => IH(it))
                    auto conclusion =
                        logic::Formulas::universal({it->symbol},
                            logic::Formulas::implication(
                                logic::Formulas::conjunction({
                                    logic::Theory::natSubEq(boundL, it),
                                    logic::Theory::natSubEq(it, boundR)
                                }),
                                inductionHypothesis(it)
                            )
                        );
                    
                    // forall enclosingIterators: forall boundL,boundR. (Premise => Conclusion) or
                    // forall enclosingIterators: forall boundL,boundR. forall pos. (Premise => Conclusion)
                    auto outerImp = logic::Formulas::implication(premise, conclusion);
                    auto universal = v->isArray ? logic::Formulas::universal({posSymbol}, outerImp) : outerImp;
                    auto bareLemma =
                        logic::Formulas::universal(enclosingIteratorsSymbols(whileStatement),
                            logic::Formulas::universal({boundLSymbol, boundRSymbol}, universal)
                        );
                    
                    if (twoTraces)
                    {
                        auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        bareLemma = logic::Formulas::universal({tr}, bareLemma);
                    }
                    
                    auto name = "value-evolution-" + predicateString + "-" + v->name + "-" + whileStatement->location;
                    items.push_back(std::make_shared<logic::Lemma>(bareLemma, name));
                }
            }
        }
    }
 
    void StaticAnalysisLemmas::generateOutputFor(const program::WhileStatement *statement, std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        auto itSymbol = iteratorSymbol(statement);
        auto it = iteratorTermForLoop(statement);
        auto n = lastIterationTermForLoop(statement, twoTraces);

        auto lStartIt = timepointForLoopStatement(statement, it);
        auto lStartZero = timepointForLoopStatement(statement, logic::Theory::natZero());
        auto lStartN = timepointForLoopStatement(statement, n);

        auto posSymbol = logic::Signature::varSymbol("pos", logic::Sorts::intSort());
        auto pos = logic::Terms::var(posSymbol);
        auto trSymbol= logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
        
        auto activeVars = locationToActiveVars.at(statement->location);
        auto assignedVars = computeAssignedVars(statement);
        
        // for each active var, which is not constant but not assigned to in any statement of the loop,
        // add a lemma asserting that var is the same in each iteration as in the first iteration.
        for (const auto& v : activeVars)
        {
            if (!v->isConstant && assignedVars.count(v) == 0)
            {
                // IH(it): v(l(it1,...,itk,zero)    ) = v(l(it1,...,itk,it)    ) or
                //         v(l(it1,...,itk,zero),pos) = v(l(it1,...,itk,it),pos)
                auto inductionHypothesis = [&](std::shared_ptr<const logic::Term> arg)
                {
                    auto lStartArg = timepointForLoopStatement(statement, arg);
                    return
                    logic::Formulas::equality(
                                              v->isArray ? toTerm(v, lStartZero, pos) : toTerm(v,lStartZero),
                                              v->isArray ? toTerm(v, lStartArg, pos) : toTerm(v,lStartArg)
                                              );
                };

                // PART 1: Add induction-axiom
                auto inductionAxiom = logic::inductionAxiom1(inductionHypothesis);
                if (v->isArray)
                {
                    inductionAxiom = logic::Formulas::universal({posSymbol}, inductionAxiom);
                }
                if (twoTraces)
                {
                    inductionAxiom = logic::Formulas::universal({trSymbol}, inductionAxiom);
                }
                
                auto axiomName = "value-static-axiom-" + v->name + "-" + statement->location;
                items.push_back(std::make_shared<logic::Axiom>(inductionAxiom, axiomName));
                
                // PART 2: Add trace lemma
                // forall enclosing iterators. forall it. {forall pos.} (it<=n => IH(it))
                auto implication =
                    logic::Formulas::implication(
                        logic::Theory::natSubEq(it, n),
                        inductionHypothesis(it)
                    );
                auto bareLemma =
                    logic::Formulas::universal(enclosingIteratorsSymbols(statement),
                        v->isArray ?
                            logic::Formulas::universal({itSymbol, posSymbol}, implication) :
                            logic::Formulas::universal({itSymbol}, implication)
                    );

                if (twoTraces)
                {
                    bareLemma = logic::Formulas::universal({trSymbol}, bareLemma);
                }
                
                auto name = "value-static-" + v->name + "-" + statement->location;
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
