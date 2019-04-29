#ifndef __PropertiesTime__
#define __PropertiesTime__

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Formula.hpp"
#include "Sort.hpp"
#include "Term.hpp"
#include "Expression.hpp"
#include "Variable.hpp"
#include "Program.hpp"
#include "ProgramTraverser.hpp"
#include "Problem.hpp"

namespace analysis {
    
    std::vector<std::shared_ptr<const logic::ProblemItem>> generateTraceLemmas(const program::Program& program,
                                                                         std::unordered_map<std::string, std::vector<std::shared_ptr<const program::Variable>>> locationToActiveVars,
                                                                         bool twoTraces);
}

#endif

