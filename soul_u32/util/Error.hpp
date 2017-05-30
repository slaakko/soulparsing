// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_UTIL_ERROR_INCLUDED
#define SOUL_UTIL_ERROR_INCLUDED
#include <stdexcept>

namespace soul { namespace util {

#ifdef NDEBUG

#define Assert(expression, message) ((void)0)

#else

#define Assert(expression, message) if (!(expression)) throw std::runtime_error(std::string("assertion failed: ") + message)

#endif

} } // namespace soul::util

#endif // SOUL_UTIL_ERROR_INCLUDED
