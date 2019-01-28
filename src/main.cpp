#include <fstream>
#include <iostream>
#include <string>

#include "logic/Theory.hpp"
#include "logic/Problem.hpp"

#include "program/Program.hpp"

#include "util/Options.hpp"
#include "util/Output.hpp"

#include "parser/WhileParserWrapper.hpp"

#include "analysis/Semantics.hpp"
#include "analysis/TraceLemmas.hpp"
#include "analysis/StaticAnalysis.hpp"

void outputUsage()
{
    std::cout << "Usage: spectre <filename>" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        outputUsage();
    }
    else
    {
        if (util::Configuration::instance().setAllValues(argc, argv))
        {
            if (util::Output::initialize())
            {
                std::string inputFile = argv[argc - 1];
                
                auto parserResult = parser::parse(inputFile);
                
                logic::Problem problem;
                analysis::Semantics s(*parserResult.program, parserResult.locationToActiveVars, parserResult.twoTraces);
                
                problem.axioms = s.generateSemantics();
                problem.conjecture = std::move(parserResult.conjecture);
                
                analysis::TraceLemmas traceLemmas(*parserResult.program, parserResult.locationToActiveVars, parserResult.twoTraces);
                problem.lemmas = traceLemmas.generate();

                analysis::StaticAnalysis staticAnalysis(*parserResult.program, parserResult.locationToActiveVars, parserResult.twoTraces);
                // hack to work static analysis lemmas into output for now
                // we should decide if we want to add them to the 'normal' lemmas
                auto staticAnalysisLemmas = staticAnalysis.generateStaticAnalysisLemmas();
                problem.lemmas.insert(problem.lemmas.end(),staticAnalysisLemmas.begin(),staticAnalysisLemmas.end());
                
                problem.outputSMTLIB(util::Output::stream());
            }
        }
        return 0;
    }
}
