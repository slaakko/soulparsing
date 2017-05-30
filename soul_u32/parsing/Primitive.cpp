// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/parsing/Primitive.hpp>
#include <soul_u32/parsing/Scanner.hpp>
#include <soul_u32/parsing/Visitor.hpp>
#include <soul_u32/parsing/Rule.hpp>
#include <soul_u32/util/Unicode.hpp>
#include <cctype>

namespace soul { namespace parsing {

using namespace soul::util;

CharParser::CharParser(char32_t c_): Parser(U"char", U"\"" + std::u32string(1, c_) + U"\""), c(c_)
{
}

Match CharParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (scanner.GetChar() == c)
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void CharParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

StringParser::StringParser(const std::u32string& s_): Parser(U"string", U"\"" + s_ + U"\""), s(s_) 
{
}

Match StringParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Match match = Match::Empty();
    int i = 0;
    int n = int(s.length());
    while (i < n && !scanner.AtEnd() && scanner.GetChar() == s[i])
    {
        ++scanner;
        ++i;
        match.Concatenate(Match::One());
    }
    if (i == n)
    {
        return match;
    }
    return Match::Nothing();
}

void StringParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

CharSetParser::CharSetParser(const std::u32string& s_): Parser(U"charSet", U"[" + s_ + U"]"), s(s_), inverse(false) 
{ 
    InitRanges(); 
}

CharSetParser::CharSetParser(const std::u32string& s_, bool inverse_): Parser(U"charSet", U"[" + s_ + U"]"), s(s_), inverse(inverse_) 
{ 
    InitRanges();
}

void CharSetParser::InitRanges()
{
    int i = 0;
    int n = int(s.length());
    while (i < n)
    {
        char32_t first = s[i];
        char32_t last = first;
        ++i;
        if (i < n)
        {
            if (s[i] == '-')
            {
                ++i;
                if (i < n)
                {
                    last = s[i];
                    ++i;
                }
                else
                {
                    ranges.push_back(CharRange(first, first));
                    first = '-';
                    last = '-';
                }
            }
        }
        ranges.push_back(CharRange(first, last));
    }
}

Match CharSetParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        char32_t c = scanner.GetChar();
        bool found = false;
        for (const CharRange& range : ranges)
        {
            if (range.Includes(c))
            {
                found = true;
                break;
            }
        }
        if (found != inverse)
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void CharSetParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

EmptyParser::EmptyParser(): Parser(U"empty", U"") 
{
}

Match EmptyParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    return Match::Empty();
}

void EmptyParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SpaceParser::SpaceParser(): Parser(U"space", U"space") 
{
}

Match SpaceParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        unsigned char c = (unsigned char)scanner.GetChar();
        if (std::isspace(c))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void SpaceParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LetterParser::LetterParser(): Parser(U"letter", U"letter") 
{
}

Match LetterParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        unsigned char c = (unsigned char)scanner.GetChar();
        if (std::isalpha(c))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void LetterParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DigitParser::DigitParser(): Parser(U"digit", U"digit") 
{
}

Match DigitParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        unsigned char c = (unsigned char)scanner.GetChar();
        if (std::isdigit(c))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void DigitParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

HexDigitParser::HexDigitParser(): Parser(U"hexdigit", U"hexdigit") 
{
}

Match HexDigitParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        int c = scanner.GetChar();
        if (std::isxdigit(c))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void HexDigitParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

PunctuationParser::PunctuationParser(): Parser(U"punctuation", U"punctuation") 
{
}

Match PunctuationParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        int c = scanner.GetChar();
        if (std::ispunct(c))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void PunctuationParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

AnyCharParser::AnyCharParser(): Parser(U"anychar", U"anychar") 
{
}

Match AnyCharParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        ++scanner;
        return Match::One();
    }
    return Match::Nothing();
}

void AnyCharParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

RangeParser::RangeParser(uint32_t start_, uint32_t end_): Parser(U"range", U"range(" + ToUtf32(std::to_string(start_)) + U"," + ToUtf32(std::to_string(end_)) + U")"), start(start_), end(end_)
{
}

Match RangeParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        char32_t c = scanner.GetChar();
        if (c >= start && c <= end)
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void RangeParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace soul::parsing
