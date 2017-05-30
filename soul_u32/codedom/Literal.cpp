// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/codedom/Literal.hpp>
#include <soul_u32/codedom/Visitor.hpp>

namespace soul { namespace codedom {

Literal::Literal(const std::u32string& name_): CppObject(name_)
{
}

void Literal::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace cminor::codedom
