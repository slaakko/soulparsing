#include "Expression.hpp"
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
#include <soul_u32/code/Declaration.hpp>
#include <soul_u32/code/Declarator.hpp>
#include <soul_u32/code/Literal.hpp>
#include <soul_u32/code/Identifier.hpp>
#include <soul_u32/util/TextUtils.hpp>
#include <soul_u32/codedom/Type.hpp>

namespace soul { namespace code {

using namespace soul::codedom;
using soul::util::Trim;
using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;

ExpressionGrammar* ExpressionGrammar::Create()
{
    return Create(new soul::parsing::ParsingDomain());
}

ExpressionGrammar* ExpressionGrammar::Create(soul::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ExpressionGrammar* grammar(new ExpressionGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ExpressionGrammar::ExpressionGrammar(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32("ExpressionGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("soul.code")), parsingDomain_)
{
    SetOwner(0);
}

soul::codedom::CppObject* ExpressionGrammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    soul::codedom::CppObject* result = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(value.get());
    stack.pop();
    return result;
}

class ExpressionGrammar::ExpressionRule : public soul::parsing::Rule
{
public:
    ExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ExpressionRule>(this, &ExpressionRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ExpressionRule>(this, &ExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::comma, 0, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::ConstantExpressionRule : public soul::parsing::Rule
{
public:
    ConstantExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ConstantExpressionRule>(this, &ConstantExpressionRule::A0Action));
        soul::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ConstantExpressionRule>(this, &ConstantExpressionRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpression;
    }
    void PostExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromExpression() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromExpression;
    };
};

class ExpressionGrammar::AssignmentExpressionRule : public soul::parsing::Rule
{
public:
    AssignmentExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("lor")));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A3Action));
        soul::parsing::NonterminalParser* logicalOrExpressionNonterminalParser = GetNonterminal(ToUtf32("LogicalOrExpression"));
        logicalOrExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostLogicalOrExpression));
        soul::parsing::NonterminalParser* assingmentOpNonterminalParser = GetNonterminal(ToUtf32("AssingmentOp"));
        assingmentOpNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostAssingmentOp));
        soul::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostAssignmentExpression));
        soul::parsing::NonterminalParser* conditionalExpressionNonterminalParser = GetNonterminal(ToUtf32("ConditionalExpression"));
        conditionalExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostConditionalExpression));
        soul::parsing::NonterminalParser* throwExpressionNonterminalParser = GetNonterminal(ToUtf32("ThrowExpression"));
        throwExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostThrowExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->lor.release(), context->fromAssingmentOp, 1, context->fromAssignmentExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->lor.reset(context->fromLogicalOrExpression);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConditionalExpression;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromThrowExpression;
    }
    void PostLogicalOrExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromLogicalOrExpression_value = std::move(stack.top());
            context->fromLogicalOrExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromLogicalOrExpression_value.get());
            stack.pop();
        }
    }
    void PostAssingmentOp(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromAssingmentOp_value = std::move(stack.top());
            context->fromAssingmentOp = *static_cast<soul::parsing::ValueObject<Operator>*>(fromAssingmentOp_value.get());
            stack.pop();
        }
    }
    void PostAssignmentExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
    void PostConditionalExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromConditionalExpression_value = std::move(stack.top());
            context->fromConditionalExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromConditionalExpression_value.get());
            stack.pop();
        }
    }
    void PostThrowExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromThrowExpression_value = std::move(stack.top());
            context->fromThrowExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromThrowExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), lor(), fromLogicalOrExpression(), fromAssingmentOp(), fromAssignmentExpression(), fromConditionalExpression(), fromThrowExpression() {}
        soul::codedom::CppObject* value;
        std::unique_ptr<CppObject> lor;
        soul::codedom::CppObject* fromLogicalOrExpression;
        Operator fromAssingmentOp;
        soul::codedom::CppObject* fromAssignmentExpression;
        soul::codedom::CppObject* fromConditionalExpression;
        soul::codedom::CppObject* fromThrowExpression;
    };
};

class ExpressionGrammar::AssingmentOpRule : public soul::parsing::Rule
{
public:
    AssingmentOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<AssingmentOpRule>(this, &AssingmentOpRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = GetOperator(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class ExpressionGrammar::ThrowExpressionRule : public soul::parsing::Rule
{
public:
    ThrowExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ThrowExpressionRule>(this, &ThrowExpressionRule::A0Action));
        soul::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ThrowExpressionRule>(this, &ThrowExpressionRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ThrowExpr(context->fromAssignmentExpression);
    }
    void PostAssignmentExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromAssignmentExpression() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromAssignmentExpression;
    };
};

class ExpressionGrammar::ConditionalExpressionRule : public soul::parsing::Rule
{
public:
    ConditionalExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::Postleft));
        soul::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PostExpression));
        soul::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConditionalExpr(context->value, context->fromExpression, context->fromAssignmentExpression);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PostAssignmentExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromExpression(), fromAssignmentExpression() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        soul::codedom::CppObject* fromExpression;
        soul::codedom::CppObject* fromAssignmentExpression;
    };
};

class ExpressionGrammar::LogicalOrExpressionRule : public soul::parsing::Rule
{
public:
    LogicalOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::or_, 2, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::LogicalAndExpressionRule : public soul::parsing::Rule
{
public:
    LogicalAndExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::and_, 3, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::InclusiveOrExpressionRule : public soul::parsing::Rule
{
public:
    InclusiveOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::bitor_, 4, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::ExclusiveOrExpressionRule : public soul::parsing::Rule
{
public:
    ExclusiveOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::bitxor, 5, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::AndExpressionRule : public soul::parsing::Rule
{
public:
    AndExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<AndExpressionRule>(this, &AndExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<AndExpressionRule>(this, &AndExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AndExpressionRule>(this, &AndExpressionRule::Postleft));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AndExpressionRule>(this, &AndExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::bitand_, 6, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::EqualityExpressionRule : public soul::parsing::Rule
{
public:
    EqualityExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Postleft));
        soul::parsing::NonterminalParser* eqOpNonterminalParser = GetNonterminal(ToUtf32("EqOp"));
        eqOpNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::PostEqOp));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromEqOp, 7, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostEqOp(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromEqOp_value = std::move(stack.top());
            context->fromEqOp = *static_cast<soul::parsing::ValueObject<Operator>*>(fromEqOp_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromEqOp(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        Operator fromEqOp;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::EqOpRule : public soul::parsing::Rule
{
public:
    EqOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<EqOpRule>(this, &EqOpRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<EqOpRule>(this, &EqOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::eq;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::notEq;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class ExpressionGrammar::RelationalExpressionRule : public soul::parsing::Rule
{
public:
    RelationalExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Postleft));
        soul::parsing::NonterminalParser* relOpNonterminalParser = GetNonterminal(ToUtf32("RelOp"));
        relOpNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::PostRelOp));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromRelOp, 8, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostRelOp(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromRelOp_value = std::move(stack.top());
            context->fromRelOp = *static_cast<soul::parsing::ValueObject<Operator>*>(fromRelOp_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromRelOp(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        Operator fromRelOp;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::RelOpRule : public soul::parsing::Rule
{
public:
    RelOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A3Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::lessOrEq;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::greaterOrEq;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::less;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::greater;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class ExpressionGrammar::ShiftExpressionRule : public soul::parsing::Rule
{
public:
    ShiftExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Postleft));
        soul::parsing::NonterminalParser* shiftOpNonterminalParser = GetNonterminal(ToUtf32("ShiftOp"));
        shiftOpNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::PostShiftOp));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromShiftOp, 9, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostShiftOp(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromShiftOp_value = std::move(stack.top());
            context->fromShiftOp = *static_cast<soul::parsing::ValueObject<Operator>*>(fromShiftOp_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromShiftOp(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        Operator fromShiftOp;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::ShiftOpRule : public soul::parsing::Rule
{
public:
    ShiftOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ShiftOpRule>(this, &ShiftOpRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<ShiftOpRule>(this, &ShiftOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::shiftLeft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::shiftRight;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class ExpressionGrammar::AdditiveExpressionRule : public soul::parsing::Rule
{
public:
    AdditiveExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Postleft));
        soul::parsing::NonterminalParser* addOpNonterminalParser = GetNonterminal(ToUtf32("AddOp"));
        addOpNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::PostAddOp));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromAddOp, 10, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostAddOp(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromAddOp_value = std::move(stack.top());
            context->fromAddOp = *static_cast<soul::parsing::ValueObject<Operator>*>(fromAddOp_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromAddOp(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        Operator fromAddOp;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::AddOpRule : public soul::parsing::Rule
{
public:
    AddOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<AddOpRule>(this, &AddOpRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<AddOpRule>(this, &AddOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::plus;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::minus;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class ExpressionGrammar::MultiplicativeExpressionRule : public soul::parsing::Rule
{
public:
    MultiplicativeExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Postleft));
        soul::parsing::NonterminalParser* mulOpNonterminalParser = GetNonterminal(ToUtf32("MulOp"));
        mulOpNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::PostMulOp));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromMulOp, 11, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostMulOp(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromMulOp_value = std::move(stack.top());
            context->fromMulOp = *static_cast<soul::parsing::ValueObject<Operator>*>(fromMulOp_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromMulOp(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        Operator fromMulOp;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::MulOpRule : public soul::parsing::Rule
{
public:
    MulOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::mul;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::div;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::rem;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class ExpressionGrammar::PmExpressionRule : public soul::parsing::Rule
{
public:
    PmExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PmExpressionRule>(this, &PmExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<PmExpressionRule>(this, &PmExpressionRule::A1Action));
        soul::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::Postleft));
        soul::parsing::NonterminalParser* pmOpNonterminalParser = GetNonterminal(ToUtf32("PmOp"));
        pmOpNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::PostPmOp));
        soul::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromPmOp, 12, context->fromright);
    }
    void Postleft(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostPmOp(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromPmOp_value = std::move(stack.top());
            context->fromPmOp = *static_cast<soul::parsing::ValueObject<Operator>*>(fromPmOp_value.get());
            stack.pop();
        }
    }
    void Postright(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromleft(), fromPmOp(), fromright() {}
        soul::codedom::CppObject* value;
        soul::codedom::CppObject* fromleft;
        Operator fromPmOp;
        soul::codedom::CppObject* fromright;
    };
};

class ExpressionGrammar::PmOpRule : public soul::parsing::Rule
{
public:
    PmOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PmOpRule>(this, &PmOpRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<PmOpRule>(this, &PmOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::dotStar;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::arrowStar;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class ExpressionGrammar::CastExpressionRule : public soul::parsing::Rule
{
public:
    CastExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("ce")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("ti")));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A3Action));
        soul::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostTypeId));
        soul::parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal(ToUtf32("CastExpression"));
        castExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostCastExpression));
        soul::parsing::NonterminalParser* unaryExpressionNonterminalParser = GetNonterminal(ToUtf32("UnaryExpression"));
        unaryExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostUnaryExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ce.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ce.reset(new CastExpr(context->ti.release(), context->fromCastExpression));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti.reset(context->fromTypeId);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ce.reset(context->fromUnaryExpression);
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
    void PostCastExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromCastExpression_value = std::move(stack.top());
            context->fromCastExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
    }
    void PostUnaryExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromUnaryExpression_value = std::move(stack.top());
            context->fromUnaryExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromUnaryExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), ce(), ti(), fromTypeId(), fromCastExpression(), fromUnaryExpression() {}
        soul::codedom::CppObject* value;
        std::unique_ptr<CppObject> ce;
        std::unique_ptr<CppObject> ti;
        soul::codedom::TypeId* fromTypeId;
        soul::codedom::CppObject* fromCastExpression;
        soul::codedom::CppObject* fromUnaryExpression;
    };
};

class ExpressionGrammar::UnaryExpressionRule : public soul::parsing::Rule
{
public:
    UnaryExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("ue")));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A3Action));
        soul::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A4Action));
        soul::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A5Action));
        soul::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A6Action));
        soul::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A7Action));
        soul::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A8Action));
        soul::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A9Action));
        soul::parsing::NonterminalParser* postfixExpressionNonterminalParser = GetNonterminal(ToUtf32("PostfixExpression"));
        postfixExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostPostfixExpression));
        soul::parsing::NonterminalParser* postCastExpressionNonterminalParser = GetNonterminal(ToUtf32("PostCastExpression"));
        postCastExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostPostCastExpression));
        soul::parsing::NonterminalParser* e1NonterminalParser = GetNonterminal(ToUtf32("e1"));
        e1NonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste1));
        soul::parsing::NonterminalParser* e2NonterminalParser = GetNonterminal(ToUtf32("e2"));
        e2NonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste2));
        soul::parsing::NonterminalParser* unaryOperatorNonterminalParser = GetNonterminal(ToUtf32("UnaryOperator"));
        unaryOperatorNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostUnaryOperator));
        soul::parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal(ToUtf32("CastExpression"));
        castExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostCastExpression));
        soul::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostTypeId));
        soul::parsing::NonterminalParser* e3NonterminalParser = GetNonterminal(ToUtf32("e3"));
        e3NonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste3));
        soul::parsing::NonterminalParser* newExpressionNonterminalParser = GetNonterminal(ToUtf32("NewExpression"));
        newExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostNewExpression));
        soul::parsing::NonterminalParser* deleteExpressionNonterminalParser = GetNonterminal(ToUtf32("DeleteExpression"));
        deleteExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostDeleteExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ue.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromPostfixExpression);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromPostCastExpression);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new PreIncrementExpr(context->frome1));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new PreDecrementExpr(context->frome2));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new UnaryOpExpr(context->fromUnaryOperator, context->fromCastExpression));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new SizeOfExpr(context->fromTypeId, true));
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new SizeOfExpr(context->frome3, false));
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromNewExpression);
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromDeleteExpression);
    }
    void PostPostfixExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromPostfixExpression_value = std::move(stack.top());
            context->fromPostfixExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromPostfixExpression_value.get());
            stack.pop();
        }
    }
    void PostPostCastExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromPostCastExpression_value = std::move(stack.top());
            context->fromPostCastExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromPostCastExpression_value.get());
            stack.pop();
        }
    }
    void Poste1(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> frome1_value = std::move(stack.top());
            context->frome1 = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(frome1_value.get());
            stack.pop();
        }
    }
    void Poste2(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> frome2_value = std::move(stack.top());
            context->frome2 = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(frome2_value.get());
            stack.pop();
        }
    }
    void PostUnaryOperator(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromUnaryOperator_value = std::move(stack.top());
            context->fromUnaryOperator = *static_cast<soul::parsing::ValueObject<Operator>*>(fromUnaryOperator_value.get());
            stack.pop();
        }
    }
    void PostCastExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromCastExpression_value = std::move(stack.top());
            context->fromCastExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
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
    void Poste3(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> frome3_value = std::move(stack.top());
            context->frome3 = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(frome3_value.get());
            stack.pop();
        }
    }
    void PostNewExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromNewExpression_value = std::move(stack.top());
            context->fromNewExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromNewExpression_value.get());
            stack.pop();
        }
    }
    void PostDeleteExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromDeleteExpression_value = std::move(stack.top());
            context->fromDeleteExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromDeleteExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), ue(), fromPostfixExpression(), fromPostCastExpression(), frome1(), frome2(), fromUnaryOperator(), fromCastExpression(), fromTypeId(), frome3(), fromNewExpression(), fromDeleteExpression() {}
        soul::codedom::CppObject* value;
        std::unique_ptr<CppObject> ue;
        soul::codedom::CppObject* fromPostfixExpression;
        soul::codedom::CppObject* fromPostCastExpression;
        soul::codedom::CppObject* frome1;
        soul::codedom::CppObject* frome2;
        Operator fromUnaryOperator;
        soul::codedom::CppObject* fromCastExpression;
        soul::codedom::TypeId* fromTypeId;
        soul::codedom::CppObject* frome3;
        soul::codedom::CppObject* fromNewExpression;
        soul::codedom::CppObject* fromDeleteExpression;
    };
};

class ExpressionGrammar::UnaryOperatorRule : public soul::parsing::Rule
{
public:
    UnaryOperatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A3Action));
        soul::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A4Action));
        soul::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new soul::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A5Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::deref;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::addrOf;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::plus;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::minus;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::not_;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::neg;
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class ExpressionGrammar::NewExpressionRule : public soul::parsing::Rule
{
public:
    NewExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("global")));
        AddLocalVariable(AttrOrVariable(ToUtf32("TypeId*"), ToUtf32("typeId")));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("parens")));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A3Action));
        soul::parsing::NonterminalParser* newPlacementNonterminalParser = GetNonterminal(ToUtf32("NewPlacement"));
        newPlacementNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewPlacement));
        soul::parsing::NonterminalParser* newTypeIdNonterminalParser = GetNonterminal(ToUtf32("NewTypeId"));
        newTypeIdNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewTypeId));
        soul::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostTypeId));
        soul::parsing::NonterminalParser* newInitializerNonterminalParser = GetNonterminal(ToUtf32("NewInitializer"));
        newInitializerNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewInitializer));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NewExpr(context->global, context->fromNewPlacement, context->typeId, context->parens, context->fromNewInitializer);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->global = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeId = context->fromNewTypeId;
        context->parens = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeId = context->fromTypeId;
        context->parens = true;
    }
    void PostNewPlacement(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromNewPlacement_value = std::move(stack.top());
            context->fromNewPlacement = *static_cast<soul::parsing::ValueObject<std::vector<soul::codedom::CppObject*>>*>(fromNewPlacement_value.get());
            stack.pop();
        }
    }
    void PostNewTypeId(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromNewTypeId_value = std::move(stack.top());
            context->fromNewTypeId = *static_cast<soul::parsing::ValueObject<soul::codedom::TypeId*>*>(fromNewTypeId_value.get());
            stack.pop();
        }
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
    void PostNewInitializer(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromNewInitializer_value = std::move(stack.top());
            context->fromNewInitializer = *static_cast<soul::parsing::ValueObject<std::vector<soul::codedom::CppObject*>>*>(fromNewInitializer_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), global(), typeId(), parens(), fromNewPlacement(), fromNewTypeId(), fromTypeId(), fromNewInitializer() {}
        soul::codedom::CppObject* value;
        bool global;
        TypeId* typeId;
        bool parens;
        std::vector<soul::codedom::CppObject*> fromNewPlacement;
        soul::codedom::TypeId* fromNewTypeId;
        soul::codedom::TypeId* fromTypeId;
        std::vector<soul::codedom::CppObject*> fromNewInitializer;
    };
};

class ExpressionGrammar::NewTypeIdRule : public soul::parsing::Rule
{
public:
    NewTypeIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::TypeId*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<TypeId>"), ToUtf32("ti")));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::TypeId*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A3Action));
        soul::parsing::NonterminalParser* typeSpecifierSeqNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifierSeq"));
        typeSpecifierSeqNonterminalParser->SetPreCall(new soul::parsing::MemberPreCall<NewTypeIdRule>(this, &NewTypeIdRule::PreTypeSpecifierSeq));
        soul::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NewTypeIdRule>(this, &NewTypeIdRule::PostTypeName));
        soul::parsing::NonterminalParser* newDeclaratorNonterminalParser = GetNonterminal(ToUtf32("NewDeclarator"));
        newDeclaratorNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NewTypeIdRule>(this, &NewTypeIdRule::PostNewDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti.reset(new TypeId);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ti.release();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti->Add(context->fromTypeName);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti->Declarator() = context->fromNewDeclarator;
    }
    void PreTypeSpecifierSeq(soul::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::TypeId*>(context->ti.get())));
    }
    void PostTypeName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromTypeName_value = std::move(stack.top());
            context->fromTypeName = *static_cast<soul::parsing::ValueObject<soul::codedom::TypeName*>*>(fromTypeName_value.get());
            stack.pop();
        }
    }
    void PostNewDeclarator(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromNewDeclarator_value = std::move(stack.top());
            context->fromNewDeclarator = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromNewDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), ti(), fromTypeName(), fromNewDeclarator() {}
        soul::codedom::TypeId* value;
        std::unique_ptr<TypeId> ti;
        soul::codedom::TypeName* fromTypeName;
        std::u32string fromNewDeclarator;
    };
};

class ExpressionGrammar::NewDeclaratorRule : public soul::parsing::Rule
{
public:
    NewDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<NewDeclaratorRule>(this, &NewDeclaratorRule::A0Action));
        soul::parsing::NonterminalParser* newDeclaratorNonterminalParser = GetNonterminal(ToUtf32("NewDeclarator"));
        newDeclaratorNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NewDeclaratorRule>(this, &NewDeclaratorRule::PostNewDeclarator));
        soul::parsing::NonterminalParser* directNewDeclaratorNonterminalParser = GetNonterminal(ToUtf32("DirectNewDeclarator"));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Trim(std::u32string(matchBegin, matchEnd));
    }
    void PostNewDeclarator(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromNewDeclarator_value = std::move(stack.top());
            context->fromNewDeclarator = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromNewDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromNewDeclarator() {}
        std::u32string value;
        std::u32string fromNewDeclarator;
    };
};

class ExpressionGrammar::DirectNewDeclaratorRule : public soul::parsing::Rule
{
public:
    DirectNewDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("e")));
    }
    virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)
    {
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::A1Action));
        soul::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::PostExpression));
        soul::parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal(ToUtf32("ConstantExpression"));
        constantExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::PostConstantExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->e.reset(context->fromExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->e.reset(context->fromConstantExpression);
    }
    void PostExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PostConstantExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromConstantExpression_value = std::move(stack.top());
            context->fromConstantExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromConstantExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): e(), fromExpression(), fromConstantExpression() {}
        std::unique_ptr<CppObject> e;
        soul::codedom::CppObject* fromExpression;
        soul::codedom::CppObject* fromConstantExpression;
    };
};

class ExpressionGrammar::NewPlacementRule : public soul::parsing::Rule
{
public:
    NewPlacementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::vector<soul::codedom::CppObject*>"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<std::vector<soul::codedom::CppObject*>>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<NewPlacementRule>(this, &NewPlacementRule::A0Action));
        soul::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NewPlacementRule>(this, &NewPlacementRule::PostExpressionList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionList;
    }
    void PostExpressionList(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromExpressionList_value = std::move(stack.top());
            context->fromExpressionList = *static_cast<soul::parsing::ValueObject<std::vector<soul::codedom::CppObject*>>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromExpressionList() {}
        std::vector<soul::codedom::CppObject*> value;
        std::vector<soul::codedom::CppObject*> fromExpressionList;
    };
};

class ExpressionGrammar::NewInitializerRule : public soul::parsing::Rule
{
public:
    NewInitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::vector<soul::codedom::CppObject*>"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<std::vector<soul::codedom::CppObject*>>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<NewInitializerRule>(this, &NewInitializerRule::A0Action));
        soul::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<NewInitializerRule>(this, &NewInitializerRule::PostExpressionList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionList;
    }
    void PostExpressionList(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromExpressionList_value = std::move(stack.top());
            context->fromExpressionList = *static_cast<soul::parsing::ValueObject<std::vector<soul::codedom::CppObject*>>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromExpressionList() {}
        std::vector<soul::codedom::CppObject*> value;
        std::vector<soul::codedom::CppObject*> fromExpressionList;
    };
};

class ExpressionGrammar::DeleteExpressionRule : public soul::parsing::Rule
{
public:
    DeleteExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("global")));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("array")));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A2Action));
        soul::parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal(ToUtf32("CastExpression"));
        castExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<DeleteExpressionRule>(this, &DeleteExpressionRule::PostCastExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DeleteExpr(context->global, context->array, context->fromCastExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->global = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->array = true;
    }
    void PostCastExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromCastExpression_value = std::move(stack.top());
            context->fromCastExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), global(), array(), fromCastExpression() {}
        soul::codedom::CppObject* value;
        bool global;
        bool array;
        soul::codedom::CppObject* fromCastExpression;
    };
};

class ExpressionGrammar::PostfixExpressionRule : public soul::parsing::Rule
{
public:
    PostfixExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("pe")));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A3Action));
        soul::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A4Action));
        soul::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A5Action));
        soul::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A6Action));
        soul::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A7Action));
        soul::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A8Action));
        soul::parsing::NonterminalParser* primaryExpressionNonterminalParser = GetNonterminal(ToUtf32("PrimaryExpression"));
        primaryExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PostPrimaryExpression));
        soul::parsing::NonterminalParser* indexNonterminalParser = GetNonterminal(ToUtf32("index"));
        indexNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postindex));
        soul::parsing::NonterminalParser* invokeNonterminalParser = GetNonterminal(ToUtf32("invoke"));
        invokeNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postinvoke));
        soul::parsing::NonterminalParser* memberNonterminalParser = GetNonterminal(ToUtf32("member"));
        memberNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postmember));
        soul::parsing::NonterminalParser* ptrmemberNonterminalParser = GetNonterminal(ToUtf32("ptrmember"));
        ptrmemberNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postptrmember));
        soul::parsing::NonterminalParser* typeSpecifierOrTypeNameNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifierOrTypeName"));
        typeSpecifierOrTypeNameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PostTypeSpecifierOrTypeName));
        soul::parsing::NonterminalParser* typeConstructionNonterminalParser = GetNonterminal(ToUtf32("typeConstruction"));
        typeConstructionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PosttypeConstruction));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->pe.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromPrimaryExpression);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new IndexExpr(context->pe.release(), context->fromindex));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new InvokeExpr(context->pe.release(), context->frominvoke));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new MemberAccessExpr(context->pe.release(), context->frommember));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new PtrMemberAccessExpr(context->pe.release(), context->fromptrmember));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new PostIncrementExpr(context->pe.release()));
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new PostDecrementExpr(context->pe.release()));
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new InvokeExpr(context->fromTypeSpecifierOrTypeName, context->fromtypeConstruction));
    }
    void PostPrimaryExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromPrimaryExpression_value = std::move(stack.top());
            context->fromPrimaryExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromPrimaryExpression_value.get());
            stack.pop();
        }
    }
    void Postindex(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromindex_value = std::move(stack.top());
            context->fromindex = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromindex_value.get());
            stack.pop();
        }
    }
    void Postinvoke(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> frominvoke_value = std::move(stack.top());
            context->frominvoke = *static_cast<soul::parsing::ValueObject<std::vector<soul::codedom::CppObject*>>*>(frominvoke_value.get());
            stack.pop();
        }
    }
    void Postmember(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> frommember_value = std::move(stack.top());
            context->frommember = *static_cast<soul::parsing::ValueObject<soul::codedom::IdExpr*>*>(frommember_value.get());
            stack.pop();
        }
    }
    void Postptrmember(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromptrmember_value = std::move(stack.top());
            context->fromptrmember = *static_cast<soul::parsing::ValueObject<soul::codedom::IdExpr*>*>(fromptrmember_value.get());
            stack.pop();
        }
    }
    void PostTypeSpecifierOrTypeName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromTypeSpecifierOrTypeName_value = std::move(stack.top());
            context->fromTypeSpecifierOrTypeName = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromTypeSpecifierOrTypeName_value.get());
            stack.pop();
        }
    }
    void PosttypeConstruction(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromtypeConstruction_value = std::move(stack.top());
            context->fromtypeConstruction = *static_cast<soul::parsing::ValueObject<std::vector<soul::codedom::CppObject*>>*>(fromtypeConstruction_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), pe(), fromPrimaryExpression(), fromindex(), frominvoke(), frommember(), fromptrmember(), fromTypeSpecifierOrTypeName(), fromtypeConstruction() {}
        soul::codedom::CppObject* value;
        std::unique_ptr<CppObject> pe;
        soul::codedom::CppObject* fromPrimaryExpression;
        soul::codedom::CppObject* fromindex;
        std::vector<soul::codedom::CppObject*> frominvoke;
        soul::codedom::IdExpr* frommember;
        soul::codedom::IdExpr* fromptrmember;
        soul::codedom::CppObject* fromTypeSpecifierOrTypeName;
        std::vector<soul::codedom::CppObject*> fromtypeConstruction;
    };
};

class ExpressionGrammar::TypeSpecifierOrTypeNameRule : public soul::parsing::Rule
{
public:
    TypeSpecifierOrTypeNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::A1Action));
        soul::parsing::NonterminalParser* simpleTypeSpecifierNonterminalParser = GetNonterminal(ToUtf32("SimpleTypeSpecifier"));
        simpleTypeSpecifierNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::PostSimpleTypeSpecifier));
        soul::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::PostTypeName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleTypeSpecifier;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypeName;
    }
    void PostSimpleTypeSpecifier(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromSimpleTypeSpecifier_value = std::move(stack.top());
            context->fromSimpleTypeSpecifier = *static_cast<soul::parsing::ValueObject<soul::codedom::TypeSpecifier*>*>(fromSimpleTypeSpecifier_value.get());
            stack.pop();
        }
    }
    void PostTypeName(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromTypeName_value = std::move(stack.top());
            context->fromTypeName = *static_cast<soul::parsing::ValueObject<soul::codedom::TypeName*>*>(fromTypeName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromSimpleTypeSpecifier(), fromTypeName() {}
        soul::codedom::CppObject* value;
        soul::codedom::TypeSpecifier* fromSimpleTypeSpecifier;
        soul::codedom::TypeName* fromTypeName;
    };
};

class ExpressionGrammar::PostCastExpressionRule : public soul::parsing::Rule
{
public:
    PostCastExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A3Action));
        soul::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new soul::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A4Action));
        soul::parsing::NonterminalParser* dctNonterminalParser = GetNonterminal(ToUtf32("dct"));
        dctNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postdct));
        soul::parsing::NonterminalParser* dceNonterminalParser = GetNonterminal(ToUtf32("dce"));
        dceNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postdce));
        soul::parsing::NonterminalParser* sctNonterminalParser = GetNonterminal(ToUtf32("sct"));
        sctNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postsct));
        soul::parsing::NonterminalParser* sceNonterminalParser = GetNonterminal(ToUtf32("sce"));
        sceNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postsce));
        soul::parsing::NonterminalParser* rctNonterminalParser = GetNonterminal(ToUtf32("rct"));
        rctNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postrct));
        soul::parsing::NonterminalParser* rceNonterminalParser = GetNonterminal(ToUtf32("rce"));
        rceNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postrce));
        soul::parsing::NonterminalParser* cctNonterminalParser = GetNonterminal(ToUtf32("cct"));
        cctNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postcct));
        soul::parsing::NonterminalParser* cceNonterminalParser = GetNonterminal(ToUtf32("cce"));
        cceNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postcce));
        soul::parsing::NonterminalParser* typeidExprNonterminalParser = GetNonterminal(ToUtf32("typeidExpr"));
        typeidExprNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::PosttypeidExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"dynamic_cast", context->fromdct, context->fromdce);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"static_cast", context->fromsct, context->fromsce);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"reinterpret_cast", context->fromrct, context->fromrce);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"const_cast", context->fromcct, context->fromcce);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TypeIdExpr(context->fromtypeidExpr);
    }
    void Postdct(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromdct_value = std::move(stack.top());
            context->fromdct = *static_cast<soul::parsing::ValueObject<soul::codedom::TypeId*>*>(fromdct_value.get());
            stack.pop();
        }
    }
    void Postdce(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromdce_value = std::move(stack.top());
            context->fromdce = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromdce_value.get());
            stack.pop();
        }
    }
    void Postsct(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromsct_value = std::move(stack.top());
            context->fromsct = *static_cast<soul::parsing::ValueObject<soul::codedom::TypeId*>*>(fromsct_value.get());
            stack.pop();
        }
    }
    void Postsce(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromsce_value = std::move(stack.top());
            context->fromsce = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromsce_value.get());
            stack.pop();
        }
    }
    void Postrct(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromrct_value = std::move(stack.top());
            context->fromrct = *static_cast<soul::parsing::ValueObject<soul::codedom::TypeId*>*>(fromrct_value.get());
            stack.pop();
        }
    }
    void Postrce(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromrce_value = std::move(stack.top());
            context->fromrce = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromrce_value.get());
            stack.pop();
        }
    }
    void Postcct(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromcct_value = std::move(stack.top());
            context->fromcct = *static_cast<soul::parsing::ValueObject<soul::codedom::TypeId*>*>(fromcct_value.get());
            stack.pop();
        }
    }
    void Postcce(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromcce_value = std::move(stack.top());
            context->fromcce = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromcce_value.get());
            stack.pop();
        }
    }
    void PosttypeidExpr(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromtypeidExpr_value = std::move(stack.top());
            context->fromtypeidExpr = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromtypeidExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromdct(), fromdce(), fromsct(), fromsce(), fromrct(), fromrce(), fromcct(), fromcce(), fromtypeidExpr() {}
        soul::codedom::CppObject* value;
        soul::codedom::TypeId* fromdct;
        soul::codedom::CppObject* fromdce;
        soul::codedom::TypeId* fromsct;
        soul::codedom::CppObject* fromsce;
        soul::codedom::TypeId* fromrct;
        soul::codedom::CppObject* fromrce;
        soul::codedom::TypeId* fromcct;
        soul::codedom::CppObject* fromcce;
        soul::codedom::CppObject* fromtypeidExpr;
    };
};

class ExpressionGrammar::ExpressionListRule : public soul::parsing::Rule
{
public:
    ExpressionListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::vector<soul::codedom::CppObject*>"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<std::vector<soul::codedom::CppObject*>>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<ExpressionListRule>(this, &ExpressionListRule::A0Action));
        soul::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<ExpressionListRule>(this, &ExpressionListRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.push_back(context->fromAssignmentExpression);
    }
    void PostAssignmentExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromAssignmentExpression() {}
        std::vector<soul::codedom::CppObject*> value;
        soul::codedom::CppObject* fromAssignmentExpression;
    };
};

class ExpressionGrammar::PrimaryExpressionRule : public soul::parsing::Rule
{
public:
    PrimaryExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("pe")));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A2Action));
        soul::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A3Action));
        soul::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new soul::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A4Action));
        soul::parsing::NonterminalParser* literalNonterminalParser = GetNonterminal(ToUtf32("Literal"));
        literalNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostLiteral));
        soul::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostExpression));
        soul::parsing::NonterminalParser* idExpressionNonterminalParser = GetNonterminal(ToUtf32("IdExpression"));
        idExpressionNonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostIdExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->pe.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromLiteral);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new ThisAccess);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromExpression);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromIdExpression);
    }
    void PostLiteral(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromLiteral_value = std::move(stack.top());
            context->fromLiteral = *static_cast<soul::parsing::ValueObject<soul::codedom::Literal*>*>(fromLiteral_value.get());
            stack.pop();
        }
    }
    void PostExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PostIdExpression(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromIdExpression_value = std::move(stack.top());
            context->fromIdExpression = *static_cast<soul::parsing::ValueObject<soul::codedom::IdExpr*>*>(fromIdExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), pe(), fromLiteral(), fromExpression(), fromIdExpression() {}
        soul::codedom::CppObject* value;
        std::unique_ptr<CppObject> pe;
        soul::codedom::Literal* fromLiteral;
        soul::codedom::CppObject* fromExpression;
        soul::codedom::IdExpr* fromIdExpression;
    };
};

class ExpressionGrammar::IdExpressionRule : public soul::parsing::Rule
{
public:
    IdExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        soul::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("soul::codedom::IdExpr*"));
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
            stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<soul::codedom::IdExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    virtual void Link()
    {
        soul::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new soul::parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A0Action));
        soul::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new soul::parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A1Action));
        soul::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new soul::parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A2Action));
        soul::parsing::NonterminalParser* id1NonterminalParser = GetNonterminal(ToUtf32("id1"));
        id1NonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<IdExpressionRule>(this, &IdExpressionRule::Postid1));
        soul::parsing::NonterminalParser* id2NonterminalParser = GetNonterminal(ToUtf32("id2"));
        id2NonterminalParser->SetPostCall(new soul::parsing::MemberPostCall<IdExpressionRule>(this, &IdExpressionRule::Postid2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdExpr(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdExpr(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdExpr(std::u32string(matchBegin, matchEnd));
    }
    void Postid1(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromid1_value = std::move(stack.top());
            context->fromid1 = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromid1_value.get());
            stack.pop();
        }
    }
    void Postid2(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<soul::parsing::Object> fromid2_value = std::move(stack.top());
            context->fromid2 = *static_cast<soul::parsing::ValueObject<std::u32string>*>(fromid2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : soul::parsing::Context
    {
        Context(): value(), fromid1(), fromid2() {}
        soul::codedom::IdExpr* value;
        std::u32string fromid1;
        std::u32string fromid2;
    };
};

void ExpressionGrammar::GetReferencedGrammars()
{
    soul::parsing::ParsingDomain* pd = GetParsingDomain();
    soul::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("soul.code.DeclaratorGrammar"));
    if (!grammar0)
    {
        grammar0 = soul::code::DeclaratorGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    soul::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("soul.code.LiteralGrammar"));
    if (!grammar1)
    {
        grammar1 = soul::code::LiteralGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    soul::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("soul.code.IdentifierGrammar"));
    if (!grammar2)
    {
        grammar2 = soul::code::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
    soul::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("soul.code.DeclarationGrammar"));
    if (!grammar3)
    {
        grammar3 = soul::code::DeclarationGrammar::Create(pd);
    }
    AddGrammarReference(grammar3);
}

void ExpressionGrammar::CreateRules()
{
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("PtrOperator"), this, ToUtf32("DeclaratorGrammar.PtrOperator")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("TypeId"), this, ToUtf32("DeclaratorGrammar.TypeId")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("Literal"), this, ToUtf32("LiteralGrammar.Literal")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("TypeSpecifierSeq"), this, ToUtf32("DeclaratorGrammar.TypeSpecifierSeq")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("IdentifierGrammar.QualifiedId")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("TypeName"), this, ToUtf32("DeclarationGrammar.TypeName")));
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("SimpleTypeSpecifier"), this, ToUtf32("DeclarationGrammar.SimpleTypeSpecifier")));
    AddRule(new ExpressionRule(ToUtf32("Expression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AssignmentExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser(','),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AssignmentExpression"), 0)))))));
    AddRule(new ConstantExpressionRule(ToUtf32("ConstantExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))));
    AddRule(new AssignmentExpressionRule(ToUtf32("AssignmentExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::ActionParser(ToUtf32("A1"),
                                new soul::parsing::NonterminalParser(ToUtf32("LogicalOrExpression"), ToUtf32("LogicalOrExpression"), 0)),
                            new soul::parsing::NonterminalParser(ToUtf32("AssingmentOp"), ToUtf32("AssingmentOp"), 0)),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0)))),
                new soul::parsing::ActionParser(ToUtf32("A2"),
                    new soul::parsing::NonterminalParser(ToUtf32("ConditionalExpression"), ToUtf32("ConditionalExpression"), 0))),
            new soul::parsing::ActionParser(ToUtf32("A3"),
                new soul::parsing::NonterminalParser(ToUtf32("ThrowExpression"), ToUtf32("ThrowExpression"), 0)))));
    AddRule(new AssingmentOpRule(ToUtf32("AssingmentOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
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
                                                    new soul::parsing::CharParser('='),
                                                    new soul::parsing::StringParser(ToUtf32("*="))),
                                                new soul::parsing::StringParser(ToUtf32("/="))),
                                            new soul::parsing::StringParser(ToUtf32("%="))),
                                        new soul::parsing::StringParser(ToUtf32("+="))),
                                    new soul::parsing::StringParser(ToUtf32("-="))),
                                new soul::parsing::StringParser(ToUtf32(">>="))),
                            new soul::parsing::StringParser(ToUtf32("<<="))),
                        new soul::parsing::StringParser(ToUtf32("&="))),
                    new soul::parsing::StringParser(ToUtf32("^="))),
                new soul::parsing::StringParser(ToUtf32("|="))))));
    AddRule(new ThrowExpressionRule(ToUtf32("ThrowExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::SequenceParser(
                new soul::parsing::KeywordParser(ToUtf32("throw")),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0))))));
    AddRule(new ConditionalExpressionRule(ToUtf32("ConditionalExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("LogicalOrExpression"), 0)),
            new soul::parsing::OptionalParser(
                new soul::parsing::ActionParser(ToUtf32("A1"),
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::CharParser('?'),
                                new soul::parsing::ExpectationParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
                            new soul::parsing::CharParser(':')),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0))))))));
    AddRule(new LogicalOrExpressionRule(ToUtf32("LogicalOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("LogicalAndExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::StringParser(ToUtf32("||")),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("LogicalAndExpression"), 0))))))));
    AddRule(new LogicalAndExpressionRule(ToUtf32("LogicalAndExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("InclusiveOrExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::StringParser(ToUtf32("&&")),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("InclusiveOrExpression"), 0))))))));
    AddRule(new InclusiveOrExpressionRule(ToUtf32("InclusiveOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ExclusiveOrExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::DifferenceParser(
                        new soul::parsing::CharParser('|'),
                        new soul::parsing::StringParser(ToUtf32("||"))),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ExclusiveOrExpression"), 0))))))));
    AddRule(new ExclusiveOrExpressionRule(ToUtf32("ExclusiveOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AndExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('^'),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AndExpression"), 0))))))));
    AddRule(new AndExpressionRule(ToUtf32("AndExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("EqualityExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::DifferenceParser(
                        new soul::parsing::CharParser('&'),
                        new soul::parsing::StringParser(ToUtf32("&&"))),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("EqualityExpression"), 0))))))));
    AddRule(new EqualityExpressionRule(ToUtf32("EqualityExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("RelationalExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("EqOp"), ToUtf32("EqOp"), 0),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelationalExpression"), 0))))))));
    AddRule(new EqOpRule(ToUtf32("EqOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::StringParser(ToUtf32("=="))),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::StringParser(ToUtf32("!="))))));
    AddRule(new RelationalExpressionRule(ToUtf32("RelationalExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ShiftExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("RelOp"), ToUtf32("RelOp"), 0),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ShiftExpression"), 0))))))));
    AddRule(new RelOpRule(ToUtf32("RelOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::ActionParser(ToUtf32("A0"),
                        new soul::parsing::StringParser(ToUtf32("<="))),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::StringParser(ToUtf32(">=")))),
                new soul::parsing::ActionParser(ToUtf32("A2"),
                    new soul::parsing::DifferenceParser(
                        new soul::parsing::CharParser('<'),
                        new soul::parsing::StringParser(ToUtf32("<<"))))),
            new soul::parsing::ActionParser(ToUtf32("A3"),
                new soul::parsing::DifferenceParser(
                    new soul::parsing::CharParser('>'),
                    new soul::parsing::StringParser(ToUtf32(">>")))))));
    AddRule(new ShiftExpressionRule(ToUtf32("ShiftExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AdditiveExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("ShiftOp"), ToUtf32("ShiftOp"), 0),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AdditiveExpression"), 0))))))));
    AddRule(new ShiftOpRule(ToUtf32("ShiftOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::DifferenceParser(
                    new soul::parsing::StringParser(ToUtf32("<<")),
                    new soul::parsing::StringParser(ToUtf32("<<=")))),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::DifferenceParser(
                    new soul::parsing::StringParser(ToUtf32(">>")),
                    new soul::parsing::StringParser(ToUtf32(">>=")))))));
    AddRule(new AdditiveExpressionRule(ToUtf32("AdditiveExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("MultiplicativeExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("AddOp"), ToUtf32("AddOp"), 0),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("MultiplicativeExpression"), 0))))))));
    AddRule(new AddOpRule(ToUtf32("AddOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::DifferenceParser(
                    new soul::parsing::CharParser('+'),
                    new soul::parsing::StringParser(ToUtf32("+=")))),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::DifferenceParser(
                    new soul::parsing::CharParser('-'),
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::StringParser(ToUtf32("-=")),
                        new soul::parsing::StringParser(ToUtf32("->"))))))));
    AddRule(new MultiplicativeExpressionRule(ToUtf32("MultiplicativeExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("PmExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("MulOp"), ToUtf32("MulOp"), 0),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("PmExpression"), 0))))))));
    AddRule(new MulOpRule(ToUtf32("MulOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::DifferenceParser(
                        new soul::parsing::CharParser('*'),
                        new soul::parsing::StringParser(ToUtf32("*=")))),
                new soul::parsing::ActionParser(ToUtf32("A1"),
                    new soul::parsing::DifferenceParser(
                        new soul::parsing::CharParser('/'),
                        new soul::parsing::StringParser(ToUtf32("/="))))),
            new soul::parsing::ActionParser(ToUtf32("A2"),
                new soul::parsing::DifferenceParser(
                    new soul::parsing::CharParser('%'),
                    new soul::parsing::StringParser(ToUtf32("%=")))))));
    AddRule(new PmExpressionRule(ToUtf32("PmExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("CastExpression"), 0)),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("PmOp"), ToUtf32("PmOp"), 0),
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("CastExpression"), 0))))))));
    AddRule(new PmOpRule(ToUtf32("PmOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::StringParser(ToUtf32(".*"))),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::StringParser(ToUtf32("->*"))))));
    AddRule(new CastExpressionRule(ToUtf32("CastExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::AlternativeParser(
                new soul::parsing::ActionParser(ToUtf32("A1"),
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::CharParser('('),
                                new soul::parsing::ActionParser(ToUtf32("A2"),
                                    new soul::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0))),
                            new soul::parsing::CharParser(')')),
                        new soul::parsing::NonterminalParser(ToUtf32("CastExpression"), ToUtf32("CastExpression"), 0))),
                new soul::parsing::ActionParser(ToUtf32("A3"),
                    new soul::parsing::NonterminalParser(ToUtf32("UnaryExpression"), ToUtf32("UnaryExpression"), 0))))));
    AddRule(new UnaryExpressionRule(ToUtf32("UnaryExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::AlternativeParser(
                                new soul::parsing::AlternativeParser(
                                    new soul::parsing::AlternativeParser(
                                        new soul::parsing::AlternativeParser(
                                            new soul::parsing::ActionParser(ToUtf32("A1"),
                                                new soul::parsing::NonterminalParser(ToUtf32("PostfixExpression"), ToUtf32("PostfixExpression"), 0)),
                                            new soul::parsing::ActionParser(ToUtf32("A2"),
                                                new soul::parsing::NonterminalParser(ToUtf32("PostCastExpression"), ToUtf32("PostCastExpression"), 0))),
                                        new soul::parsing::ActionParser(ToUtf32("A3"),
                                            new soul::parsing::SequenceParser(
                                                new soul::parsing::StringParser(ToUtf32("++")),
                                                new soul::parsing::ExpectationParser(
                                                    new soul::parsing::NonterminalParser(ToUtf32("e1"), ToUtf32("UnaryExpression"), 0))))),
                                    new soul::parsing::ActionParser(ToUtf32("A4"),
                                        new soul::parsing::SequenceParser(
                                            new soul::parsing::StringParser(ToUtf32("--")),
                                            new soul::parsing::ExpectationParser(
                                                new soul::parsing::NonterminalParser(ToUtf32("e2"), ToUtf32("UnaryExpression"), 0))))),
                                new soul::parsing::ActionParser(ToUtf32("A5"),
                                    new soul::parsing::SequenceParser(
                                        new soul::parsing::NonterminalParser(ToUtf32("UnaryOperator"), ToUtf32("UnaryOperator"), 0),
                                        new soul::parsing::ExpectationParser(
                                            new soul::parsing::NonterminalParser(ToUtf32("CastExpression"), ToUtf32("CastExpression"), 0))))),
                            new soul::parsing::ActionParser(ToUtf32("A6"),
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::SequenceParser(
                                        new soul::parsing::SequenceParser(
                                            new soul::parsing::KeywordParser(ToUtf32("sizeof")),
                                            new soul::parsing::CharParser('(')),
                                        new soul::parsing::ExpectationParser(
                                            new soul::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0))),
                                    new soul::parsing::ExpectationParser(
                                        new soul::parsing::CharParser(')'))))),
                        new soul::parsing::ActionParser(ToUtf32("A7"),
                            new soul::parsing::SequenceParser(
                                new soul::parsing::KeywordParser(ToUtf32("sizeof")),
                                new soul::parsing::ExpectationParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("e3"), ToUtf32("UnaryExpression"), 0))))),
                    new soul::parsing::ActionParser(ToUtf32("A8"),
                        new soul::parsing::NonterminalParser(ToUtf32("NewExpression"), ToUtf32("NewExpression"), 0))),
                new soul::parsing::ActionParser(ToUtf32("A9"),
                    new soul::parsing::NonterminalParser(ToUtf32("DeleteExpression"), ToUtf32("DeleteExpression"), 0))))));
    AddRule(new UnaryOperatorRule(ToUtf32("UnaryOperator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::ActionParser(ToUtf32("A0"),
                                new soul::parsing::DifferenceParser(
                                    new soul::parsing::CharParser('*'),
                                    new soul::parsing::StringParser(ToUtf32("*=")))),
                            new soul::parsing::ActionParser(ToUtf32("A1"),
                                new soul::parsing::DifferenceParser(
                                    new soul::parsing::CharParser('&'),
                                    new soul::parsing::StringParser(ToUtf32("&="))))),
                        new soul::parsing::ActionParser(ToUtf32("A2"),
                            new soul::parsing::DifferenceParser(
                                new soul::parsing::CharParser('+'),
                                new soul::parsing::StringParser(ToUtf32("+="))))),
                    new soul::parsing::ActionParser(ToUtf32("A3"),
                        new soul::parsing::DifferenceParser(
                            new soul::parsing::CharParser('-'),
                            new soul::parsing::StringParser(ToUtf32("-="))))),
                new soul::parsing::ActionParser(ToUtf32("A4"),
                    new soul::parsing::DifferenceParser(
                        new soul::parsing::CharParser('!'),
                        new soul::parsing::StringParser(ToUtf32("!="))))),
            new soul::parsing::ActionParser(ToUtf32("A5"),
                new soul::parsing::CharParser('~')))));
    AddRule(new NewExpressionRule(ToUtf32("NewExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::OptionalParser(
                                new soul::parsing::ActionParser(ToUtf32("A1"),
                                    new soul::parsing::StringParser(ToUtf32("::")))),
                            new soul::parsing::KeywordParser(ToUtf32("new"))),
                        new soul::parsing::OptionalParser(
                            new soul::parsing::NonterminalParser(ToUtf32("NewPlacement"), ToUtf32("NewPlacement"), 0))),
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::ActionParser(ToUtf32("A2"),
                            new soul::parsing::NonterminalParser(ToUtf32("NewTypeId"), ToUtf32("NewTypeId"), 0)),
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::CharParser('('),
                                new soul::parsing::ActionParser(ToUtf32("A3"),
                                    new soul::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0))),
                            new soul::parsing::CharParser(')')))),
                new soul::parsing::OptionalParser(
                    new soul::parsing::NonterminalParser(ToUtf32("NewInitializer"), ToUtf32("NewInitializer"), 0))))));
    AddRule(new NewTypeIdRule(ToUtf32("NewTypeId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::EmptyParser()),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::SequenceParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::NonterminalParser(ToUtf32("TypeSpecifierSeq"), ToUtf32("TypeSpecifierSeq"), 1),
                        new soul::parsing::ActionParser(ToUtf32("A2"),
                            new soul::parsing::NonterminalParser(ToUtf32("TypeName"), ToUtf32("TypeName"), 0))),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::ActionParser(ToUtf32("A3"),
                            new soul::parsing::NonterminalParser(ToUtf32("NewDeclarator"), ToUtf32("NewDeclarator"), 0))))))));
    AddRule(new NewDeclaratorRule(ToUtf32("NewDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::AlternativeParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::NonterminalParser(ToUtf32("PtrOperator"), ToUtf32("PtrOperator"), 0),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("NewDeclarator"), ToUtf32("NewDeclarator"), 0))),
                new soul::parsing::NonterminalParser(ToUtf32("DirectNewDeclarator"), ToUtf32("DirectNewDeclarator"), 0)))));
    AddRule(new DirectNewDeclaratorRule(ToUtf32("DirectNewDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('['),
                    new soul::parsing::ActionParser(ToUtf32("A0"),
                        new soul::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
                new soul::parsing::CharParser(']')),
            new soul::parsing::KleeneStarParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::CharParser('['),
                        new soul::parsing::ActionParser(ToUtf32("A1"),
                            new soul::parsing::NonterminalParser(ToUtf32("ConstantExpression"), ToUtf32("ConstantExpression"), 0))),
                    new soul::parsing::CharParser(']'))))));
    AddRule(new NewPlacementRule(ToUtf32("NewPlacement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('('),
                    new soul::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 0)),
                new soul::parsing::CharParser(')')))));
    AddRule(new NewInitializerRule(ToUtf32("NewInitializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::CharParser('('),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 0))),
                new soul::parsing::CharParser(')')))));
    AddRule(new DeleteExpressionRule(ToUtf32("DeleteExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::SequenceParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::OptionalParser(
                            new soul::parsing::ActionParser(ToUtf32("A1"),
                                new soul::parsing::StringParser(ToUtf32("::")))),
                        new soul::parsing::KeywordParser(ToUtf32("delete"))),
                    new soul::parsing::OptionalParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('['),
                            new soul::parsing::ActionParser(ToUtf32("A2"),
                                new soul::parsing::CharParser(']'))))),
                new soul::parsing::NonterminalParser(ToUtf32("CastExpression"), ToUtf32("CastExpression"), 0)))));
    AddRule(new PostfixExpressionRule(ToUtf32("PostfixExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::AlternativeParser(
                new soul::parsing::SequenceParser(
                    new soul::parsing::ActionParser(ToUtf32("A1"),
                        new soul::parsing::NonterminalParser(ToUtf32("PrimaryExpression"), ToUtf32("PrimaryExpression"), 0)),
                    new soul::parsing::KleeneStarParser(
                        new soul::parsing::AlternativeParser(
                            new soul::parsing::AlternativeParser(
                                new soul::parsing::AlternativeParser(
                                    new soul::parsing::AlternativeParser(
                                        new soul::parsing::AlternativeParser(
                                            new soul::parsing::ActionParser(ToUtf32("A2"),
                                                new soul::parsing::SequenceParser(
                                                    new soul::parsing::SequenceParser(
                                                        new soul::parsing::CharParser('['),
                                                        new soul::parsing::ExpectationParser(
                                                            new soul::parsing::NonterminalParser(ToUtf32("index"), ToUtf32("Expression"), 0))),
                                                    new soul::parsing::ExpectationParser(
                                                        new soul::parsing::CharParser(']')))),
                                            new soul::parsing::ActionParser(ToUtf32("A3"),
                                                new soul::parsing::SequenceParser(
                                                    new soul::parsing::SequenceParser(
                                                        new soul::parsing::CharParser('('),
                                                        new soul::parsing::OptionalParser(
                                                            new soul::parsing::NonterminalParser(ToUtf32("invoke"), ToUtf32("ExpressionList"), 0))),
                                                    new soul::parsing::ExpectationParser(
                                                        new soul::parsing::CharParser(')'))))),
                                        new soul::parsing::ActionParser(ToUtf32("A4"),
                                            new soul::parsing::SequenceParser(
                                                new soul::parsing::DifferenceParser(
                                                    new soul::parsing::CharParser('.'),
                                                    new soul::parsing::StringParser(ToUtf32(".*"))),
                                                new soul::parsing::ExpectationParser(
                                                    new soul::parsing::NonterminalParser(ToUtf32("member"), ToUtf32("IdExpression"), 0))))),
                                    new soul::parsing::ActionParser(ToUtf32("A5"),
                                        new soul::parsing::SequenceParser(
                                            new soul::parsing::DifferenceParser(
                                                new soul::parsing::StringParser(ToUtf32("->")),
                                                new soul::parsing::StringParser(ToUtf32("->*"))),
                                            new soul::parsing::ExpectationParser(
                                                new soul::parsing::NonterminalParser(ToUtf32("ptrmember"), ToUtf32("IdExpression"), 0))))),
                                new soul::parsing::ActionParser(ToUtf32("A6"),
                                    new soul::parsing::StringParser(ToUtf32("++")))),
                            new soul::parsing::ActionParser(ToUtf32("A7"),
                                new soul::parsing::StringParser(ToUtf32("--")))))),
                new soul::parsing::ActionParser(ToUtf32("A8"),
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::NonterminalParser(ToUtf32("TypeSpecifierOrTypeName"), ToUtf32("TypeSpecifierOrTypeName"), 0),
                                new soul::parsing::CharParser('(')),
                            new soul::parsing::OptionalParser(
                                new soul::parsing::NonterminalParser(ToUtf32("typeConstruction"), ToUtf32("ExpressionList"), 0))),
                        new soul::parsing::CharParser(')')))))));
    AddRule(new TypeSpecifierOrTypeNameRule(ToUtf32("TypeSpecifierOrTypeName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("SimpleTypeSpecifier"), ToUtf32("SimpleTypeSpecifier"), 0)),
            new soul::parsing::ActionParser(ToUtf32("A1"),
                new soul::parsing::NonterminalParser(ToUtf32("TypeName"), ToUtf32("TypeName"), 0)))));
    AddRule(new PostCastExpressionRule(ToUtf32("PostCastExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::ActionParser(ToUtf32("A0"),
                            new soul::parsing::SequenceParser(
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::SequenceParser(
                                        new soul::parsing::SequenceParser(
                                            new soul::parsing::SequenceParser(
                                                new soul::parsing::SequenceParser(
                                                    new soul::parsing::KeywordParser(ToUtf32("dynamic_cast")),
                                                    new soul::parsing::ExpectationParser(
                                                        new soul::parsing::CharParser('<'))),
                                                new soul::parsing::ExpectationParser(
                                                    new soul::parsing::NonterminalParser(ToUtf32("dct"), ToUtf32("TypeId"), 0))),
                                            new soul::parsing::ExpectationParser(
                                                new soul::parsing::CharParser('>'))),
                                        new soul::parsing::ExpectationParser(
                                            new soul::parsing::CharParser('('))),
                                    new soul::parsing::ExpectationParser(
                                        new soul::parsing::NonterminalParser(ToUtf32("dce"), ToUtf32("Expression"), 0))),
                                new soul::parsing::ExpectationParser(
                                    new soul::parsing::CharParser(')')))),
                        new soul::parsing::ActionParser(ToUtf32("A1"),
                            new soul::parsing::SequenceParser(
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::SequenceParser(
                                        new soul::parsing::SequenceParser(
                                            new soul::parsing::SequenceParser(
                                                new soul::parsing::SequenceParser(
                                                    new soul::parsing::KeywordParser(ToUtf32("static_cast")),
                                                    new soul::parsing::ExpectationParser(
                                                        new soul::parsing::CharParser('<'))),
                                                new soul::parsing::ExpectationParser(
                                                    new soul::parsing::NonterminalParser(ToUtf32("sct"), ToUtf32("TypeId"), 0))),
                                            new soul::parsing::ExpectationParser(
                                                new soul::parsing::CharParser('>'))),
                                        new soul::parsing::ExpectationParser(
                                            new soul::parsing::CharParser('('))),
                                    new soul::parsing::ExpectationParser(
                                        new soul::parsing::NonterminalParser(ToUtf32("sce"), ToUtf32("Expression"), 0))),
                                new soul::parsing::ExpectationParser(
                                    new soul::parsing::CharParser(')'))))),
                    new soul::parsing::ActionParser(ToUtf32("A2"),
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::SequenceParser(
                                        new soul::parsing::SequenceParser(
                                            new soul::parsing::SequenceParser(
                                                new soul::parsing::KeywordParser(ToUtf32("reinterpret_cast")),
                                                new soul::parsing::ExpectationParser(
                                                    new soul::parsing::CharParser('<'))),
                                            new soul::parsing::ExpectationParser(
                                                new soul::parsing::NonterminalParser(ToUtf32("rct"), ToUtf32("TypeId"), 0))),
                                        new soul::parsing::ExpectationParser(
                                            new soul::parsing::CharParser('>'))),
                                    new soul::parsing::ExpectationParser(
                                        new soul::parsing::CharParser('('))),
                                new soul::parsing::ExpectationParser(
                                    new soul::parsing::NonterminalParser(ToUtf32("rce"), ToUtf32("Expression"), 0))),
                            new soul::parsing::ExpectationParser(
                                new soul::parsing::CharParser(')'))))),
                new soul::parsing::ActionParser(ToUtf32("A3"),
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::SequenceParser(
                                    new soul::parsing::SequenceParser(
                                        new soul::parsing::SequenceParser(
                                            new soul::parsing::KeywordParser(ToUtf32("const_cast")),
                                            new soul::parsing::ExpectationParser(
                                                new soul::parsing::CharParser('<'))),
                                        new soul::parsing::ExpectationParser(
                                            new soul::parsing::NonterminalParser(ToUtf32("cct"), ToUtf32("TypeId"), 0))),
                                    new soul::parsing::ExpectationParser(
                                        new soul::parsing::CharParser('>'))),
                                new soul::parsing::ExpectationParser(
                                    new soul::parsing::CharParser('('))),
                            new soul::parsing::ExpectationParser(
                                new soul::parsing::NonterminalParser(ToUtf32("cce"), ToUtf32("Expression"), 0))),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::CharParser(')'))))),
            new soul::parsing::ActionParser(ToUtf32("A4"),
                new soul::parsing::SequenceParser(
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::KeywordParser(ToUtf32("typeid")),
                            new soul::parsing::ExpectationParser(
                                new soul::parsing::CharParser('('))),
                        new soul::parsing::ExpectationParser(
                            new soul::parsing::NonterminalParser(ToUtf32("typeidExpr"), ToUtf32("Expression"), 0))),
                    new soul::parsing::ExpectationParser(
                        new soul::parsing::CharParser(')')))))));
    AddRule(new ExpressionListRule(ToUtf32("ExpressionList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ListParser(
            new soul::parsing::ActionParser(ToUtf32("A0"),
                new soul::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0)),
            new soul::parsing::CharParser(','))));
    AddRule(new PrimaryExpressionRule(ToUtf32("PrimaryExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::ActionParser(ToUtf32("A0"),
            new soul::parsing::AlternativeParser(
                new soul::parsing::AlternativeParser(
                    new soul::parsing::AlternativeParser(
                        new soul::parsing::ActionParser(ToUtf32("A1"),
                            new soul::parsing::NonterminalParser(ToUtf32("Literal"), ToUtf32("Literal"), 0)),
                        new soul::parsing::ActionParser(ToUtf32("A2"),
                            new soul::parsing::KeywordParser(ToUtf32("this")))),
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::CharParser('('),
                            new soul::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0)),
                        new soul::parsing::ActionParser(ToUtf32("A3"),
                            new soul::parsing::ExpectationParser(
                                new soul::parsing::CharParser(')'))))),
                new soul::parsing::ActionParser(ToUtf32("A4"),
                    new soul::parsing::NonterminalParser(ToUtf32("IdExpression"), ToUtf32("IdExpression"), 0))))));
    AddRule(new IdExpressionRule(ToUtf32("IdExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::AlternativeParser(
            new soul::parsing::AlternativeParser(
                new soul::parsing::ActionParser(ToUtf32("A0"),
                    new soul::parsing::TokenParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::NonterminalParser(ToUtf32("id1"), ToUtf32("QualifiedId"), 0),
                                new soul::parsing::StringParser(ToUtf32("::"))),
                            new soul::parsing::NonterminalParser(ToUtf32("OperatorFunctionId"), ToUtf32("OperatorFunctionId"), 0)))),
                new soul::parsing::ActionParser(ToUtf32("A1"),
                    new soul::parsing::TokenParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::OptionalParser(
                                new soul::parsing::StringParser(ToUtf32("::"))),
                            new soul::parsing::NonterminalParser(ToUtf32("OperatorFunctionId"), ToUtf32("OperatorFunctionId"), 0))))),
            new soul::parsing::ActionParser(ToUtf32("A2"),
                new soul::parsing::NonterminalParser(ToUtf32("id2"), ToUtf32("QualifiedId"), 0)))));
    AddRule(new soul::parsing::Rule(ToUtf32("OperatorFunctionId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::SequenceParser(
            new soul::parsing::KeywordParser(ToUtf32("operator")),
            new soul::parsing::NonterminalParser(ToUtf32("Operator"), ToUtf32("Operator"), 0))));
    AddRule(new soul::parsing::Rule(ToUtf32("Operator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                                                                                                                                            new soul::parsing::SequenceParser(
                                                                                                                                                                                new soul::parsing::CharParser('['),
                                                                                                                                                                                new soul::parsing::CharParser(']')),
                                                                                                                                                                            new soul::parsing::SequenceParser(
                                                                                                                                                                                new soul::parsing::CharParser('('),
                                                                                                                                                                                new soul::parsing::CharParser(')'))),
                                                                                                                                                                        new soul::parsing::StringParser(ToUtf32("->*"))),
                                                                                                                                                                    new soul::parsing::StringParser(ToUtf32("->"))),
                                                                                                                                                                new soul::parsing::CharParser(',')),
                                                                                                                                                            new soul::parsing::StringParser(ToUtf32("--"))),
                                                                                                                                                        new soul::parsing::StringParser(ToUtf32("++"))),
                                                                                                                                                    new soul::parsing::StringParser(ToUtf32("||"))),
                                                                                                                                                new soul::parsing::StringParser(ToUtf32("&&"))),
                                                                                                                                            new soul::parsing::StringParser(ToUtf32("<="))),
                                                                                                                                        new soul::parsing::StringParser(ToUtf32(">="))),
                                                                                                                                    new soul::parsing::StringParser(ToUtf32("!="))),
                                                                                                                                new soul::parsing::StringParser(ToUtf32("=="))),
                                                                                                                            new soul::parsing::StringParser(ToUtf32("<<="))),
                                                                                                                        new soul::parsing::StringParser(ToUtf32(">>="))),
                                                                                                                    new soul::parsing::StringParser(ToUtf32("<<"))),
                                                                                                                new soul::parsing::StringParser(ToUtf32(">>"))),
                                                                                                            new soul::parsing::StringParser(ToUtf32("|="))),
                                                                                                        new soul::parsing::StringParser(ToUtf32("&="))),
                                                                                                    new soul::parsing::StringParser(ToUtf32("^="))),
                                                                                                new soul::parsing::StringParser(ToUtf32("%="))),
                                                                                            new soul::parsing::StringParser(ToUtf32("/="))),
                                                                                        new soul::parsing::StringParser(ToUtf32("*="))),
                                                                                    new soul::parsing::StringParser(ToUtf32("-="))),
                                                                                new soul::parsing::StringParser(ToUtf32("+="))),
                                                                            new soul::parsing::CharParser('<')),
                                                                        new soul::parsing::CharParser('>')),
                                                                    new soul::parsing::CharParser('=')),
                                                                new soul::parsing::CharParser('!')),
                                                            new soul::parsing::CharParser('~')),
                                                        new soul::parsing::CharParser('|')),
                                                    new soul::parsing::CharParser('&')),
                                                new soul::parsing::CharParser('^')),
                                            new soul::parsing::CharParser('%')),
                                        new soul::parsing::CharParser('/')),
                                    new soul::parsing::CharParser('*')),
                                new soul::parsing::CharParser('-')),
                            new soul::parsing::CharParser('+')),
                        new soul::parsing::SequenceParser(
                            new soul::parsing::SequenceParser(
                                new soul::parsing::KeywordParser(ToUtf32("new")),
                                new soul::parsing::CharParser('[')),
                            new soul::parsing::CharParser(']'))),
                    new soul::parsing::SequenceParser(
                        new soul::parsing::SequenceParser(
                            new soul::parsing::KeywordParser(ToUtf32("delete")),
                            new soul::parsing::CharParser('[')),
                        new soul::parsing::CharParser(']'))),
                new soul::parsing::KeywordParser(ToUtf32("new"))),
            new soul::parsing::KeywordParser(ToUtf32("delete")))));
}

} } // namespace soul.code
