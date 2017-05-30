// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_CODEDOM_OBJECT_INCLUDED
#define SOUL_CODEDOM_OBJECT_INCLUDED
#include <soul_u32/util/CodeFormatter.hpp>
#include <string>
#include <vector>
#include <memory>

namespace soul { namespace codedom {

using soul::util::CodeFormatter;
class Visitor;

class CppObject
{
public:
    CppObject(const std::u32string& name_);
    virtual ~CppObject();
    const std::u32string& Name() const { return name; }
    void SetName(const std::u32string& newName) { name = newName; }
    void Own(CppObject* object);
    bool IsOwned() const { return isOwned; }
    void SetOwned() { isOwned = true; }
    void ResetOwned() { isOwned = false; }
    virtual std::u32string ToString() const;
    virtual void Print(CodeFormatter& formatter);
    virtual void Accept(Visitor& visitor) = 0;
    virtual int Rank() const { return 100; }
private:
    std::u32string name;
    bool isOwned;
    std::vector<std::unique_ptr<CppObject>> ownedObjects;
};

} } // namespace cminor::codedom

#endif // SOUL_CODEDOM_OBJECT_INCLUDED
