#ifndef Identifier_hpp_3838
#define Identifier_hpp_3838

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>

namespace soul { namespace code {

class IdentifierGrammar : public soul::parsing::Grammar
{
public:
    static IdentifierGrammar* Create();
    static IdentifierGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
    std::u32string Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    IdentifierGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class IdentifierRule;
    class QualifiedIdRule;
};

} } // namespace soul.code

#endif // Identifier_hpp_3838
