// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/parsing/Namespace.hpp>
#include <soul_u32/parsing/Scope.hpp>
#include <soul_u32/parsing/Visitor.hpp>

namespace soul { namespace parsing {

Namespace::Namespace(const std::u32string& name_, Scope* enclosingScope_): ParsingObject(name_, enclosingScope_)
{
    SetScope(new Scope(Name(), EnclosingScope())); 
}

void Namespace::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    GetScope()->Accept(visitor);
    visitor.EndVisit(*this);
}

UsingObject::UsingObject(soul::codedom::UsingObject* subject_, Scope* enclosingScope_): ParsingObject(subject_->Name(), enclosingScope_), subject(subject_)
{
    if (!subject->IsOwned())
    {
        subject->SetOwned();
        ownedSubject.reset(subject);
    }
}

void UsingObject::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace soul::parsing
