#include <fstream>
#include <iostream>
#include <string>

#include "logic/Theory.hpp"
#include "logic/Problem.hpp"

#include "parser/WhileParsingContext.hpp"
#include "util/Options.hpp"
#include "util/Output.hpp"

#include "parser/WhileScanner.cpp"
#include "parser/WhileParser.hpp"

#include "analysis/Semantics.hpp"
#include "analysis/Analyzer.hpp"
#include "analysis/Properties.hpp"
#include "analysis/PropertiesTime.hpp"

#include "program/Program.hpp"

extern FILE *yyin;

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
                
                yy_flex_debug = false;
                
                // set parser input to inputFile
                FILE* f = fopen(inputFile.c_str(), "r");
                yy_buffer_state*bp = yy_create_buffer(f, YY_BUF_SIZE );
                yy_create_buffer(f,YY_BUF_SIZE);
                yy_switch_to_buffer(bp);
                
                // generate a context, whose fields are used as in/out-parameters for parsing
                parser::WhileParsingContext context;
                context.inputFile = inputFile;
                
                // parse the input-program into context
                parser::WhileParser parser(context);
                parser.set_debug_level(false); // no traces
                parser.parse();
                fclose(f);
                
                if (context.errorFlag)
                {
                    exit(1);
                }

                assert(context.program);
                assert(context.programGlobalProperties);
                assert(context.conjecture);

                util::Output::stream() << util::Output::comment;
                util::Output::stream() << *context.program;
                util::Output::stream() << util::Output::nocomment;

                logic::Problem problem;

                analysis::Semantics s(*context.program, *context.programGlobalProperties);
                
                problem.axioms = {s.generateSemantics()};
                problem.conjecture = std::move(context.conjecture);
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
