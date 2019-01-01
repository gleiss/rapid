#include "WhileParsingContext.hpp"

namespace parser
{
    bool WhileParsingContext::pushQuantifiedVars(std::vector<const logic::Symbol*> quantifiedVars)
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
    
    const logic::Symbol* WhileParsingContext::fetch(std::string name)
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

}
