#include "WhileParsingContext.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace parser
{
    bool WhileParsingContext::pushQuantifiedVars(std::vector<std::shared_ptr<const logic::Symbol>> quantifiedVars)
    {
        // TODO: later check that those don't exist yet.
        // insert each var into into map
        for (const auto& quantifiedVar : quantifiedVars)
        {
            if (quantifiedVarsDeclarations.count(quantifiedVar->name) > 0)
            {
                return false;
            }
            quantifiedVarsDeclarations[quantifiedVar->name] = quantifiedVar;
        }
        // push new level onto stack
        std::vector<std::string> quantifiedVarNames;
        for (const auto& quantifiedVar : quantifiedVars)
        {
            quantifiedVarNames.push_back(quantifiedVar->name);
        }
        quantifiedVarsStack.push_back(quantifiedVarNames);
        
        return true;
    }
    
    void WhileParsingContext::popQuantifiedVars()
    {
        // remove each var of last level from map
        for (const auto& quantifiedVarName : quantifiedVarsStack.back())
        {
            quantifiedVarsDeclarations.erase(quantifiedVarName);
        }
        // pop last level
        quantifiedVarsStack.pop_back();
    }
    
    bool WhileParsingContext::isDeclared(std::string name)
    {
        return logic::Signature::isDeclared(name) || (quantifiedVarsDeclarations.count(name) > 0);
    }
    
    std::shared_ptr<const logic::Symbol> WhileParsingContext::fetch(std::string name)
    {
        if (quantifiedVarsDeclarations.count(name) > 0)
        {
            return quantifiedVarsDeclarations[name];
        }
        return logic::Signature::fetch(name);
    }
    
    void WhileParsingContext::pushProgramVars()
    {
        programVarsStack.push_back({});
    }
    
    void WhileParsingContext::popProgramVars()
    {
        // remove each var of last level from map
        for (const auto& programVarName : programVarsStack.back())
        {
            programVarsDeclarations.erase(programVarName);
        }
        // pop last level
        programVarsStack.pop_back();
    }
    
    bool WhileParsingContext::addProgramVar(std::shared_ptr<const program::Variable> programVar)
    {
        if (programVarsDeclarations.count(programVar->name) > 0)
        {
            return false;
        }
        programVarsDeclarations[programVar->name] = programVar;
        programVarsStack.back().push_back(programVar->name);
        
        return true;
    }
    
    std::shared_ptr<const program::Variable> WhileParsingContext::getProgramVar(std::string name)
    {
        if (programVarsDeclarations.count(name) > 0)
        {
            return programVarsDeclarations[name];
        }
        else
        {
            std::cout << "program var " << name << " has not been declared!";
            assert(false);
            return nullptr;
        }
    }
    
    std::vector<std::shared_ptr<const program::Variable>> WhileParsingContext::getActiveProgramVars()
    {
        std::vector<std::shared_ptr<const program::Variable>> activeVars;
        for (const auto& pairNameVar : programVarsDeclarations)
        {
            activeVars.push_back(pairNameVar.second);
        }
        return activeVars;
    }
    
# pragma mark enclosingLoops
    
    void WhileParsingContext::addEnclosingLoops(const program::Function& function)
    {
        for (const auto& statement : function.statements)
        {
            addEnclosingLoopsForStatement(statement.get(), {});
        }
    }
    
    void WhileParsingContext::addEnclosingLoopsForStatement(const program::Statement* statement, std::vector<const program::WhileStatement*> enclosingLoops)
    {
        *statement->enclosingLoops = enclosingLoops;
        
        if (statement->type() == program::Statement::Type::IfElse)
        {
            auto castedStatement = static_cast<const program::IfElse*>(statement);
            for (const auto& statementInBranch : castedStatement->ifStatements)
            {
                addEnclosingLoopsForStatement(statementInBranch.get(), enclosingLoops);
            }
            for (const auto& statementInBranch : castedStatement->elseStatements)
            {
                addEnclosingLoopsForStatement(statementInBranch.get(), enclosingLoops);
            }
        }
        else if (statement->type() == program::Statement::Type::WhileStatement)
        {
            auto castedStatement = static_cast<const program::WhileStatement*>(statement);
            
            auto enclosingLoopsCopy = enclosingLoops;
            enclosingLoopsCopy.push_back(castedStatement);
            for (const auto& bodyStatement : castedStatement->bodyStatements)
            {
                addEnclosingLoopsForStatement(bodyStatement.get(), enclosingLoopsCopy);
            }
        }
    }
}

