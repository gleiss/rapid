%{
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <exception>
#include "WhileParser.hpp"
#include "WhileParsingContext.hpp"

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.
# undef yywrap
# define yywrap() 1

// The location of the current token.
static parser::Location loc;

void error(const parser::Location& l,
           const std::string& m)
{
    std::cout << l << m << std::endl;
    exit(1);
}

%}
%option noyywrap nounput batch debug noinput
IDENT [a-z][a-zA-Z_0-9]*
NUM   [0-9]+
BLANK [ \t]

%{
#include "WhileParsingContext.hpp"
using namespace program;
// Tell Flex the lexer's prototype ...
# define YY_DECL parser::WhileParser::symbol_type yylex(parser::WhileParsingContext &context)
// ... and declare it for the parser's sake.
YY_DECL;
%}

%{
  // Code run each time a pattern is matched.
# define YY_USER_ACTION  loc.columns (yyleng);
%}

%s programstate smtlibstate

%%

%{
  // Code run each time yylex is called.
  loc.step();
%}

"//".*       { loc.step(); }
{BLANK}+     { loc.step(); }
[\n]+        { loc.lines(yyleng); loc.step(); }

"set-traces" { return parser::WhileParser::make_SETTRACES(loc); }
"="          { return parser::WhileParser::make_ASSIGN(loc); }
if           { return parser::WhileParser::make_IF(loc); }
else         { return parser::WhileParser::make_ELSE(loc); }
while        { return parser::WhileParser::make_WHILE(loc); }
skip         { return parser::WhileParser::make_SKIP(loc); }
func         { BEGIN(programstate); return parser::WhileParser::make_FUNC(loc);}
const        { return parser::WhileParser::make_CONST(loc); }

and         	{ return parser::WhileParser::make_ANDSMTLIB(loc); }
or         		{ return parser::WhileParser::make_ORSMTLIB(loc); }
not         	{ return parser::WhileParser::make_NOTSMTLIB(loc); }
"=>"         	{ return parser::WhileParser::make_IMPSMTLIB(loc); }
forall      	{ return parser::WhileParser::make_FORALLSMTLIB(loc); }
exists      	{ return parser::WhileParser::make_EXISTSSMTLIB(loc); }
"axiom"       { BEGIN(smtlibstate); return parser::WhileParser::make_AXIOM(loc);}
"lemma"       { BEGIN(smtlibstate); return parser::WhileParser::make_LEMMA(loc);}
"conjecture"  { BEGIN(smtlibstate); return parser::WhileParser::make_CONJECTURE(loc);}

"("          { return parser::WhileParser::make_LPAR(loc); }
")"          { return parser::WhileParser::make_RPAR(loc); }
"["          { return parser::WhileParser::make_LBRA(loc); }
"]"          { return parser::WhileParser::make_RBRA(loc); }
"{"          { return parser::WhileParser::make_LCUR(loc); }
"}"          { return parser::WhileParser::make_RCUR(loc); }

";"          { return parser::WhileParser::make_SCOL(loc); }

"*"          { return parser::WhileParser::make_MUL(loc); }
"+"          { return parser::WhileParser::make_PLUS(loc); }
"-"          { return parser::WhileParser::make_MINUS(loc); }
mod          { return parser::WhileParser::make_MOD(loc); }
">"          { return parser::WhileParser::make_GT(loc); }
">="         { return parser::WhileParser::make_GE(loc); }
"<"          { return parser::WhileParser::make_LT(loc); }
"<="         { return parser::WhileParser::make_LE(loc); }
"=="         { return parser::WhileParser::make_EQ(loc); }
"!="         { return parser::WhileParser::make_NEQ(loc); }
"||"         { return parser::WhileParser::make_OR(loc); }
"&&"         { return parser::WhileParser::make_AND(loc); }
"!"          { return parser::WhileParser::make_NOT(loc); }

"true"       { return parser::WhileParser::make_TRUE(loc); }
"false"      { return parser::WhileParser::make_FALSE(loc); }

"Int"|"Bool"|"Nat"|"Time"|"Trace" { return parser::WhileParser::make_TYPE(yytext, loc); }

<programstate>{IDENT}     { return parser::WhileParser::make_PROGRAM_ID(yytext, loc); }
<smtlibstate>{IDENT}      { return parser::WhileParser::make_SMTLIB_ID(yytext, loc); }
{NUM}        {
  errno = 0;
  long n = strtol (yytext, NULL, 10);
  if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
    error(loc, "integer out of range");
  return parser::WhileParser::make_INTEGER(n, loc);
}
.            { error(loc, "invalid character");}
<<EOF>>      { return parser::WhileParser::make_END(loc); }

%%

