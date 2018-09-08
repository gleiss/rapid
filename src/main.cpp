#include <fstream>
#include <iostream>
#include <string>

#include "logic/Theory.hpp"
#include "logic/Problem.hpp"

#include "parser/WhileParsingContext.hpp"
#include "util/Options.hpp"
#include "util/Output.hpp"

#include "parser/WhileParser.hpp"

#include "analysis/Semantics.hpp"
#include "analysis/Analyzer.hpp"
#include "analysis/Properties.hpp"
#include "analysis/PropertiesTime.hpp"

#include "program/Program.hpp"

extern FILE *yyin;
extern bool yy_flex_debug;

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

                // test readbility, easier than catching exception thrown by parser
                std::ifstream istr(inputFile);
                if (!istr)
                {
                    std::cerr << "Unable to read file " << inputFile << std::endl;
                    return 0;
                }

                std::shared_ptr<const program::Program> p;
                if (true)
                {
                    // set input for parser
                    // TODO: remove double checking of file,move setting yyin into parser?
                    const char *fname = inputFile.c_str();
                    yy_flex_debug = false;
                    yyin = fopen(fname, "r");

                    // generate a context, whose fields are used as in/out-parameters for parsing
                    parser::WhileParsingContext context;
                    context.inputFile = inputFile;

                    // parse the input-program into context
                    parser::WhileParser parser(context);
                    parser.set_debug_level(false); // no traces
                    parser.parse();
                    fclose(yyin);

                    if (!context.errorFlag)
                    {
                        p = context.program;
                    }
                    else
                    {
                        exit(1);
                    }
                }
                assert(p);
                util::Output::stream() << util::Output::comment;
                util::Output::stream() << *p;
                util::Output::stream() << util::Output::nocomment;

                logic::Problem problem;

                analysis::Semantics s;
                auto semantics = s.generateSemantics(*p);
                problem.axioms = {std::make_pair("semantics", semantics)};
                problem.conjecture = logic::Theory::boolTrue();
                problem.outputSMTLIB(util::Output::stream());
                
//                auto map = s.computeEndLocations(*p);
//                for (const auto& pair : map)
//                {
//                    util::Output::stream() << pair.first->location << ", " << pair.second << "\n";
//                }
//                // run lightweight analysis
//                program::Analyzer a(*p);
//                program::AnalyzerResult aRes = a.computeVariableProperties();
//
//                util::Output::stream() << util::Output::comment;
//                util::Output::stream() << aRes.toString();
//                util::Output::stream() << util::Output::nocomment;
//
//                // create properties and dump them to SMTLIB
//                if (!util::Configuration::instance().timepoints().getValue())
//                {
//                    program::Properties props(*p, aRes);
//                    props.analyze();
//                    props.output();
//                }
//                else
//                {
//                    program::PropertiesTime props(*p, aRes);
//                    props.analyze();
//                    props.output();
//                }
            }
        }
        return 0;
    }
}
