#include "SymbolDeclarations.hpp"
#include "SemanticsInliner.hpp"
#include "SemanticsHelper.hpp"
#include "Theory.hpp"

#include <cassert>

namespace analysis
{
    void SemanticsInliner::computePersistentTerms(std::vector<std::shared_ptr<const logic::ProblemItem>>& problemItems)
    {
        for (auto item : problemItems)
        {
            computePersistentTermsRec(item->formula);
        }
    }

    void SemanticsInliner::computePersistentTermsRec(std::shared_ptr<const logic::Formula> f)
    {
        switch (f->type())
        {
            case logic::Formula::Type::Predicate:
            {
                auto castedFormula = std::static_pointer_cast<const logic::PredicateFormula>(f);
                for (const auto& subterm : castedFormula->subterms)
                {
                    computePersistentTermsRec(subterm);
                }
                break;
            }
            case logic::Formula::Type::Equality:
            {
                auto castedFormula = std::static_pointer_cast<const logic::EqualityFormula>(f);
                computePersistentTermsRec(castedFormula->left);
                computePersistentTermsRec(castedFormula->right);
                break;
            }
            case logic::Formula::Type::Conjunction:
            {
                auto castedFormula = std::static_pointer_cast<const logic::ConjunctionFormula>(f);
                for (const auto& subterm : castedFormula->conj)
                {
                    computePersistentTermsRec(subterm);
                }
                break;
            }
            case logic::Formula::Type::Disjunction:
            {
                auto castedFormula = std::static_pointer_cast<const logic::DisjunctionFormula>(f);
                for (const auto& subterm : castedFormula->disj)
                {
                    computePersistentTermsRec(subterm);
                }
                break;
            }
            case logic::Formula::Type::Negation:
            {
                auto castedFormula = std::static_pointer_cast<const logic::NegationFormula>(f);
                computePersistentTermsRec(castedFormula->f);
                break;
            }
            case logic::Formula::Type::Existential:
            {
                auto castedFormula = std::static_pointer_cast<const logic::ExistentialFormula>(f);
                computePersistentTermsRec(castedFormula->f);
                break;
            }
            case logic::Formula::Type::Universal:
            {
                auto castedFormula = std::static_pointer_cast<const logic::UniversalFormula>(f);
                computePersistentTermsRec(castedFormula->f);
                break;
            }
            case logic::Formula::Type::Implication:
            {
                auto castedFormula = std::static_pointer_cast<const logic::ImplicationFormula>(f);
                computePersistentTermsRec(castedFormula->f1);
                computePersistentTermsRec(castedFormula->f2);
                break;
            }
            case logic::Formula::Type::Equivalence:
            {
                auto castedFormula = std::static_pointer_cast<const logic::EquivalenceFormula>(f);
                computePersistentTermsRec(castedFormula->f1);
                computePersistentTermsRec(castedFormula->f2);
                break;
            }
            case logic::Formula::Type::True:
            {
                break;
            }
            case logic::Formula::Type::False:
            {
                break;
            }
            default:
            {
                assert(false);
                break;
            }
        }
    }

    void SemanticsInliner::computePersistentTermsRec(std::shared_ptr<const logic::Term> t)
    {
        switch (t->type())
        {
            case logic::Term::Type::Variable:
            {
                // do nothing
                break;
            }
            case logic::Term::Type::FuncTerm:
            {
                auto castedTerm = std::static_pointer_cast<const logic::FuncTerm>(t);

                // handle term: search for a term of sort Int, which has a subterm of sort Location
                // TODO: could make this check more precise, so that it always only detects terms v(l(...)), where v is a variable occuring in the program and l is a location of the program
                if (castedTerm->symbol->rngSort == logic::Sorts::intSort())
                {
                    // check whether castedTerm could denote mutable program variable
                    if (castedTerm->subterms.size() >= 1 && castedTerm->subterms[0]->symbol->rngSort == logic::Sorts::timeSort())
                    {
                        auto programVarName = castedTerm->symbol->name;
                        auto timepointName = castedTerm->subterms[0]->symbol->name;

                        if (persistentVarTerms.find(programVarName) == persistentVarTerms.end())
                        {
                            persistentVarTerms[programVarName] = {timepointName};
                        }
                        else
                        {
                            persistentVarTerms[programVarName].push_back(timepointName);
                        }
                    } 
                    else 
                    {
                        // check whether castedTerm could denote constant program variable
                        // TODO: could make this check more precise, currently just ensure that no subterm is of sort time.
                        bool noTimepointSubterms = true;
                        for (const auto& subterm : castedTerm->subterms)
                        {
                            if (subterm->symbol->rngSort == logic::Sorts::timeSort())
                            {
                                noTimepointSubterms = false;
                                break;
                            }
                        }
                        if (noTimepointSubterms)
                        {
                            auto programVarName = castedTerm->symbol->name;
                            if (programVarName != "0" && programVarName != "1" && programVarName != "-")
                            {
                                persistentConstVarTerms.insert(programVarName);
                            }
                        }
                    }
                }

                // recurse on subterms
                for (const auto& subterm : castedTerm->subterms)
                {
                    computePersistentTermsRec(subterm);
                }
                break;
            }
            default:
            {
                assert(false);
                break;
            }
        }
    }

    std::shared_ptr<const logic::Term> SemanticsInliner::toCachedTermFull(std::shared_ptr<const program::Variable> var)
    {
        assert(var != nullptr);
        assert(!var->isArray);
        assert(currTimepoint != nullptr);

        // if no value is cached yet, initialize cache (note that we use a free variable as trace (which has to be universally quantified later))
        if(cachedIntVarValues.find(var) == cachedIntVarValues.end())
        {
            cachedIntVarValues[var] = toTerm(var, currTimepoint, trace);
        }
        // return cached value
        return cachedIntVarValues.at(var);
    }

    std::shared_ptr<const logic::Term> SemanticsInliner::toCachedTermFull(std::shared_ptr<const program::Variable> arrayVar, std::shared_ptr<const logic::Term> position)
    {
        assert(arrayVar != nullptr);
        assert(position != nullptr);
        assert(arrayVar->isArray);
        assert(currTimepoint != nullptr);

        if(cachedArrayVarTimepoints.find(arrayVar) == cachedArrayVarTimepoints.end())
        {
            cachedArrayVarTimepoints[arrayVar] = currTimepoint;
        }
        auto cachedTimepoint = cachedArrayVarTimepoints.at(arrayVar);
        return toTerm(arrayVar, cachedTimepoint, position, trace);
    }

    std::shared_ptr<const logic::Term> SemanticsInliner::toCachedTerm(std::shared_ptr<const program::IntExpression> expr)
    {
        assert(expr != nullptr);

        switch (expr->type())
        {
            case program::IntExpression::Type::ArithmeticConstant:
            {
                auto castedExpr = std::static_pointer_cast<const program::ArithmeticConstant>(expr);
                return logic::Theory::intConstant(castedExpr->value);
            }
            case program::IntExpression::Type::Addition:
            {
                auto castedExpr = std::static_pointer_cast<const program::Addition>(expr);
                return logic::Theory::intAddition(toCachedTerm(castedExpr->summand1), toCachedTerm(castedExpr->summand2));
            }
            case program::IntExpression::Type::Subtraction:
            {
                auto castedExpr = std::static_pointer_cast<const program::Subtraction>(expr);
                return logic::Theory::intSubtraction(toCachedTerm(castedExpr->child1), toCachedTerm(castedExpr->child2));
            }
            case program::IntExpression::Type::Modulo:
            {
                auto castedExpr = std::static_pointer_cast<const program::Modulo>(expr);
                return logic::Theory::intModulo(toCachedTerm(castedExpr->child1), toCachedTerm(castedExpr->child2));
            }
            case program::IntExpression::Type::Multiplication:
            {
                auto castedExpr = std::static_pointer_cast<const program::Multiplication>(expr);
                return logic::Theory::intMultiplication(toCachedTerm(castedExpr->factor1), toCachedTerm(castedExpr->factor2));
            }
            case program::IntExpression::Type::IntVariableAccess:
            {
                auto castedExpr = std::static_pointer_cast<const program::IntVariableAccess>(expr);
                return toCachedTermFull(castedExpr->var);
            }
            case program::IntExpression::Type::IntArrayApplication:
            {
                auto castedExpr = std::static_pointer_cast<const program::IntArrayApplication>(expr);
                return toCachedTermFull(castedExpr->array, toCachedTerm(castedExpr->index));
            }
        }
    }

    std::shared_ptr<const logic::Formula> SemanticsInliner::toCachedFormula(std::shared_ptr<const program::BoolExpression> expr)
    {
        assert(expr != nullptr);

        switch (expr->type())
        {
            case program::BoolExpression::Type::BooleanConstant:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanConstant>(expr);
                return castedExpr->value ? logic::Theory::boolTrue() : logic::Theory::boolFalse();
            }
            case program::BoolExpression::Type::BooleanAnd:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanAnd>(expr);
                return logic::Formulas::conjunction({toCachedFormula(castedExpr->child1), toCachedFormula(castedExpr->child2)});
            }
            case program::BoolExpression::Type::BooleanOr:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanOr>(expr);
                return logic::Formulas::disjunction({toCachedFormula(castedExpr->child1), toCachedFormula(castedExpr->child2)});
            }
            case program::BoolExpression::Type::BooleanNot:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanNot>(expr);
                return logic::Formulas::negation(toCachedFormula(castedExpr->child));
            }
            case program::BoolExpression::Type::ArithmeticComparison:
            {
                auto castedExpr = std::static_pointer_cast<const program::ArithmeticComparison>(expr);
                switch (castedExpr->kind)
                {
                    case program::ArithmeticComparison::Kind::GT:
                        return logic::Theory::intGreater(toCachedTerm(castedExpr->child1), toCachedTerm(castedExpr->child2));
                    case program::ArithmeticComparison::Kind::GE:
                        return logic::Theory::intGreaterEqual(toCachedTerm(castedExpr->child1), toCachedTerm(castedExpr->child2));
                    case program::ArithmeticComparison::Kind::LT:
                        return logic::Theory::intLess(toCachedTerm(castedExpr->child1), toCachedTerm(castedExpr->child2));
                    case program::ArithmeticComparison::Kind::LE:
                        return logic::Theory::intLessEqual(toCachedTerm(castedExpr->child1), toCachedTerm(castedExpr->child2));
                    case program::ArithmeticComparison::Kind::EQ:
                        return logic::Formulas::equality(toCachedTerm(castedExpr->child1), toCachedTerm(castedExpr->child2));
                }
            }
        }
    }

    std::shared_ptr<const logic::Formula> SemanticsInliner::handlePersistence(std::shared_ptr<const logic::Term> timepoint, const std::vector<std::shared_ptr<const program::Variable>>& activeVars, std::string label)
    {
        // define persistent terms for non-const variables
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;
        for (const auto& var : activeVars)
        {
            if (!var->isConstant)
            {
                // check whether value of variable term should be defined
                bool varTermShouldBeDefined = false;
                if (persistentVarTerms.find(var->name) != persistentVarTerms.end())
                {
                    for (const auto& locationName : persistentVarTerms[var->name])
                    {
                        if (locationName == timepoint->symbol->name)
                        {
                            // we have now established that var(timepoint,...) is referenced, so we need to define it
                            varTermShouldBeDefined = true;
                            break;
                        }
                    }
                }

                if (varTermShouldBeDefined)
                {
                    if (!var->isArray)
                    {
                        auto currValue = toTerm(var, timepoint, trace);

                        // if we already know a value for the variable
                        if (cachedIntVarValues.find(var) != cachedIntVarValues.end())
                        {
                            auto cachedValue = cachedIntVarValues[var];

                            // add formula
                            auto f = logic::Formulas::equalitySimp(currValue, cachedValue);
                            conjuncts.push_back(f);
                        }
                        else
                        {
                            // set currValue as cachedValue, so that other references share the same cachedValue
                            // this edge case does matter e.g. in the following edge case:
                            // a variable x exists at locations l1 and l2 but the first assignment to x only appears in l3, and we want to prove the conjecture x(l1)=x(l2).
                            // using this branch of execution, we set the cached value of x at location l1 to x(l1). As soon as we visit location l2, the equality x(l1)=x(l2) is then added to the semantics, which enables us to prove the property.
                            // we can interpret this edge case as covering the fact that uninitialized memory stays the same if no assigment occurs.
                            cachedIntVarValues[var] = currValue;
                        }
                    }
                    else
                    {
                        if (cachedArrayVarTimepoints.find(var) != cachedArrayVarTimepoints.end())
                        {
                            auto cachedTimepoint = cachedArrayVarTimepoints[var];

                            // add formula
                            auto posSymbol = posVarSymbol();
                            auto pos = posVar();
                            auto f = 
                                logic::Formulas::universalSimp({posSymbol},
                                    logic::Formulas::equalitySimp(
                                        toTerm(var, currTimepoint, pos, trace),
                                        toTerm(var, cachedTimepoint, pos, trace)
                                    )
                                );
                            conjuncts.push_back(f);
                        }
                        else
                        {
                            // note: cf. comment about why we need this case in the corresponding int-var case above
                            cachedArrayVarTimepoints[var] = currTimepoint;
                        }
                    }
                }
            }
        }

        return logic::Formulas::conjunctionSimp(conjuncts, label);
    }

    // note: this method is independent from currTimepoint.
    std::shared_ptr<const logic::Formula> SemanticsInliner::handlePersistenceOfLoop(std::shared_ptr<const logic::Term> startTimepoint, std::shared_ptr<const logic::Term> iterationTimepoint, const std::vector<std::shared_ptr<const program::Variable>>& vars)
    {
        std::vector<std::shared_ptr<const logic::Formula>> conjuncts;
        for (const auto& var : vars)
        {
            assert(!var->isConstant);

            // check whether value of variable term should be defined
            bool varTermShouldBeDefined = false;
            if (persistentVarTerms.find(var->name) != persistentVarTerms.end())
            {
                for (const auto& locationName : persistentVarTerms[var->name])
                {
                    if (locationName == startTimepoint->symbol->name)
                    {
                        // we have now established that var(timepoint,...) is referenced (where timepoint is equal to startTimepoint up to the iteration-term), so we need to define it
                        varTermShouldBeDefined = true;
                        break;
                    }
                }
            }

            if (varTermShouldBeDefined)
            {
                if (!var->isArray)
                {
                    if (cachedIntVarValues.find(var) == cachedIntVarValues.end())
                    {
                        // set x(l(zero)) as cachedValue, so that other references share the same cachedValue
                        // this edge case does matter e.g. in the following edge case:
                        // a variable x exists at loop locations l1 and l2 but the first assignment to x only appears after the loop, and we want to prove the conjecture forall it. (s(it)<n => x(l(it))=x(l(s(it))))
                        // using this branch of execution, we set the cached value of x at location l1 to x(l(zero)). Afterwards (but still in this method-invocation), we add an equality x(l(it))=x(l(zero)) which will then be used to prove the conjecture.
                        // we can interpret this edge case as covering the fact that uninitialized memory stays the same in each iteration, if no assigment occurs.
                        cachedIntVarValues[var] = toTerm(var, startTimepoint, trace);
                    }

                    auto f =
                        logic::Formulas::equality(
                            toTerm(var, iterationTimepoint, trace), 
                            cachedIntVarValues[var]
                        );
                    conjuncts.push_back(f);
                }
                else
                {

                    if (cachedArrayVarTimepoints.find(var) == cachedArrayVarTimepoints.end())
                    {
                        // set x(l(zero)) as cachedValue, so that other references share the same cachedValue
                        // this edge case does matter e.g. in the following edge case:
                        // a variable x exists at loop locations l1 and l2 but the first assignment to x only appears after the loop, and we want to prove the conjecture forall it. (s(it)<n => x(l(it))=x(l(s(it))))
                        // using this branch of execution, we set the cached value of x at location l1 to x(l(zero)). Afterwards (but still in this method-invocation), we add an equality x(l(it))=x(l(zero)) which will then be used to prove the conjecture.
                        // we can interpret this edge case as covering the fact that uninitialized memory stays the same in each iteration, if no assigment occurs.
                        cachedArrayVarTimepoints[var] = startTimepoint;
                    }

                    auto pos = posVar();
                    auto f =
                        logic::Formulas::equality(
                            toTerm(var, iterationTimepoint, pos, trace),
                            toTerm(var, cachedArrayVarTimepoints[var], pos, trace)
                        );
                    conjuncts.push_back(f);
                }
            }
        }

        return logic::Formulas::conjunctionSimp(conjuncts);
    }


    std::shared_ptr<const logic::Formula> SemanticsInliner::setIntVarValue(std::shared_ptr<const program::Variable> var, std::shared_ptr<const logic::Term> value)
    {
        assert(var != nullptr);
        assert(value != nullptr);
        assert(!var->isArray);
        assert(currTimepoint != nullptr);

        cachedIntVarValues[var] = value;

        // handle persistance for const-vars
        if (var->isConstant)
        {
            // if variable-term has to be persistent
            if (persistentConstVarTerms.find(var->name) != persistentConstVarTerms.end())
            {
                // add formula
                return 
                    logic::Formulas::equality(
                        toTerm(var, currTimepoint, trace), 
                        cachedIntVarValues[var]
                    );
            }
        }
        return logic::Formulas::trueFormula();
    }

    void SemanticsInliner::setArrayVarTimepoint(std::shared_ptr<const program::Variable> arrayVar, std::shared_ptr<const logic::Term> timepoint)
    {
        assert(arrayVar != nullptr);
        assert(timepoint != nullptr);
        assert(arrayVar->isArray);

        cachedArrayVarTimepoints[arrayVar] = timepoint;

        // note: there is nothing to do here for handling persistance for const-array-vars (it is not possible to assign a concrete value to a constant array)
    }

    InlinedVariableValues::InlinedVariableValues(std::vector<std::shared_ptr<const logic::Term>> traces) : values()
    {
        for (const auto& trace : traces)
        {
            // initialize an empty dictionary for each trace
            values[trace]={};
            arrayValues[trace]={};
        }
    }

    void InlinedVariableValues::initializeWhileStatement(const program::WhileStatement* whileStatement)
    {
        for (auto& m : values)
        {
            if(m.second.find(whileStatement) == m.second.end())
            {
                m.second[whileStatement] = {};
            }
        }
        for (auto& m : arrayValues)
        {
            if(m.second.find(whileStatement) == m.second.end())
            {
                m.second[whileStatement] = {};
            }
        }
    }

    void InlinedVariableValues::setValue(const program::WhileStatement* whileStatement, std::shared_ptr<const program::Variable> var, std::shared_ptr<const logic::Term> trace, std::shared_ptr<const logic::Term> value)
    {
        assert(whileStatement != nullptr);
        assert(var != nullptr);
        assert(value != nullptr);
        assert(trace != nullptr);
        assert(!var->isArray);
        assert(AnalysisPreComputation::computeAssignedVars(whileStatement).count(var) == 0);
        values.at(trace);
        values.at(trace).at(whileStatement);

        values.at(trace).at(whileStatement)[var] = value;
    }

    void InlinedVariableValues::setArrayTimepoint(const program::WhileStatement* whileStatement, std::shared_ptr<const program::Variable> arrayVar, std::shared_ptr<const logic::Term> trace, std::shared_ptr<const logic::Term> timepoint)
    {
        assert(whileStatement != nullptr);
        assert(arrayVar != nullptr);
        assert(trace != nullptr);
        assert(timepoint != nullptr);
        assert(arrayVar->isArray);
        assert(AnalysisPreComputation::computeAssignedVars(whileStatement).count(arrayVar) == 0);

        arrayValues.at(trace).at(whileStatement)[arrayVar] = timepoint;
    }

    std::shared_ptr<const logic::Term> InlinedVariableValues::toInlinedTerm(const program::WhileStatement* whileStatement, std::shared_ptr<const program::Variable> var, std::shared_ptr<const logic::Term> trace)
    {
        assert(whileStatement != nullptr);
        assert(var != nullptr);
        assert(trace != nullptr);
        assert(!var->isArray);

        return values.at(trace).at(whileStatement).at(var);
    }

    std::shared_ptr<const logic::Term> InlinedVariableValues::toInlinedTerm(const program::WhileStatement* whileStatement, std::shared_ptr<const program::Variable> arrayVar, std::shared_ptr<const logic::Term> position, std::shared_ptr<const logic::Term> trace)
    {
        assert(whileStatement != nullptr);
        assert(arrayVar != nullptr);
        assert(position != nullptr);
        assert(arrayVar->isArray);

        auto timepoint = arrayValues.at(trace).at(whileStatement).at(arrayVar);
        return toTerm(arrayVar, timepoint, position, trace);
    }

    std::shared_ptr<const logic::Term> InlinedVariableValues::toInlinedTerm(const program::WhileStatement* whileStatement, std::shared_ptr<const program::IntExpression> expr, std::shared_ptr<const logic::Term> timepoint, std::shared_ptr<const logic::Term> trace)
    {
        assert(expr != nullptr);
        assert(whileStatement != nullptr);

        switch (expr->type())
        {
            case program::IntExpression::Type::ArithmeticConstant:
            {
                auto castedExpr = std::static_pointer_cast<const program::ArithmeticConstant>(expr);
                return logic::Theory::intConstant(castedExpr->value);
            }
            case program::IntExpression::Type::Addition:
            {
                auto castedExpr = std::static_pointer_cast<const program::Addition>(expr);
                return logic::Theory::intAddition(toInlinedTerm(whileStatement, castedExpr->summand1, timepoint, trace), toInlinedTerm(whileStatement, castedExpr->summand2, timepoint, trace));
            }
            case program::IntExpression::Type::Subtraction:
            {
                auto castedExpr = std::static_pointer_cast<const program::Subtraction>(expr);
                return logic::Theory::intSubtraction(toInlinedTerm(whileStatement, castedExpr->child1, timepoint, trace), toInlinedTerm(whileStatement, castedExpr->child2, timepoint, trace));
            }
            case program::IntExpression::Type::Modulo:
            {
                auto castedExpr = std::static_pointer_cast<const program::Modulo>(expr);
                return logic::Theory::intModulo(toInlinedTerm(whileStatement, castedExpr->child1, timepoint, trace), toInlinedTerm(whileStatement, castedExpr->child2, timepoint, trace));
            }
            case program::IntExpression::Type::Multiplication:
            {
                auto castedExpr = std::static_pointer_cast<const program::Multiplication>(expr);
                return logic::Theory::intMultiplication(toInlinedTerm(whileStatement, castedExpr->factor1, timepoint, trace), toInlinedTerm(whileStatement, castedExpr->factor2, timepoint, trace));
            }
            case program::IntExpression::Type::IntVariableAccess:
            {
                auto var = std::static_pointer_cast<const program::IntVariableAccess>(expr)->var;
                if (AnalysisPreComputation::computeAssignedVars(whileStatement).count(var) == 0)
                {
                    // 'var' was not assigned to in 'whileStatement', so use inlined value (which must exist)
                    return toInlinedTerm(whileStatement, var, trace);
                }
                else
                {
                    // 'var' was assigned to in 'whileStatement', so use original value
                    return toTerm(var, timepoint, trace);
                }
            }
            case program::IntExpression::Type::IntArrayApplication:
            {
                auto castedExpr = std::static_pointer_cast<const program::IntArrayApplication>(expr);
                auto arrayVar = castedExpr->array;
                auto arrayIndex = castedExpr->index;

                auto position = toInlinedTerm(whileStatement, arrayIndex, timepoint, trace);
                if (AnalysisPreComputation::computeAssignedVars(whileStatement).count(arrayVar) == 0)
                {
                    // 'arrayVar' was not assigned to in 'whileStatement', so use inlined value (which must exist)
                    return toInlinedTerm(whileStatement, arrayVar, position, trace);
                }
                else
                {
                    // 'arrayVar' was assigned to in 'whileStatement', so use original value
                    return toTerm(arrayVar, timepoint, position, trace);
                }
            }
        }
    }

    std::shared_ptr<const logic::Formula> InlinedVariableValues::toInlinedFormula(const program::WhileStatement* whileStatement, std::shared_ptr<const program::BoolExpression> expr, std::shared_ptr<const logic::Term> timepoint, std::shared_ptr<const logic::Term> trace)
    {
        assert(expr != nullptr);

        switch (expr->type())
        {
            case program::BoolExpression::Type::BooleanConstant:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanConstant>(expr);
                return castedExpr->value ? logic::Theory::boolTrue() : logic::Theory::boolFalse();
            }
            case program::BoolExpression::Type::BooleanAnd:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanAnd>(expr);
                return logic::Formulas::conjunction({toInlinedFormula(whileStatement, castedExpr->child1, timepoint, trace), toInlinedFormula(whileStatement, castedExpr->child2, timepoint, trace)});
            }
            case program::BoolExpression::Type::BooleanOr:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanOr>(expr);
                return logic::Formulas::disjunction({toInlinedFormula(whileStatement, castedExpr->child1, timepoint, trace), toInlinedFormula(whileStatement, castedExpr->child2, timepoint, trace)});
            }
            case program::BoolExpression::Type::BooleanNot:
            {
                auto castedExpr = std::static_pointer_cast<const program::BooleanNot>(expr);
                return logic::Formulas::negation(toInlinedFormula(whileStatement, castedExpr->child, timepoint, trace));
            }
            case program::BoolExpression::Type::ArithmeticComparison:
            {
                auto castedExpr = std::static_pointer_cast<const program::ArithmeticComparison>(expr);
                switch (castedExpr->kind)
                {
                    case program::ArithmeticComparison::Kind::GT:
                        return logic::Theory::intGreater(toInlinedTerm(whileStatement, castedExpr->child1, timepoint, trace), toInlinedTerm(whileStatement, castedExpr->child2, timepoint, trace));
                    case program::ArithmeticComparison::Kind::GE:
                        return logic::Theory::intGreaterEqual(toInlinedTerm(whileStatement, castedExpr->child1, timepoint, trace), toInlinedTerm(whileStatement, castedExpr->child2, timepoint, trace));
                    case program::ArithmeticComparison::Kind::LT:
                        return logic::Theory::intLess(toInlinedTerm(whileStatement, castedExpr->child1, timepoint, trace), toInlinedTerm(whileStatement, castedExpr->child2, timepoint, trace));
                    case program::ArithmeticComparison::Kind::LE:
                        return logic::Theory::intLessEqual(toInlinedTerm(whileStatement, castedExpr->child1, timepoint, trace), toInlinedTerm(whileStatement, castedExpr->child2, timepoint, trace));
                    case program::ArithmeticComparison::Kind::EQ:
                        return logic::Formulas::equality(toInlinedTerm(whileStatement, castedExpr->child1, timepoint, trace), toInlinedTerm(whileStatement, castedExpr->child2, timepoint, trace));
                }
            }
        }
    }
}