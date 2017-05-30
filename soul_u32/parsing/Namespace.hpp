// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_PARSING_NAMESPACE_INCLUDED
#define SOUL_PARSING_NAMESPACE_INCLUDED
#include <soul_u32/parsing/ParsingObject.hpp>
#include <soul_u32/codedom/Declaration.hpp>

namespace soul { namespace parsing {

class Namespace: public ParsingObject
{
public:
    Namespace(const std::u32string& name_, Scope* enclosingScope_);
    virtual void Accept(Visitor& visitor);
    virtual bool IsNamespace() const { return true; }
};

class Scope;

class UsingObject: public ParsingObject
{
public:
    UsingObject(soul::codedom::UsingObject* subject_, Scope* enclosingScope_);
    virtual void Accept(Visitor& visitor);
    soul::codedom::UsingObject* Subject() const { return subject; }
private:
    soul::codedom::UsingObject* subject;
    std::unique_ptr<soul::codedom::UsingObject> ownedSubject;
};

} } // namespace soul::parsing

#endif // SOUL_PARSING_NAMESPACE_INCLUDED
