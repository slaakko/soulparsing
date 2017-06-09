#ifndef Rule_hpp_13455
#define Rule_hpp_13455

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/parsing/Rule.hpp>

namespace soul { namespace syntax {

class RuleGrammar : public soul::parsing::Grammar
{
public:
    static RuleGrammar* Create();
    static RuleGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
    soul::parsing::Rule* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, soul::parsing::Scope* enclosingScope);
private:
    RuleGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class RuleRule;
    class RuleHeaderRule;
    class RuleBodyRule;
};

} } // namespace soul.syntax

#endif // Rule_hpp_13455
