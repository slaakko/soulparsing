// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/parsing/ParsingData.hpp>

namespace soul { namespace parsing {

Context::~Context()
{
}

ParsingData::ParsingData(int numRules)
{
    ruleData.resize(numRules);
}

} } // namespace soul::parsing
