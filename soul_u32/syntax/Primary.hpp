#ifndef Primary_hpp_4488
#define Primary_hpp_4488

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/parsing/Parser.hpp>
#include <soul_u32/parsing/Scope.hpp>

namespace soul { namespace syntax {

class PrimaryGrammar : public soul::parsing::Grammar
{
public:
    static PrimaryGrammar* Create();
    static PrimaryGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
    soul::parsing::Parser* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, soul::parsing::Scope* enclosingScope);
private:
    PrimaryGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class PrimaryRule;
    class RuleCallRule;
    class NonterminalRule;
    class AliasRule;
    class GroupingRule;
    class TokenRule;
    class ExpectationRule;
    class ActionRule;
};

} } // namespace soul.syntax

#endif // Primary_hpp_4488
