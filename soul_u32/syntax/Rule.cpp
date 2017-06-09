#include "Rule.hpp"
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
#include <soul_u32/syntax/Composite.hpp>

namespace soul { namespace syntax {

using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;

RuleGrammar* RuleGrammar::Create()
{
    return Create(new soul::parsing::ParsingDomain());
}

RuleGrammar* RuleGrammar::Create(soul::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    RuleGrammar* grammar(new RuleGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

RuleGrammar::RuleGrammar(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32("RuleGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("soul.syntax")), parsingDomain_)
{
    SetOwner(0);
}

soul::parsing::Rule* RuleGrammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, soul::parsing::Scope* enclosingScope)
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
    soul::parsing::Rule* result = *static_cast<soul::parsing::ValueObject<soul::parsing::Rule*>*>(value.get());
    stack.pop();
    return result;
}

class RuleGrammar::RuleRule : public soul::parsing::Rule
{
public:
    RuleRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("soul::parsing::Rule*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Rule*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<RuleRule>(this, &RuleRule::A0Action));
        soul::parsing::NonterminalParser* ruleHeaderNonterminalParser = GetNonterminal(ToUtf32("RuleHeader"));
        ruleHeaderNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<RuleRule>(this, &RuleRule::PreRuleHeader));
        ruleHeaderNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<RuleRule>(this, &RuleRule::PostRuleHeader));
        soul::parsing::NonterminalParser* ruleBodyNonterminalParser = GetNonterminal(ToUtf32("RuleBody"));
        ruleBodyNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<RuleRule>(this, &RuleRule::PreRuleBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromRuleHeader;
    }
    void PreRuleHeader(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->enclosingScope)));
    }
    void PostRuleHeader(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromRuleHeader_value = std::move(stack.top());
            context->fromRuleHeader = *static_cast<soul::parsing::ValueObject<soul::parsing::Rule*>*>(fromRuleHeader_value.get());
            stack.pop();
        }
    }
    void PreRuleBody(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Rule*>(context->value)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): enclosingScope(), value(), fromRuleHeader() {}
        soul::parsing::Scope* enclosingScope;
        soul::parsing::Rule* value;
        soul::parsing::Rule* fromRuleHeader;
    };
};

class RuleGrammar::RuleHeaderRule : public soul::parsing::Rule
{
public:
    RuleHeaderRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("soul::parsing::Rule*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Rule*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<RuleHeaderRule>(this, &RuleHeaderRule::A0Action));
        soul::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("name"));
        nameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<RuleHeaderRule>(this, &RuleHeaderRule::Postname));
        soul::parsing::NonterminalParser* signatureNonterminalParser = GetNonterminal(ToUtf32("Signature"));
        signatureNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<RuleHeaderRule>(this, &RuleHeaderRule::PreSignature));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Rule(context->fromname, context->enclosingScope);
        context->value->SetSpan(span);
    }
    void Postname(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromname_value = std::move(stack.top());
            context->fromname = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromname_value.get());
            stack.pop();
        }
    }
    void PreSignature(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Rule*>(context->value)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): enclosingScope(), value(), fromname() {}
        soul::parsing::Scope* enclosingScope;
        soul::parsing::Rule* value;
        std::u32string fromname;
    };
};

class RuleGrammar::RuleBodyRule : public soul::parsing::Rule
{
public:
    RuleBodyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Rule*"), ToUtf32("rule")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> rule_value = std::move(stack.top());
        context->rule = *static_cast<soul::parsing::ValueObject<soul::parsing::Rule*>*>(rule_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<RuleBodyRule>(this, &RuleBodyRule::A0Action));
        soul::parsing::NonterminalParser* definitionNonterminalParser = GetNonterminal(ToUtf32("definition"));
        definitionNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<RuleBodyRule>(this, &RuleBodyRule::Predefinition));
        definitionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<RuleBodyRule>(this, &RuleBodyRule::Postdefinition));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->SetDefinition(context->fromdefinition);
    }
    void Predefinition(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->rule->GetScope())));
    }
    void Postdefinition(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromdefinition_value = std::move(stack.top());
            context->fromdefinition = *static_cast<soul::parsing::ValueObject<soul::parsing::Parser*>*>(fromdefinition_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): rule(), fromdefinition() {}
        soul::parsing::Rule* rule;
        soul::parsing::Parser* fromdefinition;
    };
};

void RuleGrammar::GetReferencedGrammars()
{
    soul::parsing::ParsingDomain* pd = GetParsingDomain();
    soul::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("soul.syntax.ElementGrammar"));
    if (!grammar0)
    {
        grammar0 = soul::syntax::ElementGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    soul::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("soul.syntax.CompositeGrammar"));
    if (!grammar1)
    {
        grammar1 = soul::syntax::CompositeGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    soul::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("soul.parsing.stdlib"));
    if (!grammar2)
    {
        grammar2 = soul::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void RuleGrammar::CreateRules()
{
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("ElementGrammar.Identifier")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("Signature"), this, ToUtf32("ElementGrammar.Signature")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("Alternative"), this, ToUtf32("CompositeGrammar.Alternative")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("soul.parsing.stdlib.string")));
    AddRule(new RuleRule(ToUtf32("Rule"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::NonterminalParser(ToUtf32("RuleHeader"), ToUtf32("RuleHeader"), 1)),
                new soul::parsing::StringParser(ToUtf32("::="))),
            new soul::parsing::NonterminalParser(ToUtf32("RuleBody"), ToUtf32("RuleBody"), 1))));
    AddRule(new RuleHeaderRule(ToUtf32("RuleHeader"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("name"), ToUtf32("Identifier"), 0)),
            new soul::parsing::NonterminalParser(ToUtf32("Signature"), ToUtf32("Signature"), 1))));
    AddRule(new RuleBodyRule(ToUtf32("RuleBody"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::SequenceParser(
                new soul::parsing::ExpectationParser(
                    new soul::parsing::NonterminalParser(ToUtf32("definition"), ToUtf32("Alternative"), 1)),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::CharParser(';'))))));
}

} } // namespace soul.syntax
