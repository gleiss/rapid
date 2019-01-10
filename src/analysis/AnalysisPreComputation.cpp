#include "AnalysisPreComputation.hpp"

#include <memory>
#include <vector>

#include "Theory.hpp"
#include "SymbolDeclarations.hpp"

namespace analysis
{

# pragma mark - StartTimePointMap
    
    StartTimePointMap AnalysisPreComputation::computeStartTimePointMap(const program::Program& program)
    {
        StartTimePointMap startTimePointMap;
        for(const auto& function : program.functions)
        {
            for (const auto& statement : function->statements)
            {
                addStartTimePointForStatement(statement.get(), startTimePointMap);
            }
        }
        return startTimePointMap;
    }
    
    void AnalysisPreComputation::addStartTimePointForStatement(const program::Statement* statement,
                                                                   StartTimePointMap& startTimePointMap)
    {
        auto enclosingLoops = *statement->enclosingLoops;
        auto enclosingIteratorTerms = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingLoop : enclosingLoops)
        {
            auto enclosingIteratorSymbol = iteratorSymbol(enclosingLoop);
            enclosingIteratorTerms.push_back(logic::Terms::var(enclosingIteratorSymbol));
        }
        
        if (statement->type() == program::Statement::Type::WhileStatement)
        {
            enclosingIteratorTerms.push_back(logic::Theory::natZero());
        }
        startTimePointMap[statement] = logic::Terms::func(locationSymbolForStatement(statement), enclosingIteratorTerms);
        
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            for (const auto& statement : castedStatement->ifStatements)
            {
                addStartTimePointForStatement(statement.get(), startTimePointMap);
            }
            for (const auto& statement : castedStatement->elseStatements)
            {
                addStartTimePointForStatement(statement.get(), startTimePointMap);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            for (const auto& statement : castedStatement->bodyStatements)
            {
                addStartTimePointForStatement(statement.get(), startTimePointMap);
            }
        }
    }
    
# pragma mark - End location computation
    
    EndTimePointMap AnalysisPreComputation::computeEndTimePointMap(const program::Program& program,
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
            auto t = logic::Terms::func(locationSymbolEndLocation(function.get()),{});
            endTimePointMap[lastStatement] = t;
            
            // recurse on the statements
            for(const auto& statement : function->statements)
            {
                addEndTimePointForStatement(statement.get(), startTimePointMap, endTimePointMap);
            }
        }
        return endTimePointMap;
    }
    
    void AnalysisPreComputation::addEndTimePointForStatement(const program::Statement* statement,
                                                                 const StartTimePointMap& startTimePointMap,
                                                                 EndTimePointMap& endTimePointMap)
    {
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            addEndTimePointForIfElseStatement(castedStatement, startTimePointMap, endTimePointMap);
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            addEndTimePointForWhileStatement(castedStatement, startTimePointMap, endTimePointMap);
        }
    }
    
    void AnalysisPreComputation::addEndTimePointForIfElseStatement(const program::IfElse* ifElse,
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
        // for each statement in the right branch except the first, set the end-location of the previous statement to the begin-location of this statement
        for(int i=1; i < ifElse->elseStatements.size(); ++i)
        {
            auto currentStatement = ifElse->elseStatements[i].get();
            auto lastStatement = ifElse->elseStatements[i-1].get();
            
            endTimePointMap[lastStatement] = startTimePointMap.at(currentStatement);
        }
        // for both left and right branch, use a new location as end location for the last statement of that branch
        auto enclosingIteratorTerms = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingLoop : *ifElse->enclosingLoops)
        {
            auto enclosingIteratorSymbol = iteratorSymbol(enclosingLoop);
            enclosingIteratorTerms.push_back(logic::Terms::var(enclosingIteratorSymbol));
        }
        
        auto lLeftEnd = logic::Terms::func(locationSymbolLeftBranch(ifElse), enclosingIteratorTerms);
        auto lRightEnd = logic::Terms::func(locationSymbolRightBranch(ifElse), enclosingIteratorTerms);
        
        auto lastStatementLeft = ifElse->ifStatements.back().get();
        auto lastStatementRight = ifElse->elseStatements.back().get();
        
        endTimePointMap[lastStatementLeft] = lLeftEnd;
        endTimePointMap[lastStatementRight] = lRightEnd;
        
        // recurse on the statements
        for(const auto& statement : ifElse->ifStatements)
        {
            addEndTimePointForStatement(statement.get(), startTimePointMap, endTimePointMap);
        }
        for(const auto& statement : ifElse->elseStatements)
        {
            addEndTimePointForStatement(statement.get(), startTimePointMap, endTimePointMap);
        }
    }
    
    void AnalysisPreComputation::addEndTimePointForWhileStatement(const program::WhileStatement* whileStatement,
                                                                      const StartTimePointMap& startTimePointMap,
                                                                      EndTimePointMap& endTimePointMap)
    {
        // for the last statement in the body, set as end-timepoint the start-location of the while-statement in the next iteration
        auto lastStatement = whileStatement->bodyStatements.back().get();
        
        auto enclosingIteratorTerms = std::vector<std::shared_ptr<const logic::Term>>();
        for (const auto& enclosingLoop : *whileStatement->enclosingLoops)
        {
            auto enclosingIteratorSymbol = iteratorSymbol(enclosingLoop);
            enclosingIteratorTerms.push_back(logic::Terms::var(enclosingIteratorSymbol));
        }
        auto t1 = logic::Terms::var(iteratorSymbol(whileStatement));
        auto t2 = logic::Theory::natSucc(t1);
        enclosingIteratorTerms.push_back(t2);
        auto t3 = logic::Terms::func(locationSymbolForStatement(whileStatement), enclosingIteratorTerms);
        
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
            addEndTimePointForStatement(statement.get(), startTimePointMap, endTimePointMap);
        }
    }
    
}
