// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/parsing/Parser.hpp>

namespace soul { namespace parsing {

Object::~Object()
{
}

Parser::Parser(const std::u32string& name_, const std::u32string& info_): ParsingObject(name_), info(info_)
{
}

} } // namespace soul::parsing
