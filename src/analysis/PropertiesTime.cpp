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
                generateStandardInductionLemmas(statement.get(), function->vars, lemmas);
            }
        }
    }

    void TraceLemmas::generateStandardInductionLemmas(const program::Statement* statement,
                                             const std::vector<std::shared_ptr<const program::Variable>>& vars,
                                             std::vector<std::shared_ptr<const logic::Formula>>& lemmas)
{
    if (statement->type() == program::Statement::Type::IfElse)
    {
        auto castedStatement = static_cast<const program::IfElse*>(statement);
        // recurse on both branches
        for (const auto& statement : castedStatement->ifStatements)
        {
            generateStandardInductionLemmas(statement.get(), vars, lemmas);
        }
        for (const auto& statement : castedStatement->elseStatements)
        {
            generateStandardInductionLemmas(statement.get(), vars, lemmas);
        }
    }
    else if (statement->type() == program::Statement::Type::WhileStatement)
    {
        auto castedStatement = static_cast<const program::WhileStatement*>(statement);
        // generate lemmas
        generateStandardInductionLemmas(castedStatement, vars, lemmas, InductionKind::Equal);
        generateStandardInductionLemmas(castedStatement, vars, lemmas, InductionKind::Less);
        generateStandardInductionLemmas(castedStatement, vars, lemmas, InductionKind::Greater);
        generateStandardInductionLemmas(castedStatement, vars, lemmas, InductionKind::LessEqual);
        generateStandardInductionLemmas(castedStatement, vars, lemmas, InductionKind::GreaterEqual);
        
        // recurse on body
        for (const auto& statement : castedStatement->bodyStatements)
        {
            generateStandardInductionLemmas(statement.get(), vars, lemmas);
        }
    }
}
    
    void TraceLemmas::generateStandardInductionLemmas(const program::WhileStatement* whileStatement,
                                                      const std::vector<std::shared_ptr<const program::Variable>>& vars,
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
        for (const auto& v : vars)
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
        for (const auto& v : vars)
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
  
//    
//    void PropertiesTime::analyze()
//    {        
//        constnessProps();
//        monotonicityProps();
//        
//        intermediateTheoremProps();
//        
//        updatePredicatesOfArrays();
//        loopConditionHypothesis();
//    }
//    
//#pragma mark - General Properties
//    
//    // represents induction on = for loops
//    void PropertiesTime::constnessProps()
//    {
//        for (const auto& var : _vars)
//        {
//            if (!_updated.at(var))
//            {
//                auto it = Terms::lVariable(Sorts::timeSort(), "It");
//                
//                std::shared_ptr<const Formula> eq;
//                // eq(it) := x(it) = x(0)
//                if (!isArrayType(var->type))
//                {
//                    Symbol* var0Symbol = Signature::fetchOrDeclare(var->name+"0", toSort(var->type));
//                    auto var0 = Terms::funcTerm(var0Symbol, {});
//                    
//                    eq = Formulas::equalityFormula(true,
//                                             var->toTerm(it),
//                                             var0);
//                }
//                // eq(i) := forall p. x(i,p) = x(0,p)
//                else
//                {
//                    // suppport for other options not implemented yet
//                    assert(!util::Configuration::instance().arrayTheory().getValue());
//                    
//                    auto p = Terms::lVariable(Sorts::intSort(), "P");
//                    
//                    Symbol* var0Symbol = Signature::fetchOrDeclare(var->name+"0", { Sorts::intSort() }, toSort(var->type));
//                    auto var0 = Terms::funcTerm(var0Symbol, {p});
//                    
//                    auto eqWithoutQuantifiers = Formulas::equalityFormula(true,
//                                                                        var->toTerm(it, p),
//                                                                        var0);
//                    eq = Formulas::universalFormula({p}, eqWithoutQuantifiers);
//                }
//                
//                // forall i. eq(i)
//                auto f = Formulas::universalFormula({it}, eq);
//                
//                // add property
//                addProperty("not_updated_" + var->name, f);
//            }
//        }
//    }
//    
//    /*
//     * Constness prop for array
//     */
//    void PropertiesTime::constnessPropsOfArrays()
//    {
//        for (const auto& v : _vars)
//        {
//            // only check updates array variables
//            if (!isArrayType(v->type) || !_updated.at(v))
//                continue;
//            
//            if (util::Configuration::instance().existentialAxioms().getValue()) {
//                // these axioms introduce skolem symbols
//                addProperty("stability_" + v->name, constnessArrayProp(v));
//            }
//        }
//    }
//    
//    /** forall p, (forall i, iter(i) => !update_a(i, p)) => a(n, p) = a(0, p) */
//    std::shared_ptr<const logic::Formula> PropertiesTime::constnessArrayProp(const PVariable *a)
//    {
//        assert(isArrayType(a->type));
//        assert(_updated.at(a));
//        
//        auto p = Terms::lVariable(Sorts::intSort(), "P");
//        auto i = Terms::lVariable(Sorts::timeSort(), "It");
//        
//        auto fa = Formulas::implicationFormula(iter(i),
//                                               Formulas::negationFormula(arrayUpdatePredicate(a, i, p, nullptr)));
//        
//        auto fb = Formulas::universalFormula( {i}, fa);
//        auto fc = Formulas::equalityFormula(true,
//                                            a->toTerm(Theory::timeZero(), p),
//                                            a->toTerm(loopCounterSymbol(), p));
//        
//        return Formulas::universalFormula( {p}, Formulas::implicationFormula(fb, fc));
//    }
//    
//    
//#pragma mark - Monotonicity Properties
//    
//    /*
//     * Monotonicity propreties
//     * represents induction on <=, >=, < and > for loops
//     */
//    void PropertiesTime::monotonicityProps()
//    {
//        for (auto it = _vars.begin(); it != _vars.end(); ++it)
//        {
//            const PVariable *v = (*it);
//            if(_monotonic.at(v) == Monotonicity::OTHER)
//                continue;
//            
//            if (_strict.at(v))
//            {
//                addProperty("injectivity_" + v->name, injectivityProp(v)); // TODO: this is at least logically redundant (and also not a monotonicity property)
//                addProperty("strict_" + v->name, strictProp(v));
//            }
//            else
//            {
//                addProperty("non_strict_" + v->name, nonStrictProp(v));
//            }
//        }
//    }
//    
//    /** forall i j. (i != j => v(i) != v(j) ) */
//    std::shared_ptr<const logic::Formula> PropertiesTime::injectivityProp(const PVariable *v)
//    {
//        assert(_monotonic.at(v) != Monotonicity::OTHER);
//        assert(_updated.at(v));
//        assert(_strict.at(v));
//        
//        auto i = Terms::lVariable(Sorts::timeSort(), "It1");
//        auto j = Terms::lVariable(Sorts::timeSort(), "It2");
//        
//        auto imp = Formulas::implicationFormula(Formulas::equalityFormula(false, i, j),
//                               Formulas::equalityFormula(false, v->toTerm(i), v->toTerm(j)));
//        return Formulas::universalFormula({i,j}, imp);
//    }
//    
//    /** forall i j. (i < j => v(i) < v(j)) [v(j) < v(i) if v is decreasing] */
//    std::shared_ptr<const logic::Formula> PropertiesTime::strictProp(const PVariable *v)
//    {
//        assert(_updated.at(v));
//        assert(_monotonic.at(v) != Monotonicity::OTHER);
//        assert(_strict.at(v));
//        
//        auto i = Terms::lVariable(Sorts::timeSort(), "It1");
//        auto j = Terms::lVariable(Sorts::timeSort(), "It2");
//        
//        if (_monotonic.at(v) == Monotonicity::INC)
//        {
//
//            auto imp = Formulas::implicationFormula(Formulas::predicateFormula(Theory::timeSub(i, j)),
//                                                    Formulas::predicateFormula(Terms::predTerm(Theory::getSymbol(InterpretedSymbol::INT_LESS),{ v->toTerm(i), v->toTerm(j) })));
//            return Formulas::universalFormula({i,j}, imp);
//        }
//        else
//        {
//            auto imp = Formulas::implicationFormula(Formulas::predicateFormula(Theory::timeSub(i, j)),
//                                                    Formulas::predicateFormula(Terms::predTerm(Theory::getSymbol(InterpretedSymbol::INT_LESS),{ v->toTerm(j), v->toTerm(i) })));
//            return Formulas::universalFormula({i,j}, imp);
//        }
//    }
//    
//    /** forall i j. (i<j => v(i)<=v(j)) [v(j)<=v(i) if v is decreasing] */
//    std::shared_ptr<const logic::Formula> PropertiesTime::nonStrictProp(const PVariable *v)
//    {
//        assert(_updated.at(v));
//        assert(_monotonic.at(v) != Monotonicity::OTHER);
//        assert(!_strict.at(v));
//        
//        auto i = Terms::lVariable(Sorts::timeSort(), "It1");
//        auto j = Terms::lVariable(Sorts::timeSort(), "It2");
//        
//        if (_monotonic.at(v) == Monotonicity::INC)
//        {
//            auto imp = Formulas::implicationFormula(Formulas::predicateFormula(Theory::timeSub(i, j)),
//                                                    Formulas::predicateFormula(Terms::predTerm(Theory::getSymbol(InterpretedSymbol::INT_LESS_EQUAL),{ v->toTerm(i), v->toTerm(j) })));
//            return Formulas::universalFormula({i,j}, imp);
//        }
//        else
//        {
//            auto imp = Formulas::implicationFormula(Formulas::predicateFormula(Theory::timeSub(i, j)),
//                                                    Formulas::predicateFormula(Terms::predTerm(Theory::getSymbol(InterpretedSymbol::INT_LESS_EQUAL),{ v->toTerm(j), v->toTerm(i) })));
//            return Formulas::universalFormula({i,j}, imp);
//        }
//    }
//    
//#pragma mark - Intermediate theorems
//
//    void PropertiesTime::intermediateTheoremProps()
//    {
//        for (auto it = _vars.begin(); it != _vars.end(); ++it)
//        {
//            const PVariable *v = (*it);
//            addProperty("update_" + v->name, intermediateTheoremForVar(v));
//        }
//    }
//    
//    /*
//     * Update properties of guarded assignments
//     */
//    
//    /** This property indicates that if a monotonic variable has been
//     *  modified, then there exists a program point at which conditions
//     *  for this change have been enabled.
//     *
//     *  forall x, (x >= v(0) & v(n) > x => exists i, DISJ(u) (G_u(i) & pred))
//     *  where if v is dense pred <=> v(i) = x
//     *  and otherwise       pred <=> x >= v(i) & v(s(i)) > x [resp. <=, < if decreasing]
//     */
//    std::shared_ptr<const logic::Formula> PropertiesTime::intermediateTheoremForVar(const PVariable *v)
//    {
//        assert(_updated.at(v));
//        assert(_monotonic.at(v) != Monotonicity::OTHER);
//        
//        auto x = Terms::lVariable(Sorts::intSort(), "X");
//        auto i = Terms::lVariable(Sorts::timeSort(), "It");
//        auto succOfI = Theory::timeSucc(i);
//        
//        InterpretedSymbol geOrLe = (_monotonic.at(v) == Monotonicity::INC
//                                    ? InterpretedSymbol::INT_GREATER_EQUAL
//                                    : InterpretedSymbol::INT_LESS_EQUAL);
//        InterpretedSymbol gtOrLt = (_monotonic.at(v) == Monotonicity::INC
//                                    ? InterpretedSymbol::INT_GREATER
//                                    : InterpretedSymbol::INT_LESS);
//        
//        // build the disjunction
//        std::vector<std::shared_ptr<const Formula>> disj;
//        for (const auto& command : _loop.commands)
//        {
//            // only take into account commands that do affect v
//            if (!command->findAssignment(*v))
//                continue;
//            
//            std::vector<std::shared_ptr<const Formula>> conj { command->guard->toFormula(i) } ;
//            if (_dense.at(v))
//            {
//                conj.push_back(Formulas::equalityFormula(true, v->toTerm(i), x));
//            }
//            else
//            {
//                conj.push_back(Formulas::predicateFormula(Terms::predTerm(Theory::getSymbol(geOrLe),
//                                                                          { x, v->toTerm(i) })));
//                conj.push_back(Formulas::predicateFormula(Terms::predTerm(Theory::getSymbol(gtOrLt),
//                                                                          { v->toTerm(succOfI), x })));
//            }
//            disj.push_back(Formulas::conjunctionFormula(conj));
//        }
//        
//        // since v is monotonic, there should be at least one guard that updates it
//        assert(disj.size() > 0);
//        
//        auto f = Formulas::conjunctionFormula( { iter(i), Formulas::disjunctionFormula(disj) } );
//        
//        auto p1 = Terms::predTerm(Theory::getSymbol(InterpretedSymbol::INT_GREATER_EQUAL),
//                                  { x, v->toTerm(Theory::timeZero()) });
//        auto p2 = Terms::predTerm(Theory::getSymbol(InterpretedSymbol::INT_GREATER),
//                                  { v->toTerm(loopCounterSymbol()), x });
//        std::vector<std::shared_ptr<const Formula>> conds { Formulas::predicateFormula(p1), Formulas::predicateFormula(p2) };
//        
//        return Formulas::universalFormula( { x },Formulas::implicationFormula(Formulas::conjunctionFormula(conds),
//                                                                              Formulas::existentialFormula( { i } , f)));
//    }
//    
//#pragma mark - Update predicates of arrays
//    
////    void PropertiesTime::uniqueUpdateAxioms()
////    {
////        for (const auto& v : _vars)
////        {
////            // only check updates array variables
////            if (!isArrayType(v->type) || !_updated.at(v))
////                continue;
////
////            if (util::Configuration::instance().existentialAxioms().getValue()) {
////                // these axioms introduce skolem symbols
////                addProperty("unique_update_" + v->name, uniqueUpdateAxiom(v));
////            }
////        }
////    }
////
////    /** forall i p v, (iter(i) & update_a(i, p, v) & (forall j, iter(j) & j != i => !update_a(j, p))) => a(n, p) = v */
////    /* this property is only useful if the array is written at most once by the loop! */
////    std::shared_ptr<const logic::Formula> PropertiesTime::uniqueUpdateAxiom(const PVariable *a)
////    {
////        assert(isArrayType(a->type));
////        assert(_updated.at(a));
////
////        auto i = Terms::lVariable(Sorts::timeSort(), "It1");
////        auto j = Terms::lVariable(Sorts::timeSort(), "It2");
////        auto p = Terms::lVariable(Sorts::intSort(), "P");
////        auto v = Terms::lVariable(toSort(a->type), "V");
////
////        auto fa = Formulas::implicationFormula(Formulas::conjunctionFormula({ Formulas::equalityFormula(false, i,j), iter(j) }),
////                                             Formulas::negationFormula(arrayUpdatePredicate(a, j, p, nullptr)));
////        auto fb = Formulas::conjunctionFormula(
////                                               { Formulas::universalFormula({j}, fa),
////                                                 arrayUpdatePredicate(a, i, p, v),
////                                                 iter(i) }
////                                             );
////
////        auto fc = Formulas::equalityFormula(true,
////                                          v,
////                                          a->toTerm(loopCounterSymbol(), p));
////
////        auto imp = Formulas::implicationFormula(fb, fc);
////        return Formulas::universalFormula({i, p, v}, imp);
////    }
//    
//    // if v is nullptr, updatePredicate with 2 args
//    std::shared_ptr<const logic::Formula> PropertiesTime::arrayUpdatePredicate(const PVariable *a,
//                                              std::shared_ptr<const logic::Term> i,
//                                              std::shared_ptr<const logic::Term> p,
//                                              std::shared_ptr<const logic::Term> v)
//    {
//        std::vector<std::shared_ptr<const logic::Formula>> disj {};
//        
//        for(const auto& command : _loop.commands)
//        {
//            for (const auto& assignment : command->assignments)
//            {
//                if (assignment->hasLhs(*a))
//                {
//                    disj.push_back(arrayAssignmentConditions(assignment, command->guard, i, p, v));
//                }
//            }
//        }
//        
//        // a is updated, this shouldn't be empty
//        assert(!disj.empty());
//        
//        return Formulas::disjunctionFormula(disj);
//    }
//    
//    std::shared_ptr<const logic::Formula> PropertiesTime::arrayAssignmentConditions(const Assignment *asg,
//                                                   const FExpression *guard,
//                                                   std::shared_ptr<const logic::Term> i,
//                                                   std::shared_ptr<const logic::Term> p,
//                                                   std::shared_ptr<const logic::Term> v)
//    {
//        std::vector<std::shared_ptr<const logic::Formula>> conj;
//        conj.push_back(guard->toFormula(i));
//        conj.push_back(Formulas::equalityFormula(true,
//                                           p,
//                                           asg->lhs->child(0)->toTerm(i)));
//        if (v)
//            conj.push_back(Formulas::equalityFormula(true,
//                                               v,
//                                               asg->rhs->toTerm(i)));
//        
//        return Formulas::conjunctionFormula(conj);
//    }
//    
//#pragma mark - loop condition properties
//    
//    // forall i, iter(i) => cond(i)
//    void PropertiesTime::loopConditionHypothesis()
//    {
//        auto i = Terms::lVariable(Sorts::timeSort(), "It");
//        
//        addProperty("loop_condition", Formulas::universalFormula({i},
//                                                                 Formulas::implicationFormula(iter(i),
//                                                                                  _loop.loopCondition->toFormula(i))));
//    }
//    
//    //    std::shared_ptr<const FuncTerm> PropertiesTime::loopCounterSymbol()
//    //    {
//    //        // initialization note that the syntax of the guarded command
//    //        // language does not allow special characters such as $
//    //        Symbol* s = Signature::fetchOrDeclare("$n", Sorts::timeSort(), false, true);
//    //        return Terms::funcTerm(s, {});
//    //    }
//    //
//    //    // iter(i) := i < n
//    //    // we use i < n instead of 0 <= i < n, since for our term algebra, 0 <= i for any i.
//    //    std::shared_ptr<const Formula> PropertiesTime::iter(std::shared_ptr<const Term> i)
//    //    {
//    //        return Formulas::predicateFormula(Theory::timeSub(i, loopCounterSymbol()));
//    //    }
//
}


