#include "Literal.hpp"
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

namespace soul { namespace code {

using namespace soul::codedom;
using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;

LiteralGrammar* LiteralGrammar::Create()
{
    return Create(new soul::parsing::ParsingDomain());
}

LiteralGrammar* LiteralGrammar::Create(soul::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    LiteralGrammar* grammar(new LiteralGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

LiteralGrammar::LiteralGrammar(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32("LiteralGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("soul.code")), parsingDomain_)
{
    SetOwner(0);
}

soul::codedom::Literal* LiteralGrammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    soul::codedom::Literal* result = *static_cast<soul::parsing::ValueObject<soul::codedom::Literal*>*>(value.get());
    stack.pop();
    return result;
}

class LiteralGrammar::LiteralRule : public soul::parsing::Rule
{
public:
    LiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::Literal*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A3Action));
        soul::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new soul::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A4Action));
        soul::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new soul::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A5Action));
        soul::parsing::NonterminalParser* integerLiteralNonterminalParser = GetNonterminal(ToUtf32("IntegerLiteral"));
        integerLiteralNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostIntegerLiteral));
        soul::parsing::NonterminalParser* characterLiteralNonterminalParser = GetNonterminal(ToUtf32("CharacterLiteral"));
        characterLiteralNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostCharacterLiteral));
        soul::parsing::NonterminalParser* floatingLiteralNonterminalParser = GetNonterminal(ToUtf32("FloatingLiteral"));
        floatingLiteralNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostFloatingLiteral));
        soul::parsing::NonterminalParser* stringLiteralNonterminalParser = GetNonterminal(ToUtf32("StringLiteral"));
        stringLiteralNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostStringLiteral));
        soul::parsing::NonterminalParser* booleanLiteralNonterminalParser = GetNonterminal(ToUtf32("BooleanLiteral"));
        booleanLiteralNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostBooleanLiteral));
        soul::parsing::NonterminalParser* pointerLiteralNonterminalParser = GetNonterminal(ToUtf32("PointerLiteral"));
        pointerLiteralNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostPointerLiteral));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIntegerLiteral;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCharacterLiteral;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFloatingLiteral;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromStringLiteral;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBooleanLiteral;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPointerLiteral;
    }
    void PostIntegerLiteral(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromIntegerLiteral_value = std::move(stack.top());
            context->fromIntegerLiteral = *static_cast<soul::parsing::ValueObject<soul::codedom::Literal*>*>(fromIntegerLiteral_value.get());
            stack.pop();
        }
    }
    void PostCharacterLiteral(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromCharacterLiteral_value = std::move(stack.top());
            context->fromCharacterLiteral = *static_cast<soul::parsing::ValueObject<soul::codedom::Literal*>*>(fromCharacterLiteral_value.get());
            stack.pop();
        }
    }
    void PostFloatingLiteral(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromFloatingLiteral_value = std::move(stack.top());
            context->fromFloatingLiteral = *static_cast<soul::parsing::ValueObject<soul::codedom::Literal*>*>(fromFloatingLiteral_value.get());
            stack.pop();
        }
    }
    void PostStringLiteral(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromStringLiteral_value = std::move(stack.top());
            context->fromStringLiteral = *static_cast<soul::parsing::ValueObject<soul::codedom::Literal*>*>(fromStringLiteral_value.get());
            stack.pop();
        }
    }
    void PostBooleanLiteral(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromBooleanLiteral_value = std::move(stack.top());
            context->fromBooleanLiteral = *static_cast<soul::parsing::ValueObject<soul::codedom::Literal*>*>(fromBooleanLiteral_value.get());
            stack.pop();
        }
    }
    void PostPointerLiteral(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromPointerLiteral_value = std::move(stack.top());
            context->fromPointerLiteral = *static_cast<soul::parsing::ValueObject<soul::codedom::Literal*>*>(fromPointerLiteral_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromIntegerLiteral(), fromCharacterLiteral(), fromFloatingLiteral(), fromStringLiteral(), fromBooleanLiteral(), fromPointerLiteral() {}
        soul::codedom::Literal* value;
        soul::codedom::Literal* fromIntegerLiteral;
        soul::codedom::Literal* fromCharacterLiteral;
        soul::codedom::Literal* fromFloatingLiteral;
        soul::codedom::Literal* fromStringLiteral;
        soul::codedom::Literal* fromBooleanLiteral;
        soul::codedom::Literal* fromPointerLiteral;
    };
};

class LiteralGrammar::IntegerLiteralRule : public soul::parsing::Rule
{
public:
    IntegerLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::Literal*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Literal(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Literal(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::codedom::Literal* value;
    };
};

class LiteralGrammar::CharacterLiteralRule : public soul::parsing::Rule
{
public:
    CharacterLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::Literal*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Literal(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Literal(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::codedom::Literal* value;
    };
};

class LiteralGrammar::CCharSequenceRule : public soul::parsing::Rule
{
public:
    CCharSequenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CCharSequenceRule>(this, &CCharSequenceRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        std::u32string value;
    };
};

class LiteralGrammar::FloatingLiteralRule : public soul::parsing::Rule
{
public:
    FloatingLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::Literal*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Literal(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::codedom::Literal* value;
    };
};

class LiteralGrammar::StringLiteralRule : public soul::parsing::Rule
{
public:
    StringLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::Literal*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::codedom::Literal* value;
    };
};

class LiteralGrammar::BooleanLiteralRule : public soul::parsing::Rule
{
public:
    BooleanLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::Literal*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<BooleanLiteralRule>(this, &BooleanLiteralRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<BooleanLiteralRule>(this, &BooleanLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Literal(U"true");
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Literal(U"false");
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::codedom::Literal* value;
    };
};

class LiteralGrammar::PointerLiteralRule : public soul::parsing::Rule
{
public:
    PointerLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::Literal*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PointerLiteralRule>(this, &PointerLiteralRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new soul::codedom::Literal(U"nullptr");
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        soul::codedom::Literal* value;
    };
};

void LiteralGrammar::GetReferencedGrammars()
{
}

void LiteralGrammar::CreateRules()
{
    AddRule(new LiteralRule(ToUtf32("Literal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::ActionParser(ToUtf32("A0"),
                                new soul::parsing::NonterminalParser(ToUtf32("IntegerLiteral"), ToUtf32("IntegerLiteral"), 0)),
                            new soul::parsing::ActionParser(ToUtf32("A1"),
                                new soul::parsing::NonterminalParser(ToUtf32("CharacterLiteral"), ToUtf32("CharacterLiteral"), 0))),
                        new soul::parsing::ActionParser(ToUtf32("A2"),
                            new soul::parsing::NonterminalParser(ToUtf32("FloatingLiteral"), ToUtf32("FloatingLiteral"), 0))),
                    new soul::parsing::ActionParser(ToUtf32("A3"),
                        new soul::parsing::NonterminalParser(ToUtf32("StringLiteral"), ToUtf32("StringLiteral"), 0))),
                new soul::parsing::ActionParser(ToUtf32("A4"),
                    new soul::parsing::NonterminalParser(ToUtf32("BooleanLiteral"), ToUtf32("BooleanLiteral"), 0))),
            new soul::parsing::ActionParser(ToUtf32("A5"),
                new soul::parsing::NonterminalParser(ToUtf32("PointerLiteral"), ToUtf32("PointerLiteral"), 0)))));
    AddRule(new IntegerLiteralRule(ToUtf32("IntegerLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::TokenParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::NonterminalParser(ToUtf32("dl"), ToUtf32("DecimalLiteral"), 0),
                            new soul::parsing::OptionalParser(
                                new soul::parsing::NonterminalParser(ToUtf32("is1"), ToUtf32("IntegerSuffix"), 0))))),
                new soul::parsing::ActionParser(ToUtf32("A1"),
                    new soul::parsing::TokenParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::NonterminalParser(ToUtf32("ol"), ToUtf32("OctalLiteral"), 0),
                            new soul::parsing::OptionalParser(
                                new soul::parsing::NonterminalParser(ToUtf32("is2"), ToUtf32("IntegerSuffix"), 0)))))),
            new soul::parsing::ActionParser(ToUtf32("A2"),
                new soul::parsing::TokenParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::NonterminalParser(ToUtf32("hl"), ToUtf32("HexadecimalLiteral"), 0),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("is3"), ToUtf32("IntegerSuffix"), 0))))))));
    AddRule(new soul::parsing::Rule(ToUtf32("DecimalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::TokenParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::CharSetParser(ToUtf32("1-9")),
                new soul::parsing::KleeneStarParser(
                    new soul::parsing::DigitParser())))));
    AddRule(new soul::parsing::Rule(ToUtf32("OctalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::TokenParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::CharParser('0'),
                new soul::parsing::KleeneStarParser(
                    new soul::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0))))));
    AddRule(new soul::parsing::Rule(ToUtf32("OctalDigit"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::CharSetParser(ToUtf32("0-7"))));
    AddRule(new soul::parsing::Rule(ToUtf32("HexadecimalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::TokenParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::StringParser(ToUtf32("0x")),
                    new soul::parsing::StringParser(ToUtf32("0X"))),
                new soul::parsing::PositiveParser(
                    new soul::parsing::HexDigitParser())))));
    AddRule(new soul::parsing::Rule(ToUtf32("IntegerSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::TokenParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::NonterminalParser(ToUtf32("UnsignedSuffix"), ToUtf32("UnsignedSuffix"), 0),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::AlternativeParser(
                                new soul::parsing::NonterminalParser(ToUtf32("LongLongSuffix"), ToUtf32("LongLongSuffix"), 0),
                                new soul::parsing::NonterminalParser(ToUtf32("LongSuffix"), ToUtf32("LongSuffix"), 0))))),
                new soul::parsing::TokenParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::NonterminalParser(ToUtf32("LongLongSuffix"), ToUtf32("LongLongSuffix"), 0),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("UnsignedSuffix"), ToUtf32("UnsignedSuffix"), 0))))),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("LongSuffix"), ToUtf32("LongSuffix"), 0),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("UnsignedSuffix"), ToUtf32("UnsignedSuffix"), 0)))))));
    AddRule(new soul::parsing::Rule(ToUtf32("UnsignedSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::CharParser('u'),
            new soul::parsing::CharParser('U'))));
    AddRule(new soul::parsing::Rule(ToUtf32("LongLongSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::StringParser(ToUtf32("ll")),
            new soul::parsing::StringParser(ToUtf32("LL")))));
    AddRule(new soul::parsing::Rule(ToUtf32("LongSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::CharParser('l'),
            new soul::parsing::CharParser('L'))));
    AddRule(new CharacterLiteralRule(ToUtf32("CharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::NonterminalParser(ToUtf32("NarrowCharacterLiteral"), ToUtf32("NarrowCharacterLiteral"), 0)),
                new soul::parsing::ActionParser(ToUtf32("A1"),
                    new soul::parsing::NonterminalParser(ToUtf32("UniversalCharacterLiteral"), ToUtf32("UniversalCharacterLiteral"), 0))),
            new soul::parsing::ActionParser(ToUtf32("A2"),
                new soul::parsing::NonterminalParser(ToUtf32("WideCharacterLiteral"), ToUtf32("WideCharacterLiteral"), 0)))));
    AddRule(new soul::parsing::Rule(ToUtf32("NarrowCharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::TokenParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('\''),
                    new soul::parsing::NonterminalParser(ToUtf32("CCharSequence"), ToUtf32("CCharSequence"), 0)),
                new soul::parsing::CharParser('\'')))));
    AddRule(new soul::parsing::Rule(ToUtf32("UniversalCharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('u'),
                            new soul::parsing::CharParser('\'')),
                        new soul::parsing::NonterminalParser(ToUtf32("cs1"), ToUtf32("CCharSequence"), 0)),
                    new soul::parsing::CharParser('\''))),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('U'),
                            new soul::parsing::CharParser('\'')),
                        new soul::parsing::NonterminalParser(ToUtf32("cs2"), ToUtf32("CCharSequence"), 0)),
                    new soul::parsing::CharParser('\''))))));
    AddRule(new soul::parsing::Rule(ToUtf32("WideCharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::TokenParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::CharParser('L'),
                        new soul::parsing::CharParser('\'')),
                    new soul::parsing::NonterminalParser(ToUtf32("CCharSequence"), ToUtf32("CCharSequence"), 0)),
                new soul::parsing::CharParser('\'')))));
    AddRule(new CCharSequenceRule(ToUtf32("CCharSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::PositiveParser(
                    new soul::parsing::NonterminalParser(ToUtf32("CChar"), ToUtf32("CChar"), 0))))));
    AddRule(new soul::parsing::Rule(ToUtf32("CChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::CharSetParser(ToUtf32("\'\\\n"), true),
            new soul::parsing::NonterminalParser(ToUtf32("EscapeSequence"), ToUtf32("EscapeSequence"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("EscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::NonterminalParser(ToUtf32("SimpleEscapeSequence"), ToUtf32("SimpleEscapeSequence"), 0),
                new soul::parsing::NonterminalParser(ToUtf32("OctalEscapeSequence"), ToUtf32("OctalEscapeSequence"), 0)),
            new soul::parsing::NonterminalParser(ToUtf32("HexadecimalEscapeSequence"), ToUtf32("HexadecimalEscapeSequence"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("SimpleEscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                new soul::parsing::StringParser(ToUtf32("\\\'")),
                                                new soul::parsing::StringParser(ToUtf32("\\\""))),
                                            new soul::parsing::StringParser(ToUtf32("\\?"))),
                                        new soul::parsing::StringParser(ToUtf32("\\\\"))),
                                    new soul::parsing::StringParser(ToUtf32("\\a"))),
                                new soul::parsing::StringParser(ToUtf32("\\b"))),
                            new soul::parsing::StringParser(ToUtf32("\\f"))),
                        new soul::parsing::StringParser(ToUtf32("\\n"))),
                    new soul::parsing::StringParser(ToUtf32("\\r"))),
                new soul::parsing::StringParser(ToUtf32("\\t"))),
            new soul::parsing::StringParser(ToUtf32("\\v")))));
    AddRule(new soul::parsing::Rule(ToUtf32("OctalEscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('\\'),
                    new soul::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0)),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0))),
            new soul::parsing::OptionalParser(
                new soul::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0)))));
    AddRule(new soul::parsing::Rule(ToUtf32("HexadecimalEscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::StringParser(ToUtf32("\\x")),
            new soul::parsing::PositiveParser(
                new soul::parsing::HexDigitParser()))));
    AddRule(new FloatingLiteralRule(ToUtf32("FloatingLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::TokenParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::NonterminalParser(ToUtf32("FractionalConstant"), ToUtf32("FractionalConstant"), 0),
                            new soul::parsing::OptionalParser(
                                new soul::parsing::NonterminalParser(ToUtf32("ExponentPart"), ToUtf32("ExponentPart"), 0))),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("FloatingSuffix"), ToUtf32("FloatingSuffix"), 0))))),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::TokenParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0),
                            new soul::parsing::NonterminalParser(ToUtf32("ExponentPart"), ToUtf32("ExponentPart"), 0)),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("FloatingSuffix"), ToUtf32("FloatingSuffix"), 0))))))));
    AddRule(new soul::parsing::Rule(ToUtf32("FractionalConstant"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0)),
                    new soul::parsing::CharParser('.')),
                new soul::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0)),
            new soul::parsing::SequenceParser(
                new soul::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0),
                new soul::parsing::CharParser('.')))));
    AddRule(new soul::parsing::Rule(ToUtf32("DigitSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::PositiveParser(
            new soul::parsing::DigitParser())));
    AddRule(new soul::parsing::Rule(ToUtf32("ExponentPart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::CharParser('e'),
                    new soul::parsing::CharParser('E')),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("Sign"), ToUtf32("Sign"), 0))),
            new soul::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("Sign"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::CharParser('+'),
            new soul::parsing::CharParser('-'))));
    AddRule(new soul::parsing::Rule(ToUtf32("FloatingSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::CharSetParser(ToUtf32("fFlL"))));
    AddRule(new StringLiteralRule(ToUtf32("StringLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::OptionalParser(
                                new soul::parsing::NonterminalParser(ToUtf32("EncodingPrefix"), ToUtf32("EncodingPrefix"), 0)),
                            new soul::parsing::CharParser('\"')),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("SCharSequence"), ToUtf32("SCharSequence"), 0))),
                    new soul::parsing::CharParser('\"'))))));
    AddRule(new soul::parsing::Rule(ToUtf32("EncodingPrefix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::StringParser(ToUtf32("u8")),
                    new soul::parsing::CharParser('u')),
                new soul::parsing::CharParser('U')),
            new soul::parsing::CharParser('L'))));
    AddRule(new soul::parsing::Rule(ToUtf32("SCharSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::PositiveParser(
            new soul::parsing::NonterminalParser(ToUtf32("SChar"), ToUtf32("SChar"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("SChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::CharSetParser(ToUtf32("\"\\\n"), true),
            new soul::parsing::NonterminalParser(ToUtf32("EscapeSequence"), ToUtf32("EscapeSequence"), 0))));
    AddRule(new BooleanLiteralRule(ToUtf32("BooleanLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::KeywordParser(ToUtf32("true"))),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::KeywordParser(ToUtf32("false"))))));
    AddRule(new PointerLiteralRule(ToUtf32("PointerLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::KeywordParser(ToUtf32("nullptr")))));
}

} } // namespace soul.code
