#ifndef __SemanticsHelper__
#define __SemanticsHelper__

#include <memory>
#include <string>
#include <vector>

#include "Term.hpp"
#include "Formula.hpp"

#include "Variable.hpp"
#include "Statements.hpp"

namespace analysis {

# pragma mark - Methods for generating most used variables
    std::shared_ptr<const logic::LVariable> posVar();

# pragma mark - Methods for generating most used trace terms
    std::shared_ptr<const logic::Term> traceTerm(unsigned traceNumber);
    std::vector<std::shared_ptr<const logic::Term>> traceTerms(unsigned numberOfTraces);

# pragma mark - Methods for generating most used timepoint terms and symbols
    std::shared_ptr<const logic::LVariable> iteratorTermForLoop(const program::WhileStatement* whileStatement);
    std::shared_ptr<const logic::Term> lastIterationTermForLoop(const program::WhileStatement* whileStatement, unsigned numberOfTraces, std::shared_ptr<const logic::Term> trace);

    std::shared_ptr<const logic::Term> timepointForNonLoopStatement(const program::Statement* statement);
    std::shared_ptr<const logic::Term> timepointForLoopStatement(const program::WhileStatement* whileStatement, std::shared_ptr<const logic::Term> innerIteration);

    std::shared_ptr<const logic::Term> startTimepointForStatement(const program::Statement* statement);

    std::vector<std::shared_ptr<const logic::Symbol>> enclosingIteratorsSymbols(const program::Statement* statement);
    
# pragma mark - Methods for generating most used terms/predicates denoting program-expressions
    /*
     * convert a program variable to a logical term refering to the value of Variable var at the Timepoint timepoint in the Trace trace.
     * The first version must only be used for non-array variables, the second version must only be used for array-variables (where position refers to the position in the array).
     */
    std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const program::Variable> var, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> trace);
    std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const program::Variable> arrayVar, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> position, std::shared_ptr<const logic::Term> trace);
    
    /*
     * convert the expression expr to a logical term refering to the value of the Expression expr at the Timepoint timepoint.
     * calls toTerm(var,...) internally.
     */
    std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const program::IntExpression> expr, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> trace);
    
    /*
     * convert the boolean expression expr to a logical predicate refering to the value of the Expression expr at the Timepoint timepoint.
     * calls toTerm(expr,...) internally.
     */
    std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const program::BoolExpression> expr, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> trace);

# pragma mark - Methods for generating most used formulas for describing changes of state
    /*
     * generate a formula asserting that the values of variable v at timepoint1 and timepoint2 are the same.
     */
    std::shared_ptr<const logic::Formula> varEqual(std::shared_ptr<const program::Variable> v, std::shared_ptr<const logic::Term> timePoint1, std::shared_ptr<const logic::Term> timePoint2, std::shared_ptr<const logic::Term> trace);
    /*
     * generate a formula asserting that for each variable v in activeVars, the values of v at timepoint1 and timepoint2 are the same.
     * ignores any v in activeVars which is constant.
     */
    std::shared_ptr<const logic::Formula> allVarEqual(const std::vector<std::shared_ptr<const program::Variable>>& activeVars, std::shared_ptr<const logic::Term> timePoint1, std::shared_ptr<const logic::Term> timePoint2, std::shared_ptr<const logic::Term> trace, std::string label="");
}


#endif

