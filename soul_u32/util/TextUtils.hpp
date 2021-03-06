// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_UTIL_TEXTUTILS_INCLUDED
#define SOUL_UTIL_TEXTUTILS_INCLUDED
#include <string>
#include <vector>
#include <stdint.h>

namespace soul { namespace util {

std::string Trim(const std::string& s);
std::string TrimAll(const std::string& s);
std::u32string Trim(const std::u32string& s);
std::u32string TrimAll(const std::u32string& s);
std::vector<std::string> Split(const std::string& s, char c);
std::string Replace(const std::string& s, char oldChar, char newChar);
std::string Replace(const std::string& s, const std::string& oldString, const std::string& newString);
std::string HexEscape(char c);
std::u32string HexEscape(uint32_t c);
std::string CharStr(char c);
std::u32string CharStr(char32_t c);
std::string StringStr(const std::string& s);
std::u32string StringStr(const std::u32string& s);
std::string QuotedPath(const std::string& path);
bool LastComponentsEqual(const std::string& s0, const std::string& s1, char componentSeparator);
bool StartsWith(const std::string& s, const std::string& prefix);
bool EndsWith(const std::string& s, const std::string& suffix);
std::string NarrowString(const char* str, int length);
std::string ToUpper(const std::string& s);
std::string ToLower(const std::string& s);
std::string ToString(double x);
std::string ToString(double x, int maxNumDecimals);
std::string ToString(double x, int minNumDecimals, int maxNumDecimals);
std::string ToHexString(uint8_t x);
std::string ToHexString(uint16_t x);
std::string ToHexString(uint32_t x);
std::string ToHexString(uint64_t x);

} } // namespace soul::util

#endif // SOUL_UTIL_TEXTUTILS_INCLUDED
