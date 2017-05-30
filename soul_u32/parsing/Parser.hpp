// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_PARSING_PARSER_INCLUDED
#define SOUL_PARSING_PARSER_INCLUDED
#include <soul_u32/parsing/Match.hpp>
#include <soul_u32/parsing/ParsingData.hpp>
#include <soul_u32/parsing/ParsingObject.hpp>
#include <stack>

namespace soul { namespace parsing {

class Object
{
public:
    virtual ~Object();
};

typedef std::stack<std::unique_ptr<Object>> ObjectStack;

template<typename ValueType>
class ValueObject: public Object
{
public:
    ValueObject(): value() {}
    ValueObject(const ValueType& value_): value(value_) {}
    ValueObject<ValueType>& operator=(const ValueType& v) { value = v; return *this; }
    operator ValueType() const { return value; }
private:
    ValueType value;
};

class Scanner;

class Parser: public ParsingObject
{
public:
    Parser(const std::u32string& name_, const std::u32string& info_);
    const std::u32string& Info() const { return info; }
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) = 0;
private:
    std::u32string info;
};

} } // namespace soul::parsing

#endif // SOUL_PARSING_PARSER_INCLUDED

