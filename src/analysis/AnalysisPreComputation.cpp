#include "AnalysisPreComputation.hpp"

#include <memory>
#include <vector>

#include "Theory.hpp"
#include "SymbolDeclarations.hpp"
#include "SemanticsHelper.hpp"

namespace analysis
{    
    EndTimePointMap AnalysisPreComputation::computeEndTimePointMap(const program::Program& program)
    {
        EndTimePointMap endTimePointMap;
        for(const auto& function : program.functions)
        {
            // for each statement except the first, set the end-location of the previous statement to the begin-location of this statement
            for(int i=1; i < function->statements.size(); ++i)
            {
                auto lastStatement = function->statements[i-1].get();
                auto nextTimepoint = startTimepointForStatement(function->statements[i].get());
                addEndTimePointForStatement(lastStatement, nextTimepoint, endTimePointMap);
            }
            // for the last statement, set the end-location to be the end-location of the function.
            auto lastStatement = function->statements.back().get();
            auto nextTimepoint = logic::Terms::func(locationSymbolEndLocation(function.get()),{});
            addEndTimePointForStatement(lastStatement, nextTimepoint, endTimePointMap);
        }
        return endTimePointMap;
    }
    
    void AnalysisPreComputation::addEndTimePointForStatement(const program::Statement* statement,
                                                             const std::shared_ptr<const logic::Term> nextTimepoint,
                                                             EndTimePointMap& endTimePointMap)
    {
        // for an ifElse statement, just recurse
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            addEndTimePointForIfElseStatement(castedStatement, nextTimepoint, endTimePointMap);
        }
        // for a while statement, set endTimepoint and recurse
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            endTimePointMap[statement] = nextTimepoint;

            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            addEndTimePointForWhileStatement(castedStatement, nextTimepoint, endTimePointMap);
        }
        // set endTimepoint for atomic statement
        else
        {
            assert(statement->type() == program::Statement::Type::IntAssignment ||
                   statement->type() == program::Statement::Type::SkipStatement);
            endTimePointMap[statement] = nextTimepoint;
        }
    }
    
    void AnalysisPreComputation::addEndTimePointForIfElseStatement(const program::IfElse* ifElse,
                                                                   const std::shared_ptr<const logic::Term> nextTimepoint,
                                                                   EndTimePointMap& endTimePointMap)
    {
        // for each statement in the left branch except the first, set the end-location of the previous statement to the begin-location of this statement
        for(int i=1; i < ifElse->ifStatements.size(); ++i)
        {
            auto lastStatement = ifElse->ifStatements[i-1].get();
            auto nextTimepointForStatement = startTimepointForStatement(ifElse->ifStatements[i].get());
            addEndTimePointForStatement(lastStatement, nextTimepointForStatement, endTimePointMap);
        }
        // for each statement in the right branch except the first, set the end-location of the previous statement to the begin-location of this statement
        for(int i=1; i < ifElse->elseStatements.size(); ++i)
        {
            auto lastStatement = ifElse->elseStatements[i-1].get();
            auto nextTimepointForStatement = startTimepointForStatement(ifElse->elseStatements[i].get());
            addEndTimePointForStatement(lastStatement, nextTimepointForStatement, endTimePointMap);
        }
        // for both left and right branch, set the endTimePoint of the ifElse-statement as endTimePoint for the last statement
        auto lastStatementLeft = ifElse->ifStatements.back().get();
        auto lastStatementRight = ifElse->elseStatements.back().get();
        addEndTimePointForStatement(lastStatementLeft, nextTimepoint, endTimePointMap);
        addEndTimePointForStatement(lastStatementRight, nextTimepoint, endTimePointMap);
    }
    
    void AnalysisPreComputation::addEndTimePointForWhileStatement(const program::WhileStatement* whileStatement,
                                                                  const std::shared_ptr<const logic::Term> nextTimepoint,
                                                                  EndTimePointMap& endTimePointMap)
    {
        // for each statement in the body except the first, set the end-location of the previous statement to the begin-location of this statement
        for(int i=1; i < whileStatement->bodyStatements.size(); ++i)
        {
            auto lastStatement = whileStatement->bodyStatements[i-1].get();
            auto nextTimepointForStatement = startTimepointForStatement(whileStatement->bodyStatements[i].get());
            addEndTimePointForStatement(lastStatement, nextTimepointForStatement, endTimePointMap);
        }

        // for the last statement in the body, set as end-timepoint the start-location of the while-statement in the next iteration
        auto lastStatement = whileStatement->bodyStatements.back().get();
        auto nextTimepointForStatement = timepointForLoopStatement(whileStatement, logic::Theory::natSucc(iteratorTermForLoop(whileStatement)));
        addEndTimePointForStatement(lastStatement, nextTimepointForStatement, endTimePointMap);
    }
}
