// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_UTIL_UNICODE_INCLUDED
#define SOUL_UTIL_UNICODE_INCLUDED
#include <string>
#include <stdint.h>

namespace soul { namespace util {

std::u32string ToUtf32(const std::string& utf8Str);
std::u32string ToUtf32(const std::u16string& utf16Str);
std::u16string ToUtf16(const std::u32string& utf32Str);
std::u16string ToUtf16(const std::string& utf8Str);
std::string ToUtf8(const std::u32string& utf32Str);
std::string ToUtf8(const std::u16string& utf16Str);

void UnicodeInit();
void UnicodeDone();

} } // namespace soul::util

#endif // SOUL_UTIL_UNICODE_INCLUDED
