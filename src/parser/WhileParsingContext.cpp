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

}
