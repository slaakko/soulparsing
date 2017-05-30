// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/syntax/CodeGeneratorVisitor.hpp>
#include <soul_u32/util/TextUtils.hpp>
#include <soul_u32/util/Unicode.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Rule.hpp>
#include <soul_u32/parsing/Action.hpp>
#include <soul_u32/parsing/Nonterminal.hpp>
#include <soul_u32/parsing/Namespace.hpp>

namespace soul { namespace syntax {

using namespace soul::parsing;
using soul::util::ToUtf8;
using soul::util::ToUtf32;
using soul::util::StringStr;
using soul::util::CharStr;
using soul::util::Replace;

class KeywordListCreator : public soul::parsing::Visitor
{
public:
    KeywordListCreator (CodeFormatter& cppFormatter_, CodeFormatter& hppFormatter_): 
        cppFormatter(cppFormatter_), hppFormatter(hppFormatter_), keywordListNumber(0), keywordRuleListNumber(0)
    {
    }
    virtual void Visit(KeywordListParser& parser)
    {
        std::string keywordVecName = "keywords" + std::to_string(keywordListNumber);
        ++keywordListNumber;
        hppFormatter.WriteLine("std::vector<std::u32string> " + keywordVecName + ";");
        KeywordListParser::KeywordSetIt e = parser.Keywords().end();
        for (KeywordListParser::KeywordSetIt i = parser.Keywords().begin(); i != e; ++i)
        {
            std::string keywordStr = "ToUtf32(\"" + StringStr(ToUtf8(*i)) + "\")";
            cppFormatter.WriteLine(keywordVecName + ".push_back(" + keywordStr + ");");
        }
        parser.KeywordVecName() = ToUtf32(keywordVecName);
    }
private:
    CodeFormatter& cppFormatter;
    CodeFormatter& hppFormatter;
    int keywordListNumber;
    int keywordRuleListNumber;
};

CodeGeneratorVisitor::CodeGeneratorVisitor(CodeFormatter& cppFormatter_, CodeFormatter& hppFormatter_): 
    cppFormatter(cppFormatter_), hppFormatter(hppFormatter_)
{
}

void CodeGeneratorVisitor::BeginVisit(Grammar& grammar)
{
    std::string grammarPtrType = ToUtf8(grammar.Name()) + "*";
    hppFormatter.WriteLine("class " + ToUtf8(grammar.Name()) + " : public soul::parsing::Grammar");
    hppFormatter.WriteLine("{");
    hppFormatter.WriteLine("public:");
    hppFormatter.IncIndent();
    hppFormatter.WriteLine("static " + grammarPtrType + " Create();");
    hppFormatter.WriteLine("static " + grammarPtrType + " Create(soul::parsing::ParsingDomain* parsingDomain);");

    std::string valueTypeName;
    std::string parameters;
    if (grammar.StartRule())
    {
        valueTypeName = !grammar.StartRule()->ValueTypeName().empty() ? ToUtf8(grammar.StartRule()->ValueTypeName()) : "void";
        parameters = "";
        if (grammar.StartRule()->TakesOrReturnsValuesOrHasLocals())
        {
            int n = int(grammar.StartRule()->InheritedAttributes().size());
            for (int i = 0; i < n; ++i)
            {
                const AttrOrVariable& attr = grammar.StartRule()->InheritedAttributes()[i];
                parameters.append(", " + ToUtf8(attr.TypeName()) + " " + ToUtf8(attr.Name()));
            }
            hppFormatter.WriteLine(valueTypeName + " Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName" + parameters + ");");
        }
    }
    hppFormatter.DecIndent();
    hppFormatter.WriteLine("private:");
    hppFormatter.IncIndent();

    cppFormatter.WriteLine(grammarPtrType + " " + ToUtf8(grammar.Name()) + "::Create()");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    cppFormatter.WriteLine("return Create(new soul::parsing::ParsingDomain());");
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();

    cppFormatter.WriteLine(grammarPtrType + " " + ToUtf8(grammar.Name()) + "::Create(soul::parsing::ParsingDomain* parsingDomain)");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    cppFormatter.WriteLine("RegisterParsingDomain(parsingDomain);");
    cppFormatter.WriteLine(grammarPtrType + " grammar(new " + ToUtf8(grammar.Name()) + "(parsingDomain));");
    cppFormatter.WriteLine("parsingDomain->AddGrammar(grammar);");
    cppFormatter.WriteLine("grammar->CreateRules();");
    cppFormatter.WriteLine("grammar->Link();");
    cppFormatter.WriteLine("return grammar;");
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();

    std::string grammarNamespaceName = ToUtf8(grammar.Ns()->FullName());
    cppFormatter.WriteLine(ToUtf8(grammar.Name()) + "::" + ToUtf8(grammar.Name()) + "(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32(\"" + ToUtf8(grammar.Name()) + 
        "\"), parsingDomain_->GetNamespaceScope(ToUtf32(\"" + grammarNamespaceName + "\")), parsingDomain_)");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    cppFormatter.WriteLine("SetOwner(0);");
    KeywordListCreator creator(cppFormatter, hppFormatter);
    int n = int(grammar.Rules().size());
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = grammar.Rules()[i];
        rule->Accept(creator);
    }
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();

    if (grammar.StartRule())
    {
        if (grammar.StartRule()->TakesOrReturnsValuesOrHasLocals())
        {
            cppFormatter.WriteLine(valueTypeName + " " + ToUtf8(grammar.Name()) + 
                "::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName" + parameters + ")");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("soul::parsing::Scanner scanner(start, end, fileName, fileIndex, SkipRule());");
            cppFormatter.WriteLine("std::unique_ptr<soul::parsing::XmlLog> xmlLog;");
            cppFormatter.WriteLine("if (Log())");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("xmlLog.reset(new soul::parsing::XmlLog(*Log(), MaxLogLineLength()));");
            cppFormatter.WriteLine("scanner.SetLog(xmlLog.get());");
            cppFormatter.WriteLine("xmlLog->WriteBeginRule(\"parse\");");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("soul::parsing::ObjectStack stack;");
            cppFormatter.WriteLine("std::unique_ptr<soul::parsing::ParsingData> parsingData(new soul::parsing::ParsingData(GetParsingDomain()->GetNumRules()));");
            cppFormatter.WriteLine("scanner.SetParsingData(parsingData.get());");

            int n = int(grammar.StartRule()->InheritedAttributes().size());
            for (int i = 0; i < n; ++i)
            {
                const AttrOrVariable& attr = grammar.StartRule()->InheritedAttributes()[i];
                cppFormatter.WriteLine("stack.push(std::unique_ptr<soul::parsing::Object>(new ValueObject<" + ToUtf8(attr.TypeName()) + ">(" + ToUtf8(attr.Name()) + ")));");
            }

            cppFormatter.WriteLine("soul::parsing::Match match = soul::parsing::Grammar::Parse(scanner, stack, parsingData.get());");
            cppFormatter.WriteLine("soul::parsing::Span stop = scanner.GetSpan();");
            cppFormatter.WriteLine("if (Log())");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("xmlLog->WriteEndRule(\"parse\");");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("if (!match.Hit() || stop.Start() != int(end - start))");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("if (StartRule())");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("throw soul::parsing::ExpectationFailure(StartRule()->Info(), fileName, stop, start, end);");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("else");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("throw soul::parsing::ParsingException(\"grammar '\" + ToUtf8(Name()) + \"' has no start rule\", fileName, scanner.GetSpan(), start, end);");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            if (valueTypeName != "void")
            {
                cppFormatter.WriteLine("std::unique_ptr<soul::parsing::Object> value = std::move(stack.top());");
                cppFormatter.WriteLine(valueTypeName + " result = *static_cast<soul::parsing::ValueObject<" + valueTypeName + ">*>(value.get());");
                cppFormatter.WriteLine("stack.pop();");
                cppFormatter.WriteLine("return result;");
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.NewLine();
        }
    }
    n = int(grammar.Rules().size());
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = grammar.Rules()[i];
        if (rule->Specialized())
        {
            cppFormatter.WriteLine("class " + ToUtf8(grammar.Name()) + "::" + ToUtf8(rule->SpecializedTypeName()) + " : public soul::parsing::Rule");
            cppFormatter.WriteLine("{");
            cppFormatter.WriteLine("public:");
            cppFormatter.IncIndent();
            // constructor:
            cppFormatter.WriteLine(ToUtf8(rule->SpecializedTypeName()) + "(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("soul::parsing::Rule(name_, enclosingScope_, id_, definition_)");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            int m = int(rule->InheritedAttributes().size());
            for (int i = 0; i < m; ++i)
            {
                const AttrOrVariable& attr = rule->InheritedAttributes()[i];
                cppFormatter.WriteLine("AddInheritedAttribute(AttrOrVariable(ToUtf32(\"" + ToUtf8(attr.TypeName()) + "\"), ToUtf32(\"" + ToUtf8(attr.Name()) + "\")));");
            }
            if (!rule->ValueTypeName().empty())
            {
                cppFormatter.WriteLine("SetValueTypeName(ToUtf32(\"" + ToUtf8(rule->ValueTypeName()) + "\"));");
            }
            m = int(rule->LocalVariables().size());
            for (int i = 0; i < m; ++i)
            {
                const AttrOrVariable& var = rule->LocalVariables()[i];
                cppFormatter.WriteLine("AddLocalVariable(AttrOrVariable(ToUtf32(\"" + ToUtf8(var.TypeName()) + "\"), ToUtf32(\"" + ToUtf8(var.Name()) + "\")));");
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            // enter:
            cppFormatter.WriteLine("virtual void Enter(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData)");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("parsingData->PushContext(Id(), new Context());");
            cppFormatter.WriteLine("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
            m = int(rule->InheritedAttributes().size());
            for (int i = m - 1; i >= 0; --i)
            {
                const AttrOrVariable& attr = rule->InheritedAttributes()[i];
                cppFormatter.WriteLine("std::unique_ptr<soul::parsing::Object> " + ToUtf8(attr.Name()) + "_value = std::move(stack.top());");
                cppFormatter.WriteLine("context->" + ToUtf8(attr.Name()) + " = *static_cast<soul::parsing::ValueObject<" + ToUtf8(attr.TypeName()) + ">*>(" + ToUtf8(attr.Name()) + "_value.get());");
                cppFormatter.WriteLine("stack.pop();");
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            // leave:
            cppFormatter.WriteLine("virtual void Leave(soul::parsing::ObjectStack& stack, soul::parsing::ParsingData* parsingData, bool matched)");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            if (!rule->ValueTypeName().empty())
            {
                cppFormatter.WriteLine("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
                cppFormatter.WriteLine("if (matched)");
                cppFormatter.WriteLine("{");
                cppFormatter.IncIndent();
                cppFormatter.WriteLine("stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<" + ToUtf8(rule->ValueTypeName()) + ">(context->value)));");
                cppFormatter.DecIndent();
                cppFormatter.WriteLine("}");
            }
            cppFormatter.WriteLine("parsingData->PopContext(Id());");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            // link:
            cppFormatter.WriteLine("virtual void Link()");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            m = int(rule->Actions().size());
            for (int i = 0; i < m; ++i)
            {
                ActionParser* action = rule->Actions()[i];
                cppFormatter.WriteLine("soul::parsing::ActionParser* " + ToUtf8(action->VariableName()) + " = GetAction(ToUtf32(\"" + ToUtf8(action->Name()) + "\"));");
                cppFormatter.WriteLine(ToUtf8(action->VariableName()) + "->SetAction(new soul::parsing::MemberParsingAction<" + ToUtf8(rule->SpecializedTypeName()) + ">(this, &" + 
                    ToUtf8(rule->SpecializedTypeName()) + "::" + ToUtf8(action->MethodName()) + "));");
                if (action->FailCode())
                {
                    cppFormatter.WriteLine(ToUtf8(action->VariableName()) + "->SetFailureAction(new soul::parsing::MemberFailureAction<" + ToUtf8(rule->SpecializedTypeName()) + ">(this, &" + 
                        ToUtf8(rule->SpecializedTypeName()) + "::" + ToUtf8(action->MethodName()) + "Fail));");
                }
            }
            m = int(rule->Nonterminals().size());
            for (int i = 0; i < m; ++i)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[i];
                if (nonterminal->Specialized())
                {
                    cppFormatter.WriteLine("soul::parsing::NonterminalParser* " + ToUtf8(nonterminal->VariableName()) + " = GetNonterminal(ToUtf32(\"" + ToUtf8(nonterminal->Name()) + "\"));");
                    if (!nonterminal->Arguments().empty())
                    {
                        cppFormatter.WriteLine(ToUtf8(nonterminal->VariableName()) + "->SetPreCall(new soul::parsing::MemberPreCall<" + ToUtf8(rule->SpecializedTypeName()) + ">(this, &" + 
                            ToUtf8(rule->SpecializedTypeName()) + "::" + ToUtf8(nonterminal->PreCallMethodName()) + "));");
                    }
                    if (!nonterminal->GetRule()->ValueTypeName().empty())
                    {
                        cppFormatter.WriteLine(ToUtf8(nonterminal->VariableName()) + "->SetPostCall(new soul::parsing::MemberPostCall<" + ToUtf8(rule->SpecializedTypeName()) + ">(this, &" + 
                            ToUtf8(rule->SpecializedTypeName()) + "::" + ToUtf8(nonterminal->PostCallMethodName()) + "));");
                    }
                }
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            // action methods:
            m = int(rule->Actions().size());
            for (int i = 0; i < m; ++i)
            {
                ActionParser* action = rule->Actions()[i];
                cppFormatter.WriteLine("void " + ToUtf8(action->MethodName()) +
                    "(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)");
                cppFormatter.SetStart();
                cppFormatter.SetStartText("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
                action->SuccessCode()->Print(cppFormatter);
                if (action->FailCode())
                {
                    cppFormatter.WriteLine("void " + ToUtf8(action->MethodName()) + "Fail(ParsingData* parsingData)");
                    cppFormatter.SetStart();
                    cppFormatter.SetStartText("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
                    action->FailCode()->Print(cppFormatter);
                }
            }

            // pre-post calls:
            m = int(rule->Nonterminals().size());
            for (int i = 0; i < m; ++i)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[i];
                if (nonterminal->Specialized())
                {
                    if (!nonterminal->Arguments().empty())
                    {
                        cppFormatter.WriteLine("void " + ToUtf8(nonterminal->PreCallMethodName()) + "(soul::parsing::ObjectStack& stack, ParsingData* parsingData)");
                        cppFormatter.WriteLine("{");
                        cppFormatter.IncIndent();
                        cppFormatter.WriteLine("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
                        int p = int(nonterminal->Arguments().size());
                        for (int j = 0; j < p; ++j)
                        {
                            soul::codedom::CppObject* argument = nonterminal->Arguments()[j];
                            std::string argumentTypeName = ToUtf8(nonterminal->GetRule()->InheritedAttributes()[j].TypeName());
                            cppFormatter.Write("stack.push(std::unique_ptr<soul::parsing::Object>(new soul::parsing::ValueObject<" + argumentTypeName + ">(");
                            argument->Print(cppFormatter);
                            cppFormatter.WriteLine(")));");
                        }
                        cppFormatter.DecIndent();
                        cppFormatter.WriteLine("}");
                    }
                    if (!nonterminal->GetRule()->ValueTypeName().empty())
                    {
                        cppFormatter.WriteLine("void " + ToUtf8(nonterminal->PostCallMethodName()) + "(soul::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)");
                        cppFormatter.WriteLine("{");
                        cppFormatter.IncIndent();
                        cppFormatter.WriteLine("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
                        cppFormatter.WriteLine("if (matched)");
                        cppFormatter.WriteLine("{");
                        cppFormatter.IncIndent();
                        cppFormatter.WriteLine("std::unique_ptr<soul::parsing::Object> " + ToUtf8(nonterminal->ValueFieldName()) + "_value = std::move(stack.top());");
                        cppFormatter.WriteLine("context->" + ToUtf8(nonterminal->ValueFieldName()) + " = *static_cast<soul::parsing::ValueObject<" + 
                            ToUtf8(nonterminal->GetRule()->ValueTypeName()) + ">*>(" + ToUtf8(nonterminal->ValueFieldName()) + "_value.get());");
                        cppFormatter.WriteLine("stack.pop();");
                        cppFormatter.DecIndent();
                        cppFormatter.WriteLine("}");
                        cppFormatter.DecIndent();
                        cppFormatter.WriteLine("}");
                    }
                }
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("private:");
            cppFormatter.IncIndent();

            // Context:
            cppFormatter.WriteLine("struct Context : soul::parsing::Context");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.Write("Context(): ");
            bool first = true;
            m = int(rule->InheritedAttributes().size());
            for (int j = 0; j < m; ++j)
            {
                if (first) { first = false; } else { cppFormatter.Write(", "); }
                const AttrOrVariable& attr = rule->InheritedAttributes()[j];
                cppFormatter.Write(ToUtf8(attr.Name()) + "()");
            }
            if (!rule->ValueTypeName().empty())
            {
                if (first) { first = false; } else { cppFormatter.Write(", "); }
                cppFormatter.Write("value()");
            }
            m = int(rule->LocalVariables().size());
            for (int j = 0; j < m; ++j)
            {
                if (first) { first = false; } else { cppFormatter.Write(", "); }
                const AttrOrVariable& var = rule->LocalVariables()[j];
                cppFormatter.Write(ToUtf8(var.Name()) + "()");
            }
            m = int(rule->Nonterminals().size());
            for (int j = 0; j < m; ++j)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[j];
                if (!nonterminal->GetRule()->ValueTypeName().empty())
                {
                    if (first) { first = false; } else { cppFormatter.Write(", "); }
                    cppFormatter.Write(ToUtf8(nonterminal->ValueFieldName()) + "()");
                }
            }
            cppFormatter.WriteLine(" {}");
            m = int(rule->InheritedAttributes().size());
            for (int j = 0; j < m; ++j)
            {
                const AttrOrVariable& attr = rule->InheritedAttributes()[j];
                cppFormatter.WriteLine(ToUtf8(attr.TypeName()) + " " + ToUtf8(attr.Name()) + ";");
            }
            if (!rule->ValueTypeName().empty())
            {
                cppFormatter.WriteLine(ToUtf8(rule->ValueTypeName()) + " value;");
            }
            m = int(rule->LocalVariables().size());
            for (int j = 0; j < m; ++j)
            {
                const AttrOrVariable& var = rule->LocalVariables()[j];
                cppFormatter.WriteLine(ToUtf8(var.TypeName()) + " " + ToUtf8(var.Name()) + ";");
            }
            m = int(rule->Nonterminals().size());
            for (int j = 0; j < m; ++j)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[j];
                if (!nonterminal->GetRule()->ValueTypeName().empty())
                {
                    cppFormatter.WriteLine(ToUtf8(nonterminal->GetRule()->ValueTypeName()) + " " + ToUtf8(nonterminal->ValueFieldName()) + ";");
                }
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("};");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("};");
            cppFormatter.NewLine();
        }
    }

    cppFormatter.WriteLine("void " + ToUtf8(grammar.Name()) + "::GetReferencedGrammars()");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    int grammarIndex = 0;
    if (!grammar.GrammarReferences().empty())
    {
        cppFormatter.WriteLine("soul::parsing::ParsingDomain* pd = GetParsingDomain();");
        GrammarSetIt e = grammar.GrammarReferences().cend();
        for (GrammarSetIt i = grammar.GrammarReferences().cbegin(); i != e; ++i)
        {
            Grammar* grammarReference = *i;
            std::string grammarReferenceName = ToUtf8(grammarReference->FullName());
            std::string grammarVar = "grammar" + std::to_string(grammarIndex);
            ++grammarIndex;
            cppFormatter.WriteLine("soul::parsing::Grammar* " + grammarVar + " = pd->GetGrammar(ToUtf32(\"" + grammarReferenceName + "\"));");
            cppFormatter.WriteLine("if (!" + grammarVar + ")");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine(grammarVar + " = " + Replace(grammarReferenceName, ".", "::") + "::Create(pd);");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("AddGrammarReference(" + grammarVar + ");");
        }
    }
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();
    cppFormatter.WriteLine("void " + ToUtf8(grammar.Name()) + "::CreateRules()");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
}

void CodeGeneratorVisitor::EndVisit(Grammar& grammar)
{
    if (!grammar.StartRuleName().empty())
    {
        cppFormatter.WriteLine("SetStartRuleName(ToUtf32(\"" + ToUtf8(grammar.StartRuleName()) + "\"));");
    }
    if (!grammar.SkipRuleName().empty())
    {
        cppFormatter.WriteLine("SetSkipRuleName(ToUtf32(\"" + ToUtf8(grammar.SkipRuleName()) + "\"));");
    }
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.WriteLine();

    hppFormatter.WriteLine(ToUtf8(grammar.Name()) + "(soul::parsing::ParsingDomain* parsingDomain_);");
    hppFormatter.WriteLine("virtual void CreateRules();");
    hppFormatter.WriteLine("virtual void GetReferencedGrammars();");

    int n = int(grammar.Rules().size());
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = grammar.Rules()[i];
        if (rule->Specialized())
        {
            hppFormatter.WriteLine("class " + ToUtf8(rule->SpecializedTypeName()) + ";");
        }
    }

    hppFormatter.DecIndent();
    hppFormatter.WriteLine("};");
    hppFormatter.WriteLine();
}

void CodeGeneratorVisitor::Visit(CharParser& parser)
{
    cppFormatter.Write("new soul::parsing::CharParser('");
    cppFormatter.Write(ToUtf8(CharStr(parser.GetChar())));
    cppFormatter.Write("')");
}

void CodeGeneratorVisitor::Visit(StringParser& parser)
{
    cppFormatter.Write("new soul::parsing::StringParser(ToUtf32(\"" + ToUtf8(StringStr(parser.GetString())) + "\"))");
}

void CodeGeneratorVisitor::Visit(CharSetParser& parser)
{
    cppFormatter.Write("new soul::parsing::CharSetParser(ToUtf32(\"" + ToUtf8(StringStr(parser.Set())) + "\")");
    if (parser.Inverse())
    {
        cppFormatter.Write(", true)");
    }
    else
    {
        cppFormatter.Write(")");
    }
}

void CodeGeneratorVisitor::Visit(KeywordParser& parser)
{
    if (parser.ContinuationRuleName().empty())
    {
        cppFormatter.Write("new soul::parsing::KeywordParser(ToUtf32(\"" + ToUtf8(StringStr(parser.Keyword())) + "\"))");
    }
    else
    {
        cppFormatter.Write("new soul::parsing::KeywordParser(ToUtf32(\"" + ToUtf8(StringStr(parser.Keyword())) + "\"), ToUtf32(\"" + ToUtf8(parser.ContinuationRuleName()) + "\"))");
    }
}

void CodeGeneratorVisitor::Visit(KeywordListParser& parser)
{
    cppFormatter.Write("new soul::parsing::KeywordListParser(ToUtf32(\"" + ToUtf8(StringStr(parser.SelectorRuleName())) + "\"), " + ToUtf8(parser.KeywordVecName()) + ")");
}

void CodeGeneratorVisitor::Visit(EmptyParser& parser)
{
    cppFormatter.Write("new soul::parsing::EmptyParser()");
}

void CodeGeneratorVisitor::Visit(SpaceParser& parser)
{
    cppFormatter.Write("new soul::parsing::SpaceParser()");
}

void CodeGeneratorVisitor::Visit(LetterParser& parser)
{
    cppFormatter.Write("new soul::parsing::LetterParser()");
}

void CodeGeneratorVisitor::Visit(DigitParser& parser)
{
    cppFormatter.Write("new soul::parsing::DigitParser()");
}

void CodeGeneratorVisitor::Visit(HexDigitParser& parser)
{
    cppFormatter.Write("new soul::parsing::HexDigitParser()");
}

void CodeGeneratorVisitor::Visit(PunctuationParser& parser)
{
    cppFormatter.Write("new soul::parsing::PunctuationParser()");
}

void CodeGeneratorVisitor::Visit(AnyCharParser& parser)
{
    cppFormatter.Write("new soul::parsing::AnyCharParser()");
}

void CodeGeneratorVisitor::Visit(RangeParser& parser)
{
    cppFormatter.Write("new soul::parsing::RangeParser(" + std::to_string(parser.Start()) + ", " + std::to_string(parser.End()) + ")");
}

void CodeGeneratorVisitor::Visit(NonterminalParser& parser)
{
    cppFormatter.Write("new soul::parsing::NonterminalParser(ToUtf32(\"" + ToUtf8(parser.Name()) + "\"), ToUtf32(\"" + ToUtf8(parser.RuleName()) + "\"), " + 
        std::to_string(parser.NumberOfArguments()) + ")");
}

void CodeGeneratorVisitor::Visit(RuleLink& link)
{
    cppFormatter.WriteLine("AddRuleLink(new soul::parsing::RuleLink(ToUtf32(\"" + ToUtf8(link.Name()) + "\"), this, ToUtf32(\"" + ToUtf8(link.LinkedRuleName()) + "\")));");
}

void CodeGeneratorVisitor::BeginVisit(Rule& rule)
{
    if (rule.Specialized())
    {
        cppFormatter.Write("AddRule(new " + ToUtf8(rule.SpecializedTypeName()) + "(ToUtf32(\"" + ToUtf8(rule.Name()) + "\"), GetScope(), GetParsingDomain()->GetNextRuleId(),");
    }
    else
    {
        cppFormatter.Write("AddRule(new soul::parsing::Rule(ToUtf32(\"" + ToUtf8(rule.Name()) + "\"), GetScope(), GetParsingDomain()->GetNextRuleId(),");
    }
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(Rule& rule)
{
    cppFormatter.Write("));");
    cppFormatter.DecIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::BeginVisit(OptionalParser& parser)
{
    cppFormatter.Write("new soul::parsing::OptionalParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(OptionalParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(PositiveParser& parser)
{
    cppFormatter.Write("new soul::parsing::PositiveParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(PositiveParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(KleeneStarParser& parser)
{
    cppFormatter.Write("new soul::parsing::KleeneStarParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(KleeneStarParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ActionParser& parser)
{
    cppFormatter.Write("new soul::parsing::ActionParser(ToUtf32(\"" + ToUtf8(parser.Name()) + "\"),");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(ActionParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ExpectationParser& parser)
{
    cppFormatter.Write("new soul::parsing::ExpectationParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(ExpectationParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(TokenParser& parser)
{
    cppFormatter.Write("new soul::parsing::TokenParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(TokenParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(SequenceParser& parser)
{
    cppFormatter.Write("new soul::parsing::SequenceParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(SequenceParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(SequenceParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(AlternativeParser& parser)
{
    cppFormatter.Write("new soul::parsing::AlternativeParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(AlternativeParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(AlternativeParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(DifferenceParser& parser)
{
    cppFormatter.Write("new soul::parsing::DifferenceParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(DifferenceParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(DifferenceParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(IntersectionParser& parser)
{
    cppFormatter.Write("new soul::parsing::IntersectionParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(IntersectionParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(IntersectionParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ExclusiveOrParser& parser)
{
    cppFormatter.Write("new soul::parsing::ExclusiveOrParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(ExclusiveOrParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(ExclusiveOrParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ListParser& parser)
{
    cppFormatter.Write("new soul::parsing::ListParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(ListParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(ListParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

} } // namespace soul::syntax