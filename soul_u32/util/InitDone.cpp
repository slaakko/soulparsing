// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/util/InitDone.hpp>
#include <soul_u32/util/Unicode.hpp>

namespace soul { namespace util {

void Init()
{
    soul::unicode::UnicodeInit();
}

void Done()
{
    soul::unicode::UnicodeDone();
}

} } // namespace soul::util

