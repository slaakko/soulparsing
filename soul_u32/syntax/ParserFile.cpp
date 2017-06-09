#include "ParserFile.hpp"
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
#include <soul_u32/parsing/Namespace.hpp>
#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/code/Declaration.hpp>
#include <soul_u32/syntax/Element.hpp>
#include <soul_u32/syntax/Grammar.hpp>

namespace soul { namespace syntax {

using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;

ParserFileGrammar* ParserFileGrammar::Create()
{
    return Create(new soul::parsing::ParsingDomain());
}

ParserFileGrammar* ParserFileGrammar::Create(soul::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ParserFileGrammar* grammar(new ParserFileGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ParserFileGrammar::ParserFileGrammar(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32("ParserFileGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("soul.syntax")), parsingDomain_)
{
    SetOwner(0);
}

ParserFileContent* ParserFileGrammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, int id_, soul::parsing::ParsingDomain* parsingDomain_)
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
    stack.push(std::unique_ptr<soul::parsing::Object>(new ValueObject<int>(id_)));
    stack.push(std::unique_ptr<soul::parsing::Object>(new ValueObject<soul::parsing::ParsingDomain*>(parsingDomain_)));
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
    ParserFileContent* result = *static_cast<soul::parsing::ValueObject<ParserFileContent*>*>(value.get());
    stack.pop();
    return result;
}

class ParserFileGrammar::ParserFileRule : public soul::parsing::Rule
{
public:
    ParserFileRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("int"), ToUtf32("id_")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("soul::parsing::ParsingDomain*"), ToUtf32("parsingDomain_")));
        SetValueTypeName(ToUtf32("ParserFileContent*"));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> parsingDomain__value = std::move(stack.top());
        context->parsingDomain_ = *static_cast<soul::parsing::ValueObject<soul::parsing::ParsingDomain*>*>(parsingDomain__value.get());
        stack.pop();
        std::unique_ptr<soul::parsing::Object> id__value = std::move(stack.top());
        context->id_ = *static_cast<soul::parsing::ValueObject<int>*>(id__value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<ParserFileContent*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ParserFileRule>(this, &ParserFileRule::A0Action));
        soul::parsing::NonterminalParser* includeDirectivesNonterminalParser = GetNonterminal(ToUtf32("IncludeDirectives"));
        includeDirectivesNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ParserFileRule>(this, &ParserFileRule::PreIncludeDirectives));
        soul::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<ParserFileRule>(this, &ParserFileRule::PreNamespaceContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ParserFileContent(context->id_, context->parsingDomain_);
        context->value->SetFilePath(fileName);
    }
    void PreIncludeDirectives(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<ParserFileContent*>(context->value)));
    }
    void PreNamespaceContent(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<ParserFileContent*>(context->value)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): id_(), parsingDomain_(), value() {}
        int id_;
        soul::parsing::ParsingDomain* parsingDomain_;
        ParserFileContent* value;
    };
};

class ParserFileGrammar::IncludeDirectivesRule : public soul::parsing::Rule
{
public:
    IncludeDirectivesRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<soul::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::NonterminalParser* includeDirectiveNonterminalParser = GetNonterminal(ToUtf32("IncludeDirective"));
        includeDirectiveNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<IncludeDirectivesRule>(this, &IncludeDirectivesRule::PreIncludeDirective));
    }
    void PreIncludeDirective(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<ParserFileContent*>(context->parserFileContent)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): parserFileContent() {}
        ParserFileContent* parserFileContent;
    };
};

class ParserFileGrammar::IncludeDirectiveRule : public soul::parsing::Rule
{
public:
    IncludeDirectiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<soul::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<IncludeDirectiveRule>(this, &IncludeDirectiveRule::A0Action));
        soul::parsing::NonterminalParser* fileAttributeNonterminalParser = GetNonterminal(ToUtf32("FileAttribute"));
        fileAttributeNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<IncludeDirectiveRule>(this, &IncludeDirectiveRule::PostFileAttribute));
        soul::parsing::NonterminalParser* includeFileNameNonterminalParser = GetNonterminal(ToUtf32("IncludeFileName"));
        includeFileNameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<IncludeDirectiveRule>(this, &IncludeDirectiveRule::PostIncludeFileName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->AddIncludeDirective(ToUtf8(context->fromIncludeFileName), ToUtf8(context->fromFileAttribute));
    }
    void PostFileAttribute(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromFileAttribute_value = std::move(stack.top());
            context->fromFileAttribute = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromFileAttribute_value.get());
            stack.pop();
        }
    }
    void PostIncludeFileName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromIncludeFileName_value = std::move(stack.top());
            context->fromIncludeFileName = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromIncludeFileName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): parserFileContent(), fromFileAttribute(), fromIncludeFileName() {}
        ParserFileContent* parserFileContent;
        std::u32string fromFileAttribute;
        std::u32string fromIncludeFileName;
    };
};

class ParserFileGrammar::FileAttributeRule : public soul::parsing::Rule
{
public:
    FileAttributeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<FileAttributeRule>(this, &FileAttributeRule::A0Action));
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

class ParserFileGrammar::IncludeFileNameRule : public soul::parsing::Rule
{
public:
    IncludeFileNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<IncludeFileNameRule>(this, &IncludeFileNameRule::A0Action));
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

class ParserFileGrammar::NamespaceContentRule : public soul::parsing::Rule
{
public:
    NamespaceContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<soul::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A3Action));
        soul::parsing::NonterminalParser* fileAttributeNonterminalParser = GetNonterminal(ToUtf32("FileAttribute"));
        fileAttributeNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostFileAttribute));
        soul::parsing::NonterminalParser* usingDeclarationNonterminalParser = GetNonterminal(ToUtf32("UsingDeclaration"));
        usingDeclarationNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostUsingDeclaration));
        soul::parsing::NonterminalParser* usingDirectiveNonterminalParser = GetNonterminal(ToUtf32("UsingDirective"));
        usingDirectiveNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostUsingDirective));
        soul::parsing::NonterminalParser* namespaceAliasDefinitionNonterminalParser = GetNonterminal(ToUtf32("NamespaceAliasDefinition"));
        namespaceAliasDefinitionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostNamespaceAliasDefinition));
        soul::parsing::NonterminalParser* grammarNonterminalParser = GetNonterminal(ToUtf32("Grammar"));
        grammarNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreGrammar));
        grammarNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostGrammar));
        soul::parsing::NonterminalParser* namespaceNonterminalParser = GetNonterminal(ToUtf32("Namespace"));
        namespaceNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreNamespace));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fromUsingDeclaration->SetFileAttr(context->fromFileAttribute);
        context->parserFileContent->AddUsingObject(context->fromUsingDeclaration);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fromUsingDirective->SetFileAttr(context->fromFileAttribute);
        context->parserFileContent->AddUsingObject(context->fromUsingDirective);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fromNamespaceAliasDefinition->SetFileAttr(context->fromFileAttribute);
        context->parserFileContent->AddUsingObject(context->fromNamespaceAliasDefinition);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->AddGrammar(context->fromGrammar);
    }
    void PostFileAttribute(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromFileAttribute_value = std::move(stack.top());
            context->fromFileAttribute = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromFileAttribute_value.get());
            stack.pop();
        }
    }
    void PostUsingDeclaration(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromUsingDeclaration_value = std::move(stack.top());
            context->fromUsingDeclaration = *static_cast<soul::parsing::ValueObject<soul::codedom::UsingObject*>*>(fromUsingDeclaration_value.get());
            stack.pop();
        }
    }
    void PostUsingDirective(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromUsingDirective_value = std::move(stack.top());
            context->fromUsingDirective = *static_cast<soul::parsing::ValueObject<soul::codedom::UsingObject*>*>(fromUsingDirective_value.get());
            stack.pop();
        }
    }
    void PostNamespaceAliasDefinition(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromNamespaceAliasDefinition_value = std::move(stack.top());
            context->fromNamespaceAliasDefinition = *static_cast<soul::parsing::ValueObject<soul::codedom::UsingObject*>*>(fromNamespaceAliasDefinition_value.get());
            stack.pop();
        }
    }
    void PreGrammar(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::parsing::Scope*>(context->parserFileContent->CurrentScope())));
    }
    void PostGrammar(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromGrammar_value = std::move(stack.top());
            context->fromGrammar = *static_cast<soul::parsing::ValueObject<soul::parsing::Grammar*>*>(fromGrammar_value.get());
            stack.pop();
        }
    }
    void PreNamespace(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<ParserFileContent*>(context->parserFileContent)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): parserFileContent(), fromFileAttribute(), fromUsingDeclaration(), fromUsingDirective(), fromNamespaceAliasDefinition(), fromGrammar() {}
        ParserFileContent* parserFileContent;
        std::u32string fromFileAttribute;
        soul::codedom::UsingObject* fromUsingDeclaration;
        soul::codedom::UsingObject* fromUsingDirective;
        soul::codedom::UsingObject* fromNamespaceAliasDefinition;
        soul::parsing::Grammar* fromGrammar;
    };
};

class ParserFileGrammar::NamespaceRule : public soul::parsing::Rule
{
public:
    NamespaceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<soul::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<soul::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A1Action));
        soul::parsing::NonterminalParser* nsNonterminalParser = GetNonterminal(ToUtf32("ns"));
        nsNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NamespaceRule>(this, &NamespaceRule::Postns));
        soul::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<NamespaceRule>(this, &NamespaceRule::PreNamespaceContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->BeginNamespace(context->fromns);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->EndNamespace();
    }
    void Postns(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromns_value = std::move(stack.top());
            context->fromns = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromns_value.get());
            stack.pop();
        }
    }
    void PreNamespaceContent(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<ParserFileContent*>(context->parserFileContent)));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): parserFileContent(), fromns() {}
        ParserFileContent* parserFileContent;
        std::u32string fromns;
    };
};

void ParserFileGrammar::GetReferencedGrammars()
{
    soul::parsing::ParsingDomain* pd = GetParsingDomain();
    soul::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("soul.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = soul::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    soul::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("soul.code.DeclarationGrammar"));
    if (!grammar1)
    {
        grammar1 = soul::code::DeclarationGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    soul::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("soul.syntax.GrammarGrammar"));
    if (!grammar2)
    {
        grammar2 = soul::syntax::GrammarGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void ParserFileGrammar::CreateRules()
{
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("spaces_and_comments"), this, ToUtf32("soul.parsing.stdlib.spaces_and_comments")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("newline"), this, ToUtf32("soul.parsing.stdlib.newline")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("NamespaceAliasDefinition"), this, ToUtf32("soul.code.DeclarationGrammar.NamespaceAliasDefinition")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("qualified_id"), this, ToUtf32("soul.parsing.stdlib.qualified_id")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("Grammar"), this, ToUtf32("GrammarGrammar.Grammar")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("UsingDeclaration"), this, ToUtf32("soul.code.DeclarationGrammar.UsingDeclaration")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("UsingDirective"), this, ToUtf32("soul.code.DeclarationGrammar.UsingDirective")));
    AddRule(new ParserFileRule(ToUtf32("ParserFile"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::EmptyParser()),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("IncludeDirectives"), ToUtf32("IncludeDirectives"), 1))),
            new soul::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 1))));
    AddRule(new IncludeDirectivesRule(ToUtf32("IncludeDirectives"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::PositiveParser(
            new soul::parsing::NonterminalParser(ToUtf32("IncludeDirective"), ToUtf32("IncludeDirective"), 1))));
    AddRule(new IncludeDirectiveRule(ToUtf32("IncludeDirective"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("FileAttribute"), ToUtf32("FileAttribute"), 0)),
                    new soul::parsing::CharParser('#')),
                new soul::parsing::KeywordParser(ToUtf32("include"))),
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::TokenParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::NonterminalParser(ToUtf32("IncludeFileName"), ToUtf32("IncludeFileName"), 0),
                            new soul::parsing::KleeneStarParser(
                                new soul::parsing::CharSetParser(ToUtf32("\r\n"), true))),
                        new soul::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0)))))));
    AddRule(new FileAttributeRule(ToUtf32("FileAttribute"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::CharParser('['),
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::StringParser(ToUtf32("cpp")),
                        new soul::parsing::StringParser(ToUtf32("hpp"))))),
            new soul::parsing::CharParser(']'))));
    AddRule(new IncludeFileNameRule(ToUtf32("IncludeFileName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::CharParser('<'),
                        new soul::parsing::PositiveParser(
                            new soul::parsing::CharSetParser(ToUtf32(">\r\n"), true))),
                    new soul::parsing::CharParser('>'))))));
    AddRule(new NamespaceContentRule(ToUtf32("NamespaceContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::KleeneStarParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("FileAttribute"), ToUtf32("FileAttribute"), 0)),
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::AlternativeParser(
                                new soul::parsing::ActionParser(ToUtf32("A0"),
                                    new soul::parsing::NonterminalParser(ToUtf32("UsingDeclaration"), ToUtf32("UsingDeclaration"), 0)),
                                new soul::parsing::ActionParser(ToUtf32("A1"),
                                    new soul::parsing::NonterminalParser(ToUtf32("UsingDirective"), ToUtf32("UsingDirective"), 0))),
                            new soul::parsing::ActionParser(ToUtf32("A2"),
                                new soul::parsing::NonterminalParser(ToUtf32("NamespaceAliasDefinition"), ToUtf32("NamespaceAliasDefinition"), 0)))),
                    new soul::parsing::ActionParser(ToUtf32("A3"),
                        new soul::parsing::NonterminalParser(ToUtf32("Grammar"), ToUtf32("Grammar"), 1))),
                new soul::parsing::NonterminalParser(ToUtf32("Namespace"), ToUtf32("Namespace"), 1)))));
    AddRule(new NamespaceRule(ToUtf32("Namespace"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::KeywordParser(ToUtf32("namespace")),
                        new soul::parsing::ActionParser(ToUtf32("A0"),
                            new soul::parsing::ExpectationParser(
                                new soul::parsing::NonterminalParser(ToUtf32("ns"), ToUtf32("qualified_id"), 0)))),
                    new soul::parsing::ExpectationParser(
                        new soul::parsing::CharParser('{'))),
                new soul::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 1)),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::CharParser('}'))))));
    SetSkipRuleName(ToUtf32("spaces_and_comments"));
}

} } // namespace soul.syntax
