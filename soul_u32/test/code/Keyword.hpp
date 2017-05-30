#ifndef Keyword_hpp_22232
#define Keyword_hpp_22232

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>

namespace soul { namespace code {

class KeywordGrammar : public soul::parsing::Grammar
{
public:
    static KeywordGrammar* Create();
    static KeywordGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
private:
    std::vector<std::u32string> keywords0;
    KeywordGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
};

} } // namespace soul.code

#endif // Keyword_hpp_22232
