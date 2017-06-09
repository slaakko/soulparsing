// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/codedom/Object.hpp>
#include <soul_u32/util/Unicode.hpp>

namespace soul { namespace codedom {

using namespace soul::util;
using namespace soul::unicode;

CppObject::CppObject(const std::u32string& name_): name(name_), isOwned(false)
{
}

CppObject::~CppObject()
{
}

void CppObject::Own(CppObject* object)
{
    if (object)
    {
        if (!object->IsOwned())
        {
            object->SetOwned();
            ownedObjects.push_back(std::unique_ptr<CppObject>(object));
        }
    }
}

std::u32string CppObject::ToString() const
{
    return name;
}

void CppObject::Print(CodeFormatter& formatter)
{
    formatter.Write(ToUtf8(name));
}

} } // namespace cminor::codedom
