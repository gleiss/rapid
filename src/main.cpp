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
                
                analysis::Semantics s(*parserResult.program, parserResult.locationToActiveVars);
                
                problem.axioms = s.generateSemantics();
                problem.conjecture = std::move(parserResult.conjecture);
                
                analysis::TraceLemmas traceLemmas(*parserResult.program, parserResult.locationToActiveVars, parserResult.twoTraces);
                problem.lemmas = traceLemmas.generate();
                
                problem.outputSMTLIB(util::Output::stream());
            }
        }
        return 0;
    }
}
