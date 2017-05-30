// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_SYNTAX_COMPILER_INCLUDED
#define SOUL_SYNTAX_COMPILER_INCLUDED

#include <string>
#include <vector>

namespace soul { namespace syntax {

void Generate(const std::string& projectFilePath, const std::vector<std::string>& libraryDirectories);

} } // namespace soul::syntax

#endif // SOUL_SYNTAX_COMPILER_INCLUDED

