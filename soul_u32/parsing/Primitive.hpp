// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_PARSING_PRIMITIVE_INCLUDED
#define SOUL_PARSING_PRIMITIVE_INCLUDED
#include <soul_u32/parsing/Parser.hpp>
#include <limits.h>
#include <bitset>

namespace soul { namespace parsing {

class Visitor;
class Rule;

class CharParser : public Parser
{
public:
    CharParser(char32_t c_);
    char32_t GetChar() const { return c; }
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    virtual void Accept(Visitor& visitor);
private:
    char32_t c;
};

class StringParser : public Parser
{
public:
    StringParser(const std::u32string& s_);
    const std::u32string& GetString() const { return s; }
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    virtual void Accept(Visitor& visitor);
private:
    std::u32string s;
};

struct CharRange
{
    CharRange(char32_t start_, char32_t end_) : start(start_), end(end_) {}
    bool Includes(char32_t c) const { return c >= start && c <= end; }
    char32_t start;
    char32_t end;
};

class CharSetParser : public Parser
{
public:
    CharSetParser(const std::u32string& s_);
    CharSetParser(const std::u32string& s_, bool inverse_);
    const std::u32string& Set() const { return s; }
    bool Inverse() const { return inverse; }
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    virtual void Accept(Visitor& visitor);
private:
    std::u32string s;
    bool inverse;
    std::vector<CharRange> ranges;
    void InitRanges();
};

class EmptyParser : public Parser
{
public:
    EmptyParser();
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    virtual void Accept(Visitor& visitor);
};

class SpaceParser : public Parser
{
public:
    SpaceParser();
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    virtual void Accept(Visitor& visitor);
};

class LetterParser : public Parser
{
public:
    LetterParser();
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    virtual void Accept(Visitor& visitor);
};

class DigitParser : public Parser
{
public:
    DigitParser();
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    virtual void Accept(Visitor& visitor);
};

class HexDigitParser : public Parser
{
public:
    HexDigitParser();
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    virtual void Accept(Visitor& visitor);
};

class PunctuationParser : public Parser
{
public:
    PunctuationParser();
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    virtual void Accept(Visitor& visitor);
};

class AnyCharParser : public Parser
{
public:
    AnyCharParser();
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    virtual void Accept(Visitor& visitor);
};

class RangeParser : public Parser
{
public:
    RangeParser(uint32_t start_, uint32_t end_);
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    virtual void Accept(Visitor& visitor);
    uint32_t Start() const { return start; }
    uint32_t End() const { return end; }
private:
    uint32_t start;
    uint32_t end;
};

} } // namespace soul::parsing

#endif // SOUL_PARSING_PRIMITIVE_INCLUDED
