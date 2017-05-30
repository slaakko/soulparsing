#ifndef Literal_hpp_22232
#define Literal_hpp_22232

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/codedom/Literal.hpp>

namespace soul { namespace code {

class LiteralGrammar : public soul::parsing::Grammar
{
public:
    static LiteralGrammar* Create();
    static LiteralGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
    soul::codedom::Literal* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    LiteralGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class LiteralRule;
    class IntegerLiteralRule;
    class CharacterLiteralRule;
    class CCharSequenceRule;
    class FloatingLiteralRule;
    class StringLiteralRule;
    class BooleanLiteralRule;
    class PointerLiteralRule;
};

} } // namespace soul.code

#endif // Literal_hpp_22232
