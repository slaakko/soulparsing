#include "Composite.hpp"
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
#include <soul_u32/parsing/Composite.hpp>
#include <soul_u32/syntax/Primary.hpp>

namespace soul { namespace syntax {

using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;

CompositeGrammar* CompositeGrammar::Create()
{
    return Create(new soul::parsing::ParsingDomain());
}

CompositeGrammar* CompositeGrammar::Create(soul::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    CompositeGrammar* grammar(new CompositeGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

CompositeGrammar::CompositeGrammar(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32("CompositeGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("soul.syntax")), parsingDomain_)
{
    SetOwner(0);
}

soul::parsing::Parser* CompositeGrammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, soul::parsing::Scope* enclosingScope)
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
    stack.push(std::unique_ptr<soul::parsing::Object>(new ValueObject<soul::parsing::Scope*>(enclosingScope)));
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

class CompositeGrammar::AlternativeRule : public soul::parsing::Rule
{
public:
    AlternativeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<soul::parsing::ValueObject<soul::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<AlternativeRule>(this, &AlternativeRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<AlternativeRule>(this, &AlternativeRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<AlternativeRule>(this, &AlternativeRule::Preleft));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AlternativeRule>(this, &AlternativeRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<AlternativeRule>(this, &AlternativeRule::Preright));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AlternativeRule>(this, &AlternativeRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AlternativeParser(context->value, context->fromright);
    }
    void Preleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        soul::parsing::Scope* enclosingScope;
        soul::parsing::Parser* value;
        soul::parsing::Parser* fromleft;
        soul::parsing::Parser* fromright;
    };
};

class CompositeGrammar::SequenceRule : public soul::parsing::Rule
{
public:
    SequenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<soul::parsing::ValueObject<soul::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<SequenceRule>(this, &SequenceRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<SequenceRule>(this, &SequenceRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<SequenceRule>(this, &SequenceRule::Preleft));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<SequenceRule>(this, &SequenceRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<SequenceRule>(this, &SequenceRule::Preright));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<SequenceRule>(this, &SequenceRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SequenceParser(context->value, context->fromright);
    }
    void Preleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        soul::parsing::Scope* enclosingScope;
        soul::parsing::Parser* value;
        soul::parsing::Parser* fromleft;
        soul::parsing::Parser* fromright;
    };
};

class CompositeGrammar::DifferenceRule : public soul::parsing::Rule
{
public:
    DifferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<soul::parsing::ValueObject<soul::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<DifferenceRule>(this, &DifferenceRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<DifferenceRule>(this, &DifferenceRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<DifferenceRule>(this, &DifferenceRule::Preleft));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<DifferenceRule>(this, &DifferenceRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<DifferenceRule>(this, &DifferenceRule::Preright));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<DifferenceRule>(this, &DifferenceRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DifferenceParser(context->value, context->fromright);
    }
    void Preleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        soul::parsing::Scope* enclosingScope;
        soul::parsing::Parser* value;
        soul::parsing::Parser* fromleft;
        soul::parsing::Parser* fromright;
    };
};

class CompositeGrammar::ExclusiveOrRule : public soul::parsing::Rule
{
public:
    ExclusiveOrRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<soul::parsing::ValueObject<soul::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ExclusiveOrRule>(this, &ExclusiveOrRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<ExclusiveOrRule>(this, &ExclusiveOrRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Preleft));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Preright));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExclusiveOrParser(context->value, context->fromright);
    }
    void Preleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        soul::parsing::Scope* enclosingScope;
        soul::parsing::Parser* value;
        soul::parsing::Parser* fromleft;
        soul::parsing::Parser* fromright;
    };
};

class CompositeGrammar::IntersectionRule : public soul::parsing::Rule
{
public:
    IntersectionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<soul::parsing::ValueObject<soul::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<IntersectionRule>(this, &IntersectionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<IntersectionRule>(this, &IntersectionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<IntersectionRule>(this, &IntersectionRule::Preleft));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<IntersectionRule>(this, &IntersectionRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<IntersectionRule>(this, &IntersectionRule::Preright));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<IntersectionRule>(this, &IntersectionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IntersectionParser(context->value, context->fromright);
    }
    void Preleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        soul::parsing::Scope* enclosingScope;
        soul::parsing::Parser* value;
        soul::parsing::Parser* fromleft;
        soul::parsing::Parser* fromright;
    };
};

class CompositeGrammar::ListRule : public soul::parsing::Rule
{
public:
    ListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<soul::parsing::ValueObject<soul::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ListRule>(this, &ListRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<ListRule>(this, &ListRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ListRule>(this, &ListRule::Preleft));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ListRule>(this, &ListRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ListRule>(this, &ListRule::Preright));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ListRule>(this, &ListRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ListParser(context->value, context->fromright);
    }
    void Preleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        soul::parsing::Scope* enclosingScope;
        soul::parsing::Parser* value;
        soul::parsing::Parser* fromleft;
        soul::parsing::Parser* fromright;
    };
};

class CompositeGrammar::PostfixRule : public soul::parsing::Rule
{
public:
    PostfixRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("soul::parsing::Parser*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<soul::parsing::ValueObject<soul::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A3Action));
        soul::parsing::NonterminalParser* primaryNonterminalParser = GetNonterminal(ToUtf32("Primary"));
        primaryNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<PostfixRule>(this, &PostfixRule::PrePrimary));
        primaryNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostfixRule>(this, &PostfixRule::PostPrimary));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPrimary;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KleeneStarParser(context->value);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PositiveParser(context->value);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OptionalParser(context->value);
    }
    void PrePrimary(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void PostPrimary(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromPrimary_value = std::move(stack.top());
            context->fromPrimary = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromPrimary_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): enclosingScope(), value(), fromPrimary() {}
        soul::parsing::Scope* enclosingScope;
        soul::parsing::Parser* value;
        soul::parsing::Parser* fromPrimary;
    };
};

void CompositeGrammar::GetReferencedGrammars()
{
    soul::parsing::ParsingDomain* pd = GetParsingDomain();
    soul::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("soul.syntax.PrimaryGrammar"));
    if (!grammar0)
    {
        grammar0 = soul::syntax::PrimaryGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void CompositeGrammar::CreateRules()
{
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("Primary"), this, ToUtf32("PrimaryGrammar.Primary")));
    AddRule(new AlternativeRule(ToUtf32("Alternative"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Sequence"), 1)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('|'),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Sequence"), 1))))))));
    AddRule(new SequenceRule(ToUtf32("Sequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Difference"), 1)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::ActionParser(ToUtf32("A1"),
                    new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Difference"), 1))))));
    AddRule(new DifferenceRule(ToUtf32("Difference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ExclusiveOr"), 1)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('-'),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ExclusiveOr"), 1))))))));
    AddRule(new ExclusiveOrRule(ToUtf32("ExclusiveOr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Intersection"), 1)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('^'),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Intersection"), 1))))))));
    AddRule(new IntersectionRule(ToUtf32("Intersection"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("List"), 1)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('&'),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("List"), 1)))))));
    AddRule(new ListRule(ToUtf32("List"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Postfix"), 1)),
            new soul::parsing::OptionalParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('%'),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Postfix"), 1)))))));
    AddRule(new PostfixRule(ToUtf32("Postfix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("Primary"), ToUtf32("Primary"), 1)),
            new soul::parsing::OptionalParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::ActionParser(ToUtf32("A1"),
                            new soul::parsing::CharParser('*')),
                        new soul::parsing::ActionParser(ToUtf32("A2"),
                            new soul::parsing::CharParser('+'))),
                    new soul::parsing::ActionParser(ToUtf32("A3"),
                        new soul::parsing::CharParser('?')))))));
}

} } // namespace soul.syntax
