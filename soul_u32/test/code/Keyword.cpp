#include "Keyword.hpp"
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

using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;

KeywordGrammar* KeywordGrammar::Create()
{
    return Create(new soul::parsing::ParsingDomain());
}

KeywordGrammar* KeywordGrammar::Create(soul::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    KeywordGrammar* grammar(new KeywordGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

KeywordGrammar::KeywordGrammar(soul::parsing::ParsingDomain* parsingDomain_): soul::parsing::Grammar(ToUtf32("KeywordGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("soul.code")), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back(ToUtf32("alignas"));
    keywords0.push_back(ToUtf32("alignof"));
    keywords0.push_back(ToUtf32("asm"));
    keywords0.push_back(ToUtf32("auto"));
    keywords0.push_back(ToUtf32("bool"));
    keywords0.push_back(ToUtf32("break"));
    keywords0.push_back(ToUtf32("case"));
    keywords0.push_back(ToUtf32("catch"));
    keywords0.push_back(ToUtf32("char"));
    keywords0.push_back(ToUtf32("class"));
    keywords0.push_back(ToUtf32("const"));
    keywords0.push_back(ToUtf32("const_cast"));
    keywords0.push_back(ToUtf32("constexpr"));
    keywords0.push_back(ToUtf32("continue"));
    keywords0.push_back(ToUtf32("decltype"));
    keywords0.push_back(ToUtf32("default"));
    keywords0.push_back(ToUtf32("delete"));
    keywords0.push_back(ToUtf32("do"));
    keywords0.push_back(ToUtf32("double"));
    keywords0.push_back(ToUtf32("dynamic_cast"));
    keywords0.push_back(ToUtf32("else"));
    keywords0.push_back(ToUtf32("enum"));
    keywords0.push_back(ToUtf32("explicit"));
    keywords0.push_back(ToUtf32("export"));
    keywords0.push_back(ToUtf32("extern"));
    keywords0.push_back(ToUtf32("false"));
    keywords0.push_back(ToUtf32("float"));
    keywords0.push_back(ToUtf32("for"));
    keywords0.push_back(ToUtf32("friend"));
    keywords0.push_back(ToUtf32("goto"));
    keywords0.push_back(ToUtf32("if"));
    keywords0.push_back(ToUtf32("inline"));
    keywords0.push_back(ToUtf32("int"));
    keywords0.push_back(ToUtf32("long"));
    keywords0.push_back(ToUtf32("mutable"));
    keywords0.push_back(ToUtf32("namespace"));
    keywords0.push_back(ToUtf32("new"));
    keywords0.push_back(ToUtf32("noexcept"));
    keywords0.push_back(ToUtf32("nullptr"));
    keywords0.push_back(ToUtf32("operator"));
    keywords0.push_back(ToUtf32("private"));
    keywords0.push_back(ToUtf32("protected"));
    keywords0.push_back(ToUtf32("public"));
    keywords0.push_back(ToUtf32("register"));
    keywords0.push_back(ToUtf32("reinterpret_cast"));
    keywords0.push_back(ToUtf32("return"));
    keywords0.push_back(ToUtf32("short"));
    keywords0.push_back(ToUtf32("signed"));
    keywords0.push_back(ToUtf32("sizeof"));
    keywords0.push_back(ToUtf32("static"));
    keywords0.push_back(ToUtf32("static_assert"));
    keywords0.push_back(ToUtf32("static_cast"));
    keywords0.push_back(ToUtf32("struct"));
    keywords0.push_back(ToUtf32("switch"));
    keywords0.push_back(ToUtf32("template"));
    keywords0.push_back(ToUtf32("this"));
    keywords0.push_back(ToUtf32("thread_local"));
    keywords0.push_back(ToUtf32("throw"));
    keywords0.push_back(ToUtf32("true"));
    keywords0.push_back(ToUtf32("try"));
    keywords0.push_back(ToUtf32("typedef"));
    keywords0.push_back(ToUtf32("typeid"));
    keywords0.push_back(ToUtf32("typename"));
    keywords0.push_back(ToUtf32("union"));
    keywords0.push_back(ToUtf32("unsigned"));
    keywords0.push_back(ToUtf32("using"));
    keywords0.push_back(ToUtf32("virtual"));
    keywords0.push_back(ToUtf32("void"));
    keywords0.push_back(ToUtf32("volatile"));
    keywords0.push_back(ToUtf32("wchar_t"));
    keywords0.push_back(ToUtf32("while"));
}

void KeywordGrammar::GetReferencedGrammars()
{
    soul::parsing::ParsingDomain* pd = GetParsingDomain();
    soul::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("soul.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = soul::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void KeywordGrammar::CreateRules()
{
    AddRuleLink(new soul::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("soul.parsing.stdlib.identifier")));
    AddRule(new soul::parsing::Rule(ToUtf32("Keyword"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new soul::parsing::KeywordListParser(ToUtf32("identifier"), keywords0)));
}

} } // namespace soul.code
