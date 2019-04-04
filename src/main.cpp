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
#include "analysis/TheoryAxioms.hpp"

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
                
                // check that inputFile ends in ".spec"
                std::string extension = ".spec";
                assert(inputFile.size() > extension.size());
                assert(inputFile.compare(inputFile.size()-extension.size(), extension.size(),extension) == 0);
                
                // setup file for output
                auto inputFileWithoutExtension = inputFile.substr(0,inputFile.size()-extension.size());
                std::ofstream outfile (inputFileWithoutExtension + "-conj.smt2");
                
                // parse inputFile
                auto parserResult = parser::parse(inputFile);
                
                if(!util::Configuration::instance().generateBenchmark().getValue())
                {
                    outfile << util::Output::comment;
                    outfile << *parserResult.program;
                    outfile << util::Output::nocomment;
                }
                
                // generate problem
                logic::Problem problem;
                analysis::Semantics s(*parserResult.program, parserResult.locationToActiveVars, parserResult.twoTraces);
                
                problem.axioms = s.generateSemantics();
                problem.conjecture = std::move(parserResult.conjecture);
                
                problem.lemmas = analysis::generateTraceLemmas(*parserResult.program, parserResult.locationToActiveVars, parserResult.twoTraces);

                analysis::StaticAnalysisLemmas staticAnalysisLemmas(*parserResult.program, parserResult.locationToActiveVars, parserResult.twoTraces);
                // hack to work static analysis lemmas into output for now
                // we should decide if we want to add them to the 'normal' lemmas
                staticAnalysisLemmas.generateFormulas(problem.lemmas);
                
                analysis::TheoryAxioms theoryAxioms;
                auto theoryAxiomLemmas = theoryAxioms.generate();
                problem.lemmas.insert(problem.lemmas.end(),theoryAxiomLemmas.begin(),theoryAxiomLemmas.end());

                // output problem
                problem.outputSMTLIB(outfile);
            }
        }
        return 0;
    }
}
