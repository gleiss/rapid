#include "WhileParserPostComputation.hpp"

#include "Theory.hpp"

using namespace program;

namespace parser
{
# pragma mark - compute
    
    std::unique_ptr<const program::ProgramGlobalProperties>  WhileParserPostComputation::compute(const program::Program& program, bool twoTraces)
    {
        auto iteratorMap = computeIteratorMap(program);
        auto lastIterationMap = computeLastIterationMap(program, twoTraces);
        
        auto enclosingIteratorsMap = computeEnclosingIteratorsMap(program, iteratorMap);
        auto locationSymbolMap = computeLocationSymbolMap(program, enclosingIteratorsMap);
        auto startTimePointMap = computeStartTimePointMap(program, enclosingIteratorsMap, locationSymbolMap);
        auto endTimePointMap = computeEndTimePointMap(program, iteratorMap, enclosingIteratorsMap, startTimePointMap);

        return std::make_unique<const ProgramGlobalProperties>(iteratorMap,
                                       lastIterationMap,
                                       enclosingIteratorsMap,
                                       locationSymbolMap,
                                       startTimePointMap,
                                       endTimePointMap);
    }
    
# pragma mark - IteratorMap
    
    IteratorMap WhileParserPostComputation::computeIteratorMap(const program::Program& program)
    {
        IteratorMap iteratorMap;
        for(const auto& function : program.functions)
        {
            for (const auto& statement : function->statements)
            {
                addIteratorsForStatement(statement.get(), iteratorMap);
            }
        }
        return iteratorMap;
    }
    
    void WhileParserPostComputation::addIteratorsForStatement(const program::Statement* statement, IteratorMap& iteratorMap)
    {
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            for (const auto& statement : castedStatement->ifStatements)
            {
                addIteratorsForStatement(statement.get(), iteratorMap);
            }
            for (const auto& statement : castedStatement->elseStatements)
            {
                addIteratorsForStatement(statement.get(), iteratorMap);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            iteratorMap[castedStatement] = logic::Signature::varSymbol("It" + castedStatement->location, logic::Sorts::timeSort());
            for (const auto& statement : castedStatement->bodyStatements)
            {
                addIteratorsForStatement(statement.get(), iteratorMap);
            }
        }
    }
    
# pragma mark - lastIterationMap
    
    LastIterationMap WhileParserPostComputation::computeLastIterationMap(const program::Program& program, bool twoTraces)
    {
        LastIterationMap lastIterationMap;
        for(const auto& function : program.functions)
        {
            for (const auto& statement : function->statements)
            {
                addLastIterationsForStatement(statement.get(), lastIterationMap, twoTraces);
            }
        }
        return lastIterationMap;
    }
    
    void WhileParserPostComputation::addLastIterationsForStatement(const program::Statement* statement, LastIterationMap& lastIterationMap, bool twoTraces)
    {
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            for (const auto& statement : castedStatement->ifStatements)
            {
                addLastIterationsForStatement(statement.get(), lastIterationMap, twoTraces);
            }
            for (const auto& statement : castedStatement->elseStatements)
            {
                addLastIterationsForStatement(statement.get(), lastIterationMap, twoTraces);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            std::vector<std::shared_ptr<const logic::Term>> arguments;
            if (twoTraces)
            {
                auto tr = logic::Signature::varSymbol("tr", logic::Sorts::traceSort());
                arguments.push_back(logic::Terms::var(tr));
            }
            lastIterationMap[castedStatement] = logic::Terms::func("n" + castedStatement->location, arguments, logic::Sorts::timeSort());
            for (const auto& statement : castedStatement->bodyStatements)
            {
                addLastIterationsForStatement(statement.get(), lastIterationMap, twoTraces);
            }
        }
    }
    
# pragma mark enclosingIteratorsMap

    EnclosingIteratorsMap WhileParserPostComputation::computeEnclosingIteratorsMap(const program::Program& program, const IteratorMap& iteratorMap)
    {
        EnclosingIteratorsMap enclosingIteratorsMap;
        for(const auto& function : program.functions)
        {
            for (const auto& statement : function->statements)
            {
                addEnclosingIteratorsForStatement(statement.get(), iteratorMap, {}, enclosingIteratorsMap);
            }
        }
        return enclosingIteratorsMap;
    }

    void WhileParserPostComputation::addEnclosingIteratorsForStatement(const program::Statement* statement, const IteratorMap& iteratorMap, std::vector<std::shared_ptr<const logic::Symbol>> enclosingIterators, EnclosingIteratorsMap& enclosingIteratorsMap)
    {
        if (statement->type() == program::Statement::Type::IntAssignment)
        {
            auto castedStatement = static_cast<const program::IntAssignment*>(statement);
            enclosingIteratorsMap[castedStatement] = enclosingIterators;
        }
        else if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            enclosingIteratorsMap[castedStatement] = enclosingIterators;
            for (const auto& statement : castedStatement->ifStatements)
            {
                addEnclosingIteratorsForStatement(statement.get(), iteratorMap, enclosingIterators, enclosingIteratorsMap);
            }
            for (const auto& statement : castedStatement->elseStatements)
            {
                addEnclosingIteratorsForStatement(statement.get(), iteratorMap, enclosingIterators, enclosingIteratorsMap);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);

            enclosingIteratorsMap[castedStatement] = enclosingIterators;

            auto enclosingIteratorsCopy = enclosingIterators;
            enclosingIteratorsCopy.push_back(iteratorMap.at(castedStatement));
            for (const auto& statement : castedStatement->bodyStatements)
            {
                addEnclosingIteratorsForStatement(statement.get(), iteratorMap, enclosingIteratorsCopy, enclosingIteratorsMap);
            }
        }
        else
        {
            assert(statement->type() == program::Statement::Type::SkipStatement);
            auto castedStatement = static_cast<const program::SkipStatement*>(statement);
            enclosingIteratorsMap[castedStatement] = enclosingIterators;
        }
    }
    
# pragma mark - LocationSymbolMap
    
    LocationSymbolMap WhileParserPostComputation::computeLocationSymbolMap(const program::Program& program, const program::EnclosingIteratorsMap& enclosingIteratorsMap)
    {
        LocationSymbolMap locationSymbolMap;
        for(const auto& function : program.functions)
        {
            for (const auto& statement : function->statements)
            {
                addLocationSymbolForStatement(statement.get(), enclosingIteratorsMap, locationSymbolMap);
            }
        }
        return locationSymbolMap;
    }
    void WhileParserPostComputation::addLocationSymbolForStatement(const program::Statement* statement,
                                              const program::EnclosingIteratorsMap& enclosingIteratorsMap,
                                              program::LocationSymbolMap& locationSymbolMap)
    {
        auto enclosingIteratorTypes = std::vector<const logic::Sort*>();
        for (const auto& enclosingIterator : enclosingIteratorsMap.at(statement))
        {
            enclosingIteratorTypes.push_back(enclosingIterator->rngSort);
        }
        if (statement->type() == program::Statement::Type::WhileStatement)
        {
            enclosingIteratorTypes.push_back(logic::Sorts::timeSort());
        }
        locationSymbolMap[statement] = logic::Signature::add(statement->location, enclosingIteratorTypes, logic::Sorts::timeSort());
        
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            for (const auto& statement : castedStatement->ifStatements)
            {
                addLocationSymbolForStatement(statement.get(), enclosingIteratorsMap, locationSymbolMap);
            }
            for (const auto& statement : castedStatement->elseStatements)
            {
                addLocationSymbolForStatement(statement.get(), enclosingIteratorsMap, locationSymbolMap);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            for (const auto& statement : castedStatement->bodyStatements)
            {
                addLocationSymbolForStatement(statement.get(), enclosingIteratorsMap, locationSymbolMap);
            }
        }
    }

# pragma mark - StartTimePointMap

    StartTimePointMap WhileParserPostComputation::computeStartTimePointMap(const program::Program& program,
                                                                           const EnclosingIteratorsMap& enclosingIteratorsMap,
                                                                           const program::LocationSymbolMap& locationSymbolMap)
    {
        StartTimePointMap startTimePointMap;
        for(const auto& function : program.functions)
        {
            for (const auto& statement : function->statements)
            {
                addStartTimePointForStatement(statement.get(), enclosingIteratorsMap, locationSymbolMap, startTimePointMap);
            }
        }
        return startTimePointMap;
    }

    void WhileParserPostComputation::addStartTimePointForStatement(const program::Statement* statement,
                                                                   const EnclosingIteratorsMap& enclosingIteratorsMap,
                                                                   const program::LocationSymbolMap& locationSymbolMap,
                                                                   StartTimePointMap& startTimePointMap)
    {
        auto enclosingIterators = enclosingIteratorsMap.at(statement);
        auto enclosingIteratorTerms = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingIterator : enclosingIterators)
        {
            enclosingIteratorTerms.push_back(logic::Terms::var(enclosingIterator));
        }
        
        if (statement->type() == program::Statement::Type::WhileStatement)
        {
            enclosingIteratorTerms.push_back(logic::Theory::timeZero());
        }
        startTimePointMap[statement] = logic::Terms::func(locationSymbolMap.at(statement), enclosingIteratorTerms);
        
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            for (const auto& statement : castedStatement->ifStatements)
            {
                addStartTimePointForStatement(statement.get(), enclosingIteratorsMap, locationSymbolMap, startTimePointMap);
            }
            for (const auto& statement : castedStatement->elseStatements)
            {
                addStartTimePointForStatement(statement.get(), enclosingIteratorsMap, locationSymbolMap, startTimePointMap);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            for (const auto& statement : castedStatement->bodyStatements)
            {
                addStartTimePointForStatement(statement.get(), enclosingIteratorsMap, locationSymbolMap, startTimePointMap);
            }
        }
    }

# pragma mark - End location computation

    EndTimePointMap WhileParserPostComputation::computeEndTimePointMap(const program::Program& program,
                                                                       const IteratorMap& iteratorMap,
                                                                       const EnclosingIteratorsMap& enclosingIteratorsMap,
                                                                       const StartTimePointMap& startTimePointMap)
    {
        EndTimePointMap endTimePointMap;
        for(const auto& function : program.functions)
        {
            // for each statement except the first, set the end-location of the previous statement to the begin-location of this statement
            for(int i=1; i < function->statements.size(); ++i)
            {
                auto currentStatement = function->statements[i].get();
                auto lastStatement = function->statements[i-1].get();
                
                endTimePointMap[lastStatement] = startTimePointMap.at(currentStatement);
            }
            // for the last statement, set the end-location to be the end-location of the function.
            auto lastStatement = function->statements.back().get();
            auto t = logic::Terms::func(function->name + "_end", {}, logic::Sorts::timeSort());
            endTimePointMap[lastStatement] = t;
            
            // recurse on the statements
            for(const auto& statement : function->statements)
            {
                addEndTimePointForStatement(statement.get(), iteratorMap, enclosingIteratorsMap, startTimePointMap, endTimePointMap);
            }
        }
        return endTimePointMap;
    }

    void WhileParserPostComputation::addEndTimePointForStatement(const program::Statement* statement,
                                                                 const IteratorMap& iteratorMap,
                                                                 const EnclosingIteratorsMap& enclosingIteratorsMap,
                                                                 const StartTimePointMap& startTimePointMap,
                                                                 EndTimePointMap& endTimePointMap)
    {
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            addEndTimePointForIfElseStatement(castedStatement, iteratorMap, enclosingIteratorsMap, startTimePointMap, endTimePointMap);
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            addEndTimePointForWhileStatement(castedStatement, iteratorMap, enclosingIteratorsMap, startTimePointMap, endTimePointMap);
        }
    }

    void WhileParserPostComputation::addEndTimePointForIfElseStatement(const program::IfElse* ifElse,
                                                                 const IteratorMap& iteratorMap,
                                                                 const EnclosingIteratorsMap& enclosingIteratorsMap,
                                                                 const StartTimePointMap& startTimePointMap,
                                                                 EndTimePointMap& endTimePointMap)
    {
        // for each statement in the left branch except the first, set the end-location of the previous statement to the begin-location of this statement
        for(int i=1; i < ifElse->ifStatements.size(); ++i)
        {
            auto currentStatement = ifElse->ifStatements[i].get();
            auto lastStatement = ifElse->ifStatements[i-1].get();

            endTimePointMap[lastStatement] = startTimePointMap.at(currentStatement);
        }
        // use a new location as end location for last statement of the left branch
        auto lastStatementLeft = ifElse->ifStatements.back().get();
        
        auto enclosingIteratorTerms = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingIterator : enclosingIteratorsMap.at(ifElse))
        {
            enclosingIteratorTerms.push_back(logic::Terms::var(enclosingIterator));
        }
        
        auto lLeftEnd = logic::Terms::func(ifElse->location + "_lEnd", enclosingIteratorTerms, logic::Sorts::timeSort());
        endTimePointMap[lastStatementLeft] = lLeftEnd;

        // for each statement in the right branch except the first, set the end-location of the previous statement to the begin-location of this statement
        for(int i=1; i < ifElse->elseStatements.size(); ++i)
        {
            auto currentStatement = ifElse->elseStatements[i].get();
            auto lastStatement = ifElse->elseStatements[i-1].get();
            
            endTimePointMap[lastStatement] = startTimePointMap.at(currentStatement);
        }
        // use a new location as end location for last statement of the right branch
        auto lastStatementRight = ifElse->elseStatements.back().get();

        auto enclosingIteratorTermsRight = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingIterator : enclosingIteratorsMap.at(ifElse))
        {
            enclosingIteratorTermsRight.push_back(logic::Terms::var(enclosingIterator));
        }

        auto lRightEnd = logic::Terms::func(ifElse->location + "_rEnd", enclosingIteratorTerms, logic::Sorts::timeSort());
        endTimePointMap[lastStatementRight] = lRightEnd;

        // recurse on the statements
        for(const auto& statement : ifElse->ifStatements)
        {
            addEndTimePointForStatement(statement.get(), iteratorMap, enclosingIteratorsMap, startTimePointMap, endTimePointMap);
        }
        for(const auto& statement : ifElse->elseStatements)
        {
            addEndTimePointForStatement(statement.get(), iteratorMap, enclosingIteratorsMap, startTimePointMap, endTimePointMap);
        }
    }

    void WhileParserPostComputation::addEndTimePointForWhileStatement(const program::WhileStatement* whileStatement,
                                                                 const IteratorMap& iteratorMap,
                                                                 const EnclosingIteratorsMap& enclosingIteratorsMap,
                                                                 const StartTimePointMap& startTimePointMap,
                                                                 EndTimePointMap& endTimePointMap)
    {
        // for the last statement in the body, set as end-timepoint the start-location of the while-statement in the next iteration
        auto lastStatement = whileStatement->bodyStatements.back().get();

        auto enclosingIteratorTerms = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingIterator : enclosingIteratorsMap.at(whileStatement))
        {
            enclosingIteratorTerms.push_back(logic::Terms::var(enclosingIterator));
        }
        auto t1 = logic::Terms::var(iteratorMap.at(whileStatement));
        auto t2 = logic::Theory::timeSucc(t1);
        enclosingIteratorTerms.push_back(t2);
        auto t3 = logic::Terms::func(whileStatement->location, enclosingIteratorTerms, logic::Sorts::timeSort());
        
        endTimePointMap[lastStatement] = t3;

        // for each statement in the body except the first, set the end-location of the previous statement to the begin-location of this statement
        for(int i=1; i < whileStatement->bodyStatements.size(); ++i)
        {
            auto currentStatement = whileStatement->bodyStatements[i].get();
            auto lastStatement = whileStatement->bodyStatements[i-1].get();

            endTimePointMap[lastStatement] = startTimePointMap.at(currentStatement);
        }

        // recurse on the statements
        for(const auto& statement : whileStatement->bodyStatements)
        {
            addEndTimePointForStatement(statement.get(), iteratorMap, enclosingIteratorsMap, startTimePointMap, endTimePointMap);
        }
    }


}

