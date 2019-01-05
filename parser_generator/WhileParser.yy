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
#include "Statements.hpp"
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
  CONST         "const"
  TWOTRACES     "(two-traces)"
;
%token <std::string> PROGRAM_ID "program identifier"
%token <std::string> SMTLIB_ID "smtlib identifier"
%token <std::string> TYPE "type identifier"
%token <int> INTEGER "number"

%type < std::shared_ptr<const logic::Formula> > smtlib_conjecture
%type < std::vector<std::shared_ptr<const logic::Formula>> > smtlib_formula_list
%type < std::shared_ptr<const logic::Formula> > smtlib_formula
%type < std::vector<std::shared_ptr<const logic::Symbol>> > smtlib_quantvar_list
%type < std::shared_ptr<const logic::Symbol> > smtlib_quantvar
%type < std::vector<std::shared_ptr<const logic::Term>> > smtlib_term_list
%type < std::shared_ptr<const logic::Term> > smtlib_term


%type < std::shared_ptr<const program::Program> > program
%type < std::vector< std::shared_ptr<const program::Function>> > function_list
%type < std::shared_ptr<const program::Function> > function

%type < std::shared_ptr<const program::Variable> > var_definition_head

%type < std::vector<std::shared_ptr<const program::Statement>> > statement_list
%type < std::shared_ptr<const program::Statement> > statement
%type < std::vector<std::shared_ptr<const program::Variable>> > active_vars_dummy
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
|
  TWOTRACES
  {
    context.twoTraces = true;
  }
  program smtlib_conjecture {}
;

program:
  function_list 
  { 
    context.program = std::unique_ptr<const program::Program>(new program::Program($1)); 
    context.programGlobalProperties = WhileParserPostComputation::compute(*context.program, context.twoTraces);
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
  TRUE                                       { $$ = logic::Theory::boolTrue();}
| FALSE                                      { $$ = logic::Theory::boolFalse();}
| LPAR ASSIGN smtlib_term smtlib_term RPAR   
  { 
    auto leftSort = $3->symbol->rngSort;
    auto rightSort = $4->symbol->rngSort;
    
    if(leftSort != rightSort)
    {
      error(@4, "Argument types " + leftSort->name + " and " + rightSort->name + " don't match!");
    }
    $$ = logic::Formulas::equality(std::move($3), std::move($4));
  }
| LPAR GT smtlib_term smtlib_term RPAR       
{
  if($3->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@3, "Left argument type needs to be Int");
  }
  if($4->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@4, "Right argument type needs to be Int");
  }
  $$ = logic::Theory::intGreater(std::move($3), std::move($4));
}
| LPAR GE smtlib_term smtlib_term RPAR       
{
  if($3->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@3, "Left argument type needs to be Int");
  }
  if($4->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@4, "Right argument type needs to be Int");
  } 
  $$ = logic::Theory::intGreaterEqual(std::move($3), std::move($4));
}
| LPAR LT smtlib_term smtlib_term RPAR      
{ 
  if($3->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@3, "Left argument type needs to be Int");
  }
  if($4->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@4, "Right argument type needs to be Int");
  } 
  $$ = logic::Theory::intLess(std::move($3), std::move($4));
}
| LPAR LE smtlib_term smtlib_term RPAR      
{ 
  if($3->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@3, "Left argument type needs to be Int");
  }
  if($4->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@4, "Right argument type needs to be Int");
  } 
  $$ = logic::Theory::intLessEqual(std::move($3), std::move($4));
}
| LPAR ANDSMTLIB smtlib_formula_list RPAR    { $$ = logic::Formulas::conjunction(std::move($3));}
| LPAR ORSMTLIB smtlib_formula_list RPAR     { $$ = logic::Formulas::disjunction(std::move($3));}
| LPAR NOTSMTLIB smtlib_formula RPAR         { $$ = logic::Formulas::negation(std::move($3));}
| LPAR IMPSMTLIB smtlib_formula smtlib_formula RPAR  { $$ = logic::Formulas::implication(std::move($3), std::move($4));}
| LPAR FORALLSMTLIB LPAR smtlib_quantvar_list RPAR 
  {
    // TODO: propagate existing-var-error to parser and raise error
    context.pushQuantifiedVars($4);
  } 
  smtlib_formula RPAR 
  { 
    context.popQuantifiedVars();
    $$ = logic::Formulas::universal(std::move($4), std::move($7));
  }
| LPAR EXISTSSMTLIB LPAR smtlib_quantvar_list RPAR 
  {
    // TODO: propagate existing-var-error to parser and raise error
    context.pushQuantifiedVars($4);
  } 
  smtlib_formula RPAR 
  { 
    context.popQuantifiedVars();
    $$ = logic::Formulas::existential(std::move($4), std::move($7));
  }
;

smtlib_quantvar_list:
  smtlib_quantvar {auto vec = std::vector<std::shared_ptr<const logic::Symbol>>(); vec.push_back(std::move($1)); $$ = std::move(vec);}
| smtlib_quantvar_list smtlib_quantvar {$1.push_back(std::move($2)); $$ = std::move($1);}
;

smtlib_quantvar:
  LPAR SMTLIB_ID TYPE RPAR 
  { 
    if($3 == "Int")
    { 
      $$ = logic::Signature::varSymbol($2, logic::Sorts::intSort());
    }
    else if($3 == "Bool")
    {
      $$ = logic::Signature::varSymbol($2, logic::Sorts::boolSort());
    }
    else
    {
      if($3 != "Time")
      {
        error(@3, "Only the sorts Int, Bool and Time are supported");
      }
      $$ = logic::Signature::varSymbol($2, logic::Sorts::timeSort());
    }
  }
;

smtlib_term_list:
  smtlib_term {$$ = std::vector<std::shared_ptr<const logic::Term>>(); $$.push_back($1);}
| smtlib_term_list smtlib_term {$1.push_back(std::move($2)); $$ = std::move($1);}
;

smtlib_term:
SMTLIB_ID                               
{
  // std::cout << "parsing smtlib const " << std::string($1) << "\n"; 
  // TODO: propagate nonexisting-definition-error to parser and raise error
  auto symbol = context.fetch($1); 
  $$ = logic::Terms::func(symbol, std::vector<std::shared_ptr<const logic::Term>>());
}
| INTEGER                                 {$$ = logic::Theory::intConstant($1);}
| LPAR SMTLIB_ID smtlib_term_list RPAR    
{
  // std::cout << "parsing smtlib term " << std::string($2) << "\n"; 
  // TODO: propagate nonexisting-definition-error to parser and raise error
  // TODO: propagate wrong-argument-sort(s)-error to parser and raise error
  auto symbol = context.fetch($2); 
  $$ = logic::Terms::func(symbol, std::move($3));
}
| LPAR PLUS smtlib_term smtlib_term RPAR 
{
  if($3->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@3, "Left argument type needs to be Int");
  }
  if($4->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@4, "Right argument type needs to be Int");
  } 
  $$ = logic::Theory::intAddition(std::move($3), std::move($4));
}
| LPAR MINUS smtlib_term smtlib_term RPAR 
{
  if($3->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@3, "Left argument type needs to be Int");
  }
  if($4->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@4, "Right argument type needs to be Int");
  } 
  $$ = logic::Theory::intSubtraction(std::move($3), std::move($4));
}
| LPAR MUL smtlib_term smtlib_term RPAR 
{
  if($3->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@3, "Left argument type needs to be Int");
  }
  if($4->symbol->rngSort != logic::Sorts::intSort())
  {
    error(@4, "Right argument type needs to be Int");
  } 
  $$ = logic::Theory::intMultiplication(std::move($3), std::move($4));
}
| LPAR MINUS smtlib_term RPAR
  {
    if($3->symbol->rngSort != logic::Sorts::intSort())
    {
      error(@3, "Argument type needs to be Int");
    }
    $$ = logic::Theory::intUnaryMinus(std::move($3));
  }
;

function_list:
  function             	  {auto v = std::vector< std::shared_ptr<const program::Function>>(); v.push_back(std::move($1)); $$ = std::move(v);}
| function_list function  {$1.push_back(std::move($2)); $$ = std::move($1);}
;

function:
  FUNC PROGRAM_ID LPAR RPAR LCUR
  {
    context.pushProgramVars();
  }
  statement_list RCUR
  {
    auto functionEndLocationName =  $2 + "_end";
    context.locationToActiveVars[functionEndLocationName] = context.getActiveProgramVars();
    context.popProgramVars();
  	$$ = std::shared_ptr<const program::Function>(new program::Function($2, std::move($7)));
  }
;

statement_list:
  %empty {$$ = std::vector<std::shared_ptr<const program::Statement>>();}
| statement_list active_vars_dummy statement
  {
    auto locationName = $3->location;
    context.locationToActiveVars[locationName] = $2;
    $1.push_back(std::move($3)); $$ = std::move($1);
  }
| statement_list active_vars_dummy var_definition_head SCOL
  {
    // dummy is not used here, but silences a shift-reduce conflict
    context.addProgramVar($3);
    $3->addSymbolToSignature();
    $$ = std::move($1);
  }
;

statement:
  assignment_statement {$$ = std::move($1);}
| if_else_statement {$$ = std::move($1);}
| while_statement {$$ = std::move($1);}
| skip_statement {$$ = std::move($1);}
;

assignment_statement:
  location ASSIGN expr SCOL 
  {
    // TODO: check that location and expr have the same type (need refactoring first so that IntExpression and BoolExpression have a common superclass)

    if($1->type() == IntExpression::Type::IntVariableAccess)
    {
      auto intVariableAccess = std::static_pointer_cast<const program::IntVariableAccess>($1);
      if(intVariableAccess->var->isConstant)
      {
        error(@1, "Assignment to const var " + intVariableAccess->var->name);
      }
    }
    else
    {
      assert($1->type() == IntExpression::Type::IntArrayApplication);
      auto intArrayApplication = std::static_pointer_cast<const program::IntArrayApplication>($1);
      if(intArrayApplication->array->isConstant)
      {
        error(@1, "Assignment to const var " + intArrayApplication->array->name);
      }
    }
    $$ = std::shared_ptr<const program::IntAssignment>(new program::IntAssignment(@2.begin.line, std::move($1), std::move($3)));
  }
| var_definition_head ASSIGN expr SCOL
  {
    // declare var
    context.addProgramVar($1);
    $1->addSymbolToSignature();

    // construct location
    if($1->isArray)
    {
      error(@1, "Combined declaration and assignment not allowed, since " + $1->name + " is array variable");
    }
    auto intVariableAccess = std::shared_ptr<const program::IntVariableAccess>(new IntVariableAccess(std::move($1)));
   
    // build assignment
    $$ = std::shared_ptr<const program::IntAssignment>(new program::IntAssignment(@2.begin.line, std::move(intVariableAccess), std::move($3)));
  }
;

if_else_statement:
  IF LPAR formula RPAR 
  {
    context.pushProgramVars();
  }
  LCUR statement_list active_vars_dummy RCUR 
  {
    context.popProgramVars();
  }
  ELSE 
  {
    context.pushProgramVars();
  }  
  LCUR statement_list active_vars_dummy RCUR 
  {
    context.popProgramVars();

    auto leftEndLocationName = "l" + std::to_string(@1.begin.line) + "_lEnd";
    auto rightEndLocationName = "l" + std::to_string(@1.begin.line) + "_rEnd";
    context.locationToActiveVars[leftEndLocationName] = $8;
    context.locationToActiveVars[rightEndLocationName] = $15;
    $$ = std::shared_ptr<const program::IfElse>(new program::IfElse(@1.begin.line, std::move($3), std::move($7), std::move($14)));
  }
;

while_statement:
  WHILE formula 
  {
    context.pushProgramVars();
  }
  LCUR statement_list RCUR
  {
    context.popProgramVars();
    $$ = std::shared_ptr<const program::WhileStatement>(new program::WhileStatement(@1.begin.line, std::move($2), std::move($5)));
  }
;

skip_statement:
  SKIP {$$ = std::shared_ptr<const program::SkipStatement>(new program::SkipStatement(@1.begin.line));}
;

active_vars_dummy:
  %empty 
  {
    $$ = context.getActiveProgramVars(); 
  }
;

var_definition_head:
  TYPE PROGRAM_ID
  {
    // TODO: check that TYPE is not Time
    // TODO: support Bool or check that TYPE is not Bool
    $$ = std::shared_ptr<const program::Variable>(new program::Variable($2, false, false, context.twoTraces));
  }
| CONST TYPE PROGRAM_ID
  {
    // TODO: check that TYPE is not Time
    // TODO: support Bool or check that TYPE is not Bool
    $$ = std::shared_ptr<const program::Variable>(new program::Variable($3, true, false, context.twoTraces));
  }
| TYPE LBRA RBRA PROGRAM_ID
  {
    // TODO: check that TYPE is not Time
    // TODO: support Bool or check that TYPE is not Bool
    $$ = std::shared_ptr<const program::Variable>(new program::Variable($4, false, true, context.twoTraces));
  }
| CONST TYPE LBRA RBRA PROGRAM_ID
  {
    // TODO: check that TYPE is not Time
    // TODO: support Bool or check that TYPE is not Bool
    $$ = std::shared_ptr<const program::Variable>(new program::Variable($5, true, true, context.twoTraces));
  }
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
  PROGRAM_ID                
  { 
  	auto var = context.getProgramVar($1);
    if(var->isArray)
    {
      error(@1, "Array variable " + var->name + " needs index for access");
    }
    $$ = std::shared_ptr<const program::IntVariableAccess>(new IntVariableAccess(std::move(var)));
  }
| PROGRAM_ID LBRA expr RBRA 
  {
	  auto var = context.getProgramVar($1);
    if(!var->isArray)
    {
      error(@1, "Variable " + var->name + " is not an array");
    }
	  $$ = std::shared_ptr<const program::IntArrayApplication>(new IntArrayApplication(std::move(var), std::move($3)));
  }
;

%%
void parser::WhileParser::error(const location_type& l,
                              const std::string& m)
{
  std::cout << "Error while parsing location " << l << ":\n" << m << std::endl;
  context.errorFlag = true;
}

