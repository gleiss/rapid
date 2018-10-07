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

#include "Term.hpp"
#include "Formula.hpp"
#include "Theory.hpp"

#include "Expression.hpp"
#include "Variable.hpp"
#include "Assignment.hpp"
#include "Program.hpp"

#include "Location.hpp"
#include "WhileParserPostComputation.hpp"


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
  FUNC          "func"
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
  ANDSMTLIB     "and"
  ORSMTLIB      "or"
  NOTSMTLIB     "not"
  IMPSMTLIB     "=>"
  FORALLSMTLIB  "forall"
  EXISTSSMTLIB  "exists"
  ASSERTNOT     "assert-not"
;
%token <std::string> ID "identifier"
%token <std::string> TYPE "type identifier"
%token <int> INTEGER "number"

%type < std::shared_ptr<const logic::Formula> > smtlib_conjecture
%type < std::vector<std::shared_ptr<const logic::Formula>> > smtlib_formula_list
%type < std::shared_ptr<const logic::Formula> > smtlib_formula
%type < std::vector<std::shared_ptr<const logic::LVariable>> > smtlib_quantvar_list
%type < std::shared_ptr<const logic::LVariable> > smtlib_quantvar
%type < std::vector<std::shared_ptr<const logic::Term>> > smtlib_term_list
%type < std::shared_ptr<const logic::Term> > smtlib_term


%type < std::shared_ptr<const program::Program> > program
%type < std::vector< std::shared_ptr<const program::Function>> > function_list
%type < std::shared_ptr<const program::Function> > function

%type < std::pair<std::vector< std::shared_ptr<const program::IntVariable>>, std::vector< std::shared_ptr<const program::IntArrayVariable>>> > var_definition_list
%type < std::shared_ptr<const program::IntVariable> > int_var_definition
%type < std::shared_ptr<const program::IntArrayVariable> > int_array_var_definition

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

%start problem;

problem:
  program smtlib_conjecture {}
;

program:
  function_list 
  { 
    context.program = std::unique_ptr<const program::Program>(new program::Program($1)); 
    context.programGlobalProperties = WhileParserPostComputation::compute(*context.program);
  }
;

smtlib_conjecture:
  LPAR ASSERTNOT smtlib_formula RPAR {context.conjecture = std::move($3);}
;

smtlib_formula_list:
  %empty {$$ = std::vector<std::shared_ptr<const logic::Formula>>();}
| smtlib_formula_list smtlib_formula {$1.push_back(std::move($2)); $$ = std::move($1);}
;

smtlib_formula:
LPAR ID smtlib_term_list RPAR                { $$ = logic::Formulas::predicate($2, std::move($3));}
| TRUE                                       { $$ = logic::Theory::boolTrue();}
| FALSE                                      { $$ = logic::Theory::boolFalse();}
| LPAR ASSIGN smtlib_term smtlib_term RPAR       { $$ = logic::Formulas::equality(std::move($3), std::move($4));}
| LPAR GT smtlib_term smtlib_term RPAR       { $$ = logic::Theory::intLess(std::move($3), std::move($4));}
| LPAR GE smtlib_term smtlib_term RPAR       { $$ = logic::Theory::intLess(std::move($3), std::move($4));}
| LPAR LT smtlib_term smtlib_term RPAR       { $$ = logic::Theory::intLess(std::move($3), std::move($4));}
| LPAR LE smtlib_term smtlib_term RPAR       { $$ = logic::Theory::intLess(std::move($3), std::move($4));}
| LPAR ANDSMTLIB smtlib_formula_list RPAR    { $$ = logic::Formulas::conjunction(std::move($3));}
| LPAR ORSMTLIB smtlib_formula_list RPAR     { $$ = logic::Formulas::disjunction(std::move($3));}
| LPAR NOT smtlib_formula RPAR               { $$ = logic::Formulas::negation(std::move($3));}
| LPAR IMPSMTLIB smtlib_formula smtlib_formula RPAR  { $$ = logic::Formulas::implication(std::move($3), std::move($4));}
| LPAR FORALLSMTLIB LPAR smtlib_quantvar_list RPAR smtlib_formula RPAR       { $$ = logic::Formulas::universal(std::move($4), std::move($6));}
| LPAR EXISTSSMTLIB LPAR smtlib_quantvar_list RPAR smtlib_formula RPAR       { $$ = logic::Formulas::existential(std::move($4), std::move($6));}
;

smtlib_quantvar_list:
  smtlib_quantvar {auto vec = std::vector<std::shared_ptr<const logic::LVariable>>(); vec.push_back(std::move($1)); $$ = std::move(vec);}
| smtlib_quantvar_list smtlib_quantvar {$1.push_back(std::move($2)); $$ = std::move($1);}
;

smtlib_quantvar:
  LPAR ID TYPE RPAR 
  { 
    if($3 == "Int")
    { 
      $$ = logic::Terms::var($2, logic::Sorts::intSort());
    }
    else if($3 == "Bool")
    {
      $$ = logic::Terms::var($2, logic::Sorts::boolSort());
    }
    else
    {
      if($3 != "Time")
      {
        error(@3, "Currently only the sorts Int, Bool and Time are supported");
      }
      $$ = logic::Terms::var($2, logic::Sorts::timeSort());
    }
  }
;

smtlib_term_list:
  %empty {$$ = std::vector<std::shared_ptr<const logic::Term>>();}
| smtlib_term_list smtlib_term {$1.push_back(std::move($2)); $$ = std::move($1);}
;

smtlib_term:
  ID                                      {auto symbol = logic::Signature::fetch($1); $$ = logic::Terms::func(symbol, std::vector<std::shared_ptr<const logic::Term>>());}
| INTEGER                                 {$$ = logic::Theory::intConstant($1);}
| LPAR ID smtlib_term_list RPAR           {auto symbol = logic::Signature::fetch($2); $$ = logic::Terms::func(symbol, std::move($3));}
| LPAR PLUS  smtlib_term smtlib_term RPAR {$$ = logic::Theory::intAddition(std::move($3), std::move($4));}
| LPAR MINUS smtlib_term smtlib_term RPAR {$$ = logic::Theory::intSubtraction(std::move($3), std::move($4));}
| LPAR MUL   smtlib_term smtlib_term RPAR {$$ = logic::Theory::intMultiplication(std::move($3), std::move($4));}
| LPAR MINUS smtlib_term RPAR             {$$ = logic::Theory::intUnaryMinus(std::move($3));}
;

function_list:
  function             	  {auto v = std::vector< std::shared_ptr<const program::Function>>(); v.push_back(std::move($1)); $$ = std::move(v);}
| function_list function  {$1.push_back(std::move($2)); $$ = std::move($1);}
;

function:
  FUNC ID LPAR RPAR LCUR var_definition_list statement_list RCUR 
  {
  	$$ = std::shared_ptr<const program::Function>(new program::Function($2, std::move($6.first), std::move($6.second), std::move($7)));
  }
;

var_definition_list:
  %empty {$$ = std::pair<std::vector<std::shared_ptr<const program::IntVariable>>, std::vector<std::shared_ptr<const program::IntArrayVariable>>>({},{});}
| var_definition_list int_var_definition 
  {  
    for(const auto& var : $1.first)
    {
      if(var->name == $2->name)
      {
        error(@2, "Variable with name " + var->name + " is already defined.");
      }
    }
    for(const auto& var : $1.second)
    {
      if(var->name == $2->name)
      {
        error(@2, "Array variable with name " + var->name + " is already defined.");
      }
    }
    // add the symbol after error handling
    $2->addSymbolToSignature();
    $1.first.push_back(std::move($2)); $$ = std::move($1);
  }
| var_definition_list int_array_var_definition
  {  
    for(const auto& var : $1.first)
    {
      if(var->name == $2->name)
      {
        error(@2, "Variable with name " + var->name + " is already defined.");
      }
    }
    for(const auto& var : $1.second)
    {
      if(var->name == $2->name)
      {
        error(@2, "Array variable with name " + var->name + " is already defined.");
      }
    }
    $1.second.push_back(std::move($2)); $$ = std::move($1);
  }
;

int_var_definition:
  TYPE ID SCOL {$$ = std::shared_ptr<const program::IntVariable>(new program::IntVariable($2));}
;

int_array_var_definition:
  TYPE LBRA RBRA ID SCOL {$$ = std::shared_ptr<const program::IntArrayVariable>(new program::IntArrayVariable($4));}
;

statement_list:
  %empty {$$ = std::vector<std::shared_ptr<const program::Statement>>();}
| statement_list assignment_statement {$1.push_back(std::move($2)); $$ = std::move($1);}
| statement_list if_else_statement {$1.push_back(std::move($2)); $$ = std::move($1);}
| statement_list while_statement {$1.push_back(std::move($2)); $$ = std::move($1);}
| statement_list skip_statement {$1.push_back(std::move($2)); $$ = std::move($1);}
;

assignment_statement:
  location ASSIGN expr SCOL {$$ = std::shared_ptr<const program::IntAssignment>(new program::IntAssignment(@2.begin.line, std::move($1), std::move($3)));}
;

if_else_statement:
  IF LPAR formula RPAR LCUR statement_list RCUR ELSE LCUR statement_list RCUR 
  {$$ = std::shared_ptr<const program::IfElse>(new program::IfElse(@1.begin.line, std::move($3), std::move($6), std::move($10)));}
;

while_statement:
  WHILE formula LCUR statement_list RCUR {$$ = std::shared_ptr<const program::WhileStatement>(new program::WhileStatement(@1.begin.line, std::move($2), std::move($4)));}
;

skip_statement:
  SKIP {$$ = std::shared_ptr<const program::SkipStatement>(new program::SkipStatement(@1.begin.line));}
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
  ID                
  { 
  	// TODO: add check that variable exists
  	$$ = std::shared_ptr<const program::IntVariable>(new program::IntVariable($1));
  }
| ID LBRA expr RBRA 
  { 
  	// TODO: add check that variable exists
	auto formula = std::shared_ptr<const program::IntArrayVariable>(new IntArrayVariable($1));
	$$ = std::shared_ptr<const program::IntArrayApplication>(new IntArrayApplication(std::move(formula), std::move($3)));
  }
;

%%
void parser::WhileParser::error(const location_type& l,
                              const std::string& m)
{
  std::cout << "Error while parsing location " << l << ":\n" << m << std::endl;
  context.errorFlag = true;
}

