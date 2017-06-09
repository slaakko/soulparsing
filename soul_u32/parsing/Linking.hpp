// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_PARSING_LINKING_INCLUDED
#define SOUL_PARSING_LINKING_INCLUDED
#include <soul_u32/parsing/Visitor.hpp>

namespace soul { namespace parsing {

class Grammar;
class Rule;
class ParsingDomain;

class LinkerVisitor : public Visitor
{
public:
    LinkerVisitor();
    void BeginVisit(Grammar& grammar) override;
    void EndVisit(Grammar& grammar) override;
    void Visit(RuleLink& link) override;
    void BeginVisit(Rule& rule) override;
    void EndVisit(Rule& rule) override;
    void BeginVisit(ActionParser& parser) override;
    void Visit(NonterminalParser& parser) override;
    void Visit(KeywordParser& parser) override;
    void Visit(KeywordListParser& parser) override;
private:
    Grammar* currentGrammar;
    Rule* currentRule;
    int actionNumber;
};

void Link(ParsingDomain* parsingDomain);
void ExpandCode(ParsingDomain* parsingDomain);

} } // namespace soul::parsing

#endif // SOUL_PARSING_LINKING_INCLUDED
