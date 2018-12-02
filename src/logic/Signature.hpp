#ifndef __Signature__
#define __Signature__

#include <cassert>
#include <unordered_map>
#include <string>
#include <vector>
#include "Sort.hpp"

# pragma mark - Symbol

namespace logic {
    
    class Symbol {
        // we need each symbol to be either defined in the signature or to be a variable (which will be defined by the quantifier)
        // We use the Signature-class below as a manager-class for symbols of the first kind
        friend class Signature;
        
    private:
        Symbol(std::string name, const Sort* rngSort, bool noDeclaration) :
        name(name),
        argSorts(),
        rngSort(rngSort),
        noDeclaration(noDeclaration)
        {
            assert(!name.empty());
        }

        Symbol(std::string name, std::vector<const Sort*> argSorts, const Sort* rngSort, bool noDeclaration) :
        name(name),
        argSorts(std::move(argSorts)),
        rngSort(rngSort),
        noDeclaration(noDeclaration)
        {
            assert(!name.empty());
        }
     
    public:
        const std::string name;
        const std::vector<const Sort*> argSorts;
        const Sort* rngSort;
        const bool noDeclaration; // true iff the symbol needs no declaration in smtlib (i.e. true only for interpreted symbols and variables)

        bool isPredicateSymbol() const { return rngSort == Sorts::boolSort(); }
         
        std::string toSMTLIB() const;
        std::string declareSymbolSMTLIB() const;
        std::string declareSymbolColorSMTLIB() const;
        
        bool operator==(const Symbol &s) const {return name == s.name;}
    };
    
    // hack needed for bison: std::vector has no overload for ostream, but these overloads are needed for bison
    std::ostream& operator<<(std::ostream& ostr, const std::vector<std::shared_ptr<const logic::Symbol>>& f);
    
}

namespace std
{
    template<> struct hash<logic::Symbol>
    {
        using argument_type = logic::Symbol;
        using result_type = std::size_t;
        
        result_type operator ()(argument_type const& s) const
        {
            return std::hash<std::string>()(s.name);
        }
    };
}

# pragma mark - Signature

namespace logic {

    struct SymbolPtrEquality
    {
        bool operator()(const std::unique_ptr<Symbol>& a, const std::unique_ptr<Symbol>& b) const {return *a == *b;}
    };
    
    struct SymbolPtrHash
    {
        size_t operator()(const std::unique_ptr<Symbol>& ptr) const {return std::hash<Symbol>()(*ptr);}
    };
    
    // We use Signature as a manager-class for Symbol-instances
    class Signature
    {
    public:
        // construct new symbols
        static const Symbol* add(std::string name, std::vector<const Sort*> argSorts, const Sort* rngSort, bool noDeclaration=false);
        static const Symbol* fetch(std::string name);
        static const Symbol* fetchOrAdd(std::string name, std::vector<const Sort*> argSorts, const Sort* rngSort, bool noDeclaration=false);

        // check that variable doesn't use name which already occurs in Signature
        // return Symbol without adding it to Signature
        static std::shared_ptr<const Symbol> varSymbol(std::string name, const Sort* rngSort);

        static const std::unordered_map<std::string, std::unique_ptr<const Symbol>>& signature(){return _signature;}
        
    private:
        // _signature collects all symbols used so far.
        static std::unordered_map<std::string, std::unique_ptr<const Symbol>> _signature;
    };
}
#endif

