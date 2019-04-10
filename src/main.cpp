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
                auto inputFileWithoutExtension = inputFile.substr(0,inputFile.size()-extension.size());

                // parse inputFile
                auto parserResult = parser::parse(inputFile);
                
                // generate problem
                std::vector<std::shared_ptr<const logic::ProblemItem>> problemItems;
                
                analysis::Semantics s(*parserResult.program, parserResult.locationToActiveVars, parserResult.twoTraces);
                auto semantics = s.generateSemantics();
                
                for (const auto& axiom : semantics)
                {
                    problemItems.push_back(std::make_shared<logic::Axiom>(axiom));
                }

                auto traceLemmas = analysis::generateTraceLemmas(*parserResult.program, parserResult.locationToActiveVars, parserResult.twoTraces);

                analysis::StaticAnalysisLemmas staticAnalysisLemmas(*parserResult.program, parserResult.locationToActiveVars, parserResult.twoTraces);
                staticAnalysisLemmas.generateFormulas(traceLemmas);

                for (const auto& lemma : traceLemmas)
                {
                    problemItems.push_back(std::make_shared<logic::Lemma>(lemma));
                }
                
                analysis::TheoryAxioms theoryAxiomsGenerator;
                auto theoryAxioms = theoryAxiomsGenerator.generate();
               
                for (const auto& axiom : theoryAxioms)
                {
                    problemItems.push_back(std::make_shared<logic::Axiom>(axiom));
                }
                
                problemItems.insert(problemItems.end(), parserResult.problemItems.begin(), parserResult.problemItems.end());
                
                logic::Problem problem(problemItems);
                
                // generate reasoning tasks, convert each reasoning task to smtlib, and output it to output-file
                auto tasks = problem.generateReasoningTasks();
                
                std::ofstream outfile (inputFileWithoutExtension + "-conj.smt2");
                
                if(!util::Configuration::instance().generateBenchmark().getValue())
                {
                    outfile << util::Output::comment;
                    outfile << *parserResult.program;
                    outfile << util::Output::nocomment;
                }
                
                // only output the final task for now.
                tasks.back().outputSMTLIB(outfile);
            }
        }
        return 0;
    }
}
