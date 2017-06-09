#include "Grammar.hpp"
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
#include <soul_u32/syntax/Rule.hpp>

namespace soul { namespace syntax {

using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;

GrammarGrammar* GrammarGrammar::Create()
{
    return Create(new soul::parsing::ParsingDomain());
}

GrammarGrammar* GrammarGrammar::Create(soul::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    GrammarGrammar* grammar(new GrammarGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

GrammarGrammar::GrammarGrammar(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32("GrammarGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("soul.syntax")), parsingDomain_)
{
    SetOwner(0);
}

soul::parsing::Grammar* GrammarGrammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, soul::parsing::Scope* enclosingScope)
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
    soul::parsing::Grammar* result = *static_cast<soul::parsing::ValueObject<soul::parsing::Grammar*>*>(value.get());
    stack.pop();
    return result;
}

class GrammarGrammar::GrammarRule : public soul::parsing::Rule
{
public:
    GrammarRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("soul::parsing::Grammar*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Grammar*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<GrammarRule>(this, &GrammarRule::A0Action));
        soul::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("name"));
        nameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<GrammarRule>(this, &GrammarRule::Postname));
        soul::parsing::NonterminalParser* grammarContentNonterminalParser = GetNonterminal(ToUtf32("GrammarContent"));
        grammarContentNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<GrammarRule>(this, &GrammarRule::PreGrammarContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Grammar(context->fromname, context->enclosingScope);
        context->value->SetSpan(span);
        context->value->SetNs(context->enclosingScope->Ns());
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
    void PreGrammarContent(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Grammar*>(context->value)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): enclosingScope(), value(), fromname() {}
        soul::parsing::Scope* enclosingScope;
        soul::parsing::Grammar* value;
        std::u32string fromname;
    };
};

class GrammarGrammar::GrammarContentRule : public soul::parsing::Rule
{
public:
    GrammarContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Grammar*"), ToUtf32("grammar")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> grammar_value = std::move(stack.top());
        context->grammar = *static_cast<soul::parsing::ValueObject<soul::parsing::Grammar*>*>(grammar_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<GrammarContentRule>(this, &GrammarContentRule::A0Action));
        soul::parsing::NonterminalParser* startClauseNonterminalParser = GetNonterminal(ToUtf32("StartClause"));
        startClauseNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreStartClause));
        soul::parsing::NonterminalParser* skipClauseNonterminalParser = GetNonterminal(ToUtf32("SkipClause"));
        skipClauseNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreSkipClause));
        soul::parsing::NonterminalParser* ruleLinkNonterminalParser = GetNonterminal(ToUtf32("RuleLink"));
        ruleLinkNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreRuleLink));
        soul::parsing::NonterminalParser* ruleNonterminalParser = GetNonterminal(ToUtf32("Rule"));
        ruleNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreRule));
        ruleNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<GrammarContentRule>(this, &GrammarContentRule::PostRule));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->grammar->AddRule(context->fromRule);
    }
    void PreStartClause(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Grammar*>(context->grammar)));
    }
    void PreSkipClause(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Grammar*>(context->grammar)));
    }
    void PreRuleLink(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Grammar*>(context->grammar)));
    }
    void PreRule(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->grammar->GetScope())));
    }
    void PostRule(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromRule_value = std::move(stack.top());
            context->fromRule = *static_cast<soul::parsing::ValueObject<soul::parsing::Rule*>*>(fromRule_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): grammar(), fromRule() {}
        soul::parsing::Grammar* grammar;
        soul::parsing::Rule* fromRule;
    };
};

class GrammarGrammar::StartClauseRule : public soul::parsing::Rule
{
public:
    StartClauseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Grammar*"), ToUtf32("grammar")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> grammar_value = std::move(stack.top());
        context->grammar = *static_cast<soul::parsing::ValueObject<soul::parsing::Grammar*>*>(grammar_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<StartClauseRule>(this, &StartClauseRule::A0Action));
        soul::parsing::NonterminalParser* strtNonterminalParser = GetNonterminal(ToUtf32("strt"));
        strtNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<StartClauseRule>(this, &StartClauseRule::Poststrt));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->grammar->SetStartRuleName(context->fromstrt);
    }
    void Poststrt(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromstrt_value = std::move(stack.top());
            context->fromstrt = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromstrt_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): grammar(), fromstrt() {}
        soul::parsing::Grammar* grammar;
        std::u32string fromstrt;
    };
};

class GrammarGrammar::SkipClauseRule : public soul::parsing::Rule
{
public:
    SkipClauseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::Grammar*"), ToUtf32("grammar")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> grammar_value = std::move(stack.top());
        context->grammar = *static_cast<soul::parsing::ValueObject<soul::parsing::Grammar*>*>(grammar_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<SkipClauseRule>(this, &SkipClauseRule::A0Action));
        soul::parsing::NonterminalParser* skpNonterminalParser = GetNonterminal(ToUtf32("skp"));
        skpNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<SkipClauseRule>(this, &SkipClauseRule::Postskp));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->grammar->SetSkipRuleName(context->fromskp);
    }
    void Postskp(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromskp_value = std::move(stack.top());
            context->fromskp = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromskp_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): grammar(), fromskp() {}
        soul::parsing::Grammar* grammar;
        std::u32string fromskp;
    };
};

void GrammarGrammar::GetReferencedGrammars()
{
    soul::parsing::ParsingDomain* pd = GetParsingDomain();
    soul::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("soul.syntax.ElementGrammar"));
    if (!grammar0)
    {
        grammar0 = soul::syntax::ElementGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    soul::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("soul.parsing.stdlib"));
    if (!grammar1)
    {
        grammar1 = soul::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
    soul::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("soul.syntax.RuleGrammar"));
    if (!grammar2)
    {
        grammar2 = soul::syntax::RuleGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void GrammarGrammar::CreateRules()
{
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("ElementGrammar.Identifier")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("Signature"), this, ToUtf32("ElementGrammar.Signature")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("ElementGrammar.QualifiedId")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("char"), this, ToUtf32("soul.parsing.stdlib.char")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("RuleLink"), this, ToUtf32("ElementGrammar.RuleLink")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("Rule"), this, ToUtf32("RuleGrammar.Rule")));
    AddRule(new GrammarRule(ToUtf32("Grammar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::KeywordParser(ToUtf32("grammar")),
                        new soul::parsing::ActionParser(ToUtf32("A0"),
                            new soul::parsing::ExpectationParser(
                                new soul::parsing::NonterminalParser(ToUtf32("name"), ToUtf32("Identifier"), 0)))),
                    new soul::parsing::ExpectationParser(
                        new soul::parsing::CharParser('{'))),
                new soul::parsing::NonterminalParser(ToUtf32("GrammarContent"), ToUtf32("GrammarContent"), 1)),
            new soul::parsing::ExpectationParser(
                new soul::parsing::CharParser('}')))));
    AddRule(new GrammarContentRule(ToUtf32("GrammarContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::KleeneStarParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::NonterminalParser(ToUtf32("StartClause"), ToUtf32("StartClause"), 1),
                        new soul::parsing::NonterminalParser(ToUtf32("SkipClause"), ToUtf32("SkipClause"), 1)),
                    new soul::parsing::NonterminalParser(ToUtf32("RuleLink"), ToUtf32("RuleLink"), 1)),
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::NonterminalParser(ToUtf32("Rule"), ToUtf32("Rule"), 1))))));
    AddRule(new StartClauseRule(ToUtf32("StartClause"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::KeywordParser(ToUtf32("start")),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::NonterminalParser(ToUtf32("strt"), ToUtf32("Identifier"), 0))),
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::CharParser(';')))));
    AddRule(new SkipClauseRule(ToUtf32("SkipClause"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::KeywordParser(ToUtf32("skip")),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::NonterminalParser(ToUtf32("skp"), ToUtf32("QualifiedId"), 0))),
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::CharParser(';')))));
}

} } // namespace soul.syntax
