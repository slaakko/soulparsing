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
#include <sstream>
#include <soul_u32/util/Unicode.hpp>

namespace soul { namespace parsing {

using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;

stdlib* stdlib::Create()
{
    return Create(new soul::parsing::ParsingDomain());
}

stdlib* stdlib::Create(soul::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    stdlib* grammar(new stdlib(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

stdlib::stdlib(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32("stdlib"), parsingDomain_->GetNamespaceScope(ToUtf32("soul.parsing")), parsingDomain_)
{
    SetOwner(0);
}

class stdlib::intRule : public soul::parsing::Rule
{
public:
    intRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("int32_t"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<int32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<intRule>(this, &intRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> context->value;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        int32_t value;
    };
};

class stdlib::uintRule : public soul::parsing::Rule
{
public:
    uintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<uintRule>(this, &uintRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> context->value;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        uint32_t value;
    };
};

class stdlib::longRule : public soul::parsing::Rule
{
public:
    longRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("int64_t"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<int64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<longRule>(this, &longRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> context->value;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        int64_t value;
    };
};

class stdlib::ulongRule : public soul::parsing::Rule
{
public:
    ulongRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ulongRule>(this, &ulongRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> context->value;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        uint64_t value;
    };
};

class stdlib::hexuintRule : public soul::parsing::Rule
{
public:
    hexuintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<hexuintRule>(this, &hexuintRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> std::hex >> context->value;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        uint32_t value;
    };
};

class stdlib::hexRule : public soul::parsing::Rule
{
public:
    hexRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<hexRule>(this, &hexRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> std::hex >> context->value;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        uint64_t value;
    };
};

class stdlib::hex_literalRule : public soul::parsing::Rule
{
public:
    hex_literalRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<hex_literalRule>(this, &hex_literalRule::A0Action));
        soul::parsing::NonterminalParser* hexNonterminalParser = GetNonterminal(ToUtf32("hex"));
        hexNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<hex_literalRule>(this, &hex_literalRule::Posthex));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromhex;
    }
    void Posthex(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromhex_value = std::move(stack.top());
            context->fromhex = *static_cast<soul::parsing::ValueObject<uint64_t>*>(fromhex_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromhex() {}
        uint64_t value;
        uint64_t fromhex;
    };
};

class stdlib::realRule : public soul::parsing::Rule
{
public:
    realRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("double"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<double>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<realRule>(this, &realRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> context->value;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        double value;
    };
};

class stdlib::urealRule : public soul::parsing::Rule
{
public:
    urealRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("double"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<double>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<urealRule>(this, &urealRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> context->value;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        double value;
    };
};

class stdlib::numRule : public soul::parsing::Rule
{
public:
    numRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("double"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<double>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<numRule>(this, &numRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<numRule>(this, &numRule::A1Action));
        soul::parsing::NonterminalParser* rNonterminalParser = GetNonterminal(ToUtf32("r"));
        rNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<numRule>(this, &numRule::Postr));
        soul::parsing::NonterminalParser* iNonterminalParser = GetNonterminal(ToUtf32("i"));
        iNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<numRule>(this, &numRule::Posti));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromr;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromi;
    }
    void Postr(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromr_value = std::move(stack.top());
            context->fromr = *static_cast<soul::parsing::ValueObject<double>*>(fromr_value.get());
            stack.pop();
        }
    }
    void Posti(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromi_value = std::move(stack.top());
            context->fromi = *static_cast<soul::parsing::ValueObject<int32_t>*>(fromi_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromr(), fromi() {}
        double value;
        double fromr;
        int32_t fromi;
    };
};

class stdlib::boolRule : public soul::parsing::Rule
{
public:
    boolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("bool"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<bool>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<boolRule>(this, &boolRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<boolRule>(this, &boolRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = true;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = false;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        bool value;
    };
};

class stdlib::identifierRule : public soul::parsing::Rule
{
public:
    identifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<identifierRule>(this, &identifierRule::A0Action));
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

class stdlib::qualified_idRule : public soul::parsing::Rule
{
public:
    qualified_idRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<qualified_idRule>(this, &qualified_idRule::A0Action));
        soul::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<qualified_idRule>(this, &qualified_idRule::Postfirst));
        soul::parsing::NonterminalParser* restNonterminalParser = GetNonterminal(ToUtf32("rest"));
        restNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<qualified_idRule>(this, &qualified_idRule::Postrest));
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

class stdlib::escapeRule : public soul::parsing::Rule
{
public:
    escapeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<escapeRule>(this, &escapeRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<escapeRule>(this, &escapeRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<escapeRule>(this, &escapeRule::A2Action));
        soul::parsing::NonterminalParser* xNonterminalParser = GetNonterminal(ToUtf32("x"));
        xNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<escapeRule>(this, &escapeRule::Postx));
        soul::parsing::NonterminalParser* decimalEscapeNonterminalParser = GetNonterminal(ToUtf32("decimalEscape"));
        decimalEscapeNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<escapeRule>(this, &escapeRule::PostdecimalEscape));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromx);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromdecimalEscape);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        char32_t c = *matchBegin;
        switch (c)
        {
            case 'a': context->value = '\a';
            break;
            case 'b': context->value = '\b';
            break;
            case 'f': context->value = '\f';
            break;
            case 'n': context->value = '\n';
            break;
            case 'r': context->value = '\r';
            break;
            case 't': context->value = '\t';
            break;
            case 'v': context->value = '\v';
            break;
            case '0': context->value = '\0';
            break;
            default: context->value = c;
            break;
        }
    }
    void Postx(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromx_value = std::move(stack.top());
            context->fromx = *static_cast<soul::parsing::ValueObject<uint64_t>*>(fromx_value.get());
            stack.pop();
        }
    }
    void PostdecimalEscape(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromdecimalEscape_value = std::move(stack.top());
            context->fromdecimalEscape = *static_cast<soul::parsing::ValueObject<uint32_t>*>(fromdecimalEscape_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromx(), fromdecimalEscape() {}
        char32_t value;
        uint64_t fromx;
        uint32_t fromdecimalEscape;
    };
};

class stdlib::charRule : public soul::parsing::Rule
{
public:
    charRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<charRule>(this, &charRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<charRule>(this, &charRule::A1Action));
        soul::parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal(ToUtf32("escape"));
        escapeNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<charRule>(this, &charRule::Postescape));
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

class stdlib::stringRule : public soul::parsing::Rule
{
public:
    stringRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<stringRule>(this, &stringRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<stringRule>(this, &stringRule::A1Action));
        soul::parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal(ToUtf32("escape"));
        escapeNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<stringRule>(this, &stringRule::Postescape));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, context->fromescape);
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
        std::u32string value;
        char32_t fromescape;
    };
};

void stdlib::GetReferencedGrammars()
{
}

void stdlib::CreateRules()
{
    AddRule(new soul::parsing::Rule(ToUtf32("spaces"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::PositiveParser(
            new soul::parsing::SpaceParser())));
    AddRule(new soul::parsing::Rule(ToUtf32("newline"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::StringParser(ToUtf32("\r\n")),
                new soul::parsing::StringParser(ToUtf32("\n"))),
            new soul::parsing::StringParser(ToUtf32("\r")))));
    AddRule(new soul::parsing::Rule(ToUtf32("comment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::NonterminalParser(ToUtf32("line_comment"), ToUtf32("line_comment"), 0),
            new soul::parsing::NonterminalParser(ToUtf32("block_comment"), ToUtf32("block_comment"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("line_comment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::StringParser(ToUtf32("//")),
                new soul::parsing::KleeneStarParser(
                    new soul::parsing::CharSetParser(ToUtf32("\r\n"), true))),
            new soul::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("block_comment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::StringParser(ToUtf32("/*")),
                new soul::parsing::KleeneStarParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::NonterminalParser(ToUtf32("string"), ToUtf32("string"), 0),
                            new soul::parsing::NonterminalParser(ToUtf32("char"), ToUtf32("char"), 0)),
                        new soul::parsing::DifferenceParser(
                            new soul::parsing::AnyCharParser(),
                            new soul::parsing::StringParser(ToUtf32("*/")))))),
            new soul::parsing::StringParser(ToUtf32("*/")))));
    AddRule(new soul::parsing::Rule(ToUtf32("spaces_and_comments"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::PositiveParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::SpaceParser(),
                new soul::parsing::NonterminalParser(ToUtf32("comment"), ToUtf32("comment"), 0)))));
    AddRule(new soul::parsing::Rule(ToUtf32("digit_sequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::TokenParser(
            new soul::parsing::PositiveParser(
                new soul::parsing::DigitParser()))));
    AddRule(new soul::parsing::Rule(ToUtf32("sign"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::CharParser('+'),
            new soul::parsing::CharParser('-'))));
    AddRule(new intRule(ToUtf32("int"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("sign"), ToUtf32("sign"), 0)),
                    new soul::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0))))));
    AddRule(new uintRule(ToUtf32("uint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0))));
    AddRule(new longRule(ToUtf32("long"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("sign"), ToUtf32("sign"), 0)),
                    new soul::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0))))));
    AddRule(new ulongRule(ToUtf32("ulong"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0))));
    AddRule(new hexuintRule(ToUtf32("hexuint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::PositiveParser(
                    new soul::parsing::HexDigitParser())))));
    AddRule(new hexRule(ToUtf32("hex"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::PositiveParser(
                    new soul::parsing::HexDigitParser())))));
    AddRule(new hex_literalRule(ToUtf32("hex_literal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::StringParser(ToUtf32("0x")),
                        new soul::parsing::StringParser(ToUtf32("0X"))),
                    new soul::parsing::ExpectationParser(
                        new soul::parsing::NonterminalParser(ToUtf32("hex"), ToUtf32("hex"), 0)))))));
    AddRule(new realRule(ToUtf32("real"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("sign"), ToUtf32("sign"), 0)),
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::NonterminalParser(ToUtf32("fractional_real"), ToUtf32("fractional_real"), 0),
                        new soul::parsing::NonterminalParser(ToUtf32("exponent_real"), ToUtf32("exponent_real"), 0)))))));
    AddRule(new urealRule(ToUtf32("ureal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::AlternativeParser(
                new soul::parsing::NonterminalParser(ToUtf32("fractional_real"), ToUtf32("fractional_real"), 0),
                new soul::parsing::NonterminalParser(ToUtf32("exponent_real"), ToUtf32("exponent_real"), 0)))));
    AddRule(new soul::parsing::Rule(ToUtf32("fractional_real"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::OptionalParser(
                                new soul::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0)),
                            new soul::parsing::CharParser('.')),
                        new soul::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0)),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("exponent_part"), ToUtf32("exponent_part"), 0)))),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0),
                    new soul::parsing::CharParser('.'))))));
    AddRule(new soul::parsing::Rule(ToUtf32("exponent_real"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::TokenParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0),
                new soul::parsing::NonterminalParser(ToUtf32("exponent_part"), ToUtf32("exponent_part"), 0)))));
    AddRule(new soul::parsing::Rule(ToUtf32("exponent_part"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::TokenParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharSetParser(ToUtf32("eE")),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("sign"), ToUtf32("sign"), 0))),
                new soul::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0)))));
    AddRule(new numRule(ToUtf32("num"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("r"), ToUtf32("real"), 0)),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::NonterminalParser(ToUtf32("i"), ToUtf32("int"), 0)))));
    AddRule(new boolRule(ToUtf32("bool"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::KeywordParser(ToUtf32("true"))),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::KeywordParser(ToUtf32("false"))))));
    AddRule(new identifierRule(ToUtf32("identifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::IdStartParser(),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::IdContParser()))))));
    AddRule(new qualified_idRule(ToUtf32("qualified_id"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::TokenParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("identifier"), 0),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('.'),
                            new soul::parsing::NonterminalParser(ToUtf32("rest"), ToUtf32("identifier"), 0))))))));
    AddRule(new escapeRule(ToUtf32("escape"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::TokenParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::CharParser('\\'),
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharSetParser(ToUtf32("xX")),
                            new soul::parsing::ActionParser(ToUtf32("A0"),
                                new soul::parsing::NonterminalParser(ToUtf32("x"), ToUtf32("hex"), 0))),
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharSetParser(ToUtf32("dD")),
                            new soul::parsing::ActionParser(ToUtf32("A1"),
                                new soul::parsing::NonterminalParser(ToUtf32("decimalEscape"), ToUtf32("uint"), 0)))),
                    new soul::parsing::ActionParser(ToUtf32("A2"),
                        new soul::parsing::CharSetParser(ToUtf32("dDxX"), true)))))));
    AddRule(new charRule(ToUtf32("char"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::TokenParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('\''),
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::ActionParser(ToUtf32("A0"),
                            new soul::parsing::CharSetParser(ToUtf32("\\\r\n"), true)),
                        new soul::parsing::ActionParser(ToUtf32("A1"),
                            new soul::parsing::NonterminalParser(ToUtf32("escape"), ToUtf32("escape"), 0)))),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::CharParser('\''))))));
    AddRule(new stringRule(ToUtf32("string"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::TokenParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('\"'),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::ActionParser(ToUtf32("A0"),
                                new soul::parsing::PositiveParser(
                                    new soul::parsing::CharSetParser(ToUtf32("\"\\\r\n"), true))),
                            new soul::parsing::ActionParser(ToUtf32("A1"),
                                new soul::parsing::NonterminalParser(ToUtf32("escape"), ToUtf32("escape"), 0))))),
                new soul::parsing::ExpectationParser(
                    new soul::parsing::CharParser('\"'))))));
}

} } // namespace soul.parsing
