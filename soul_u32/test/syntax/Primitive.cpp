#include "Primitive.hpp"
#include <soul_u32/parsing/Action.hpp>
#include <soul_u32/parsing/Rule.hpp>
#include <soul_u32/parsing/ParsingDomain.hpp>
#include <soul_u32/parsing/Primitive.hpp>
#include <soul_u32/parsing/Composite.hpp>
#include <soul_u32/parsing/Nonterminal.hpp>
#include <soul_u32/parsing/Exception.hpp>
#include <soul_u32/parsing/StdLib.hpp>
#include <soul_u32/parsing/XmlLog.hpp>
#include <soul_u32/util/Unicode.hpp>
#include <soul_u32/syntax/Element.hpp>

namespace soul { namespace syntax {

using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;

PrimitiveGrammar* PrimitiveGrammar::Create()
{
    return Create(new soul::parsing::ParsingDomain());
}

PrimitiveGrammar* PrimitiveGrammar::Create(soul::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    PrimitiveGrammar* grammar(new PrimitiveGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

PrimitiveGrammar::PrimitiveGrammar(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32("PrimitiveGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("soul.syntax")), parsingDomain_)
{
    SetOwner(0);
}

soul::parsing::Parser* PrimitiveGrammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
{
    soul::parsing::Scanner scanner(start, end, fileName, fileIndex, SkipRule());
    std::unique_ptr<soul::parsing::XmlLog> xmlLog;
    if (Log())
    {
        xmlLog.reset(new soul::parsing::XmlLog(*Log(), MaxLogLineLength()));
        scanner.SetLog(xmlLog.get());
        xmlLog->WriteBeginRule("parse");
    }
    soul::parsing::ObjectStack stack;
    std::unique_ptr<soul::parsing::ParsingData> parsingData(new soul::parsing::ParsingData(GetParsingDomain()->GetNumRules()));
    scanner.SetParsingData(parsingData.get());
    soul::parsing::Match match = soul::parsing::Grammar::Parse(scanner, stack, parsingData.get());
    soul::parsing::Span stop = scanner.GetSpan();
    if (Log())
    {
        xmlLog->WriteEndRule("parse");
    }
    if (!match.Hit() || stop.Start() != int(end - start))
    {
        if (StartRule())
        {
            throw soul::parsing::ExpectationFailure(StartRule()->Info(), fileName, stop, start, end);
        }
        else
        {
            throw soul::parsing::ParsingException("grammar '" + ToUtf8(Name()) + "' has no start rule", fileName, scanner.GetSpan(), start, end);
        }
    }
    std::unique_ptr<soul::parsing::Object> value = std::move(stack.top());
    soul::parsing::Parser* result = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(value.get());
    stack.pop();
    return result;
}

class PrimitiveGrammar::PrimitiveRule : public soul::parsing::Rule
{
public:
    PrimitiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A3Action));
        soul::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A4Action));
        soul::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A5Action));
        soul::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A6Action));
        soul::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A7Action));
        soul::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A8Action));
        soul::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A9Action));
        soul::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A10Action));
        soul::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A11Action));
        soul::parsing::ActionParser* a12ActionParser = GetAction(ToUtf32("A12"));
        a12ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A12Action));
        soul::parsing::ActionParser* a13ActionParser = GetAction(ToUtf32("A13"));
        a13ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A13Action));
        soul::parsing::ActionParser* a14ActionParser = GetAction(ToUtf32("A14"));
        a14ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A14Action));
        soul::parsing::ActionParser* a15ActionParser = GetAction(ToUtf32("A15"));
        a15ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A15Action));
        soul::parsing::ActionParser* a16ActionParser = GetAction(ToUtf32("A16"));
        a16ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A16Action));
        soul::parsing::ActionParser* a17ActionParser = GetAction(ToUtf32("A17"));
        a17ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A17Action));
        soul::parsing::ActionParser* a18ActionParser = GetAction(ToUtf32("A18"));
        a18ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A18Action));
        soul::parsing::ActionParser* a19ActionParser = GetAction(ToUtf32("A19"));
        a19ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A19Action));
        soul::parsing::ActionParser* a20ActionParser = GetAction(ToUtf32("A20"));
        a20ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A20Action));
        soul::parsing::ActionParser* a21ActionParser = GetAction(ToUtf32("A21"));
        a21ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A21Action));
        soul::parsing::ActionParser* a22ActionParser = GetAction(ToUtf32("A22"));
        a22ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A22Action));
        soul::parsing::ActionParser* a23ActionParser = GetAction(ToUtf32("A23"));
        a23ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A23Action));
        soul::parsing::ActionParser* a24ActionParser = GetAction(ToUtf32("A24"));
        a24ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A24Action));
        soul::parsing::ActionParser* a25ActionParser = GetAction(ToUtf32("A25"));
        a25ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A25Action));
        soul::parsing::ActionParser* a26ActionParser = GetAction(ToUtf32("A26"));
        a26ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A26Action));
        soul::parsing::ActionParser* a27ActionParser = GetAction(ToUtf32("A27"));
        a27ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A27Action));
        soul::parsing::ActionParser* a28ActionParser = GetAction(ToUtf32("A28"));
        a28ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A28Action));
        soul::parsing::ActionParser* a29ActionParser = GetAction(ToUtf32("A29"));
        a29ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A29Action));
        soul::parsing::ActionParser* a30ActionParser = GetAction(ToUtf32("A30"));
        a30ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A30Action));
        soul::parsing::ActionParser* a31ActionParser = GetAction(ToUtf32("A31"));
        a31ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A31Action));
        soul::parsing::ActionParser* a32ActionParser = GetAction(ToUtf32("A32"));
        a32ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A32Action));
        soul::parsing::ActionParser* a33ActionParser = GetAction(ToUtf32("A33"));
        a33ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A33Action));
        soul::parsing::ActionParser* a34ActionParser = GetAction(ToUtf32("A34"));
        a34ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A34Action));
        soul::parsing::ActionParser* a35ActionParser = GetAction(ToUtf32("A35"));
        a35ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A35Action));
        soul::parsing::ActionParser* a36ActionParser = GetAction(ToUtf32("A36"));
        a36ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A36Action));
        soul::parsing::ActionParser* a37ActionParser = GetAction(ToUtf32("A37"));
        a37ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A37Action));
        soul::parsing::ActionParser* a38ActionParser = GetAction(ToUtf32("A38"));
        a38ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A38Action));
        soul::parsing::ActionParser* a39ActionParser = GetAction(ToUtf32("A39"));
        a39ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A39Action));
        soul::parsing::ActionParser* a40ActionParser = GetAction(ToUtf32("A40"));
        a40ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A40Action));
        soul::parsing::ActionParser* a41ActionParser = GetAction(ToUtf32("A41"));
        a41ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A41Action));
        soul::parsing::ActionParser* a42ActionParser = GetAction(ToUtf32("A42"));
        a42ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A42Action));
        soul::parsing::ActionParser* a43ActionParser = GetAction(ToUtf32("A43"));
        a43ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A43Action));
        soul::parsing::ActionParser* a44ActionParser = GetAction(ToUtf32("A44"));
        a44ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A44Action));
        soul::parsing::ActionParser* a45ActionParser = GetAction(ToUtf32("A45"));
        a45ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A45Action));
        soul::parsing::ActionParser* a46ActionParser = GetAction(ToUtf32("A46"));
        a46ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A46Action));
        soul::parsing::ActionParser* a47ActionParser = GetAction(ToUtf32("A47"));
        a47ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A47Action));
        soul::parsing::ActionParser* a48ActionParser = GetAction(ToUtf32("A48"));
        a48ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A48Action));
        soul::parsing::ActionParser* a49ActionParser = GetAction(ToUtf32("A49"));
        a49ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A49Action));
        soul::parsing::ActionParser* a50ActionParser = GetAction(ToUtf32("A50"));
        a50ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A50Action));
        soul::parsing::ActionParser* a51ActionParser = GetAction(ToUtf32("A51"));
        a51ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A51Action));
        soul::parsing::ActionParser* a52ActionParser = GetAction(ToUtf32("A52"));
        a52ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A52Action));
        soul::parsing::ActionParser* a53ActionParser = GetAction(ToUtf32("A53"));
        a53ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A53Action));
        soul::parsing::NonterminalParser* charNonterminalParser = GetNonterminal(ToUtf32("Char"));
        charNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostChar));
        soul::parsing::NonterminalParser* stringNonterminalParser = GetNonterminal(ToUtf32("String"));
        stringNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostString));
        soul::parsing::NonterminalParser* charSetNonterminalParser = GetNonterminal(ToUtf32("CharSet"));
        charSetNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostCharSet));
        soul::parsing::NonterminalParser* keywordNonterminalParser = GetNonterminal(ToUtf32("Keyword"));
        keywordNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostKeyword));
        soul::parsing::NonterminalParser* keywordListNonterminalParser = GetNonterminal(ToUtf32("KeywordList"));
        keywordListNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostKeywordList));
        soul::parsing::NonterminalParser* rangeNonterminalParser = GetNonterminal(ToUtf32("Range"));
        rangeNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostRange));
        soul::parsing::NonterminalParser* emptyNonterminalParser = GetNonterminal(ToUtf32("Empty"));
        emptyNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostEmpty));
        soul::parsing::NonterminalParser* spaceNonterminalParser = GetNonterminal(ToUtf32("Space"));
        spaceNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSpace));
        soul::parsing::NonterminalParser* anyCharNonterminalParser = GetNonterminal(ToUtf32("AnyChar"));
        anyCharNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostAnyChar));
        soul::parsing::NonterminalParser* letterNonterminalParser = GetNonterminal(ToUtf32("Letter"));
        letterNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLetter));
        soul::parsing::NonterminalParser* digitNonterminalParser = GetNonterminal(ToUtf32("Digit"));
        digitNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostDigit));
        soul::parsing::NonterminalParser* hexDigitNonterminalParser = GetNonterminal(ToUtf32("HexDigit"));
        hexDigitNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostHexDigit));
        soul::parsing::NonterminalParser* upperLetterNonterminalParser = GetNonterminal(ToUtf32("UpperLetter"));
        upperLetterNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostUpperLetter));
        soul::parsing::NonterminalParser* lowerLetterNonterminalParser = GetNonterminal(ToUtf32("LowerLetter"));
        lowerLetterNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLowerLetter));
        soul::parsing::NonterminalParser* titleLetterNonterminalParser = GetNonterminal(ToUtf32("TitleLetter"));
        titleLetterNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostTitleLetter));
        soul::parsing::NonterminalParser* modifierLetterNonterminalParser = GetNonterminal(ToUtf32("ModifierLetter"));
        modifierLetterNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostModifierLetter));
        soul::parsing::NonterminalParser* otherLetterNonterminalParser = GetNonterminal(ToUtf32("OtherLetter"));
        otherLetterNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherLetter));
        soul::parsing::NonterminalParser* casedLetterNonterminalParser = GetNonterminal(ToUtf32("CasedLetter"));
        casedLetterNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostCasedLetter));
        soul::parsing::NonterminalParser* markNonterminalParser = GetNonterminal(ToUtf32("Mark"));
        markNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostMark));
        soul::parsing::NonterminalParser* nonspacingMarkNonterminalParser = GetNonterminal(ToUtf32("NonspacingMark"));
        nonspacingMarkNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostNonspacingMark));
        soul::parsing::NonterminalParser* enclosingMarkNonterminalParser = GetNonterminal(ToUtf32("EnclosingMark"));
        enclosingMarkNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostEnclosingMark));
        soul::parsing::NonterminalParser* spacingMarkNonterminalParser = GetNonterminal(ToUtf32("SpacingMark"));
        spacingMarkNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSpacingMark));
        soul::parsing::NonterminalParser* numberNonterminalParser = GetNonterminal(ToUtf32("Number"));
        numberNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostNumber));
        soul::parsing::NonterminalParser* decimalNumberNonterminalParser = GetNonterminal(ToUtf32("DecimalNumber"));
        decimalNumberNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostDecimalNumber));
        soul::parsing::NonterminalParser* letterNumberNonterminalParser = GetNonterminal(ToUtf32("LetterNumber"));
        letterNumberNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLetterNumber));
        soul::parsing::NonterminalParser* otherNumberNonterminalParser = GetNonterminal(ToUtf32("OtherNumber"));
        otherNumberNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherNumber));
        soul::parsing::NonterminalParser* punctuationNonterminalParser = GetNonterminal(ToUtf32("Punctuation"));
        punctuationNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostPunctuation));
        soul::parsing::NonterminalParser* connectorPunctuationNonterminalParser = GetNonterminal(ToUtf32("ConnectorPunctuation"));
        connectorPunctuationNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostConnectorPunctuation));
        soul::parsing::NonterminalParser* dashPunctuationNonterminalParser = GetNonterminal(ToUtf32("DashPunctuation"));
        dashPunctuationNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostDashPunctuation));
        soul::parsing::NonterminalParser* openPunctuationNonterminalParser = GetNonterminal(ToUtf32("OpenPunctuation"));
        openPunctuationNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOpenPunctuation));
        soul::parsing::NonterminalParser* closePunctuationNonterminalParser = GetNonterminal(ToUtf32("ClosePunctuation"));
        closePunctuationNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostClosePunctuation));
        soul::parsing::NonterminalParser* initialPunctuationNonterminalParser = GetNonterminal(ToUtf32("InitialPunctuation"));
        initialPunctuationNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostInitialPunctuation));
        soul::parsing::NonterminalParser* finalPunctuationNonterminalParser = GetNonterminal(ToUtf32("FinalPunctuation"));
        finalPunctuationNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostFinalPunctuation));
        soul::parsing::NonterminalParser* otherPunctuationNonterminalParser = GetNonterminal(ToUtf32("OtherPunctuation"));
        otherPunctuationNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherPunctuation));
        soul::parsing::NonterminalParser* symbolNonterminalParser = GetNonterminal(ToUtf32("Symbol"));
        symbolNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSymbol));
        soul::parsing::NonterminalParser* mathSymbolNonterminalParser = GetNonterminal(ToUtf32("MathSymbol"));
        mathSymbolNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostMathSymbol));
        soul::parsing::NonterminalParser* currencySymbolNonterminalParser = GetNonterminal(ToUtf32("CurrencySymbol"));
        currencySymbolNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostCurrencySymbol));
        soul::parsing::NonterminalParser* modifierSymbolNonterminalParser = GetNonterminal(ToUtf32("ModifierSymbol"));
        modifierSymbolNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostModifierSymbol));
        soul::parsing::NonterminalParser* otherSymbolNonterminalParser = GetNonterminal(ToUtf32("OtherSymbol"));
        otherSymbolNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherSymbol));
        soul::parsing::NonterminalParser* separatorNonterminalParser = GetNonterminal(ToUtf32("Separator"));
        separatorNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSeparator));
        soul::parsing::NonterminalParser* spaceSeparatorNonterminalParser = GetNonterminal(ToUtf32("SpaceSeparator"));
        spaceSeparatorNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSpaceSeparator));
        soul::parsing::NonterminalParser* lineSeparatorNonterminalParser = GetNonterminal(ToUtf32("LineSeparator"));
        lineSeparatorNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLineSeparator));
        soul::parsing::NonterminalParser* paragraphSeparatorNonterminalParser = GetNonterminal(ToUtf32("ParagraphSeparator"));
        paragraphSeparatorNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostParagraphSeparator));
        soul::parsing::NonterminalParser* otherNonterminalParser = GetNonterminal(ToUtf32("Other"));
        otherNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOther));
        soul::parsing::NonterminalParser* controlNonterminalParser = GetNonterminal(ToUtf32("Control"));
        controlNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostControl));
        soul::parsing::NonterminalParser* formatNonterminalParser = GetNonterminal(ToUtf32("Format"));
        formatNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostFormat));
        soul::parsing::NonterminalParser* surrogateNonterminalParser = GetNonterminal(ToUtf32("Surrogate"));
        surrogateNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSurrogate));
        soul::parsing::NonterminalParser* privateUseNonterminalParser = GetNonterminal(ToUtf32("PrivateUse"));
        privateUseNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostPrivateUse));
        soul::parsing::NonterminalParser* unassignedNonterminalParser = GetNonterminal(ToUtf32("Unassigned"));
        unassignedNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostUnassigned));
        soul::parsing::NonterminalParser* graphicNonterminalParser = GetNonterminal(ToUtf32("Graphic"));
        graphicNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostGraphic));
        soul::parsing::NonterminalParser* baseCharNonterminalParser = GetNonterminal(ToUtf32("BaseChar"));
        baseCharNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostBaseChar));
        soul::parsing::NonterminalParser* alphabeticNonterminalParser = GetNonterminal(ToUtf32("Alphabetic"));
        alphabeticNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostAlphabetic));
        soul::parsing::NonterminalParser* idStartNonterminalParser = GetNonterminal(ToUtf32("IdStart"));
        idStartNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostIdStart));
        soul::parsing::NonterminalParser* idContNonterminalParser = GetNonterminal(ToUtf32("IdCont"));
        idContNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostIdCont));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromChar;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromString;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCharSet;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeyword;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeywordList;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromRange;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEmpty;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpace;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAnyChar;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLetter;
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDigit;
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromHexDigit;
    }
    void A12Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUpperLetter;
    }
    void A13Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLowerLetter;
    }
    void A14Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTitleLetter;
    }
    void A15Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromModifierLetter;
    }
    void A16Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherLetter;
    }
    void A17Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCasedLetter;
    }
    void A18Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromMark;
    }
    void A19Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNonspacingMark;
    }
    void A20Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEnclosingMark;
    }
    void A21Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpacingMark;
    }
    void A22Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNumber;
    }
    void A23Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDecimalNumber;
    }
    void A24Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLetterNumber;
    }
    void A25Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherNumber;
    }
    void A26Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPunctuation;
    }
    void A27Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConnectorPunctuation;
    }
    void A28Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDashPunctuation;
    }
    void A29Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOpenPunctuation;
    }
    void A30Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClosePunctuation;
    }
    void A31Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromInitialPunctuation;
    }
    void A32Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFinalPunctuation;
    }
    void A33Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherPunctuation;
    }
    void A34Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSymbol;
    }
    void A35Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromMathSymbol;
    }
    void A36Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCurrencySymbol;
    }
    void A37Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromModifierSymbol;
    }
    void A38Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherSymbol;
    }
    void A39Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSeparator;
    }
    void A40Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpaceSeparator;
    }
    void A41Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLineSeparator;
    }
    void A42Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromParagraphSeparator;
    }
    void A43Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOther;
    }
    void A44Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromControl;
    }
    void A45Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFormat;
    }
    void A46Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSurrogate;
    }
    void A47Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPrivateUse;
    }
    void A48Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUnassigned;
    }
    void A49Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromGraphic;
    }
    void A50Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBaseChar;
    }
    void A51Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAlphabetic;
    }
    void A52Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdStart;
    }
    void A53Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdCont;
    }
    void PostChar(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromChar_value = std::move(stack.top());
            context->fromChar = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromChar_value.get());
            stack.pop();
        }
    }
    void PostString(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromString_value = std::move(stack.top());
            context->fromString = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromString_value.get());
            stack.pop();
        }
    }
    void PostCharSet(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromCharSet_value = std::move(stack.top());
            context->fromCharSet = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromCharSet_value.get());
            stack.pop();
        }
    }
    void PostKeyword(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromKeyword_value = std::move(stack.top());
            context->fromKeyword = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromKeyword_value.get());
            stack.pop();
        }
    }
    void PostKeywordList(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromKeywordList_value = std::move(stack.top());
            context->fromKeywordList = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromKeywordList_value.get());
            stack.pop();
        }
    }
    void PostRange(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromRange_value = std::move(stack.top());
            context->fromRange = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromRange_value.get());
            stack.pop();
        }
    }
    void PostEmpty(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromEmpty_value = std::move(stack.top());
            context->fromEmpty = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromEmpty_value.get());
            stack.pop();
        }
    }
    void PostSpace(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromSpace_value = std::move(stack.top());
            context->fromSpace = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromSpace_value.get());
            stack.pop();
        }
    }
    void PostAnyChar(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromAnyChar_value = std::move(stack.top());
            context->fromAnyChar = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromAnyChar_value.get());
            stack.pop();
        }
    }
    void PostLetter(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromLetter_value = std::move(stack.top());
            context->fromLetter = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromLetter_value.get());
            stack.pop();
        }
    }
    void PostDigit(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromDigit_value = std::move(stack.top());
            context->fromDigit = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromDigit_value.get());
            stack.pop();
        }
    }
    void PostHexDigit(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromHexDigit_value = std::move(stack.top());
            context->fromHexDigit = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromHexDigit_value.get());
            stack.pop();
        }
    }
    void PostUpperLetter(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromUpperLetter_value = std::move(stack.top());
            context->fromUpperLetter = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromUpperLetter_value.get());
            stack.pop();
        }
    }
    void PostLowerLetter(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromLowerLetter_value = std::move(stack.top());
            context->fromLowerLetter = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromLowerLetter_value.get());
            stack.pop();
        }
    }
    void PostTitleLetter(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromTitleLetter_value = std::move(stack.top());
            context->fromTitleLetter = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromTitleLetter_value.get());
            stack.pop();
        }
    }
    void PostModifierLetter(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromModifierLetter_value = std::move(stack.top());
            context->fromModifierLetter = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromModifierLetter_value.get());
            stack.pop();
        }
    }
    void PostOtherLetter(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromOtherLetter_value = std::move(stack.top());
            context->fromOtherLetter = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromOtherLetter_value.get());
            stack.pop();
        }
    }
    void PostCasedLetter(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromCasedLetter_value = std::move(stack.top());
            context->fromCasedLetter = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromCasedLetter_value.get());
            stack.pop();
        }
    }
    void PostMark(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromMark_value = std::move(stack.top());
            context->fromMark = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromMark_value.get());
            stack.pop();
        }
    }
    void PostNonspacingMark(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromNonspacingMark_value = std::move(stack.top());
            context->fromNonspacingMark = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromNonspacingMark_value.get());
            stack.pop();
        }
    }
    void PostEnclosingMark(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromEnclosingMark_value = std::move(stack.top());
            context->fromEnclosingMark = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromEnclosingMark_value.get());
            stack.pop();
        }
    }
    void PostSpacingMark(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromSpacingMark_value = std::move(stack.top());
            context->fromSpacingMark = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromSpacingMark_value.get());
            stack.pop();
        }
    }
    void PostNumber(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromNumber_value = std::move(stack.top());
            context->fromNumber = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromNumber_value.get());
            stack.pop();
        }
    }
    void PostDecimalNumber(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromDecimalNumber_value = std::move(stack.top());
            context->fromDecimalNumber = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromDecimalNumber_value.get());
            stack.pop();
        }
    }
    void PostLetterNumber(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromLetterNumber_value = std::move(stack.top());
            context->fromLetterNumber = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromLetterNumber_value.get());
            stack.pop();
        }
    }
    void PostOtherNumber(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromOtherNumber_value = std::move(stack.top());
            context->fromOtherNumber = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromOtherNumber_value.get());
            stack.pop();
        }
    }
    void PostPunctuation(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromPunctuation_value = std::move(stack.top());
            context->fromPunctuation = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromPunctuation_value.get());
            stack.pop();
        }
    }
    void PostConnectorPunctuation(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromConnectorPunctuation_value = std::move(stack.top());
            context->fromConnectorPunctuation = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromConnectorPunctuation_value.get());
            stack.pop();
        }
    }
    void PostDashPunctuation(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromDashPunctuation_value = std::move(stack.top());
            context->fromDashPunctuation = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromDashPunctuation_value.get());
            stack.pop();
        }
    }
    void PostOpenPunctuation(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromOpenPunctuation_value = std::move(stack.top());
            context->fromOpenPunctuation = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromOpenPunctuation_value.get());
            stack.pop();
        }
    }
    void PostClosePunctuation(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromClosePunctuation_value = std::move(stack.top());
            context->fromClosePunctuation = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromClosePunctuation_value.get());
            stack.pop();
        }
    }
    void PostInitialPunctuation(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromInitialPunctuation_value = std::move(stack.top());
            context->fromInitialPunctuation = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromInitialPunctuation_value.get());
            stack.pop();
        }
    }
    void PostFinalPunctuation(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromFinalPunctuation_value = std::move(stack.top());
            context->fromFinalPunctuation = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromFinalPunctuation_value.get());
            stack.pop();
        }
    }
    void PostOtherPunctuation(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromOtherPunctuation_value = std::move(stack.top());
            context->fromOtherPunctuation = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromOtherPunctuation_value.get());
            stack.pop();
        }
    }
    void PostSymbol(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromSymbol_value = std::move(stack.top());
            context->fromSymbol = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromSymbol_value.get());
            stack.pop();
        }
    }
    void PostMathSymbol(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromMathSymbol_value = std::move(stack.top());
            context->fromMathSymbol = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromMathSymbol_value.get());
            stack.pop();
        }
    }
    void PostCurrencySymbol(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromCurrencySymbol_value = std::move(stack.top());
            context->fromCurrencySymbol = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromCurrencySymbol_value.get());
            stack.pop();
        }
    }
    void PostModifierSymbol(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromModifierSymbol_value = std::move(stack.top());
            context->fromModifierSymbol = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromModifierSymbol_value.get());
            stack.pop();
        }
    }
    void PostOtherSymbol(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromOtherSymbol_value = std::move(stack.top());
            context->fromOtherSymbol = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromOtherSymbol_value.get());
            stack.pop();
        }
    }
    void PostSeparator(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromSeparator_value = std::move(stack.top());
            context->fromSeparator = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromSeparator_value.get());
            stack.pop();
        }
    }
    void PostSpaceSeparator(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromSpaceSeparator_value = std::move(stack.top());
            context->fromSpaceSeparator = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromSpaceSeparator_value.get());
            stack.pop();
        }
    }
    void PostLineSeparator(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromLineSeparator_value = std::move(stack.top());
            context->fromLineSeparator = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromLineSeparator_value.get());
            stack.pop();
        }
    }
    void PostParagraphSeparator(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromParagraphSeparator_value = std::move(stack.top());
            context->fromParagraphSeparator = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromParagraphSeparator_value.get());
            stack.pop();
        }
    }
    void PostOther(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromOther_value = std::move(stack.top());
            context->fromOther = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromOther_value.get());
            stack.pop();
        }
    }
    void PostControl(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromControl_value = std::move(stack.top());
            context->fromControl = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromControl_value.get());
            stack.pop();
        }
    }
    void PostFormat(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromFormat_value = std::move(stack.top());
            context->fromFormat = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromFormat_value.get());
            stack.pop();
        }
    }
    void PostSurrogate(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromSurrogate_value = std::move(stack.top());
            context->fromSurrogate = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromSurrogate_value.get());
            stack.pop();
        }
    }
    void PostPrivateUse(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromPrivateUse_value = std::move(stack.top());
            context->fromPrivateUse = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromPrivateUse_value.get());
            stack.pop();
        }
    }
    void PostUnassigned(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromUnassigned_value = std::move(stack.top());
            context->fromUnassigned = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromUnassigned_value.get());
            stack.pop();
        }
    }
    void PostGraphic(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromGraphic_value = std::move(stack.top());
            context->fromGraphic = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromGraphic_value.get());
            stack.pop();
        }
    }
    void PostBaseChar(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromBaseChar_value = std::move(stack.top());
            context->fromBaseChar = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromBaseChar_value.get());
            stack.pop();
        }
    }
    void PostAlphabetic(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromAlphabetic_value = std::move(stack.top());
            context->fromAlphabetic = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromAlphabetic_value.get());
            stack.pop();
        }
    }
    void PostIdStart(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromIdStart_value = std::move(stack.top());
            context->fromIdStart = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromIdStart_value.get());
            stack.pop();
        }
    }
    void PostIdCont(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromIdCont_value = std::move(stack.top());
            context->fromIdCont = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromIdCont_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromChar(), fromString(), fromCharSet(), fromKeyword(), fromKeywordList(), fromRange(), fromEmpty(), fromSpace(), fromAnyChar(), fromLetter(), fromDigit(), fromHexDigit(), fromUpperLetter(), fromLowerLetter(), fromTitleLetter(), fromModifierLetter(), fromOtherLetter(), fromCasedLetter(), fromMark(), fromNonspacingMark(), fromEnclosingMark(), fromSpacingMark(), fromNumber(), fromDecimalNumber(), fromLetterNumber(), fromOtherNumber(), fromPunctuation(), fromConnectorPunctuation(), fromDashPunctuation(), fromOpenPunctuation(), fromClosePunctuation(), fromInitialPunctuation(), fromFinalPunctuation(), fromOtherPunctuation(), fromSymbol(), fromMathSymbol(), fromCurrencySymbol(), fromModifierSymbol(), fromOtherSymbol(), fromSeparator(), fromSpaceSeparator(), fromLineSeparator(), fromParagraphSeparator(), fromOther(), fromControl(), fromFormat(), fromSurrogate(), fromPrivateUse(), fromUnassigned(), fromGraphic(), fromBaseChar(), fromAlphabetic(), fromIdStart(), fromIdCont() {}
        soul::parsing::Parser* value;
        soul::parsing::Parser* fromChar;
        soul::parsing::Parser* fromString;
        soul::parsing::Parser* fromCharSet;
        soul::parsing::Parser* fromKeyword;
        soul::parsing::Parser* fromKeywordList;
        soul::parsing::Parser* fromRange;
        soul::parsing::Parser* fromEmpty;
        soul::parsing::Parser* fromSpace;
        soul::parsing::Parser* fromAnyChar;
        soul::parsing::Parser* fromLetter;
        soul::parsing::Parser* fromDigit;
        soul::parsing::Parser* fromHexDigit;
        soul::parsing::Parser* fromUpperLetter;
        soul::parsing::Parser* fromLowerLetter;
        soul::parsing::Parser* fromTitleLetter;
        soul::parsing::Parser* fromModifierLetter;
        soul::parsing::Parser* fromOtherLetter;
        soul::parsing::Parser* fromCasedLetter;
        soul::parsing::Parser* fromMark;
        soul::parsing::Parser* fromNonspacingMark;
        soul::parsing::Parser* fromEnclosingMark;
        soul::parsing::Parser* fromSpacingMark;
        soul::parsing::Parser* fromNumber;
        soul::parsing::Parser* fromDecimalNumber;
        soul::parsing::Parser* fromLetterNumber;
        soul::parsing::Parser* fromOtherNumber;
        soul::parsing::Parser* fromPunctuation;
        soul::parsing::Parser* fromConnectorPunctuation;
        soul::parsing::Parser* fromDashPunctuation;
        soul::parsing::Parser* fromOpenPunctuation;
        soul::parsing::Parser* fromClosePunctuation;
        soul::parsing::Parser* fromInitialPunctuation;
        soul::parsing::Parser* fromFinalPunctuation;
        soul::parsing::Parser* fromOtherPunctuation;
        soul::parsing::Parser* fromSymbol;
        soul::parsing::Parser* fromMathSymbol;
        soul::parsing::Parser* fromCurrencySymbol;
        soul::parsing::Parser* fromModifierSymbol;
        soul::parsing::Parser* fromOtherSymbol;
        soul::parsing::Parser* fromSeparator;
        soul::parsing::Parser* fromSpaceSeparator;
        soul::parsing::Parser* fromLineSeparator;
        soul::parsing::Parser* fromParagraphSeparator;
        soul::parsing::Parser* fromOther;
        soul::parsing::Parser* fromControl;
        soul::parsing::Parser* fromFormat;
        soul::parsing::Parser* fromSurrogate;
        soul::parsing::Parser* fromPrivateUse;
        soul::parsing::Parser* fromUnassigned;
        soul::parsing::Parser* fromGraphic;
        soul::parsing::Parser* fromBaseChar;
        soul::parsing::Parser* fromAlphabetic;
        soul::parsing::Parser* fromIdStart;
        soul::parsing::Parser* fromIdCont;
    };
};

class PrimitiveGrammar::CharRule : public soul::parsing::Rule
{
public:
    CharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharRule>(this, &CharRule::A0Action));
        soul::parsing::NonterminalParser* chrNonterminalParser = GetNonterminal(ToUtf32("chr"));
        chrNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CharRule>(this, &CharRule::Postchr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CharParser(context->fromchr);
    }
    void Postchr(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromchr_value = std::move(stack.top());
            context->fromchr = *static_cast<soul::parsing::ValueObject<char32_t>*>(fromchr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromchr() {}
        soul::parsing::Parser* value;
        char32_t fromchr;
    };
};

class PrimitiveGrammar::StringRule : public soul::parsing::Rule
{
public:
    StringRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<StringRule>(this, &StringRule::A0Action));
        soul::parsing::NonterminalParser* strNonterminalParser = GetNonterminal(ToUtf32("str"));
        strNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<StringRule>(this, &StringRule::Poststr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new StringParser(context->fromstr);
    }
    void Poststr(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromstr_value = std::move(stack.top());
            context->fromstr = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromstr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromstr() {}
        soul::parsing::Parser* value;
        std::u32string fromstr;
    };
};

class PrimitiveGrammar::CharSetRule : public soul::parsing::Rule
{
public:
    CharSetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("inverse")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("s")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A2Action));
        soul::parsing::NonterminalParser* charSetRangeNonterminalParser = GetNonterminal(ToUtf32("CharSetRange"));
        charSetRangeNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CharSetRule>(this, &CharSetRule::PostCharSetRange));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CharSetParser(context->s, context->inverse);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->inverse = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.append(context->fromCharSetRange);
    }
    void PostCharSetRange(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromCharSetRange_value = std::move(stack.top());
            context->fromCharSetRange = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromCharSetRange_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), inverse(), s(), fromCharSetRange() {}
        soul::parsing::Parser* value;
        bool inverse;
        std::u32string s;
        std::u32string fromCharSetRange;
    };
};

class PrimitiveGrammar::CharSetRangeRule : public soul::parsing::Rule
{
public:
    CharSetRangeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<std::u32string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharSetRangeRule>(this, &CharSetRangeRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharSetRangeRule>(this, &CharSetRangeRule::A1Action));
        soul::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CharSetRangeRule>(this, &CharSetRangeRule::Postfirst));
        soul::parsing::NonterminalParser* lastNonterminalParser = GetNonterminal(ToUtf32("last"));
        lastNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CharSetRangeRule>(this, &CharSetRangeRule::Postlast));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, context->fromfirst);
        context->value.append(1, '-');
        context->value.append(1, context->fromlast);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, context->fromfirst);
    }
    void Postfirst(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromfirst_value = std::move(stack.top());
            context->fromfirst = *static_cast<soul::parsing::ValueObject<char32_t>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postlast(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromlast_value = std::move(stack.top());
            context->fromlast = *static_cast<soul::parsing::ValueObject<char32_t>*>(fromlast_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromfirst(), fromlast() {}
        std::u32string value;
        char32_t fromfirst;
        char32_t fromlast;
    };
};

class PrimitiveGrammar::CharSetCharRule : public soul::parsing::Rule
{
public:
    CharSetCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("char32_t"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<char32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharSetCharRule>(this, &CharSetCharRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharSetCharRule>(this, &CharSetCharRule::A1Action));
        soul::parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal(ToUtf32("escape"));
        escapeNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CharSetCharRule>(this, &CharSetCharRule::Postescape));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = *matchBegin;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromescape;
    }
    void Postescape(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromescape_value = std::move(stack.top());
            context->fromescape = *static_cast<soul::parsing::ValueObject<char32_t>*>(fromescape_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromescape() {}
        char32_t value;
        char32_t fromescape;
    };
};

class PrimitiveGrammar::KeywordRule : public soul::parsing::Rule
{
public:
    KeywordRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<KeywordRule>(this, &KeywordRule::A0Action));
        soul::parsing::NonterminalParser* keywordBodyNonterminalParser = GetNonterminal(ToUtf32("KeywordBody"));
        keywordBodyNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<KeywordRule>(this, &KeywordRule::PostKeywordBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeywordBody;
    }
    void PostKeywordBody(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromKeywordBody_value = std::move(stack.top());
            context->fromKeywordBody = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromKeywordBody_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromKeywordBody() {}
        soul::parsing::Parser* value;
        soul::parsing::Parser* fromKeywordBody;
    };
};

class PrimitiveGrammar::KeywordBodyRule : public soul::parsing::Rule
{
public:
    KeywordBodyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<KeywordBodyRule>(this, &KeywordBodyRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<KeywordBodyRule>(this, &KeywordBodyRule::A1Action));
        soul::parsing::NonterminalParser* strNonterminalParser = GetNonterminal(ToUtf32("str"));
        strNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<KeywordBodyRule>(this, &KeywordBodyRule::Poststr));
        soul::parsing::NonterminalParser* continuationNonterminalParser = GetNonterminal(ToUtf32("continuation"));
        continuationNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<KeywordBodyRule>(this, &KeywordBodyRule::Postcontinuation));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KeywordParser(context->fromstr, context->fromcontinuation);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KeywordParser(context->fromstr);
    }
    void Poststr(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromstr_value = std::move(stack.top());
            context->fromstr = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromstr_value.get());
            stack.pop();
        }
    }
    void Postcontinuation(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromcontinuation_value = std::move(stack.top());
            context->fromcontinuation = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromcontinuation_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromstr(), fromcontinuation() {}
        soul::parsing::Parser* value;
        std::u32string fromstr;
        std::u32string fromcontinuation;
    };
};

class PrimitiveGrammar::KeywordListRule : public soul::parsing::Rule
{
public:
    KeywordListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<KeywordListRule>(this, &KeywordListRule::A0Action));
        soul::parsing::NonterminalParser* keywordListBodyNonterminalParser = GetNonterminal(ToUtf32("KeywordListBody"));
        keywordListBodyNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<KeywordListRule>(this, &KeywordListRule::PostKeywordListBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeywordListBody;
    }
    void PostKeywordListBody(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromKeywordListBody_value = std::move(stack.top());
            context->fromKeywordListBody = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromKeywordListBody_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromKeywordListBody() {}
        soul::parsing::Parser* value;
        soul::parsing::Parser* fromKeywordListBody;
    };
};

class PrimitiveGrammar::KeywordListBodyRule : public soul::parsing::Rule
{
public:
    KeywordListBodyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::vector<std::u32string>"), ToUtf32("keywords")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<KeywordListBodyRule>(this, &KeywordListBodyRule::A0Action));
        soul::parsing::NonterminalParser* selectorNonterminalParser = GetNonterminal(ToUtf32("selector"));
        selectorNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<KeywordListBodyRule>(this, &KeywordListBodyRule::Postselector));
        soul::parsing::NonterminalParser* stringArrayNonterminalParser = GetNonterminal(ToUtf32("StringArray"));
        stringArrayNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<KeywordListBodyRule>(this, &KeywordListBodyRule::PreStringArray));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KeywordListParser(context->fromselector, context->keywords);
    }
    void Postselector(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromselector_value = std::move(stack.top());
            context->fromselector = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromselector_value.get());
            stack.pop();
        }
    }
    void PreStringArray(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<std::vector<std::u32string>*>(&context->keywords)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), keywords(), fromselector() {}
        soul::parsing::Parser* value;
        std::vector<std::u32string> keywords;
        std::u32string fromselector;
    };
};

class PrimitiveGrammar::RangeRule : public soul::parsing::Rule
{
public:
    RangeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<RangeRule>(this, &RangeRule::A0Action));
        soul::parsing::NonterminalParser* sNonterminalParser = GetNonterminal(ToUtf32("s"));
        sNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<RangeRule>(this, &RangeRule::Posts));
        soul::parsing::NonterminalParser* eNonterminalParser = GetNonterminal(ToUtf32("e"));
        eNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<RangeRule>(this, &RangeRule::Poste));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new RangeParser(context->froms, context->frome);
    }
    void Posts(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> froms_value = std::move(stack.top());
            context->froms = *static_cast<soul::parsing::ValueObject<uint32_t>*>(froms_value.get());
            stack.pop();
        }
    }
    void Poste(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> frome_value = std::move(stack.top());
            context->frome = *static_cast<soul::parsing::ValueObject<uint32_t>*>(frome_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), froms(), frome() {}
        soul::parsing::Parser* value;
        uint32_t froms;
        uint32_t frome;
    };
};

class PrimitiveGrammar::CodePointRule : public soul::parsing::Rule
{
public:
    CodePointRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint32_t"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<uint32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CodePointRule>(this, &CodePointRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<CodePointRule>(this, &CodePointRule::A1Action));
        soul::parsing::NonterminalParser* xNonterminalParser = GetNonterminal(ToUtf32("x"));
        xNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CodePointRule>(this, &CodePointRule::Postx));
        soul::parsing::NonterminalParser* dNonterminalParser = GetNonterminal(ToUtf32("d"));
        dNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CodePointRule>(this, &CodePointRule::Postd));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromx;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromd;
    }
    void Postx(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromx_value = std::move(stack.top());
            context->fromx = *static_cast<soul::parsing::ValueObject<uint32_t>*>(fromx_value.get());
            stack.pop();
        }
    }
    void Postd(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromd_value = std::move(stack.top());
            context->fromd = *static_cast<soul::parsing::ValueObject<uint32_t>*>(fromd_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromx(), fromd() {}
        uint32_t value;
        uint32_t fromx;
        uint32_t fromd;
    };
};

class PrimitiveGrammar::EmptyRule : public soul::parsing::Rule
{
public:
    EmptyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<EmptyRule>(this, &EmptyRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EmptyParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::SpaceRule : public soul::parsing::Rule
{
public:
    SpaceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<SpaceRule>(this, &SpaceRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SpaceParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::AnyCharRule : public soul::parsing::Rule
{
public:
    AnyCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<AnyCharRule>(this, &AnyCharRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AnyCharParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::LetterRule : public soul::parsing::Rule
{
public:
    LetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<LetterRule>(this, &LetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LetterParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::DigitRule : public soul::parsing::Rule
{
public:
    DigitRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<DigitRule>(this, &DigitRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DigitParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::HexDigitRule : public soul::parsing::Rule
{
public:
    HexDigitRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<HexDigitRule>(this, &HexDigitRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new HexDigitParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::UpperLetterRule : public soul::parsing::Rule
{
public:
    UpperLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<UpperLetterRule>(this, &UpperLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UpperLetterParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::LowerLetterRule : public soul::parsing::Rule
{
public:
    LowerLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<LowerLetterRule>(this, &LowerLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LowerLetterParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::TitleLetterRule : public soul::parsing::Rule
{
public:
    TitleLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<TitleLetterRule>(this, &TitleLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TitleLetterParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::ModifierLetterRule : public soul::parsing::Rule
{
public:
    ModifierLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ModifierLetterRule>(this, &ModifierLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ModifierLetterParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::OtherLetterRule : public soul::parsing::Rule
{
public:
    OtherLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<OtherLetterRule>(this, &OtherLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherLetterParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::CasedLetterRule : public soul::parsing::Rule
{
public:
    CasedLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CasedLetterRule>(this, &CasedLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CasedLetterParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::MarkRule : public soul::parsing::Rule
{
public:
    MarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<MarkRule>(this, &MarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MarkParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::NonspacingMarkRule : public soul::parsing::Rule
{
public:
    NonspacingMarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<NonspacingMarkRule>(this, &NonspacingMarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NonspacingMarkParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::SpacingMarkRule : public soul::parsing::Rule
{
public:
    SpacingMarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<SpacingMarkRule>(this, &SpacingMarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SpacingMarkParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::EnclosingMarkRule : public soul::parsing::Rule
{
public:
    EnclosingMarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<EnclosingMarkRule>(this, &EnclosingMarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EnclosingMarkParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::NumberRule : public soul::parsing::Rule
{
public:
    NumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<NumberRule>(this, &NumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NumberParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::DecimalNumberRule : public soul::parsing::Rule
{
public:
    DecimalNumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<DecimalNumberRule>(this, &DecimalNumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DecimalNumberParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::LetterNumberRule : public soul::parsing::Rule
{
public:
    LetterNumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<LetterNumberRule>(this, &LetterNumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LetterNumberParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::OtherNumberRule : public soul::parsing::Rule
{
public:
    OtherNumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<OtherNumberRule>(this, &OtherNumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherNumberParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::PunctuationRule : public soul::parsing::Rule
{
public:
    PunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PunctuationRule>(this, &PunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PunctuationParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::ConnectorPunctuationRule : public soul::parsing::Rule
{
public:
    ConnectorPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ConnectorPunctuationRule>(this, &ConnectorPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConnectorPunctuationParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::DashPunctuationRule : public soul::parsing::Rule
{
public:
    DashPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<DashPunctuationRule>(this, &DashPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DashPunctuationParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::OpenPunctuationRule : public soul::parsing::Rule
{
public:
    OpenPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<OpenPunctuationRule>(this, &OpenPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OpenPunctuationParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::ClosePunctuationRule : public soul::parsing::Rule
{
public:
    ClosePunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ClosePunctuationRule>(this, &ClosePunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ClosePunctuationParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::InitialPunctuationRule : public soul::parsing::Rule
{
public:
    InitialPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<InitialPunctuationRule>(this, &InitialPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new InitialPunctuationParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::FinalPunctuationRule : public soul::parsing::Rule
{
public:
    FinalPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<FinalPunctuationRule>(this, &FinalPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new FinalPunctuationParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::OtherPunctuationRule : public soul::parsing::Rule
{
public:
    OtherPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<OtherPunctuationRule>(this, &OtherPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherPunctuationParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::SymbolRule : public soul::parsing::Rule
{
public:
    SymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<SymbolRule>(this, &SymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SymbolParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::MathSymbolRule : public soul::parsing::Rule
{
public:
    MathSymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<MathSymbolRule>(this, &MathSymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MathSymbolParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::CurrencySymbolRule : public soul::parsing::Rule
{
public:
    CurrencySymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CurrencySymbolRule>(this, &CurrencySymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CurrencySymbolParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::ModifierSymbolRule : public soul::parsing::Rule
{
public:
    ModifierSymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ModifierSymbolRule>(this, &ModifierSymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ModifierSymbolParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::OtherSymbolRule : public soul::parsing::Rule
{
public:
    OtherSymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<OtherSymbolRule>(this, &OtherSymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherSymbolParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::SeparatorRule : public soul::parsing::Rule
{
public:
    SeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<SeparatorRule>(this, &SeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SeparatorParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::SpaceSeparatorRule : public soul::parsing::Rule
{
public:
    SpaceSeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<SpaceSeparatorRule>(this, &SpaceSeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SpaceSeparatorParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::LineSeparatorRule : public soul::parsing::Rule
{
public:
    LineSeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<LineSeparatorRule>(this, &LineSeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LineSeparatorParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::ParagraphSeparatorRule : public soul::parsing::Rule
{
public:
    ParagraphSeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ParagraphSeparatorRule>(this, &ParagraphSeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ParagraphSeparatorParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::OtherRule : public soul::parsing::Rule
{
public:
    OtherRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<OtherRule>(this, &OtherRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::ControlRule : public soul::parsing::Rule
{
public:
    ControlRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ControlRule>(this, &ControlRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ControlParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::FormatRule : public soul::parsing::Rule
{
public:
    FormatRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<FormatRule>(this, &FormatRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new FormatParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::SurrogateRule : public soul::parsing::Rule
{
public:
    SurrogateRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<SurrogateRule>(this, &SurrogateRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SurrogateParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::PrivateUseRule : public soul::parsing::Rule
{
public:
    PrivateUseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrivateUseRule>(this, &PrivateUseRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PrivateUseParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::UnassignedRule : public soul::parsing::Rule
{
public:
    UnassignedRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnassignedRule>(this, &UnassignedRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UnassignedParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::GraphicRule : public soul::parsing::Rule
{
public:
    GraphicRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<GraphicRule>(this, &GraphicRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new GraphicParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::BaseCharRule : public soul::parsing::Rule
{
public:
    BaseCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<BaseCharRule>(this, &BaseCharRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BaseCharParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::AlphabeticRule : public soul::parsing::Rule
{
public:
    AlphabeticRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<AlphabeticRule>(this, &AlphabeticRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AlphabeticParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::IdStartRule : public soul::parsing::Rule
{
public:
    IdStartRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<IdStartRule>(this, &IdStartRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdStartParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

class PrimitiveGrammar::IdContRule : public soul::parsing::Rule
{
public:
    IdContRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<IdContRule>(this, &IdContRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdContParser;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::parsing::Parser* value;
    };
};

void PrimitiveGrammar::GetReferencedGrammars()
{
    soul::parsing::ParsingDomain* pd = GetParsingDomain();
    soul::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("soul.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = soul::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    soul::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("soul.syntax.ElementGrammar"));
    if (!grammar1)
    {
        grammar1 = soul::syntax::ElementGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void PrimitiveGrammar::CreateRules()
{
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("hexuint"), this, ToUtf32("soul.parsing.stdlib.hexuint")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("char"), this, ToUtf32("soul.parsing.stdlib.char")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("soul.parsing.stdlib.string")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("escape"), this, ToUtf32("soul.parsing.stdlib.escape")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("int"), this, ToUtf32("soul.parsing.stdlib.int")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("ElementGrammar.QualifiedId")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("uint"), this, ToUtf32("soul.parsing.stdlib.uint")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("StringArray"), this, ToUtf32("ElementGrammar.StringArray")));
    AddRule(new PrimitiveRule(ToUtf32("Primitive"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::AlternativeParser(
                                new soul::parsing::AlternativeParser(
                                    new soul::parsing::AlternativeParser(
                                        new soul::parsing::AlternativeParser(
                                            new soul::parsing::AlternativeParser(
                                                new soul::parsing::AlternativeParser(
                                                    new soul::parsing::AlternativeParser(
                                                        new soul::parsing::AlternativeParser(
                                                            new soul::parsing::AlternativeParser(
                                                                new soul::parsing::AlternativeParser(
                                                                    new soul::parsing::AlternativeParser(
                                                                        new soul::parsing::AlternativeParser(
                                                                            new soul::parsing::AlternativeParser(
                                                                                new soul::parsing::AlternativeParser(
                                                                                    new soul::parsing::AlternativeParser(
                                                                                        new soul::parsing::AlternativeParser(
                                                                                            new soul::parsing::AlternativeParser(
                                                                                                new soul::parsing::AlternativeParser(
                                                                                                    new soul::parsing::AlternativeParser(
                                                                                                        new soul::parsing::AlternativeParser(
                                                                                                            new soul::parsing::AlternativeParser(
                                                                                                                new soul::parsing::AlternativeParser(
                                                                                                                    new soul::parsing::AlternativeParser(
                                                                                                                        new soul::parsing::AlternativeParser(
                                                                                                                            new soul::parsing::AlternativeParser(
                                                                                                                                new soul::parsing::AlternativeParser(
                                                                                                                                    new soul::parsing::AlternativeParser(
                                                                                                                                        new soul::parsing::AlternativeParser(
                                                                                                                                            new soul::parsing::AlternativeParser(
                                                                                                                                                new soul::parsing::AlternativeParser(
                                                                                                                                                    new soul::parsing::AlternativeParser(
                                                                                                                                                        new soul::parsing::AlternativeParser(
                                                                                                                                                            new soul::parsing::AlternativeParser(
                                                                                                                                                                new soul::parsing::AlternativeParser(
                                                                                                                                                                    new soul::parsing::AlternativeParser(
                                                                                                                                                                        new soul::parsing::AlternativeParser(
                                                                                                                                                                            new soul::parsing::AlternativeParser(
                                                                                                                                                                                new soul::parsing::AlternativeParser(
                                                                                                                                                                                    new soul::parsing::AlternativeParser(
                                                                                                                                                                                        new soul::parsing::AlternativeParser(
                                                                                                                                                                                            new soul::parsing::AlternativeParser(
                                                                                                                                                                                                new soul::parsing::AlternativeParser(
                                                                                                                                                                                                    new soul::parsing::AlternativeParser(
                                                                                                                                                                                                        new soul::parsing::AlternativeParser(
                                                                                                                                                                                                            new soul::parsing::AlternativeParser(
                                                                                                                                                                                                                new soul::parsing::AlternativeParser(
                                                                                                                                                                                                                    new soul::parsing::AlternativeParser(
                                                                                                                                                                                                                        new soul::parsing::AlternativeParser(
                                                                                                                                                                                                                            new soul::parsing::ActionParser(ToUtf32("A0"),
                                                                                                                                                                                                                                new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
                                                                                                                                                                                                                            new soul::parsing::ActionParser(ToUtf32("A1"),
                                                                                                                                                                                                                                new soul::parsing::NonterminalParser(ToUtf32("String"), ToUtf32("String"), 0))),
                                                                                                                                                                                                                        new soul::parsing::ActionParser(ToUtf32("A2"),
                                                                                                                                                                                                                            new soul::parsing::NonterminalParser(ToUtf32("CharSet"), ToUtf32("CharSet"), 0))),
                                                                                                                                                                                                                    new soul::parsing::ActionParser(ToUtf32("A3"),
                                                                                                                                                                                                                        new soul::parsing::NonterminalParser(ToUtf32("Keyword"), ToUtf32("Keyword"), 0))),
                                                                                                                                                                                                                new soul::parsing::ActionParser(ToUtf32("A4"),
                                                                                                                                                                                                                    new soul::parsing::NonterminalParser(ToUtf32("KeywordList"), ToUtf32("KeywordList"), 0))),
                                                                                                                                                                                                            new soul::parsing::ActionParser(ToUtf32("A5"),
                                                                                                                                                                                                                new soul::parsing::NonterminalParser(ToUtf32("Range"), ToUtf32("Range"), 0))),
                                                                                                                                                                                                        new soul::parsing::ActionParser(ToUtf32("A6"),
                                                                                                                                                                                                            new soul::parsing::NonterminalParser(ToUtf32("Empty"), ToUtf32("Empty"), 0))),
                                                                                                                                                                                                    new soul::parsing::ActionParser(ToUtf32("A7"),
                                                                                                                                                                                                        new soul::parsing::NonterminalParser(ToUtf32("Space"), ToUtf32("Space"), 0))),
                                                                                                                                                                                                new soul::parsing::ActionParser(ToUtf32("A8"),
                                                                                                                                                                                                    new soul::parsing::NonterminalParser(ToUtf32("AnyChar"), ToUtf32("AnyChar"), 0))),
                                                                                                                                                                                            new soul::parsing::ActionParser(ToUtf32("A9"),
                                                                                                                                                                                                new soul::parsing::NonterminalParser(ToUtf32("Letter"), ToUtf32("Letter"), 0))),
                                                                                                                                                                                        new soul::parsing::ActionParser(ToUtf32("A10"),
                                                                                                                                                                                            new soul::parsing::NonterminalParser(ToUtf32("Digit"), ToUtf32("Digit"), 0))),
                                                                                                                                                                                    new soul::parsing::ActionParser(ToUtf32("A11"),
                                                                                                                                                                                        new soul::parsing::NonterminalParser(ToUtf32("HexDigit"), ToUtf32("HexDigit"), 0))),
                                                                                                                                                                                new soul::parsing::ActionParser(ToUtf32("A12"),
                                                                                                                                                                                    new soul::parsing::NonterminalParser(ToUtf32("UpperLetter"), ToUtf32("UpperLetter"), 0))),
                                                                                                                                                                            new soul::parsing::ActionParser(ToUtf32("A13"),
                                                                                                                                                                                new soul::parsing::NonterminalParser(ToUtf32("LowerLetter"), ToUtf32("LowerLetter"), 0))),
                                                                                                                                                                        new soul::parsing::ActionParser(ToUtf32("A14"),
                                                                                                                                                                            new soul::parsing::NonterminalParser(ToUtf32("TitleLetter"), ToUtf32("TitleLetter"), 0))),
                                                                                                                                                                    new soul::parsing::ActionParser(ToUtf32("A15"),
                                                                                                                                                                        new soul::parsing::NonterminalParser(ToUtf32("ModifierLetter"), ToUtf32("ModifierLetter"), 0))),
                                                                                                                                                                new soul::parsing::ActionParser(ToUtf32("A16"),
                                                                                                                                                                    new soul::parsing::NonterminalParser(ToUtf32("OtherLetter"), ToUtf32("OtherLetter"), 0))),
                                                                                                                                                            new soul::parsing::ActionParser(ToUtf32("A17"),
                                                                                                                                                                new soul::parsing::NonterminalParser(ToUtf32("CasedLetter"), ToUtf32("CasedLetter"), 0))),
                                                                                                                                                        new soul::parsing::ActionParser(ToUtf32("A18"),
                                                                                                                                                            new soul::parsing::NonterminalParser(ToUtf32("Mark"), ToUtf32("Mark"), 0))),
                                                                                                                                                    new soul::parsing::ActionParser(ToUtf32("A19"),
                                                                                                                                                        new soul::parsing::NonterminalParser(ToUtf32("NonspacingMark"), ToUtf32("NonspacingMark"), 0))),
                                                                                                                                                new soul::parsing::ActionParser(ToUtf32("A20"),
                                                                                                                                                    new soul::parsing::NonterminalParser(ToUtf32("EnclosingMark"), ToUtf32("EnclosingMark"), 0))),
                                                                                                                                            new soul::parsing::ActionParser(ToUtf32("A21"),
                                                                                                                                                new soul::parsing::NonterminalParser(ToUtf32("SpacingMark"), ToUtf32("SpacingMark"), 0))),
                                                                                                                                        new soul::parsing::ActionParser(ToUtf32("A22"),
                                                                                                                                            new soul::parsing::NonterminalParser(ToUtf32("Number"), ToUtf32("Number"), 0))),
                                                                                                                                    new soul::parsing::ActionParser(ToUtf32("A23"),
                                                                                                                                        new soul::parsing::NonterminalParser(ToUtf32("DecimalNumber"), ToUtf32("DecimalNumber"), 0))),
                                                                                                                                new soul::parsing::ActionParser(ToUtf32("A24"),
                                                                                                                                    new soul::parsing::NonterminalParser(ToUtf32("LetterNumber"), ToUtf32("LetterNumber"), 0))),
                                                                                                                            new soul::parsing::ActionParser(ToUtf32("A25"),
                                                                                                                                new soul::parsing::NonterminalParser(ToUtf32("OtherNumber"), ToUtf32("OtherNumber"), 0))),
                                                                                                                        new soul::parsing::ActionParser(ToUtf32("A26"),
                                                                                                                            new soul::parsing::NonterminalParser(ToUtf32("Punctuation"), ToUtf32("Punctuation"), 0))),
                                                                                                                    new soul::parsing::ActionParser(ToUtf32("A27"),
                                                                                                                        new soul::parsing::NonterminalParser(ToUtf32("ConnectorPunctuation"), ToUtf32("ConnectorPunctuation"), 0))),
                                                                                                                new soul::parsing::ActionParser(ToUtf32("A28"),
                                                                                                                    new soul::parsing::NonterminalParser(ToUtf32("DashPunctuation"), ToUtf32("DashPunctuation"), 0))),
                                                                                                            new soul::parsing::ActionParser(ToUtf32("A29"),
                                                                                                                new soul::parsing::NonterminalParser(ToUtf32("OpenPunctuation"), ToUtf32("OpenPunctuation"), 0))),
                                                                                                        new soul::parsing::ActionParser(ToUtf32("A30"),
                                                                                                            new soul::parsing::NonterminalParser(ToUtf32("ClosePunctuation"), ToUtf32("ClosePunctuation"), 0))),
                                                                                                    new soul::parsing::ActionParser(ToUtf32("A31"),
                                                                                                        new soul::parsing::NonterminalParser(ToUtf32("InitialPunctuation"), ToUtf32("InitialPunctuation"), 0))),
                                                                                                new soul::parsing::ActionParser(ToUtf32("A32"),
                                                                                                    new soul::parsing::NonterminalParser(ToUtf32("FinalPunctuation"), ToUtf32("FinalPunctuation"), 0))),
                                                                                            new soul::parsing::ActionParser(ToUtf32("A33"),
                                                                                                new soul::parsing::NonterminalParser(ToUtf32("OtherPunctuation"), ToUtf32("OtherPunctuation"), 0))),
                                                                                        new soul::parsing::ActionParser(ToUtf32("A34"),
                                                                                            new soul::parsing::NonterminalParser(ToUtf32("Symbol"), ToUtf32("Symbol"), 0))),
                                                                                    new soul::parsing::ActionParser(ToUtf32("A35"),
                                                                                        new soul::parsing::NonterminalParser(ToUtf32("MathSymbol"), ToUtf32("MathSymbol"), 0))),
                                                                                new soul::parsing::ActionParser(ToUtf32("A36"),
                                                                                    new soul::parsing::NonterminalParser(ToUtf32("CurrencySymbol"), ToUtf32("CurrencySymbol"), 0))),
                                                                            new soul::parsing::ActionParser(ToUtf32("A37"),
                                                                                new soul::parsing::NonterminalParser(ToUtf32("ModifierSymbol"), ToUtf32("ModifierSymbol"), 0))),
                                                                        new soul::parsing::ActionParser(ToUtf32("A38"),
                                                                            new soul::parsing::NonterminalParser(ToUtf32("OtherSymbol"), ToUtf32("OtherSymbol"), 0))),
                                                                    new soul::parsing::ActionParser(ToUtf32("A39"),
                                                                        new soul::parsing::NonterminalParser(ToUtf32("Separator"), ToUtf32("Separator"), 0))),
                                                                new soul::parsing::ActionParser(ToUtf32("A40"),
                                                                    new soul::parsing::NonterminalParser(ToUtf32("SpaceSeparator"), ToUtf32("SpaceSeparator"), 0))),
                                                            new soul::parsing::ActionParser(ToUtf32("A41"),
                                                                new soul::parsing::NonterminalParser(ToUtf32("LineSeparator"), ToUtf32("LineSeparator"), 0))),
                                                        new soul::parsing::ActionParser(ToUtf32("A42"),
                                                            new soul::parsing::NonterminalParser(ToUtf32("ParagraphSeparator"), ToUtf32("ParagraphSeparator"), 0))),
                                                    new soul::parsing::ActionParser(ToUtf32("A43"),
                                                        new soul::parsing::NonterminalParser(ToUtf32("Other"), ToUtf32("Other"), 0))),
                                                new soul::parsing::ActionParser(ToUtf32("A44"),
                                                    new soul::parsing::NonterminalParser(ToUtf32("Control"), ToUtf32("Control"), 0))),
                                            new soul::parsing::ActionParser(ToUtf32("A45"),
                                                new soul::parsing::NonterminalParser(ToUtf32("Format"), ToUtf32("Format"), 0))),
                                        new soul::parsing::ActionParser(ToUtf32("A46"),
                                            new soul::parsing::NonterminalParser(ToUtf32("Surrogate"), ToUtf32("Surrogate"), 0))),
                                    new soul::parsing::ActionParser(ToUtf32("A47"),
                                        new soul::parsing::NonterminalParser(ToUtf32("PrivateUse"), ToUtf32("PrivateUse"), 0))),
                                new soul::parsing::ActionParser(ToUtf32("A48"),
                                    new soul::parsing::NonterminalParser(ToUtf32("Unassigned"), ToUtf32("Unassigned"), 0))),
                            new soul::parsing::ActionParser(ToUtf32("A49"),
                                new soul::parsing::NonterminalParser(ToUtf32("Graphic"), ToUtf32("Graphic"), 0))),
                        new soul::parsing::ActionParser(ToUtf32("A50"),
                            new soul::parsing::NonterminalParser(ToUtf32("BaseChar"), ToUtf32("BaseChar"), 0))),
                    new soul::parsing::ActionParser(ToUtf32("A51"),
                        new soul::parsing::NonterminalParser(ToUtf32("Alphabetic"), ToUtf32("Alphabetic"), 0))),
                new soul::parsing::ActionParser(ToUtf32("A52"),
                    new soul::parsing::NonterminalParser(ToUtf32("IdStart"), ToUtf32("IdStart"), 0))),
            new soul::parsing::ActionParser(ToUtf32("A53"),
                new soul::parsing::NonterminalParser(ToUtf32("IdCont"), ToUtf32("IdCont"), 0)))));
    AddRule(new CharRule(ToUtf32("Char"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::NonterminalParser(ToUtf32("chr"), ToUtf32("char"), 0))));
    AddRule(new StringRule(ToUtf32("String"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::NonterminalParser(ToUtf32("str"), ToUtf32("string"), 0))));
    AddRule(new CharSetRule(ToUtf32("CharSet"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('['),
                            new soul::parsing::OptionalParser(
                                new soul::parsing::ActionParser(ToUtf32("A1"),
                                    new soul::parsing::CharParser('^')))),
                        new soul::parsing::KleeneStarParser(
                            new soul::parsing::ActionParser(ToUtf32("A2"),
                                new soul::parsing::NonterminalParser(ToUtf32("CharSetRange"), ToUtf32("CharSetRange"), 0)))),
                    new soul::parsing::ExpectationParser(
                        new soul::parsing::CharParser(']')))))));
    AddRule(new CharSetRangeRule(ToUtf32("CharSetRange"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("CharSetChar"), 0),
            new soul::parsing::AlternativeParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('-'),
                    new soul::parsing::ActionParser(ToUtf32("A0"),
                        new soul::parsing::NonterminalParser(ToUtf32("last"), ToUtf32("CharSetChar"), 0))),
                new soul::parsing::ActionParser(ToUtf32("A1"),
                    new soul::parsing::EmptyParser())))));
    AddRule(new CharSetCharRule(ToUtf32("CharSetChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::CharSetParser(ToUtf32("\\]"), true)),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::NonterminalParser(ToUtf32("escape"), ToUtf32("escape"), 0)))));
    AddRule(new KeywordRule(ToUtf32("Keyword"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::KeywordParser(ToUtf32("keyword")),
                    new soul::parsing::ExpectationParser(
                        new soul::parsing::CharParser('('))),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::NonterminalParser(ToUtf32("KeywordBody"), ToUtf32("KeywordBody"), 0))),
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::CharParser(')'))))));
    AddRule(new KeywordBodyRule(ToUtf32("KeywordBody"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::NonterminalParser(ToUtf32("str"), ToUtf32("string"), 0),
            new soul::parsing::AlternativeParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser(','),
                    new soul::parsing::ActionParser(ToUtf32("A0"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("continuation"), ToUtf32("QualifiedId"), 0)))),
                new soul::parsing::ActionParser(ToUtf32("A1"),
                    new soul::parsing::EmptyParser())))));
    AddRule(new KeywordListRule(ToUtf32("KeywordList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::KeywordParser(ToUtf32("keyword_list")),
                    new soul::parsing::ExpectationParser(
                        new soul::parsing::CharParser('('))),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::NonterminalParser(ToUtf32("KeywordListBody"), ToUtf32("KeywordListBody"), 0))),
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::CharParser(')'))))));
    AddRule(new KeywordListBodyRule(ToUtf32("KeywordListBody"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::ExpectationParser(
                    new soul::parsing::NonterminalParser(ToUtf32("selector"), ToUtf32("QualifiedId"), 0)),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::CharParser(','))),
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::NonterminalParser(ToUtf32("StringArray"), ToUtf32("StringArray"), 1))))));
    AddRule(new RangeRule(ToUtf32("Range"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::KeywordParser(ToUtf32("range")),
                                new soul::parsing::ExpectationParser(
                                    new soul::parsing::CharParser('('))),
                            new soul::parsing::ExpectationParser(
                                new soul::parsing::NonterminalParser(ToUtf32("s"), ToUtf32("CodePoint"), 0))),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::CharParser(','))),
                    new soul::parsing::ExpectationParser(
                        new soul::parsing::NonterminalParser(ToUtf32("e"), ToUtf32("CodePoint"), 0))),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::CharParser(')'))))));
    AddRule(new CodePointRule(ToUtf32("CodePoint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('x'),
                    new soul::parsing::ActionParser(ToUtf32("A0"),
                        new soul::parsing::NonterminalParser(ToUtf32("x"), ToUtf32("hexuint"), 0)))),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::NonterminalParser(ToUtf32("d"), ToUtf32("uint"), 0)))));
    AddRule(new EmptyRule(ToUtf32("Empty"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("empty")))));
    AddRule(new SpaceRule(ToUtf32("Space"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("space")))));
    AddRule(new AnyCharRule(ToUtf32("AnyChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("anychar")))));
    AddRule(new LetterRule(ToUtf32("Letter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("letter")))));
    AddRule(new DigitRule(ToUtf32("Digit"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("digit")))));
    AddRule(new HexDigitRule(ToUtf32("HexDigit"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("hexdigit")))));
    AddRule(new UpperLetterRule(ToUtf32("UpperLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("upper_letter")))));
    AddRule(new LowerLetterRule(ToUtf32("LowerLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("lower_letter")))));
    AddRule(new TitleLetterRule(ToUtf32("TitleLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("title_letter")))));
    AddRule(new ModifierLetterRule(ToUtf32("ModifierLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("modifier_letter")))));
    AddRule(new OtherLetterRule(ToUtf32("OtherLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("other_letter")))));
    AddRule(new CasedLetterRule(ToUtf32("CasedLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("cased_letter")))));
    AddRule(new MarkRule(ToUtf32("Mark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("mark")))));
    AddRule(new NonspacingMarkRule(ToUtf32("NonspacingMark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("nonspacing_mark")))));
    AddRule(new SpacingMarkRule(ToUtf32("SpacingMark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("spacing_mark")))));
    AddRule(new EnclosingMarkRule(ToUtf32("EnclosingMark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("enclosing_mark")))));
    AddRule(new NumberRule(ToUtf32("Number"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("number")))));
    AddRule(new DecimalNumberRule(ToUtf32("DecimalNumber"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("decimal_number")))));
    AddRule(new LetterNumberRule(ToUtf32("LetterNumber"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("letter_number")))));
    AddRule(new OtherNumberRule(ToUtf32("OtherNumber"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("other_number")))));
    AddRule(new PunctuationRule(ToUtf32("Punctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("punctuation")))));
    AddRule(new ConnectorPunctuationRule(ToUtf32("ConnectorPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("connector_punctuation")))));
    AddRule(new DashPunctuationRule(ToUtf32("DashPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("dash_punctuation")))));
    AddRule(new OpenPunctuationRule(ToUtf32("OpenPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("open_punctuation")))));
    AddRule(new ClosePunctuationRule(ToUtf32("ClosePunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("close_punctuation")))));
    AddRule(new InitialPunctuationRule(ToUtf32("InitialPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("initial_punctuation")))));
    AddRule(new FinalPunctuationRule(ToUtf32("FinalPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("final_punctuation")))));
    AddRule(new OtherPunctuationRule(ToUtf32("OtherPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("other_punctuation")))));
    AddRule(new SymbolRule(ToUtf32("Symbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("symbol")))));
    AddRule(new MathSymbolRule(ToUtf32("MathSymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("math_symbol")))));
    AddRule(new CurrencySymbolRule(ToUtf32("CurrencySymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("currency_symbol")))));
    AddRule(new ModifierSymbolRule(ToUtf32("ModifierSymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("modifier_symbol")))));
    AddRule(new OtherSymbolRule(ToUtf32("OtherSymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("other_symbol")))));
    AddRule(new SeparatorRule(ToUtf32("Separator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("separator")))));
    AddRule(new SpaceSeparatorRule(ToUtf32("SpaceSeparator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("space_separator")))));
    AddRule(new LineSeparatorRule(ToUtf32("LineSeparator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("line_separator")))));
    AddRule(new ParagraphSeparatorRule(ToUtf32("ParagraphSeparator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("paragraph_separator")))));
    AddRule(new OtherRule(ToUtf32("Other"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("other")))));
    AddRule(new ControlRule(ToUtf32("Control"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("control")))));
    AddRule(new FormatRule(ToUtf32("Format"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("format")))));
    AddRule(new SurrogateRule(ToUtf32("Surrogate"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("surrogate")))));
    AddRule(new PrivateUseRule(ToUtf32("PrivateUse"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("private_use")))));
    AddRule(new UnassignedRule(ToUtf32("Unassigned"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("unassigned")))));
    AddRule(new GraphicRule(ToUtf32("Graphic"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("graphic")))));
    AddRule(new BaseCharRule(ToUtf32("BaseChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("basechar")))));
    AddRule(new AlphabeticRule(ToUtf32("Alphabetic"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("alphabetic")))));
    AddRule(new IdStartRule(ToUtf32("IdStart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("idstart")))));
    AddRule(new IdContRule(ToUtf32("IdCont"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("idcont")))));
}

} } // namespace soul.syntax
