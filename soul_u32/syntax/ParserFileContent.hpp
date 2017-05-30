// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_SYNTAX_PARSERFILECONTENT_INCLUDED
#define SOUL_SYNTAX_PARSERFILECONTENT_INCLUDED

#include <soul_u32/parsing/ParsingDomain.hpp>
#include <soul_u32/parsing/Namespace.hpp>

namespace soul { namespace syntax {

class ParserFileContent
{
public:
    ParserFileContent(int id_, soul::parsing::ParsingDomain* parsingDomain_);
    void SetFilePath(const std::string& filePath_) { filePath = filePath_; }
    void AddIncludeDirective(const std::string& includeDirective_, const std::string& fileAttribute_);
    void AddUsingObject(soul::codedom::UsingObject* usingObjectSubject);
    void AddGrammar(soul::parsing::Grammar* grammar);
    void BeginNamespace(const std::u32string& ns);
    void EndNamespace();
    int Id() const { return id; }
    const std::string& FilePath() const { return filePath; }
    soul::parsing::ParsingDomain* ParsingDomain() const { return parsingDomain; }
    const std::vector<std::pair<std::string, std::string>>& IncludeDirectives() const { return includeDirectives; }
    const std::vector<std::unique_ptr<soul::parsing::UsingObject>>& UsingObjects() const { return usingObjects; }
    soul::parsing::Scope* CurrentScope() const { return parsingDomain->CurrentScope(); }
private:
    int id;
    std::string filePath;
    soul::parsing::ParsingDomain* parsingDomain;
    std::vector<std::pair<std::string, std::string>> includeDirectives;
    std::vector<std::unique_ptr<soul::parsing::UsingObject>> usingObjects;
};

} } // namespace soul::syntax

#endif // SOUL_SYNTAX_PARSERFILECONTENT_INCLUDED
