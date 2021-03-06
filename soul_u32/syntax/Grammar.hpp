#ifndef Grammar_hpp_4488
#define Grammar_hpp_4488

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/parsing/Grammar.hpp>

namespace soul { namespace syntax {

class GrammarGrammar : public soul::parsing::Grammar
{
public:
    static GrammarGrammar* Create();
    static GrammarGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
    soul::parsing::Grammar* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, soul::parsing::Scope* enclosingScope);
private:
    GrammarGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class GrammarRule;
    class GrammarContentRule;
    class StartClauseRule;
    class SkipClauseRule;
};

} } // namespace soul.syntax

#endif // Grammar_hpp_4488
