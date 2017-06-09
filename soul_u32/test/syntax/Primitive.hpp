#ifndef Primitive_hpp_13455
#define Primitive_hpp_13455

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
    class RangeRule;
    class CodePointRule;
    class EmptyRule;
    class SpaceRule;
    class AnyCharRule;
    class LetterRule;
    class DigitRule;
    class HexDigitRule;
    class UpperLetterRule;
    class LowerLetterRule;
    class TitleLetterRule;
    class ModifierLetterRule;
    class OtherLetterRule;
    class CasedLetterRule;
    class MarkRule;
    class NonspacingMarkRule;
    class SpacingMarkRule;
    class EnclosingMarkRule;
    class NumberRule;
    class DecimalNumberRule;
    class LetterNumberRule;
    class OtherNumberRule;
    class PunctuationRule;
    class ConnectorPunctuationRule;
    class DashPunctuationRule;
    class OpenPunctuationRule;
    class ClosePunctuationRule;
    class InitialPunctuationRule;
    class FinalPunctuationRule;
    class OtherPunctuationRule;
    class SymbolRule;
    class MathSymbolRule;
    class CurrencySymbolRule;
    class ModifierSymbolRule;
    class OtherSymbolRule;
    class SeparatorRule;
    class SpaceSeparatorRule;
    class LineSeparatorRule;
    class ParagraphSeparatorRule;
    class OtherRule;
    class ControlRule;
    class FormatRule;
    class SurrogateRule;
    class PrivateUseRule;
    class UnassignedRule;
    class GraphicRule;
    class BaseCharRule;
    class AlphabeticRule;
    class IdStartRule;
    class IdContRule;
};

} } // namespace soul.syntax

#endif // Primitive_hpp_13455
