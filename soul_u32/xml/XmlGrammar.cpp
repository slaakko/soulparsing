#include "XmlGrammar.hpp"
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

namespace soul { namespace xml {

using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;

XmlGrammar* XmlGrammar::Create()
{
    return Create(new soul::parsing::ParsingDomain());
}

XmlGrammar* XmlGrammar::Create(soul::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    XmlGrammar* grammar(new XmlGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

XmlGrammar::XmlGrammar(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32("XmlGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("soul.xml")), parsingDomain_)
{
    SetOwner(0);
}

void XmlGrammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, XmlProcessor* processor)
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
    stack.push(std::unique_ptr<soul::parsing::Object>(new ValueObject<XmlProcessor*>(processor)));
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
}

class XmlGrammar::DocumentRule : public soul::parsing::Rule
{
public:
    DocumentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<DocumentRule>(this, &DocumentRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<DocumentRule>(this, &DocumentRule::A1Action));
        soul::parsing::NonterminalParser* prologNonterminalParser = GetNonterminal(ToUtf32("Prolog"));
        prologNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<DocumentRule>(this, &DocumentRule::PreProlog));
        soul::parsing::NonterminalParser* elementNonterminalParser = GetNonterminal(ToUtf32("Element"));
        elementNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<DocumentRule>(this, &DocumentRule::PreElement));
        soul::parsing::NonterminalParser* miscNonterminalParser = GetNonterminal(ToUtf32("Misc"));
        miscNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<DocumentRule>(this, &DocumentRule::PreMisc));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->StartDocument();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndDocument();
    }
    void PreProlog(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreElement(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreMisc(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::NameRule : public soul::parsing::Rule
{
public:
    NameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<NameRule>(this, &NameRule::A0Action));
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

class XmlGrammar::EntityValueRule : public soul::parsing::Rule
{
public:
    EntityValueRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A3Action));
        soul::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new soul::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A4Action));
        soul::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new soul::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A5Action));
        soul::parsing::NonterminalParser* pr1NonterminalParser = GetNonterminal(ToUtf32("pr1"));
        pr1NonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Prepr1));
        soul::parsing::NonterminalParser* ref1NonterminalParser = GetNonterminal(ToUtf32("ref1"));
        ref1NonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Preref1));
        soul::parsing::NonterminalParser* pr2NonterminalParser = GetNonterminal(ToUtf32("pr2"));
        pr2NonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Prepr2));
        soul::parsing::NonterminalParser* ref2NonterminalParser = GetNonterminal(ToUtf32("ref2"));
        ref2NonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Preref2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void Prepr1(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Preref1(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Prepr2(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Preref2(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), value() {}
        XmlProcessor* processor;
        std::u32string value;
    };
};

class XmlGrammar::AttValueRule : public soul::parsing::Rule
{
public:
    AttValueRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A3Action));
        soul::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new soul::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A4Action));
        soul::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new soul::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A5Action));
        soul::parsing::NonterminalParser* ref1NonterminalParser = GetNonterminal(ToUtf32("ref1"));
        ref1NonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<AttValueRule>(this, &AttValueRule::Preref1));
        soul::parsing::NonterminalParser* ref2NonterminalParser = GetNonterminal(ToUtf32("ref2"));
        ref2NonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<AttValueRule>(this, &AttValueRule::Preref2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->BeginAttributeValue(&context->value);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndAttributeValue();
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->BeginAttributeValue(&context->value);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndAttributeValue();
    }
    void Preref1(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Preref2(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), value() {}
        XmlProcessor* processor;
        std::u32string value;
    };
};

class XmlGrammar::SystemLiteralRule : public soul::parsing::Rule
{
public:
    SystemLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<SystemLiteralRule>(this, &SystemLiteralRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<SystemLiteralRule>(this, &SystemLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
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

class XmlGrammar::PubidLiteralRule : public soul::parsing::Rule
{
public:
    PubidLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PubidLiteralRule>(this, &PubidLiteralRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<PubidLiteralRule>(this, &PubidLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
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

class XmlGrammar::CharDataRule : public soul::parsing::Rule
{
public:
    CharDataRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharDataRule>(this, &CharDataRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Text(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::CommentRule : public soul::parsing::Rule
{
public:
    CommentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CommentRule>(this, &CommentRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Comment(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::PIRule : public soul::parsing::Rule
{
public:
    PIRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PIRule>(this, &PIRule::A0Action));
        soul::parsing::NonterminalParser* targetNonterminalParser = GetNonterminal(ToUtf32("target"));
        targetNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PIRule>(this, &PIRule::Posttarget));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->PI(context->fromtarget, std::u32string(matchBegin, matchEnd));
    }
    void Posttarget(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromtarget_value = std::move(stack.top());
            context->fromtarget = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromtarget_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromtarget() {}
        XmlProcessor* processor;
        std::u32string fromtarget;
    };
};

class XmlGrammar::PITargetRule : public soul::parsing::Rule
{
public:
    PITargetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PITargetRule>(this, &PITargetRule::A0Action));
        soul::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PITargetRule>(this, &PITargetRule::PostName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromName;
    }
    void PostName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromName() {}
        std::u32string value;
        std::u32string fromName;
    };
};

class XmlGrammar::CDSectRule : public soul::parsing::Rule
{
public:
    CDSectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CDSectRule>(this, &CDSectRule::A0Action));
        soul::parsing::NonterminalParser* cDataNonterminalParser = GetNonterminal(ToUtf32("CData"));
        cDataNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CDSectRule>(this, &CDSectRule::PostCData));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Text(context->fromCData);
    }
    void PostCData(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromCData_value = std::move(stack.top());
            context->fromCData = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromCData_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromCData() {}
        XmlProcessor* processor;
        std::u32string fromCData;
    };
};

class XmlGrammar::CDataRule : public soul::parsing::Rule
{
public:
    CDataRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CDataRule>(this, &CDataRule::A0Action));
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

class XmlGrammar::PrologRule : public soul::parsing::Rule
{
public:
    PrologRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* xMLDeclNonterminalParser = GetNonterminal(ToUtf32("XMLDecl"));
        xMLDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<PrologRule>(this, &PrologRule::PreXMLDecl));
        soul::parsing::NonterminalParser* m1NonterminalParser = GetNonterminal(ToUtf32("m1"));
        m1NonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<PrologRule>(this, &PrologRule::Prem1));
        soul::parsing::NonterminalParser* docTypeDeclNonterminalParser = GetNonterminal(ToUtf32("DocTypeDecl"));
        docTypeDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<PrologRule>(this, &PrologRule::PreDocTypeDecl));
        soul::parsing::NonterminalParser* m2NonterminalParser = GetNonterminal(ToUtf32("m2"));
        m2NonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<PrologRule>(this, &PrologRule::Prem2));
    }
    void PreXMLDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Prem1(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreDocTypeDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Prem2(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::XMLDeclRule : public soul::parsing::Rule
{
public:
    XMLDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* versionInfoNonterminalParser = GetNonterminal(ToUtf32("VersionInfo"));
        versionInfoNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<XMLDeclRule>(this, &XMLDeclRule::PreVersionInfo));
        soul::parsing::NonterminalParser* encodingDeclNonterminalParser = GetNonterminal(ToUtf32("EncodingDecl"));
        encodingDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<XMLDeclRule>(this, &XMLDeclRule::PreEncodingDecl));
        soul::parsing::NonterminalParser* sDDeclNonterminalParser = GetNonterminal(ToUtf32("SDDecl"));
        sDDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<XMLDeclRule>(this, &XMLDeclRule::PreSDDecl));
    }
    void PreVersionInfo(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreEncodingDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreSDDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::VersionInfoRule : public soul::parsing::Rule
{
public:
    VersionInfoRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<VersionInfoRule>(this, &VersionInfoRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<VersionInfoRule>(this, &VersionInfoRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Version(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Version(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::MiscRule : public soul::parsing::Rule
{
public:
    MiscRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* commentNonterminalParser = GetNonterminal(ToUtf32("Comment"));
        commentNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<MiscRule>(this, &MiscRule::PreComment));
        soul::parsing::NonterminalParser* pINonterminalParser = GetNonterminal(ToUtf32("PI"));
        pINonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<MiscRule>(this, &MiscRule::PrePI));
    }
    void PreComment(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePI(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::DocTypeDeclRule : public soul::parsing::Rule
{
public:
    DocTypeDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* rootElementNameNonterminalParser = GetNonterminal(ToUtf32("rootElementName"));
        rootElementNameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<DocTypeDeclRule>(this, &DocTypeDeclRule::PostrootElementName));
        soul::parsing::NonterminalParser* intSubsetNonterminalParser = GetNonterminal(ToUtf32("IntSubset"));
        intSubsetNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<DocTypeDeclRule>(this, &DocTypeDeclRule::PreIntSubset));
    }
    void PostrootElementName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromrootElementName_value = std::move(stack.top());
            context->fromrootElementName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromrootElementName_value.get());
            stack.pop();
        }
    }
    void PreIntSubset(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromrootElementName() {}
        XmlProcessor* processor;
        std::u32string fromrootElementName;
    };
};

class XmlGrammar::DeclSepRule : public soul::parsing::Rule
{
public:
    DeclSepRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* pEReferenceNonterminalParser = GetNonterminal(ToUtf32("PEReference"));
        pEReferenceNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<DeclSepRule>(this, &DeclSepRule::PrePEReference));
    }
    void PrePEReference(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::IntSubsetRule : public soul::parsing::Rule
{
public:
    IntSubsetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* markupDeclNonterminalParser = GetNonterminal(ToUtf32("MarkupDecl"));
        markupDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<IntSubsetRule>(this, &IntSubsetRule::PreMarkupDecl));
        soul::parsing::NonterminalParser* declSepNonterminalParser = GetNonterminal(ToUtf32("DeclSep"));
        declSepNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<IntSubsetRule>(this, &IntSubsetRule::PreDeclSep));
    }
    void PreMarkupDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreDeclSep(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::MarkupDeclRule : public soul::parsing::Rule
{
public:
    MarkupDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* elementDeclNonterminalParser = GetNonterminal(ToUtf32("ElementDecl"));
        elementDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreElementDecl));
        soul::parsing::NonterminalParser* attlistDeclNonterminalParser = GetNonterminal(ToUtf32("AttlistDecl"));
        attlistDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreAttlistDecl));
        soul::parsing::NonterminalParser* entityDeclNonterminalParser = GetNonterminal(ToUtf32("EntityDecl"));
        entityDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreEntityDecl));
        soul::parsing::NonterminalParser* notationDeclNonterminalParser = GetNonterminal(ToUtf32("NotationDecl"));
        notationDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreNotationDecl));
        soul::parsing::NonterminalParser* pINonterminalParser = GetNonterminal(ToUtf32("PI"));
        pINonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PrePI));
        soul::parsing::NonterminalParser* commentNonterminalParser = GetNonterminal(ToUtf32("Comment"));
        commentNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreComment));
    }
    void PreElementDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreAttlistDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreEntityDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreNotationDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePI(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreComment(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::ExtSubsetRule : public soul::parsing::Rule
{
public:
    ExtSubsetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* textDeclNonterminalParser = GetNonterminal(ToUtf32("TextDecl"));
        textDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ExtSubsetRule>(this, &ExtSubsetRule::PreTextDecl));
        soul::parsing::NonterminalParser* extSubsetDeclNonterminalParser = GetNonterminal(ToUtf32("ExtSubsetDecl"));
        extSubsetDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ExtSubsetRule>(this, &ExtSubsetRule::PreExtSubsetDecl));
    }
    void PreTextDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreExtSubsetDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::ExtSubsetDeclRule : public soul::parsing::Rule
{
public:
    ExtSubsetDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* markupDeclNonterminalParser = GetNonterminal(ToUtf32("MarkupDecl"));
        markupDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ExtSubsetDeclRule>(this, &ExtSubsetDeclRule::PreMarkupDecl));
        soul::parsing::NonterminalParser* conditionalSectNonterminalParser = GetNonterminal(ToUtf32("ConditionalSect"));
        conditionalSectNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ExtSubsetDeclRule>(this, &ExtSubsetDeclRule::PreConditionalSect));
        soul::parsing::NonterminalParser* declSepNonterminalParser = GetNonterminal(ToUtf32("DeclSep"));
        declSepNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ExtSubsetDeclRule>(this, &ExtSubsetDeclRule::PreDeclSep));
    }
    void PreMarkupDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreConditionalSect(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreDeclSep(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::SDDeclRule : public soul::parsing::Rule
{
public:
    SDDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("standalone")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A3Action));
        soul::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new soul::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A4Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Standalone(context->standalone);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = true;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = false;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), standalone() {}
        XmlProcessor* processor;
        bool standalone;
    };
};

class XmlGrammar::ElementRule : public soul::parsing::Rule
{
public:
    ElementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("tagName")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ElementRule>(this, &ElementRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<ElementRule>(this, &ElementRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<ElementRule>(this, &ElementRule::A2Action));
        soul::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ElementRule>(this, &ElementRule::PostName));
        soul::parsing::NonterminalParser* attributeNonterminalParser = GetNonterminal(ToUtf32("Attribute"));
        attributeNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ElementRule>(this, &ElementRule::PreAttribute));
        soul::parsing::NonterminalParser* contentNonterminalParser = GetNonterminal(ToUtf32("Content"));
        contentNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ElementRule>(this, &ElementRule::PreContent));
        soul::parsing::NonterminalParser* eTagNonterminalParser = GetNonterminal(ToUtf32("ETag"));
        eTagNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ElementRule>(this, &ElementRule::PreETag));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->tagName = context->fromName;
        context->processor->BeginStartTag(context->tagName);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndStartTag(span, fileName);
        context->processor->EndTag(context->tagName, span, fileName);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndStartTag(span, fileName);
    }
    void PostName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
    void PreAttribute(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreContent(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreETag(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), tagName(), fromName() {}
        XmlProcessor* processor;
        std::u32string tagName;
        std::u32string fromName;
    };
};

class XmlGrammar::AttributeRule : public soul::parsing::Rule
{
public:
    AttributeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<AttributeRule>(this, &AttributeRule::A0Action));
        soul::parsing::NonterminalParser* attNameNonterminalParser = GetNonterminal(ToUtf32("attName"));
        attNameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AttributeRule>(this, &AttributeRule::PostattName));
        soul::parsing::NonterminalParser* attValueNonterminalParser = GetNonterminal(ToUtf32("attValue"));
        attValueNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<AttributeRule>(this, &AttributeRule::PreattValue));
        attValueNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AttributeRule>(this, &AttributeRule::PostattValue));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->AddAttribute(context->fromattName, context->fromattValue, span, fileName);
    }
    void PostattName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromattName_value = std::move(stack.top());
            context->fromattName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromattName_value.get());
            stack.pop();
        }
    }
    void PreattValue(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostattValue(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromattValue_value = std::move(stack.top());
            context->fromattValue = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromattValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromattName(), fromattValue() {}
        XmlProcessor* processor;
        std::u32string fromattName;
        std::u32string fromattValue;
    };
};

class XmlGrammar::ETagRule : public soul::parsing::Rule
{
public:
    ETagRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ETagRule>(this, &ETagRule::A0Action));
        soul::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ETagRule>(this, &ETagRule::PostName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndTag(context->fromName, span, fileName);
    }
    void PostName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class XmlGrammar::ContentRule : public soul::parsing::Rule
{
public:
    ContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* cd1NonterminalParser = GetNonterminal(ToUtf32("cd1"));
        cd1NonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ContentRule>(this, &ContentRule::Precd1));
        soul::parsing::NonterminalParser* elementNonterminalParser = GetNonterminal(ToUtf32("Element"));
        elementNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreElement));
        soul::parsing::NonterminalParser* referenceNonterminalParser = GetNonterminal(ToUtf32("Reference"));
        referenceNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreReference));
        soul::parsing::NonterminalParser* cDSectNonterminalParser = GetNonterminal(ToUtf32("CDSect"));
        cDSectNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreCDSect));
        soul::parsing::NonterminalParser* pINonterminalParser = GetNonterminal(ToUtf32("PI"));
        pINonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PrePI));
        soul::parsing::NonterminalParser* commentNonterminalParser = GetNonterminal(ToUtf32("Comment"));
        commentNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreComment));
        soul::parsing::NonterminalParser* cd2NonterminalParser = GetNonterminal(ToUtf32("cd2"));
        cd2NonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ContentRule>(this, &ContentRule::Precd2));
    }
    void Precd1(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreElement(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreReference(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreCDSect(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePI(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreComment(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Precd2(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::ElementDeclRule : public soul::parsing::Rule
{
public:
    ElementDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* elementNameNonterminalParser = GetNonterminal(ToUtf32("elementName"));
        elementNameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ElementDeclRule>(this, &ElementDeclRule::PostelementName));
    }
    void PostelementName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromelementName_value = std::move(stack.top());
            context->fromelementName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromelementName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromelementName() {}
        XmlProcessor* processor;
        std::u32string fromelementName;
    };
};

class XmlGrammar::AttlistDeclRule : public soul::parsing::Rule
{
public:
    AttlistDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AttlistDeclRule>(this, &AttlistDeclRule::PostName));
        soul::parsing::NonterminalParser* attDefNonterminalParser = GetNonterminal(ToUtf32("AttDef"));
        attDefNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<AttlistDeclRule>(this, &AttlistDeclRule::PreAttDef));
    }
    void PostName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
    void PreAttDef(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class XmlGrammar::AttDefRule : public soul::parsing::Rule
{
public:
    AttDefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AttDefRule>(this, &AttDefRule::PostName));
        soul::parsing::NonterminalParser* defaultDeclNonterminalParser = GetNonterminal(ToUtf32("DefaultDecl"));
        defaultDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<AttDefRule>(this, &AttDefRule::PreDefaultDecl));
    }
    void PostName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
    void PreDefaultDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class XmlGrammar::DefaultDeclRule : public soul::parsing::Rule
{
public:
    DefaultDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* attValueNonterminalParser = GetNonterminal(ToUtf32("AttValue"));
        attValueNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<DefaultDeclRule>(this, &DefaultDeclRule::PreAttValue));
        attValueNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<DefaultDeclRule>(this, &DefaultDeclRule::PostAttValue));
    }
    void PreAttValue(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostAttValue(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromAttValue_value = std::move(stack.top());
            context->fromAttValue = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromAttValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromAttValue() {}
        XmlProcessor* processor;
        std::u32string fromAttValue;
    };
};

class XmlGrammar::ConditionalSectRule : public soul::parsing::Rule
{
public:
    ConditionalSectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* includeSectNonterminalParser = GetNonterminal(ToUtf32("IncludeSect"));
        includeSectNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ConditionalSectRule>(this, &ConditionalSectRule::PreIncludeSect));
    }
    void PreIncludeSect(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::IncludeSectRule : public soul::parsing::Rule
{
public:
    IncludeSectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* extSubsetDeclNonterminalParser = GetNonterminal(ToUtf32("ExtSubsetDecl"));
        extSubsetDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<IncludeSectRule>(this, &IncludeSectRule::PreExtSubsetDecl));
    }
    void PreExtSubsetDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::CharRefRule : public soul::parsing::Rule
{
public:
    CharRefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharRefRule>(this, &CharRefRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<CharRefRule>(this, &CharRefRule::A1Action));
        soul::parsing::NonterminalParser* dcrNonterminalParser = GetNonterminal(ToUtf32("dcr"));
        dcrNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CharRefRule>(this, &CharRefRule::Postdcr));
        soul::parsing::NonterminalParser* hcrNonterminalParser = GetNonterminal(ToUtf32("hcr"));
        hcrNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CharRefRule>(this, &CharRefRule::Posthcr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = static_cast<char32_t>(context->fromdcr);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = static_cast<char32_t>(context->fromhcr);
    }
    void Postdcr(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromdcr_value = std::move(stack.top());
            context->fromdcr = *static_cast<soul::parsing::ValueObject<uint32_t>*>(fromdcr_value.get());
            stack.pop();
        }
    }
    void Posthcr(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromhcr_value = std::move(stack.top());
            context->fromhcr = *static_cast<soul::parsing::ValueObject<uint32_t>*>(fromhcr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromdcr(), fromhcr() {}
        char32_t value;
        uint32_t fromdcr;
        uint32_t fromhcr;
    };
};

class XmlGrammar::ReferenceRule : public soul::parsing::Rule
{
public:
    ReferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ReferenceRule>(this, &ReferenceRule::A0Action));
        soul::parsing::NonterminalParser* entityRefNonterminalParser = GetNonterminal(ToUtf32("EntityRef"));
        entityRefNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ReferenceRule>(this, &ReferenceRule::PreEntityRef));
        soul::parsing::NonterminalParser* charRefNonterminalParser = GetNonterminal(ToUtf32("CharRef"));
        charRefNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ReferenceRule>(this, &ReferenceRule::PostCharRef));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Text(std::u32string(1, context->fromCharRef));
    }
    void PreEntityRef(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostCharRef(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromCharRef_value = std::move(stack.top());
            context->fromCharRef = *static_cast<soul::parsing::ValueObject<char32_t>*>(fromCharRef_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromCharRef() {}
        XmlProcessor* processor;
        char32_t fromCharRef;
    };
};

class XmlGrammar::EntityRefRule : public soul::parsing::Rule
{
public:
    EntityRefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<EntityRefRule>(this, &EntityRefRule::A0Action));
        soul::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<EntityRefRule>(this, &EntityRefRule::PostName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EntityRef(context->fromName, span, fileName);
    }
    void PostName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class XmlGrammar::PEReferenceRule : public soul::parsing::Rule
{
public:
    PEReferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PEReferenceRule>(this, &PEReferenceRule::PostName));
    }
    void PostName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class XmlGrammar::EntityDeclRule : public soul::parsing::Rule
{
public:
    EntityDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* gEDeclNonterminalParser = GetNonterminal(ToUtf32("GEDecl"));
        gEDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<EntityDeclRule>(this, &EntityDeclRule::PreGEDecl));
        soul::parsing::NonterminalParser* pEDeclNonterminalParser = GetNonterminal(ToUtf32("PEDecl"));
        pEDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<EntityDeclRule>(this, &EntityDeclRule::PrePEDecl));
    }
    void PreGEDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePEDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::GEDeclRule : public soul::parsing::Rule
{
public:
    GEDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* entityNameNonterminalParser = GetNonterminal(ToUtf32("entityName"));
        entityNameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<GEDeclRule>(this, &GEDeclRule::PostentityName));
        soul::parsing::NonterminalParser* entityValueNonterminalParser = GetNonterminal(ToUtf32("entityValue"));
        entityValueNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<GEDeclRule>(this, &GEDeclRule::PreentityValue));
    }
    void PostentityName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromentityName_value = std::move(stack.top());
            context->fromentityName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromentityName_value.get());
            stack.pop();
        }
    }
    void PreentityValue(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromentityName() {}
        XmlProcessor* processor;
        std::u32string fromentityName;
    };
};

class XmlGrammar::PEDeclRule : public soul::parsing::Rule
{
public:
    PEDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* peNameNonterminalParser = GetNonterminal(ToUtf32("peName"));
        peNameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PEDeclRule>(this, &PEDeclRule::PostpeName));
        soul::parsing::NonterminalParser* peValueNonterminalParser = GetNonterminal(ToUtf32("peValue"));
        peValueNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<PEDeclRule>(this, &PEDeclRule::PrepeValue));
    }
    void PostpeName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> frompeName_value = std::move(stack.top());
            context->frompeName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(frompeName_value.get());
            stack.pop();
        }
    }
    void PrepeValue(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), frompeName() {}
        XmlProcessor* processor;
        std::u32string frompeName;
    };
};

class XmlGrammar::EntityDefRule : public soul::parsing::Rule
{
public:
    EntityDefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* entityValueNonterminalParser = GetNonterminal(ToUtf32("EntityValue"));
        entityValueNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<EntityDefRule>(this, &EntityDefRule::PreEntityValue));
        entityValueNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<EntityDefRule>(this, &EntityDefRule::PostEntityValue));
    }
    void PreEntityValue(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostEntityValue(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromEntityValue_value = std::move(stack.top());
            context->fromEntityValue = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromEntityValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromEntityValue() {}
        XmlProcessor* processor;
        std::u32string fromEntityValue;
    };
};

class XmlGrammar::PEDefRule : public soul::parsing::Rule
{
public:
    PEDefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* entityValueNonterminalParser = GetNonterminal(ToUtf32("EntityValue"));
        entityValueNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<PEDefRule>(this, &PEDefRule::PreEntityValue));
        entityValueNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PEDefRule>(this, &PEDefRule::PostEntityValue));
    }
    void PreEntityValue(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostEntityValue(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromEntityValue_value = std::move(stack.top());
            context->fromEntityValue = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromEntityValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromEntityValue() {}
        XmlProcessor* processor;
        std::u32string fromEntityValue;
    };
};

class XmlGrammar::TextDeclRule : public soul::parsing::Rule
{
public:
    TextDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* versionInfoNonterminalParser = GetNonterminal(ToUtf32("VersionInfo"));
        versionInfoNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<TextDeclRule>(this, &TextDeclRule::PreVersionInfo));
        soul::parsing::NonterminalParser* encodingDeclNonterminalParser = GetNonterminal(ToUtf32("EncodingDecl"));
        encodingDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<TextDeclRule>(this, &TextDeclRule::PreEncodingDecl));
    }
    void PreVersionInfo(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreEncodingDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::ExtParsedEntRule : public soul::parsing::Rule
{
public:
    ExtParsedEntRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* textDeclNonterminalParser = GetNonterminal(ToUtf32("TextDecl"));
        textDeclNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ExtParsedEntRule>(this, &ExtParsedEntRule::PreTextDecl));
        soul::parsing::NonterminalParser* contentNonterminalParser = GetNonterminal(ToUtf32("Content"));
        contentNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ExtParsedEntRule>(this, &ExtParsedEntRule::PreContent));
    }
    void PreTextDecl(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreContent(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class XmlGrammar::EncodingDeclRule : public soul::parsing::Rule
{
public:
    EncodingDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<EncodingDeclRule>(this, &EncodingDeclRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<EncodingDeclRule>(this, &EncodingDeclRule::A1Action));
        soul::parsing::NonterminalParser* en1NonterminalParser = GetNonterminal(ToUtf32("en1"));
        en1NonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<EncodingDeclRule>(this, &EncodingDeclRule::Posten1));
        soul::parsing::NonterminalParser* en2NonterminalParser = GetNonterminal(ToUtf32("en2"));
        en2NonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<EncodingDeclRule>(this, &EncodingDeclRule::Posten2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Encoding(context->fromen1);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Encoding(context->fromen2);
    }
    void Posten1(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromen1_value = std::move(stack.top());
            context->fromen1 = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromen1_value.get());
            stack.pop();
        }
    }
    void Posten2(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromen2_value = std::move(stack.top());
            context->fromen2 = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromen2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromen1(), fromen2() {}
        XmlProcessor* processor;
        std::u32string fromen1;
        std::u32string fromen2;
    };
};

class XmlGrammar::EncNameRule : public soul::parsing::Rule
{
public:
    EncNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<EncNameRule>(this, &EncNameRule::A0Action));
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

class XmlGrammar::NotationDeclRule : public soul::parsing::Rule
{
public:
    NotationDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<soul::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NotationDeclRule>(this, &NotationDeclRule::PostName));
    }
    void PostName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

void XmlGrammar::GetReferencedGrammars()
{
    soul::parsing::ParsingDomain* pd = GetParsingDomain();
    soul::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("soul.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = soul::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void XmlGrammar::CreateRules()
{
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("uint"), this, ToUtf32("soul.parsing.stdlib.uint")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("hexuint"), this, ToUtf32("soul.parsing.stdlib.hexuint")));
    AddRule(new DocumentRule(ToUtf32("Document"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::EmptyParser()),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::NonterminalParser(ToUtf32("Prolog"), ToUtf32("Prolog"), 1),
                        new soul::parsing::NonterminalParser(ToUtf32("Element"), ToUtf32("Element"), 1)),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::NonterminalParser(ToUtf32("Misc"), ToUtf32("Misc"), 1)))))));
    AddRule(new soul::parsing::Rule(ToUtf32("Char"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::CharSetParser(ToUtf32("\t\n\r")),
                    new soul::parsing::RangeParser(32, 55295)),
                new soul::parsing::RangeParser(57344, 65533)),
            new soul::parsing::RangeParser(65536, 1114111))));
    AddRule(new soul::parsing::Rule(ToUtf32("S"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::PositiveParser(
            new soul::parsing::CharSetParser(ToUtf32(" \t\r\n")))));
    AddRule(new soul::parsing::Rule(ToUtf32("NameStartChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                            new soul::parsing::CharParser(':'),
                                                            new soul::parsing::CharSetParser(ToUtf32("A-Z_a-z"))),
                                                        new soul::parsing::RangeParser(192, 214)),
                                                    new soul::parsing::RangeParser(216, 246)),
                                                new soul::parsing::RangeParser(248, 767)),
                                            new soul::parsing::RangeParser(880, 893)),
                                        new soul::parsing::RangeParser(895, 8191)),
                                    new soul::parsing::RangeParser(8204, 8205)),
                                new soul::parsing::RangeParser(8304, 8591)),
                            new soul::parsing::RangeParser(11264, 12271)),
                        new soul::parsing::RangeParser(12289, 55295)),
                    new soul::parsing::RangeParser(63744, 64975)),
                new soul::parsing::RangeParser(65008, 65533)),
            new soul::parsing::RangeParser(65536, 983039))));
    AddRule(new soul::parsing::Rule(ToUtf32("NameChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::NonterminalParser(ToUtf32("NameStartChar"), ToUtf32("NameStartChar"), 0),
                        new soul::parsing::CharSetParser(ToUtf32("-.0-9"))),
                    new soul::parsing::RangeParser(183, 183)),
                new soul::parsing::RangeParser(768, 879)),
            new soul::parsing::RangeParser(8255, 8256))));
    AddRule(new NameRule(ToUtf32("Name"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::SequenceParser(
                new soul::parsing::NonterminalParser(ToUtf32("NameStartChar"), ToUtf32("NameStartChar"), 0),
                new soul::parsing::KleeneStarParser(
                    new soul::parsing::NonterminalParser(ToUtf32("NameChar"), ToUtf32("NameChar"), 0))))));
    AddRule(new soul::parsing::Rule(ToUtf32("Names"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::NonterminalParser(ToUtf32("f"), ToUtf32("Name"), 0),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser(' '),
                    new soul::parsing::NonterminalParser(ToUtf32("n"), ToUtf32("Name"), 0))))));
    AddRule(new soul::parsing::Rule(ToUtf32("Nmtoken"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::PositiveParser(
            new soul::parsing::NonterminalParser(ToUtf32("NameChar"), ToUtf32("NameChar"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("Nmtokens"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser(' '),
                    new soul::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0))))));
    AddRule(new EntityValueRule(ToUtf32("EntityValue"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('\"'),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::AlternativeParser(
                                new soul::parsing::ActionParser(ToUtf32("A0"),
                                    new soul::parsing::CharSetParser(ToUtf32("%&\""), true)),
                                new soul::parsing::ActionParser(ToUtf32("A1"),
                                    new soul::parsing::NonterminalParser(ToUtf32("pr1"), ToUtf32("PEReference"), 1))),
                            new soul::parsing::ActionParser(ToUtf32("A2"),
                                new soul::parsing::NonterminalParser(ToUtf32("ref1"), ToUtf32("Reference"), 1))))),
                new soul::parsing::CharParser('\"')),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('\''),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::AlternativeParser(
                                new soul::parsing::ActionParser(ToUtf32("A3"),
                                    new soul::parsing::CharSetParser(ToUtf32("%&\'"), true)),
                                new soul::parsing::ActionParser(ToUtf32("A4"),
                                    new soul::parsing::NonterminalParser(ToUtf32("pr2"), ToUtf32("PEReference"), 1))),
                            new soul::parsing::ActionParser(ToUtf32("A5"),
                                new soul::parsing::NonterminalParser(ToUtf32("ref2"), ToUtf32("Reference"), 1))))),
                new soul::parsing::CharParser('\'')))));
    AddRule(new AttValueRule(ToUtf32("AttValue"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::ActionParser(ToUtf32("A0"),
                        new soul::parsing::CharParser('\"')),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::ActionParser(ToUtf32("A1"),
                                new soul::parsing::CharSetParser(ToUtf32("<&\""), true)),
                            new soul::parsing::NonterminalParser(ToUtf32("ref1"), ToUtf32("Reference"), 1)))),
                new soul::parsing::ActionParser(ToUtf32("A2"),
                    new soul::parsing::CharParser('\"'))),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::ActionParser(ToUtf32("A3"),
                        new soul::parsing::CharParser('\'')),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::ActionParser(ToUtf32("A4"),
                                new soul::parsing::CharSetParser(ToUtf32("<&\'"), true)),
                            new soul::parsing::NonterminalParser(ToUtf32("ref2"), ToUtf32("Reference"), 1)))),
                new soul::parsing::ActionParser(ToUtf32("A5"),
                    new soul::parsing::CharParser('\''))))));
    AddRule(new SystemLiteralRule(ToUtf32("SystemLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('\"'),
                    new soul::parsing::ActionParser(ToUtf32("A0"),
                        new soul::parsing::KleeneStarParser(
                            new soul::parsing::CharSetParser(ToUtf32("\""), true)))),
                new soul::parsing::CharParser('\"')),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('\''),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::KleeneStarParser(
                            new soul::parsing::CharSetParser(ToUtf32("\'"), true)))),
                new soul::parsing::CharParser('\'')))));
    AddRule(new PubidLiteralRule(ToUtf32("PubidLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('\"'),
                    new soul::parsing::ActionParser(ToUtf32("A0"),
                        new soul::parsing::KleeneStarParser(
                            new soul::parsing::NonterminalParser(ToUtf32("PubidChar"), ToUtf32("PubidChar"), 0)))),
                new soul::parsing::CharParser('\"')),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('\''),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::KleeneStarParser(
                            new soul::parsing::DifferenceParser(
                                new soul::parsing::NonterminalParser(ToUtf32("PubidChar"), ToUtf32("PubidChar"), 0),
                                new soul::parsing::CharParser('\''))))),
                new soul::parsing::CharParser('\'')))));
    AddRule(new soul::parsing::Rule(ToUtf32("PubidChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::CharSetParser(ToUtf32(" \r\n")),
                new soul::parsing::CharSetParser(ToUtf32("a-zA-Z0-9"))),
            new soul::parsing::CharSetParser(ToUtf32("-\'()+,./:=?;!*#@$_%")))));
    AddRule(new CharDataRule(ToUtf32("CharData"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::DifferenceParser(
                new soul::parsing::KleeneStarParser(
                    new soul::parsing::CharSetParser(ToUtf32("<&"), true)),
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::KleeneStarParser(
                            new soul::parsing::CharSetParser(ToUtf32("<&"), true)),
                        new soul::parsing::StringParser(ToUtf32("]]>"))),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::CharSetParser(ToUtf32("<&"), true)))))));
    AddRule(new CommentRule(ToUtf32("Comment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::StringParser(ToUtf32("<!--")),
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::DifferenceParser(
                                new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0),
                                new soul::parsing::CharParser('-')),
                            new soul::parsing::SequenceParser(
                                new soul::parsing::CharParser('-'),
                                new soul::parsing::DifferenceParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0),
                                    new soul::parsing::CharParser('-'))))))),
            new soul::parsing::StringParser(ToUtf32("-->")))));
    AddRule(new PIRule(ToUtf32("PI"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::StringParser(ToUtf32("<?")),
                    new soul::parsing::NonterminalParser(ToUtf32("target"), ToUtf32("PITarget"), 0)),
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                            new soul::parsing::DifferenceParser(
                                new soul::parsing::KleeneStarParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::SequenceParser(
                                        new soul::parsing::KleeneStarParser(
                                            new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
                                        new soul::parsing::StringParser(ToUtf32("?>"))),
                                    new soul::parsing::KleeneStarParser(
                                        new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)))))))),
            new soul::parsing::StringParser(ToUtf32("?>")))));
    AddRule(new PITargetRule(ToUtf32("PITarget"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::DifferenceParser(
                new soul::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0),
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::CharParser('X'),
                            new soul::parsing::CharParser('x')),
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::CharParser('M'),
                            new soul::parsing::CharParser('m'))),
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::CharParser('L'),
                        new soul::parsing::CharParser('l')))))));
    AddRule(new CDSectRule(ToUtf32("CDSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::NonterminalParser(ToUtf32("CDStart"), ToUtf32("CDStart"), 0),
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::NonterminalParser(ToUtf32("CData"), ToUtf32("CData"), 0))),
            new soul::parsing::NonterminalParser(ToUtf32("CDEnd"), ToUtf32("CDEnd"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("CDStart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::StringParser(ToUtf32("<![CDATA["))));
    AddRule(new CDataRule(ToUtf32("CData"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::DifferenceParser(
                new soul::parsing::KleeneStarParser(
                    new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::KleeneStarParser(
                            new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
                        new soul::parsing::StringParser(ToUtf32("]]>"))),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)))))));
    AddRule(new soul::parsing::Rule(ToUtf32("CDEnd"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::StringParser(ToUtf32("]]>"))));
    AddRule(new PrologRule(ToUtf32("Prolog"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("XMLDecl"), ToUtf32("XMLDecl"), 1)),
                new soul::parsing::KleeneStarParser(
                    new soul::parsing::NonterminalParser(ToUtf32("m1"), ToUtf32("Misc"), 1))),
            new soul::parsing::OptionalParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("DocTypeDecl"), ToUtf32("DocTypeDecl"), 1),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::NonterminalParser(ToUtf32("m2"), ToUtf32("Misc"), 1)))))));
    AddRule(new XMLDeclRule(ToUtf32("XMLDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::StringParser(ToUtf32("<?xml")),
                            new soul::parsing::NonterminalParser(ToUtf32("VersionInfo"), ToUtf32("VersionInfo"), 1)),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("EncodingDecl"), ToUtf32("EncodingDecl"), 1))),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("SDDecl"), ToUtf32("SDDecl"), 1))),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::StringParser(ToUtf32("?>")))));
    AddRule(new VersionInfoRule(ToUtf32("VersionInfo"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                    new soul::parsing::StringParser(ToUtf32("version"))),
                new soul::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
            new soul::parsing::AlternativeParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::CharParser('\''),
                        new soul::parsing::ActionParser(ToUtf32("A0"),
                            new soul::parsing::NonterminalParser(ToUtf32("VersionNum"), ToUtf32("VersionNum"), 0))),
                    new soul::parsing::CharParser('\'')),
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::CharParser('\"'),
                        new soul::parsing::ActionParser(ToUtf32("A1"),
                            new soul::parsing::NonterminalParser(ToUtf32("VersionNum"), ToUtf32("VersionNum"), 0))),
                    new soul::parsing::CharParser('\"'))))));
    AddRule(new soul::parsing::Rule(ToUtf32("Eq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                new soul::parsing::CharParser('=')),
            new soul::parsing::OptionalParser(
                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)))));
    AddRule(new soul::parsing::Rule(ToUtf32("VersionNum"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::StringParser(ToUtf32("1.")),
            new soul::parsing::PositiveParser(
                new soul::parsing::CharSetParser(ToUtf32("0-9"))))));
    AddRule(new MiscRule(ToUtf32("Misc"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::NonterminalParser(ToUtf32("Comment"), ToUtf32("Comment"), 1),
                new soul::parsing::NonterminalParser(ToUtf32("PI"), ToUtf32("PI"), 1)),
            new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))));
    AddRule(new DocTypeDeclRule(ToUtf32("DocTypeDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::StringParser(ToUtf32("<!DOCTYPE")),
                                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new soul::parsing::NonterminalParser(ToUtf32("rootElementName"), ToUtf32("Name"), 0)),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                                new soul::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0)))),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new soul::parsing::OptionalParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::CharParser('['),
                                new soul::parsing::NonterminalParser(ToUtf32("IntSubset"), ToUtf32("IntSubset"), 1)),
                            new soul::parsing::CharParser(']')),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))))),
            new soul::parsing::CharParser('>'))));
    AddRule(new DeclSepRule(ToUtf32("DeclSep"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::NonterminalParser(ToUtf32("PEReference"), ToUtf32("PEReference"), 1),
            new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))));
    AddRule(new IntSubsetRule(ToUtf32("IntSubset"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::KleeneStarParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::NonterminalParser(ToUtf32("MarkupDecl"), ToUtf32("MarkupDecl"), 1),
                new soul::parsing::NonterminalParser(ToUtf32("DeclSep"), ToUtf32("DeclSep"), 1)))));
    AddRule(new MarkupDeclRule(ToUtf32("MarkupDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::NonterminalParser(ToUtf32("ElementDecl"), ToUtf32("ElementDecl"), 1),
                            new soul::parsing::NonterminalParser(ToUtf32("AttlistDecl"), ToUtf32("AttlistDecl"), 1)),
                        new soul::parsing::NonterminalParser(ToUtf32("EntityDecl"), ToUtf32("EntityDecl"), 1)),
                    new soul::parsing::NonterminalParser(ToUtf32("NotationDecl"), ToUtf32("NotationDecl"), 1)),
                new soul::parsing::NonterminalParser(ToUtf32("PI"), ToUtf32("PI"), 1)),
            new soul::parsing::NonterminalParser(ToUtf32("Comment"), ToUtf32("Comment"), 1))));
    AddRule(new ExtSubsetRule(ToUtf32("ExtSubset"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::OptionalParser(
                new soul::parsing::NonterminalParser(ToUtf32("TextDecl"), ToUtf32("TextDecl"), 1)),
            new soul::parsing::NonterminalParser(ToUtf32("ExtSubsetDecl"), ToUtf32("ExtSubsetDecl"), 1))));
    AddRule(new ExtSubsetDeclRule(ToUtf32("ExtSubsetDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::KleeneStarParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::NonterminalParser(ToUtf32("MarkupDecl"), ToUtf32("MarkupDecl"), 1),
                    new soul::parsing::NonterminalParser(ToUtf32("ConditionalSect"), ToUtf32("ConditionalSect"), 1)),
                new soul::parsing::NonterminalParser(ToUtf32("DeclSep"), ToUtf32("DeclSep"), 1)))));
    AddRule(new SDDeclRule(ToUtf32("SDDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                        new soul::parsing::StringParser(ToUtf32("standalone"))),
                    new soul::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
                new soul::parsing::AlternativeParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('\''),
                            new soul::parsing::AlternativeParser(
                                new soul::parsing::ActionParser(ToUtf32("A1"),
                                    new soul::parsing::StringParser(ToUtf32("yes"))),
                                new soul::parsing::ActionParser(ToUtf32("A2"),
                                    new soul::parsing::StringParser(ToUtf32("no"))))),
                        new soul::parsing::CharParser('\'')),
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('\"'),
                            new soul::parsing::AlternativeParser(
                                new soul::parsing::ActionParser(ToUtf32("A3"),
                                    new soul::parsing::StringParser(ToUtf32("yes"))),
                                new soul::parsing::ActionParser(ToUtf32("A4"),
                                    new soul::parsing::StringParser(ToUtf32("no"))))),
                        new soul::parsing::CharParser('\"')))))));
    AddRule(new ElementRule(ToUtf32("Element"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::CharParser('<'),
                        new soul::parsing::ActionParser(ToUtf32("A0"),
                            new soul::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                            new soul::parsing::NonterminalParser(ToUtf32("Attribute"), ToUtf32("Attribute"), 1)))),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::AlternativeParser(
                new soul::parsing::ActionParser(ToUtf32("A1"),
                    new soul::parsing::StringParser(ToUtf32("/>"))),
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::ActionParser(ToUtf32("A2"),
                            new soul::parsing::CharParser('>')),
                        new soul::parsing::NonterminalParser(ToUtf32("Content"), ToUtf32("Content"), 1)),
                    new soul::parsing::NonterminalParser(ToUtf32("ETag"), ToUtf32("ETag"), 1))))));
    AddRule(new AttributeRule(ToUtf32("Attribute"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::NonterminalParser(ToUtf32("attName"), ToUtf32("Name"), 0),
                new soul::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("attValue"), ToUtf32("AttValue"), 1)))));
    AddRule(new ETagRule(ToUtf32("ETag"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::StringParser(ToUtf32("</")),
                    new soul::parsing::ActionParser(ToUtf32("A0"),
                        new soul::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::CharParser('>'))));
    AddRule(new ContentRule(ToUtf32("Content"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::OptionalParser(
                new soul::parsing::NonterminalParser(ToUtf32("cd1"), ToUtf32("CharData"), 1)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::AlternativeParser(
                                new soul::parsing::AlternativeParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("Element"), ToUtf32("Element"), 1),
                                    new soul::parsing::NonterminalParser(ToUtf32("Reference"), ToUtf32("Reference"), 1)),
                                new soul::parsing::NonterminalParser(ToUtf32("CDSect"), ToUtf32("CDSect"), 1)),
                            new soul::parsing::NonterminalParser(ToUtf32("PI"), ToUtf32("PI"), 1)),
                        new soul::parsing::NonterminalParser(ToUtf32("Comment"), ToUtf32("Comment"), 1)),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("cd2"), ToUtf32("CharData"), 1)))))));
    AddRule(new ElementDeclRule(ToUtf32("ElementDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::StringParser(ToUtf32("<!ELEMENT")),
                                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new soul::parsing::NonterminalParser(ToUtf32("elementName"), ToUtf32("Name"), 0)),
                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new soul::parsing::NonterminalParser(ToUtf32("ContentSpec"), ToUtf32("ContentSpec"), 0)),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::CharParser('>'))));
    AddRule(new soul::parsing::Rule(ToUtf32("ContentSpec"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::StringParser(ToUtf32("EMPTY")),
                    new soul::parsing::StringParser(ToUtf32("ANY"))),
                new soul::parsing::NonterminalParser(ToUtf32("Mixed"), ToUtf32("Mixed"), 0)),
            new soul::parsing::NonterminalParser(ToUtf32("Children"), ToUtf32("Children"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("Children"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::NonterminalParser(ToUtf32("Choice"), ToUtf32("Choice"), 0),
                new soul::parsing::NonterminalParser(ToUtf32("Seq"), ToUtf32("Seq"), 0)),
            new soul::parsing::OptionalParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::CharParser('?'),
                        new soul::parsing::CharParser('*')),
                    new soul::parsing::CharParser('+'))))));
    AddRule(new soul::parsing::Rule(ToUtf32("CP"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0),
                    new soul::parsing::NonterminalParser(ToUtf32("Choice"), ToUtf32("Choice"), 0)),
                new soul::parsing::NonterminalParser(ToUtf32("Seq"), ToUtf32("Seq"), 0)),
            new soul::parsing::OptionalParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::CharParser('?'),
                        new soul::parsing::CharParser('*')),
                    new soul::parsing::CharParser('+'))))));
    AddRule(new soul::parsing::Rule(ToUtf32("Choice"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('('),
                            new soul::parsing::OptionalParser(
                                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new soul::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0)),
                    new soul::parsing::PositiveParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::OptionalParser(
                                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                    new soul::parsing::CharParser('|')),
                                new soul::parsing::OptionalParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new soul::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0)))),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::CharParser(')'))));
    AddRule(new soul::parsing::Rule(ToUtf32("Seq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('('),
                            new soul::parsing::OptionalParser(
                                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new soul::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0)),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::OptionalParser(
                                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                    new soul::parsing::CharParser(',')),
                                new soul::parsing::OptionalParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new soul::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0)))),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::CharParser(')'))));
    AddRule(new soul::parsing::Rule(ToUtf32("Mixed"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::CharParser('('),
                                new soul::parsing::OptionalParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new soul::parsing::StringParser(ToUtf32("#PCDATA"))),
                        new soul::parsing::KleeneStarParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::SequenceParser(
                                        new soul::parsing::OptionalParser(
                                            new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                        new soul::parsing::CharParser('|')),
                                    new soul::parsing::OptionalParser(
                                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                new soul::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)))),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new soul::parsing::StringParser(ToUtf32(")*"))),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('('),
                            new soul::parsing::OptionalParser(
                                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new soul::parsing::StringParser(ToUtf32("#PCDATA"))),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new soul::parsing::CharParser(')')))));
    AddRule(new AttlistDeclRule(ToUtf32("AttlistDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::StringParser(ToUtf32("<!ATTLIST")),
                            new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                        new soul::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::NonterminalParser(ToUtf32("AttDef"), ToUtf32("AttDef"), 1))),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::CharParser('>'))));
    AddRule(new AttDefRule(ToUtf32("AttDef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                            new soul::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new soul::parsing::NonterminalParser(ToUtf32("AttType"), ToUtf32("AttType"), 0)),
                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
            new soul::parsing::NonterminalParser(ToUtf32("DefaultDecl"), ToUtf32("DefaultDecl"), 1))));
    AddRule(new soul::parsing::Rule(ToUtf32("AttType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::NonterminalParser(ToUtf32("StringType"), ToUtf32("StringType"), 0),
                new soul::parsing::NonterminalParser(ToUtf32("TokenizedType"), ToUtf32("TokenizedType"), 0)),
            new soul::parsing::NonterminalParser(ToUtf32("EnumeratedType"), ToUtf32("EnumeratedType"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("StringType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::StringParser(ToUtf32("CDATA"))));
    AddRule(new soul::parsing::Rule(ToUtf32("TokenizedType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::AlternativeParser(
                                new soul::parsing::StringParser(ToUtf32("ID")),
                                new soul::parsing::StringParser(ToUtf32("IDREF"))),
                            new soul::parsing::StringParser(ToUtf32("IDREFS"))),
                        new soul::parsing::StringParser(ToUtf32("ENTITY"))),
                    new soul::parsing::StringParser(ToUtf32("ENTITIES"))),
                new soul::parsing::StringParser(ToUtf32("NMTOKEN"))),
            new soul::parsing::StringParser(ToUtf32("NMTOKENS")))));
    AddRule(new soul::parsing::Rule(ToUtf32("EnumeratedType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::NonterminalParser(ToUtf32("NotationType"), ToUtf32("NotationType"), 0),
            new soul::parsing::NonterminalParser(ToUtf32("Enumeration"), ToUtf32("Enumeration"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("NotationType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::StringParser(ToUtf32("NOTATION")),
                                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                new soul::parsing::CharParser('(')),
                            new soul::parsing::OptionalParser(
                                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new soul::parsing::NonterminalParser(ToUtf32("f"), ToUtf32("Name"), 0)),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::OptionalParser(
                                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                    new soul::parsing::CharParser('|')),
                                new soul::parsing::OptionalParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new soul::parsing::NonterminalParser(ToUtf32("n"), ToUtf32("Name"), 0)))),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::CharParser(')'))));
    AddRule(new soul::parsing::Rule(ToUtf32("Enumeration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('('),
                            new soul::parsing::OptionalParser(
                                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new soul::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0)),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::OptionalParser(
                                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                    new soul::parsing::CharParser('|')),
                                new soul::parsing::OptionalParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new soul::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0)))),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::CharParser(')'))));
    AddRule(new DefaultDeclRule(ToUtf32("DefaultDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::StringParser(ToUtf32("#REQUIRED")),
                new soul::parsing::StringParser(ToUtf32("#IMPLIED"))),
            new soul::parsing::SequenceParser(
                new soul::parsing::OptionalParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::StringParser(ToUtf32("#FIXED")),
                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new soul::parsing::NonterminalParser(ToUtf32("AttValue"), ToUtf32("AttValue"), 1)))));
    AddRule(new ConditionalSectRule(ToUtf32("ConditionalSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::NonterminalParser(ToUtf32("IncludeSect"), ToUtf32("IncludeSect"), 1),
            new soul::parsing::NonterminalParser(ToUtf32("IgnoreSect"), ToUtf32("IgnoreSect"), 0))));
    AddRule(new IncludeSectRule(ToUtf32("IncludeSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::StringParser(ToUtf32("<!")),
                                new soul::parsing::OptionalParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new soul::parsing::StringParser(ToUtf32("INCLUDE"))),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                    new soul::parsing::CharParser('[')),
                new soul::parsing::NonterminalParser(ToUtf32("ExtSubsetDecl"), ToUtf32("ExtSubsetDecl"), 1)),
            new soul::parsing::StringParser(ToUtf32("]]>")))));
    AddRule(new soul::parsing::Rule(ToUtf32("IgnoreSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::StringParser(ToUtf32("<!")),
                                new soul::parsing::OptionalParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new soul::parsing::StringParser(ToUtf32("IGNORE"))),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                    new soul::parsing::CharParser('[')),
                new soul::parsing::KleeneStarParser(
                    new soul::parsing::NonterminalParser(ToUtf32("IgnoreSectContents"), ToUtf32("IgnoreSectContents"), 0))),
            new soul::parsing::StringParser(ToUtf32("]]>")))));
    AddRule(new soul::parsing::Rule(ToUtf32("IgnoreSectContents"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::NonterminalParser(ToUtf32("Ignore"), ToUtf32("Ignore"), 0),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::StringParser(ToUtf32("<![")),
                            new soul::parsing::NonterminalParser(ToUtf32("IgnoreSectContents"), ToUtf32("IgnoreSectContents"), 0)),
                        new soul::parsing::StringParser(ToUtf32("]]>"))),
                    new soul::parsing::NonterminalParser(ToUtf32("Ignore"), ToUtf32("Ignore"), 0))))));
    AddRule(new soul::parsing::Rule(ToUtf32("Ignore"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::DifferenceParser(
            new soul::parsing::KleeneStarParser(
                new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::StringParser(ToUtf32("<![")),
                        new soul::parsing::StringParser(ToUtf32("]]>")))),
                new soul::parsing::KleeneStarParser(
                    new soul::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0))))));
    AddRule(new CharRefRule(ToUtf32("CharRef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::StringParser(ToUtf32("&#")),
                    new soul::parsing::ActionParser(ToUtf32("A0"),
                        new soul::parsing::NonterminalParser(ToUtf32("dcr"), ToUtf32("uint"), 0))),
                new soul::parsing::CharParser(';')),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::StringParser(ToUtf32("&#x")),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::NonterminalParser(ToUtf32("hcr"), ToUtf32("hexuint"), 0))),
                new soul::parsing::CharParser(';')))));
    AddRule(new ReferenceRule(ToUtf32("Reference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::NonterminalParser(ToUtf32("EntityRef"), ToUtf32("EntityRef"), 1),
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("CharRef"), ToUtf32("CharRef"), 0)))));
    AddRule(new EntityRefRule(ToUtf32("EntityRef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::CharParser('&'),
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))),
            new soul::parsing::CharParser(';'))));
    AddRule(new PEReferenceRule(ToUtf32("PEReference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::CharParser('%'),
                new soul::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
            new soul::parsing::CharParser(';'))));
    AddRule(new EntityDeclRule(ToUtf32("EntityDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::NonterminalParser(ToUtf32("GEDecl"), ToUtf32("GEDecl"), 1),
            new soul::parsing::NonterminalParser(ToUtf32("PEDecl"), ToUtf32("PEDecl"), 1))));
    AddRule(new GEDeclRule(ToUtf32("GEDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::StringParser(ToUtf32("<!ENTITY")),
                                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new soul::parsing::NonterminalParser(ToUtf32("entityName"), ToUtf32("Name"), 0)),
                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new soul::parsing::NonterminalParser(ToUtf32("entityValue"), ToUtf32("EntityDef"), 1)),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::CharParser('>'))));
    AddRule(new PEDeclRule(ToUtf32("PEDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::SequenceParser(
                                        new soul::parsing::StringParser(ToUtf32("<!ENTITY")),
                                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                    new soul::parsing::CharParser('%')),
                                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new soul::parsing::NonterminalParser(ToUtf32("peName"), ToUtf32("Name"), 0)),
                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new soul::parsing::NonterminalParser(ToUtf32("peValue"), ToUtf32("PEDef"), 1)),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::CharParser('>'))));
    AddRule(new EntityDefRule(ToUtf32("EntityDef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::NonterminalParser(ToUtf32("EntityValue"), ToUtf32("EntityValue"), 1),
            new soul::parsing::SequenceParser(
                new soul::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("notation"), ToUtf32("NDataDecl"), 0))))));
    AddRule(new PEDefRule(ToUtf32("PEDef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::NonterminalParser(ToUtf32("EntityValue"), ToUtf32("EntityValue"), 1),
            new soul::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("ExternalID"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::StringParser(ToUtf32("SYSTEM")),
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                new soul::parsing::NonterminalParser(ToUtf32("s1"), ToUtf32("SystemLiteral"), 0)),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::StringParser(ToUtf32("PUBLIC")),
                            new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                        new soul::parsing::NonterminalParser(ToUtf32("p2"), ToUtf32("PubidLiteral"), 0)),
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                new soul::parsing::NonterminalParser(ToUtf32("s2"), ToUtf32("SystemLiteral"), 0)))));
    AddRule(new soul::parsing::Rule(ToUtf32("NDataDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                    new soul::parsing::StringParser(ToUtf32("NDATA"))),
                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
            new soul::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))));
    AddRule(new TextDeclRule(ToUtf32("TextDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::StringParser(ToUtf32("<?xml")),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("VersionInfo"), ToUtf32("VersionInfo"), 1))),
                    new soul::parsing::NonterminalParser(ToUtf32("EncodingDecl"), ToUtf32("EncodingDecl"), 1)),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::StringParser(ToUtf32("?>")))));
    AddRule(new ExtParsedEntRule(ToUtf32("ExtParsedEnt"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::OptionalParser(
                new soul::parsing::NonterminalParser(ToUtf32("TextDecl"), ToUtf32("TextDecl"), 1)),
            new soul::parsing::NonterminalParser(ToUtf32("Content"), ToUtf32("Content"), 1))));
    AddRule(new EncodingDeclRule(ToUtf32("EncodingDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                    new soul::parsing::StringParser(ToUtf32("encoding"))),
                new soul::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
            new soul::parsing::AlternativeParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::CharParser('\"'),
                        new soul::parsing::ActionParser(ToUtf32("A0"),
                            new soul::parsing::NonterminalParser(ToUtf32("en1"), ToUtf32("EncName"), 0))),
                    new soul::parsing::CharParser('\"')),
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::CharParser('\''),
                        new soul::parsing::ActionParser(ToUtf32("A1"),
                            new soul::parsing::NonterminalParser(ToUtf32("en2"), ToUtf32("EncName"), 0))),
                    new soul::parsing::CharParser('\''))))));
    AddRule(new EncNameRule(ToUtf32("EncName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::SequenceParser(
                new soul::parsing::CharSetParser(ToUtf32("A-Za-z")),
                new soul::parsing::KleeneStarParser(
                    new soul::parsing::CharSetParser(ToUtf32("A-Za-z0-9._-")))))));
    AddRule(new NotationDeclRule(ToUtf32("NotationDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::StringParser(ToUtf32("<!NOTATION")),
                                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new soul::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
                        new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0),
                        new soul::parsing::NonterminalParser(ToUtf32("PublicID"), ToUtf32("PublicID"), 0))),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new soul::parsing::CharParser('>'))));
    AddRule(new soul::parsing::Rule(ToUtf32("PublicID"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::StringParser(ToUtf32("PUBLIC")),
                new soul::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
            new soul::parsing::NonterminalParser(ToUtf32("PubidLiteral"), ToUtf32("PubidLiteral"), 0))));
}

} } // namespace soul.xml
