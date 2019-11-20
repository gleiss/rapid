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

                    auto nameSuffix = "-" + predicateString + "-" + v->name + "-" + whileStatement->location;
                    auto name = "value-evolution" + nameSuffix;
                    auto nameShort = "Evol" + nameSuffix;
                    auto inductionAxiomName = "induction-axiom-" + name;
                    auto inductionAxiomNameShort = "Ax" + nameShort;

                    // PART 1: Add induction-axiom
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

                    auto freeVars = enclosingIteratorsSymbols(whileStatement);
                    if (v->isArray)
                    {
                        freeVars.push_back(posSymbol);
                    }
                    if (twoTraces)
                    {
                        auto trSymbol = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                        freeVars.push_back(trSymbol);
                    }

                    auto [inductionAxBCDef, inductionAxICDef,inductionAxiomConDef, inductionAxiom] = inductionAxiom1(inductionAxiomName, inductionAxiomNameShort, inductionHypothesis, freeVars);
                    items.push_back(inductionAxBCDef);
                    items.push_back(inductionAxICDef);
                    items.push_back(inductionAxiomConDef);
                    items.push_back(inductionAxiom);

                    // PART 2: Add trace lemma
                    auto argSymbols = freeVars;
                    argSymbols.push_back(boundLSymbol);
                    argSymbols.push_back(boundRSymbol);

                    std::vector<std::shared_ptr<const logic::Term>> args = {};
                    for (const auto& symbol : argSymbols)
                    {
                        args.push_back(logic::Terms::var(symbol));
                    }

                    // Part 2A: Add definition for premise:
                    auto premise = logic::Formulas::predicate("Prem" + nameShort, args);
                    // forall it. (boundL<=it<boundR => v(l(it1,...,itk,it)    ) C v(l(it1,...,itk,s(it))    )), where C in {=,<=,>=} or
                    // forall it. (boundL<=it<boundR => v(l(it1,...,itk,it),pos) C v(l(it1,...,itk,s(it)),pos)), where C in {=,<=,>=}
                    auto premiseFormula =
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
                    auto premiseDef = 
                        std::make_shared<logic::Definition>(
                            logic::Formulas::universal(argSymbols,
                                logic::Formulas::equivalence(
                                    premise,
                                    premiseFormula
                                )
                            ),
                            "Premise for " + name, 
                            logic::ProblemItem::Visibility::Implicit
                        );
                    items.push_back(premiseDef);

                    // Part 2B: Define lemma:
                    //forall it. (boundL<=it<=boundR => IH(it))
                    auto conclusionFormula =
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
                    auto lemma =
                        logic::Formulas::universal(argSymbols,
                            logic::Formulas::implication(premise, conclusionFormula)
                        );
                    std::vector<std::shared_ptr<const logic::ProblemItem>> fromItems = {inductionAxBCDef, inductionAxICDef, inductionAxiomConDef, inductionAxiom, premiseDef};
                    items.push_back(std::make_shared<logic::Lemma>(lemma, name, logic::ProblemItem::Visibility::Implicit, fromItems));
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
                auto nameSuffix = "-" + v->name + "-" + statement->location;
                auto name = "value-static" + nameSuffix;
                auto nameShort = "Static" + nameSuffix;
                auto inductionAxiomName = "induction-axiom-" + name;
                auto inductionAxiomNameShort = "Ax" + nameShort;

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
                auto freeVars = enclosingIteratorsSymbols(statement);
                if (v->isArray)
                {
                    freeVars.push_back(posSymbol);
                }
                if (twoTraces)
                {
                    freeVars.push_back(trSymbol);
                }

                auto [inductionAxBCDef, inductionAxICDef,inductionAxiomConDef, inductionAxiom] = logic::inductionAxiom1(inductionAxiomName, inductionAxiomNameShort, inductionHypothesis, freeVars);
                items.push_back(inductionAxBCDef);
                items.push_back(inductionAxICDef);
                items.push_back(inductionAxiomConDef);
                items.push_back(inductionAxiom);

                // PART 2: Add trace lemma
                auto argSymbols = freeVars;
                argSymbols.push_back(itSymbol);
                
                // forall enclosing iterators. {forall pos.} forall it. (it<=n => IH(it))
                auto lemma =
                    logic::Formulas::universal(argSymbols,
                        logic::Formulas::implication(
                            logic::Theory::natSubEq(it, n),
                            inductionHypothesis(it)
                        )
                    );

                std::vector<std::shared_ptr<const logic::ProblemItem>> fromItems = {inductionAxBCDef, inductionAxICDef, inductionAxiomConDef, inductionAxiom};
                for (auto& item : programSemantics)
                {
                    fromItems.push_back(item);
                }
                
                items.push_back(std::make_shared<logic::Lemma>(lemma, name, logic::ProblemItem::Visibility::Implicit, fromItems));
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
