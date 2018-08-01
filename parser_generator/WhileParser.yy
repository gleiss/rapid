%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines
%define api.namespace {parser}
%define parser_class_name {WhileParser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%code requires
{
#include <cstring>
#include <iostream>
#include <vector>
#include <memory>

#include "Expression.hpp"
#include "Variable.hpp"
#include "Assignment.hpp"
#include "Program.hpp"

#include "Location.hpp"


#define YY_NULLPTR nullptr


namespace parser {
  class WhileParsingContext;
}
}


// The parsing context.
%param { parser::WhileParsingContext &context }
%locations
%define api.location.type {Location}
%initial-action
{
  // Initialize the initial location.
  @$.begin.filename = @$.end.filename = &context.inputFile;
};
%define parse.trace
%define parse.error verbose
%code
{
#include "WhileParsingContext.hpp"

using namespace program;

// Tell Flex the lexer's prototype ...
# define YY_DECL parser::WhileParser::symbol_type yylex(parser::WhileParsingContext &context)
// ... and declare it for the parser's sake.
YY_DECL;

}
%define api.token.prefix {TOK_}
%token
  END 0         "EOF"
  TRUE          "true"
  FALSE         "false"
  ASSIGN        "="
  IF            "if"
  ELSE          "else"
  WHILE         "while"
  SKIP          "skip"
  LPAR          "("
  RPAR          ")"
  LBRA          "["
  RBRA          "]"
  LCUR          "{"
  RCUR          "}"
  SCOL          ";"
  NOT           "!"
  MUL           "*"
  PLUS          "+"
  MINUS         "-"
  GT            ">"
  GE            ">="
  LT            "<"
  LE            "<="
  EQ            "=="
  NEQ           "!="
  OR            "||"
  AND           "&&"
;
%token <std::string> ID "identifier"
%token <std::string> TYPE "type identifier"
%token <int> INTEGER "number"

%type < std::shared_ptr<const program::Program> > program
%type < std::vector< std::shared_ptr<const program::Function>> > function_list
%type < std::shared_ptr<const program::Function> > function

%type < std::vector< std::shared_ptr<const program::IntVariable>> > var_definition_list
%type < std::shared_ptr<const program::IntVariable> > var_definition

%type < std::vector<std::shared_ptr<const program::Statement>> > statement_list
%type < std::shared_ptr<const program::IntAssignment> > assignment_statement
%type < std::shared_ptr<const program::IfElse> > if_else_statement
%type < std::shared_ptr<const program::WhileStatement> > while_statement
%type < std::shared_ptr<const program::SkipStatement> > skip_statement

%type < std::shared_ptr<const program::BoolExpression> > formula
%type < std::shared_ptr<const program::IntExpression> > expr
%type < std::shared_ptr<const program::IntExpression> > location

%printer { yyoutput << $$; } <*>;

%right ASSIGN
%left  AND
%left  OR
%left  EQ NEQ
%left  GT GE LT LE
%left  PLUS MINUS
%left  MUL DIV
%right NOT UMINUS

%%

%start program;

program:
  function_list { context.program = std::shared_ptr<const program::Program>(new program::Program($1));}
;

function_list:
  %empty                  {$$ = std::vector< std::shared_ptr<const program::Function>>();}
| function_list function  {$1.push_back(std::move($2)); $$ = std::move($1);}
;

function:
  ID LPAR RPAR LCUR var_definition_list statement_list RCUR 
  {
    for(const auto& var : $5)

    $$ = std::shared_ptr<const program::Function>(new program::Function($1, std::move($5), std::move($6)));
  }
;

var_definition_list:
  %empty {$$ = std::vector<std::shared_ptr<const program::IntVariable>>();}
| var_definition_list var_definition 
  {  
    for(const auto& var : $1)
    {
      if(var->name == $2->name)
      {
        error(@2, "Variable with name " + var->name + " is already defined.");
      }
    }
    $1.push_back(std::move($2)); $$ = std::move($1);
  }
;

var_definition:
  TYPE ID SCOL {$$ = std::shared_ptr<const program::IntVariable>(new program::IntVariable($2));}
;

statement_list:
  %empty {$$ = std::vector<std::shared_ptr<const program::Statement>>();}
| statement_list assignment_statement {$1.push_back(std::move($2)); $$ = std::move($1);}
| statement_list if_else_statement {$1.push_back(std::move($2)); $$ = std::move($1);}
| statement_list while_statement {$1.push_back(std::move($2)); $$ = std::move($1);}
| statement_list skip_statement {$1.push_back(std::move($2)); $$ = std::move($1);}
;

assignment_statement:
  location ASSIGN expr SCOL { $$ = std::shared_ptr<const program::IntAssignment>(new program::IntAssignment(std::move($1), std::move($3)));}
;

if_else_statement:
  IF LPAR formula RPAR LCUR statement_list RCUR ELSE LCUR statement_list RCUR 
  {$$ = std::shared_ptr<const program::IfElse>(new program::IfElse(std::move($3), std::move($6), std::move($10)));}
;

while_statement:
  WHILE formula LCUR statement_list RCUR {$$ = std::shared_ptr<const program::WhileStatement>(new program::WhileStatement(std::move($2), std::move($4)));}
;

skip_statement:
  SKIP {$$ = std::shared_ptr<const program::SkipStatement>(new program::SkipStatement());}
;

formula:
  LPAR formula RPAR        { $$ = std::move($2); }
| TRUE                     { $$ = std::shared_ptr<const program::BooleanConstant>(new program::BooleanConstant(true)); }
| FALSE                    { $$ = std::shared_ptr<const program::BooleanConstant>(new program::BooleanConstant(false)); }
| expr GT expr             { $$ = std::shared_ptr<const program::ArithmeticComparison>(new program::ArithmeticComparison(program::ArithmeticComparison::Kind::GT, std::move($1), std::move($3)));}
| expr GE expr             { $$ = std::shared_ptr<const program::ArithmeticComparison>(new program::ArithmeticComparison(program::ArithmeticComparison::Kind::GE, std::move($1), std::move($3)));}
| expr LT expr             { $$ = std::shared_ptr<const program::ArithmeticComparison>(new program::ArithmeticComparison(program::ArithmeticComparison::Kind::LT, std::move($1), std::move($3)));}
| expr LE expr             { $$ = std::shared_ptr<const program::ArithmeticComparison>(new program::ArithmeticComparison(program::ArithmeticComparison::Kind::LE, std::move($1), std::move($3)));}
| expr EQ expr             { $$ = std::shared_ptr<const program::ArithmeticComparison>(new program::ArithmeticComparison(program::ArithmeticComparison::Kind::EQ, std::move($1), std::move($3)));}
| expr NEQ expr            { auto formula = std::shared_ptr<const program::ArithmeticComparison>(new program::ArithmeticComparison(program::ArithmeticComparison::Kind::EQ, std::move($1), std::move($3)));
                             $$ = std::shared_ptr<const program::BooleanNot>(new program::BooleanNot(std::move(formula)));}
| formula AND formula      { $$ = std::shared_ptr<const program::BooleanAnd>(new program::BooleanAnd(std::move($1), std::move($3)));}
| formula OR formula       { $$ = std::shared_ptr<const program::BooleanOr>(new program::BooleanOr(std::move($1), std::move($3)));}
| NOT formula              { $$ = std::shared_ptr<const program::BooleanNot>(new program::BooleanNot(std::move($2)));}
;

expr:
  LPAR expr RPAR           { $$ = std::move($2); }
| location                 { $$ = std::move($1); }
| INTEGER                  { $$ = std::shared_ptr<const program::ArithmeticConstant>(new program::ArithmeticConstant(std::move($1)));}
| expr MUL expr            { $$ = std::shared_ptr<const program::Multiplication>(new program::Multiplication(std::move($1),std::move($3)));}
| expr PLUS expr           { $$ = std::shared_ptr<const program::Addition>(new program::Addition(std::move($1),std::move($3)));}
| expr MINUS expr          { $$ = std::shared_ptr<const program::Subtraction>(new program::Subtraction(std::move($1),std::move($3)));}
| MINUS expr %prec UMINUS  { $$ = std::shared_ptr<const program::UnaryMinus>(new program::UnaryMinus(std::move($2)));}
;

location:
  ID                { $$ = std::shared_ptr<const program::IntVariable>(new program::IntVariable($1));}
| ID LBRA expr RBRA { auto formula = std::shared_ptr<const program::IntArrayVariable>(new IntArrayVariable($1));
                      $$ = std::shared_ptr<const program::IntArrayApplication>(new IntArrayApplication(std::move(formula), std::move($3)));}
;

%%
void parser::WhileParser::error(const location_type& l,
                              const std::string& m)
{
  std::cout << "Error while parsing location " << l << ":\n" << m << std::endl;
  context.errorFlag = true;
}

