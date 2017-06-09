#include "Element.hpp"
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
#include <soul_u32/parsing/Rule.hpp>
#include <soul_u32/codedom/Type.hpp>
#include <soul_u32/code/Declarator.hpp>

namespace soul { namespace syntax {

using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;

ElementGrammar* ElementGrammar::Create()
{
    return Create(new soul::parsing::ParsingDomain());
}

ElementGrammar* ElementGrammar::Create(soul::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ElementGrammar* grammar(new ElementGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ElementGrammar::ElementGrammar(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32("ElementGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("soul.syntax")), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back(ToUtf32("alphabetic"));
    keywords0.push_back(ToUtf32("anychar"));
    keywords0.push_back(ToUtf32("basechar"));
    keywords0.push_back(ToUtf32("cased_letter"));
    keywords0.push_back(ToUtf32("close_punctuation"));
    keywords0.push_back(ToUtf32("connector_punctuation"));
    keywords0.push_back(ToUtf32("control"));
    keywords0.push_back(ToUtf32("currency_symbol"));
    keywords0.push_back(ToUtf32("dash_punctuation"));
    keywords0.push_back(ToUtf32("decimal_number"));
    keywords0.push_back(ToUtf32("digit"));
    keywords0.push_back(ToUtf32("empty"));
    keywords0.push_back(ToUtf32("enclosing_mark"));
    keywords0.push_back(ToUtf32("end"));
    keywords0.push_back(ToUtf32("final_punctuation"));
    keywords0.push_back(ToUtf32("format"));
    keywords0.push_back(ToUtf32("grammar"));
    keywords0.push_back(ToUtf32("graphic"));
    keywords0.push_back(ToUtf32("hexdigit"));
    keywords0.push_back(ToUtf32("idcont"));
    keywords0.push_back(ToUtf32("idstart"));
    keywords0.push_back(ToUtf32("initial_punctuation"));
    keywords0.push_back(ToUtf32("keyword"));
    keywords0.push_back(ToUtf32("keyword_list"));
    keywords0.push_back(ToUtf32("letter"));
    keywords0.push_back(ToUtf32("letter_number"));
    keywords0.push_back(ToUtf32("line_separator"));
    keywords0.push_back(ToUtf32("lower_letter"));
    keywords0.push_back(ToUtf32("mark"));
    keywords0.push_back(ToUtf32("math_symbol"));
    keywords0.push_back(ToUtf32("modifier_letter"));
    keywords0.push_back(ToUtf32("modifier_symbol"));
    keywords0.push_back(ToUtf32("nonspacing_mark"));
    keywords0.push_back(ToUtf32("number"));
    keywords0.push_back(ToUtf32("open_punctuation"));
    keywords0.push_back(ToUtf32("other"));
    keywords0.push_back(ToUtf32("other_letter"));
    keywords0.push_back(ToUtf32("other_number"));
    keywords0.push_back(ToUtf32("other_punctuation"));
    keywords0.push_back(ToUtf32("other_symbol"));
    keywords0.push_back(ToUtf32("paragraph_separator"));
    keywords0.push_back(ToUtf32("private_use"));
    keywords0.push_back(ToUtf32("punctuation"));
    keywords0.push_back(ToUtf32("range"));
    keywords0.push_back(ToUtf32("separator"));
    keywords0.push_back(ToUtf32("skip"));
    keywords0.push_back(ToUtf32("space"));
    keywords0.push_back(ToUtf32("space_separator"));
    keywords0.push_back(ToUtf32("spacing_mark"));
    keywords0.push_back(ToUtf32("start"));
    keywords0.push_back(ToUtf32("surrogate"));
    keywords0.push_back(ToUtf32("symbol"));
    keywords0.push_back(ToUtf32("title_letter"));
    keywords0.push_back(ToUtf32("token"));
    keywords0.push_back(ToUtf32("unassigned"));
    keywords0.push_back(ToUtf32("upper_letter"));
    keywords0.push_back(ToUtf32("using"));
    keywords0.push_back(ToUtf32("var"));
}

void ElementGrammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, soul::parsing::Grammar* grammar)
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
    stack.push(std::unique_ptr<soul::parsing::Object>(new ValueObject<soul::parsing::Grammar*>(grammar)));
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

class ElementGrammar::RuleLinkRule : public soul::parsing::Rule
{
public:
    RuleLinkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<RuleLinkRule>(this, &RuleLinkRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<RuleLinkRule>(this, &RuleLinkRule::A1Action));
        soul::parsing::NonterminalParser* aliasNameNonterminalParser = GetNonterminal(ToUtf32("aliasName"));
        aliasNameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::PostaliasName));
        soul::parsing::NonterminalParser* ruleNameNonterminalParser = GetNonterminal(ToUtf32("ruleName"));
        ruleNameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::PostruleName));
        soul::parsing::NonterminalParser* qualified_idNonterminalParser = GetNonterminal(ToUtf32("qualified_id"));
        qualified_idNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::Postqualified_id));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        RuleLink * link(new RuleLink(context->fromaliasName, context->grammar, context->fromruleName));
        link->SetSpan(span);
        context->grammar->AddRuleLink(link);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        RuleLink * link(new RuleLink(context->grammar, context->fromqualified_id));
        link->SetSpan(span);
        context->grammar->AddRuleLink(link);
    }
    void PostaliasName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromaliasName_value = std::move(stack.top());
            context->fromaliasName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromaliasName_value.get());
            stack.pop();
        }
    }
    void PostruleName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromruleName_value = std::move(stack.top());
            context->fromruleName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromruleName_value.get());
            stack.pop();
        }
    }
    void Postqualified_id(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromqualified_id_value = std::move(stack.top());
            context->fromqualified_id = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromqualified_id_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): grammar(), fromaliasName(), fromruleName(), fromqualified_id() {}
        soul::parsing::Grammar* grammar;
        std::u32string fromaliasName;
        std::u32string fromruleName;
        std::u32string fromqualified_id;
    };
};

class ElementGrammar::SignatureRule : public soul::parsing::Rule
{
public:
    SignatureRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        soul::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<SignatureRule>(this, &SignatureRule::PreParameterList));
        soul::parsing::NonterminalParser* returnTypeNonterminalParser = GetNonterminal(ToUtf32("ReturnType"));
        returnTypeNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<SignatureRule>(this, &SignatureRule::PreReturnType));
    }
    void PreParameterList(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Rule*>(context->rule)));
    }
    void PreReturnType(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Rule*>(context->rule)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): rule() {}
        soul::parsing::Rule* rule;
    };
};

class ElementGrammar::ParameterListRule : public soul::parsing::Rule
{
public:
    ParameterListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        soul::parsing::NonterminalParser* variableNonterminalParser = GetNonterminal(ToUtf32("Variable"));
        variableNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ParameterListRule>(this, &ParameterListRule::PreVariable));
        soul::parsing::NonterminalParser* parameterNonterminalParser = GetNonterminal(ToUtf32("Parameter"));
        parameterNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ParameterListRule>(this, &ParameterListRule::PreParameter));
    }
    void PreVariable(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Rule*>(context->rule)));
    }
    void PreParameter(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Rule*>(context->rule)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): rule() {}
        soul::parsing::Rule* rule;
    };
};

class ElementGrammar::VariableRule : public soul::parsing::Rule
{
public:
    VariableRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<VariableRule>(this, &VariableRule::A0Action));
        soul::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<VariableRule>(this, &VariableRule::PostTypeId));
        soul::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<VariableRule>(this, &VariableRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->AddLocalVariable(AttrOrVariable(context->fromTypeId->ToString(), context->fromDeclarator));
        delete context->fromTypeId;
    }
    void PostTypeId(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromTypeId_value = std::move(stack.top());
            context->fromTypeId = *static_cast<soul::parsing::ValueObject<soul::codedom::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostDeclarator(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): rule(), fromTypeId(), fromDeclarator() {}
        soul::parsing::Rule* rule;
        soul::codedom::TypeId* fromTypeId;
        std::u32string fromDeclarator;
    };
};

class ElementGrammar::ParameterRule : public soul::parsing::Rule
{
public:
    ParameterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ParameterRule>(this, &ParameterRule::A0Action));
        soul::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ParameterRule>(this, &ParameterRule::PostTypeId));
        soul::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ParameterRule>(this, &ParameterRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->AddInheritedAttribute(AttrOrVariable(context->fromTypeId->ToString(), context->fromDeclarator));
        delete context->fromTypeId;
    }
    void PostTypeId(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromTypeId_value = std::move(stack.top());
            context->fromTypeId = *static_cast<soul::parsing::ValueObject<soul::codedom::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostDeclarator(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): rule(), fromTypeId(), fromDeclarator() {}
        soul::parsing::Rule* rule;
        soul::codedom::TypeId* fromTypeId;
        std::u32string fromDeclarator;
    };
};

class ElementGrammar::ReturnTypeRule : public soul::parsing::Rule
{
public:
    ReturnTypeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ReturnTypeRule>(this, &ReturnTypeRule::A0Action));
        soul::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ReturnTypeRule>(this, &ReturnTypeRule::PostTypeId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->SetValueTypeName(context->fromTypeId->ToString());
        delete context->fromTypeId;
    }
    void PostTypeId(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromTypeId_value = std::move(stack.top());
            context->fromTypeId = *static_cast<soul::parsing::ValueObject<soul::codedom::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): rule(), fromTypeId() {}
        soul::parsing::Rule* rule;
        soul::codedom::TypeId* fromTypeId;
    };
};

class ElementGrammar::IdentifierRule : public soul::parsing::Rule
{
public:
    IdentifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<IdentifierRule>(this, &IdentifierRule::A0Action));
        soul::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("identifier"));
        identifierNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<IdentifierRule>(this, &IdentifierRule::Postidentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void Postidentifier(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromidentifier_value = std::move(stack.top());
            context->fromidentifier = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromidentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromidentifier() {}
        std::u32string value;
        std::u32string fromidentifier;
    };
};

class ElementGrammar::QualifiedIdRule : public soul::parsing::Rule
{
public:
    QualifiedIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<QualifiedIdRule>(this, &QualifiedIdRule::A0Action));
        soul::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postfirst));
        soul::parsing::NonterminalParser* restNonterminalParser = GetNonterminal(ToUtf32("rest"));
        restNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postrest));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void Postfirst(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromfirst_value = std::move(stack.top());
            context->fromfirst = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postrest(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromrest_value = std::move(stack.top());
            context->fromrest = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromrest_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromfirst(), fromrest() {}
        std::u32string value;
        std::u32string fromfirst;
        std::u32string fromrest;
    };
};

class ElementGrammar::StringArrayRule : public soul::parsing::Rule
{
public:
    StringArrayRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("std::vector<std::u32string>*"), ToUtf32("array")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> array_value = std::move(stack.top());
        context->array = *static_cast<soul::parsing::ValueObject<std::vector<std::u32string>*>*>(array_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<StringArrayRule>(this, &StringArrayRule::A0Action));
        soul::parsing::NonterminalParser* strNonterminalParser = GetNonterminal(ToUtf32("str"));
        strNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<StringArrayRule>(this, &StringArrayRule::Poststr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->array->push_back(context->fromstr);
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
        Context(): array(), fromstr() {}
        std::vector<std::u32string>* array;
        std::u32string fromstr;
    };
};

void ElementGrammar::GetReferencedGrammars()
{
    soul::parsing::ParsingDomain* pd = GetParsingDomain();
    soul::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("soul.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = soul::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    soul::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("soul.code.DeclaratorGrammar"));
    if (!grammar1)
    {
        grammar1 = soul::code::DeclaratorGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void ElementGrammar::CreateRules()
{
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("qualified_id"), this, ToUtf32("soul.parsing.stdlib.qualified_id")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("TypeId"), this, ToUtf32("soul.code.DeclaratorGrammar.TypeId")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("soul.parsing.stdlib.identifier")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("Declarator"), this, ToUtf32("soul.code.DeclaratorGrammar.Declarator")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("soul.parsing.stdlib.string")));
    AddRule(new RuleLinkRule(ToUtf32("RuleLink"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::KeywordParser(ToUtf32("using")),
                                new soul::parsing::NonterminalParser(ToUtf32("aliasName"), ToUtf32("identifier"), 0)),
                            new soul::parsing::CharParser('=')),
                        new soul::parsing::NonterminalParser(ToUtf32("ruleName"), ToUtf32("qualified_id"), 0)),
                    new soul::parsing::CharParser(';'))),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::KeywordParser(ToUtf32("using")),
                        new soul::parsing::NonterminalParser(ToUtf32("qualified_id"), ToUtf32("qualified_id"), 0)),
                    new soul::parsing::CharParser(';'))))));
    AddRule(new SignatureRule(ToUtf32("Signature"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::OptionalParser(
                new soul::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 1)),
            new soul::parsing::OptionalParser(
                new soul::parsing::NonterminalParser(ToUtf32("ReturnType"), ToUtf32("ReturnType"), 1)))));
    AddRule(new ParameterListRule(ToUtf32("ParameterList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::CharParser('('),
                new soul::parsing::ListParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::NonterminalParser(ToUtf32("Variable"), ToUtf32("Variable"), 1),
                        new soul::parsing::NonterminalParser(ToUtf32("Parameter"), ToUtf32("Parameter"), 1)),
                    new soul::parsing::CharParser(','))),
            new soul::parsing::ExpectationParser(
                new soul::parsing::CharParser(')')))));
    AddRule(new VariableRule(ToUtf32("Variable"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::KeywordParser(ToUtf32("var")),
                new soul::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0)),
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)))));
    AddRule(new ParameterRule(ToUtf32("Parameter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0),
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)))));
    AddRule(new ReturnTypeRule(ToUtf32("ReturnType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::DifferenceParser(
                new soul::parsing::CharParser(':'),
                new soul::parsing::StringParser(ToUtf32("::"))),
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0)))));
    AddRule(new soul::parsing::Rule(ToUtf32("Keyword"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::KeywordListParser(ToUtf32("identifier"), keywords0)));
    AddRule(new IdentifierRule(ToUtf32("Identifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::DifferenceParser(
                new soul::parsing::NonterminalParser(ToUtf32("identifier"), ToUtf32("identifier"), 0),
                new soul::parsing::NonterminalParser(ToUtf32("Keyword"), ToUtf32("Keyword"), 0)))));
    AddRule(new QualifiedIdRule(ToUtf32("QualifiedId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("Identifier"), 0),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('.'),
                            new soul::parsing::NonterminalParser(ToUtf32("rest"), ToUtf32("Identifier"), 0))))))));
    AddRule(new StringArrayRule(ToUtf32("StringArray"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::CharParser('['),
                new soul::parsing::ListParser(
                    new soul::parsing::ActionParser(ToUtf32("A0"),
                        new soul::parsing::NonterminalParser(ToUtf32("str"), ToUtf32("string"), 0)),
                    new soul::parsing::CharParser(','))),
            new soul::parsing::ExpectationParser(
                new soul::parsing::CharParser(']')))));
}

} } // namespace soul.syntax
