#include "Theory.hpp"

#include <memory>
#include <string>
#include <algorithm>

namespace logic {

    // declare each function-/predicate-symbol by constructing it
    // has additional sideeffect of declaring the involved sorts
    void Theory::declareTheories()
    {
        boolTrue();
        boolFalse();
        
        auto intConst = intConstant(0);
        intAddition(intConst,intConst);
        intSubtraction(intConst,intConst);
        intModulo(intConst,intConst);
        intMultiplication(intConst,intConst);
        intAbsolute(intConst);
        intLess(intConst, intConst);
        intLessEqual(intConst, intConst);
        intGreater(intConst, intConst);
        intGreaterEqual(intConst, intConst);
        
        auto zero = natZero();
        natSucc(zero);
        natPre(zero);
        natSub(zero, zero);
    }

    std::shared_ptr<const FuncTerm> Theory::intConstant(int i)
    {
        return Terms::func(std::to_string(i), {}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::intAddition(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        return Terms::func("+", {t1,t2}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::intSubtraction(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        return Terms::func("-", {t1,t2}, Sorts::intSort(), true);
    }

    std::shared_ptr<const FuncTerm> Theory::intModulo(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        return Terms::func("mod", {t1,t2}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::intMultiplication(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2)
    {
        return Terms::func("*", {t1,t2}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::intAbsolute(std::shared_ptr<const Term> t)
    {
        return Terms::func("abs", {t}, Sorts::intSort(), true);
    }
    
    std::shared_ptr<const Formula> Theory::intLess(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate("<", {t1,t2}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::intLessEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate("<=", {t1,t2}, label, true);
    }

    std::shared_ptr<const Formula> Theory::intGreater(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2,  std::string label)
    {
        return Formulas::predicate(">", {t1,t2}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::intGreaterEqual(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate(">=", {t1,t2}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::boolTrue(std::string label)
    {
        return Formulas::predicate("true", {}, label, true);
    }
    
    std::shared_ptr<const Formula> Theory::boolFalse(std::string label)
    {
        return Formulas::predicate("false", {}, label, true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::natZero()
    {
        return Terms::func("zero", {}, Sorts::natSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::natSucc(std::shared_ptr<const Term> term)
    {
        return Terms::func("s", {term}, Sorts::natSort(), true);
    }
    
    std::shared_ptr<const FuncTerm> Theory::natPre(std::shared_ptr<const Term> term)
    {
        return Terms::func("p", {term}, Sorts::natSort(), true);
    }
    
    std::shared_ptr<const Formula> Theory::natSub(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        return Formulas::predicate("Sub", {t1,t2}, label, false); // Sub needs a declaration, since it is not added by Vampire yet
    }
    
    std::shared_ptr<const Formula> Theory::natSubEq(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2, std::string label)
    {
        // encode t1<=t2 as t1 < s(t2).
        auto succOfT2 = natSucc(t2);
        return Formulas::predicate("Sub", {t1,succOfT2}, label, false); // Sub needs a declaration, since it is not added by Vampire yet
    }

    void addInductionAxiom1(
        std::string name,
        std::string shortName,
        std::function<std::shared_ptr<const Formula> (std::shared_ptr<const Term>)> inductionHypothesis,
        std::vector<std::shared_ptr<const Symbol>> freeVarSymbols,
        std::vector<std::shared_ptr<const logic::ProblemItem>>& items)
    {
        auto boundLSymbol = logic::Signature::varSymbol("boundL", logic::Sorts::natSort());
        auto boundRSymbol = logic::Signature::varSymbol("boundR", logic::Sorts::natSort());
        auto itIndSymbol = logic::Signature::varSymbol("itInd", logic::Sorts::natSort());
        
        auto boundL = Terms::var(boundLSymbol);
        auto boundR = Terms::var(boundRSymbol);
        auto itInd = Terms::var(itIndSymbol);

        std::vector<std::shared_ptr<const Symbol>> argSymbols1 = {boundLSymbol};
        std::vector<std::shared_ptr<const Symbol>> argSymbols2 = {boundLSymbol, boundRSymbol};
        std::vector<std::shared_ptr<const Term>> args1 = {boundL};
        std::vector<std::shared_ptr<const Term>> args2 = {boundL, boundR};
        for (const auto& varSymbol : freeVarSymbols)
        {
            argSymbols1.push_back(varSymbol);
            argSymbols2.push_back(varSymbol);
            args1.push_back(Terms::var(varSymbol));
            args2.push_back(Terms::var(varSymbol));
        }

        auto baseCase = Formulas::predicate("BC-" + shortName, args1);
        auto inductiveCase = Formulas::predicate("IC-" + shortName, args2);
        auto conclusion = Formulas::predicate("Con-" + shortName, args2);
        
        auto baseCaseDef =
            Formulas::universal(argSymbols1,
                Formulas::equivalence(
                    baseCase,
                    inductionHypothesis(boundL)
                )
            );
        auto inductiveCaseDef =
            Formulas::universal(argSymbols2,
                Formulas::equivalence(
                    inductiveCase,
                    Formulas::universal({itIndSymbol},
                        Formulas::implication(
                            Formulas::conjunction({
                                Theory::natSubEq(boundL, itInd),
                                Theory::natSub(itInd, boundR),
                                inductionHypothesis(itInd)
                            }),
                            inductionHypothesis(Theory::natSucc(itInd))
                        )
                    )
                )
            );
        auto conclusionDef =
            Formulas::universal(argSymbols2,
                Formulas::equivalence(
                    conclusion,
                    Formulas::universal({itIndSymbol},
                        Formulas::implication(
                            Formulas::conjunction({
                                Theory::natSubEq(boundL, itInd),
                                Theory::natSubEq(itInd, boundR)
                            }),
                            inductionHypothesis(itInd)
                        )
                    )
                )
            );

        auto inductionAxiom =
            Formulas::universal(argSymbols2,
                Formulas::implication(
                    Formulas::conjunction({
                        baseCase,
                        inductiveCase
                    }),
                    conclusion
                )
            );
        
        items.push_back(std::make_shared<logic::Axiom>(baseCaseDef, "Base-Case-Def for " + name));
        items.push_back(std::make_shared<logic::Axiom>(inductiveCaseDef, "Inductive-Case-Def for " + name));
        items.push_back(std::make_shared<logic::Axiom>(conclusionDef, "Conclusion-Def for " + name));
        items.push_back(std::make_shared<logic::Axiom>(inductionAxiom, name));
    }

    std::shared_ptr<const Formula> inductionAxiom2(std::function<std::shared_ptr<const Formula> (std::shared_ptr<const Term>)> inductionHypothesis)
    {
        auto boundLSymbol = logic::Signature::varSymbol("boundL", logic::Sorts::natSort());
        auto boundR1Symbol = logic::Signature::varSymbol("boundR1", logic::Sorts::natSort());
        auto boundR2Symbol = logic::Signature::varSymbol("boundR2", logic::Sorts::natSort());
        auto itIndSymbol = logic::Signature::varSymbol("itInd", logic::Sorts::natSort());
        
        auto boundL = Terms::var(boundLSymbol);
        auto boundR1 = Terms::var(boundR1Symbol);
        auto boundR2 = Terms::var(boundR2Symbol);
        auto itInd = Terms::var(itIndSymbol);
        
        auto baseCase = inductionHypothesis(boundL);
        
        auto inductiveCase =
            Formulas::universal({itIndSymbol},
                Formulas::implication(
                    Formulas::conjunction({
                        Theory::natSubEq(boundL, itInd),
                        Theory::natSub(itInd, boundR1),
                        Theory::natSub(itInd, boundR2),
                        inductionHypothesis(itInd)
                    }),
                    inductionHypothesis(Theory::natSucc(itInd))
                )
            );
        
        auto conclusion =
            Formulas::universal({itIndSymbol},
                Formulas::implication(
                    Formulas::conjunction({
                        Theory::natSubEq(boundL, itInd),
                        Theory::natSubEq(itInd, boundR1),
                        Theory::natSubEq(itInd, boundR2)
                    }),
                    inductionHypothesis(itInd)
                )
            );
        
        auto inductionAxiom =
            Formulas::universal({boundLSymbol,boundR1Symbol,boundR2Symbol},
                Formulas::implication(
                    Formulas::conjunction({
                        baseCase,
                        inductiveCase
                    }),
                    conclusion
                )
            );
        
        return inductionAxiom;
    }
}

