#ifndef Primitive_hpp_24857
#define Primitive_hpp_24857

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/parsing/Primitive.hpp>
#include <soul_u32/parsing/Keyword.hpp>

namespace soul { namespace syntax {

class PrimitiveGrammar : public soul::parsing::Grammar
{
public:
    static PrimitiveGrammar* Create();
    static PrimitiveGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
    soul::parsing::Parser* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    PrimitiveGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class PrimitiveRule;
    class CharRule;
    class StringRule;
    class CharSetRule;
    class CharSetRangeRule;
    class CharSetCharRule;
    class KeywordRule;
    class KeywordBodyRule;
    class KeywordListRule;
    class KeywordListBodyRule;
    class EmptyRule;
    class SpaceRule;
    class AnyCharRule;
    class LetterRule;
    class DigitRule;
    class HexDigitRule;
    class PunctuationRule;
    class RangeRule;
    class CodePointRule;
};

} } // namespace soul.syntax

#endif // Primitive_hpp_24857
