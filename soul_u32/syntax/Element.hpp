#ifndef Element_hpp_24857
#define Element_hpp_24857

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/parsing/Scope.hpp>
#include <soul_u32/parsing/Grammar.hpp>

namespace soul { namespace syntax {

class ElementGrammar : public soul::parsing::Grammar
{
public:
    static ElementGrammar* Create();
    static ElementGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, soul::parsing::Grammar* grammar);
private:
    std::vector<std::u32string> keywords0;
    ElementGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class RuleLinkRule;
    class SignatureRule;
    class ParameterListRule;
    class VariableRule;
    class ParameterRule;
    class ReturnTypeRule;
    class IdentifierRule;
    class QualifiedIdRule;
    class StringArrayRule;
};

} } // namespace soul.syntax

#endif // Element_hpp_24857
