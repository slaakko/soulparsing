// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_SYNTAX_CODEGENERATOR_VISITOR_INCLUDED
#define SOUL_SYNTAX_CODEGENERATOR_VISITOR_INCLUDED

#include <soul_u32/parsing/Primitive.hpp>
#include <soul_u32/parsing/Composite.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/parsing/Visitor.hpp>
#include <soul_u32/util/CodeFormatter.hpp>

namespace soul { namespace syntax {

using soul::util::CodeFormatter;
using namespace soul::parsing;

class CodeGeneratorVisitor : public soul::parsing::Visitor
{
public:
    CodeGeneratorVisitor(CodeFormatter& cppFormatter_, CodeFormatter& hppFormatter_);
    virtual void BeginVisit(Grammar& grammar);
    virtual void EndVisit(Grammar& grammar);
    virtual void Visit(CharParser& parser);
    virtual void Visit(StringParser& parser);
    virtual void Visit(CharSetParser& parser);
    virtual void Visit(KeywordParser& parser);
    virtual void Visit(KeywordListParser& parser);
    virtual void Visit(EmptyParser& parser);
    virtual void Visit(SpaceParser& parser);
    virtual void Visit(LetterParser& parser);
    virtual void Visit(DigitParser& parser);
    virtual void Visit(HexDigitParser& parser);
    virtual void Visit(PunctuationParser& parser);
    virtual void Visit(AnyCharParser& parser);
    virtual void Visit(RangeParser& parser);
    virtual void Visit(NonterminalParser& parser);
    virtual void Visit(RuleLink& link);
    virtual void BeginVisit(Rule& parser);
    virtual void EndVisit(Rule& parser);
    virtual void BeginVisit(OptionalParser& parser);
    virtual void EndVisit(OptionalParser& parser);
    virtual void BeginVisit(PositiveParser& parser);
    virtual void EndVisit(PositiveParser& parser);
    virtual void BeginVisit(KleeneStarParser& parser);
    virtual void EndVisit(KleeneStarParser& parser);
    virtual void BeginVisit(ActionParser& parser);
    virtual void EndVisit(ActionParser& parser);
    virtual void BeginVisit(ExpectationParser& parser);
    virtual void EndVisit(ExpectationParser& parser);
    virtual void BeginVisit(TokenParser& parser);
    virtual void EndVisit(TokenParser& parser);
    virtual void BeginVisit(SequenceParser& parser);
    virtual void Visit(SequenceParser& parser);
    virtual void EndVisit(SequenceParser& parser);
    virtual void BeginVisit(AlternativeParser& parser);
    virtual void Visit(AlternativeParser& parser);
    virtual void EndVisit(AlternativeParser& parser);
    virtual void BeginVisit(DifferenceParser& parser);
    virtual void Visit(DifferenceParser& parser);
    virtual void EndVisit(DifferenceParser& parser);
    virtual void BeginVisit(IntersectionParser& parser);
    virtual void Visit(IntersectionParser& parser);
    virtual void EndVisit(IntersectionParser& parser);
    virtual void BeginVisit(ExclusiveOrParser& parser);
    virtual void Visit(ExclusiveOrParser& parser);
    virtual void EndVisit(ExclusiveOrParser& parser);
    virtual void BeginVisit(ListParser& parser);
    virtual void Visit(ListParser& parser);
    virtual void EndVisit(ListParser& parser);
private:
    CodeFormatter& cppFormatter;
    CodeFormatter& hppFormatter;
    std::string CharSetStr();
};

} } // namespace soul::syntax

#endif // SOUL_SYNTAX_CODEGENERATOR_VISITOR_INCLUDED
