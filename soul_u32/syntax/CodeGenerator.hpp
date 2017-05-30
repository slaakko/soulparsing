// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_SYNTAX_CODEGENERATOR_INCLUDED
#define SOUL_SYNTAX_CODEGENERATOR_INCLUDED

#include <soul_u32/syntax/ParserFileContent.hpp>

namespace soul { namespace syntax {

void SetForceCodeGen(bool force);
void GenerateCode(const std::vector<std::unique_ptr<ParserFileContent>>& parserFiles);

} } // namespace soul::syntax

#endif // SOUL_SYNTAX_CODEGENERATOR_INCLUDED
