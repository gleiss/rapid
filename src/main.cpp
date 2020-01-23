#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "logic/Theory.hpp"
#include "logic/Problem.hpp"

#include "program/Program.hpp"

#include "util/Options.hpp"
#include "util/Output.hpp"

#include "parser/WhileParserWrapper.hpp"

#include "analysis/Semantics.hpp"
#include "analysis/TraceLemmas.hpp"
#include "analysis/TheoryAxioms.hpp"

void outputUsage()
{
    std::cout << "Usage: rapid -dir <outputDir> <filename>" << std::endl;
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
                auto inputFileWithoutExtension = inputFile.substr(0,inputFile.size()-extension.size());

                // parse inputFile
                auto parserResult = parser::parse(inputFile);
                
                // setup outputDir
                auto outputDir = util::Configuration::instance().outputDir().getValue();
                if (outputDir == "")
                {
                    std::cout << "Error: dir parameter required" << std::endl;
                    exit(1);
                }
                
                // generate problem
                std::vector<std::shared_ptr<const logic::ProblemItem>> problemItems;
                
                analysis::TheoryAxioms theoryAxiomsGenerator;
                auto theoryAxioms = theoryAxiomsGenerator.generate();
                for (const auto& axiom : theoryAxioms)
                {
                    problemItems.push_back(axiom);
                }

                analysis::Semantics s(*parserResult.program, parserResult.locationToActiveVars, parserResult.problemItems, parserResult.twoTraces);
                auto semantics = s.generateSemantics();
                problemItems.insert(problemItems.end(), semantics.begin(), semantics.end());

                auto traceLemmas = analysis::generateTraceLemmas(*parserResult.program, parserResult.locationToActiveVars, parserResult.twoTraces, semantics);
                problemItems.insert(problemItems.end(), traceLemmas.begin(), traceLemmas.end());
                

                
                problemItems.insert(problemItems.end(), parserResult.problemItems.begin(), parserResult.problemItems.end());
                
                logic::Problem problem(problemItems);
                
                // generate reasoning tasks, convert each reasoning task to smtlib, and output it to output-file
                auto tasks = problem.generateReasoningTasks();
                for (const auto& task : tasks)
                {
                    std::stringstream preamble;
                    preamble << util::Output::comment << *parserResult.program << util::Output::nocomment;
                    task.outputSMTLIBToDir(outputDir, preamble.str());
                }
            }
        }
        return 0;
    }
}
