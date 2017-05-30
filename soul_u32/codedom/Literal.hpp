// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_CODEDOM_LITERAL_INCLUDED
#define SOUL_CODEDOM_LITERAL_INCLUDED
#include <soul_u32/codedom/Object.hpp>

namespace soul { namespace codedom {

class Literal: public CppObject
{
public:
    Literal(const std::u32string& name_);
    virtual int Rank() const { return 24; }
    virtual void Accept(Visitor& visitor);
};

} } // namespace cminor::codedom

#endif // SOUL_CODEDOM_LITERAL_INCLUDED

