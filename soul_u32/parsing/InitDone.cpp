// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/parsing/InitDone.hpp>
#include <soul_u32/parsing/ParsingDomain.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/codedom/InitDone.hpp>

namespace soul { namespace parsing {

void Init()
{
    soul::codedom::Init();
    ParsingDomainInit();
    KeywordInit();
}

void Done()
{
    KeywordDone();
    ParsingDomainDone();
    soul::codedom::Done();
}

} } // namespace soul::parsing

