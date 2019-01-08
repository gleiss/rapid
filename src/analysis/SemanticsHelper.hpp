#ifndef __SemanticsHelper__
#define __SemanticsHelper__

#include "Term.hpp"
#include "Formula.hpp"

#include "Variable.hpp"
#include "Statements.hpp"

namespace analysis {

# pragma mark - Convenience methods for converting program-expressions to terms/predicates
    /*
     * convert a program variable to a logical term refering to the value of Variable var at the Timepoint timepoint in the Trace trace.
     * The first version must only be used for non-array variables, the second version must only be used for array-variables (where position refers to the position in the array).
     */
    std::shared_ptr<const logic::Term> toTermFull(std::shared_ptr<const program::Variable> var, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> trace);
    std::shared_ptr<const logic::Term> toTermFull(std::shared_ptr<const program::Variable> arrayVar, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> position, std::shared_ptr<const logic::Term> trace);
    
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

    std::shared_ptr<const logic::LVariable> iteratorTermForLoop(const program::WhileStatement* whileStatement);
    std::shared_ptr<const logic::Term> lastIterationTermForLoop(const program::WhileStatement* whileStatement, std::shared_ptr<const logic::Term> trace, bool twoTraces);

    /*
     * convenience methods which call the corresponding methods with a logical variable tr of Sort Trace as argument for trace.
     * the main use case of these methods is that we don't need to add the tr-variable everywhere in the semantics.
     */
    std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const program::Variable> var, std::shared_ptr<const logic::Term> timePoint);
    std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const program::Variable> arrayVar, std::shared_ptr<const logic::Term> timePoint, std::shared_ptr<const logic::Term> position);
    std::shared_ptr<const logic::Term> toTerm(std::shared_ptr<const program::IntExpression> expr, std::shared_ptr<const logic::Term> timePoint);
    std::shared_ptr<const logic::Formula> toFormula(std::shared_ptr<const program::BoolExpression> expr, std::shared_ptr<const logic::Term> timePoint);
    std::shared_ptr<const logic::Term> lastIterationTermForLoop(const program::WhileStatement* whileStatement, bool twoTraces);

    
    std::shared_ptr<const logic::Term> trace1Term();
    std::shared_ptr<const logic::Term> trace2Term();

}


#endif

