// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/syntax/Generator.hpp>
#include <soul_u32/syntax/ProjectFile.hpp>
#include <soul_u32/syntax/LibraryFile.hpp>
#include <soul_u32/syntax/ParserFile.hpp>
#include <soul_u32/syntax/Library.hpp>
#include <soul_u32/syntax/CodeGenerator.hpp>
#include <soul_u32/util/TextUtils.hpp>
#include <soul_u32/util/MappedInputFile.hpp>
#include <soul_u32/util/Unicode.hpp>
#include <soul_u32/parsing/Linking.hpp>
#include <soul_u32/parsing/ParsingDomain.hpp>
#include <soul_u32/parsing/Exception.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace soul { namespace syntax {

using namespace soul::util;
using namespace soul::unicode;
using namespace boost::filesystem;

std::string ResolveReferenceFilePath(const std::string& relativeReferenceFilePath, const std::string& projectBasePath, const std::vector<std::string>& libraryDirectories)
{
    path base(projectBasePath);
    path rrfp(relativeReferenceFilePath);
    path referenceFilePath = absolute(rrfp, base);
    if (exists(referenceFilePath))
    {
        return referenceFilePath.generic_string();
    }
    int n = int(libraryDirectories.size());
    for (int i = 0; i < n; ++i)
    {
        base = path(libraryDirectories[i]);
        referenceFilePath = absolute(rrfp, base);
        if (exists(referenceFilePath))
        {
            return referenceFilePath.generic_string();
        }
    }
    throw std::runtime_error("library file path '" + relativeReferenceFilePath + "' not found");
}

void Generate(const std::string& projectFilePath, const std::vector<std::string>& libraryDirectories)
{
    std::cout << "Parsing project file " << projectFilePath << "...\n";
    std::unique_ptr<soul::parsing::ParsingDomain> projectParsingDomain(new soul::parsing::ParsingDomain());
    projectParsingDomain->SetOwned();
    ProjectFileGrammar* projectFileGrammar = ProjectFileGrammar::Create(projectParsingDomain.get());
    LibraryFileGrammar* libraryFileGrammar = LibraryFileGrammar::Create(projectParsingDomain.get());
    ParserFileGrammar* parserFileGrammar = ParserFileGrammar::Create(projectParsingDomain.get());
    std::u32string projectFileContent = ToUtf32(ReadFile(projectFilePath));
    std::unique_ptr<Project> project(projectFileGrammar->Parse(&projectFileContent[0], &projectFileContent[0] + projectFileContent.length(), 0, projectFilePath));
    std::cout << "Compiling project '" << project->Name() << "'...\n";
    std::unique_ptr<soul::parsing::ParsingDomain> parsingDomain(new soul::parsing::ParsingDomain());
    parsingDomain->SetOwned();
    std::cout << "Parsing library files...\n";
    const std::vector<std::string>& referenceFiles = project->ReferenceFiles();
    int nr = int(referenceFiles.size());
    for (int i = 0; i < nr; ++i)
    {
        std::string relativeReferenceFilePath = referenceFiles[i];
        std::string referenceFilePath = ResolveReferenceFilePath(relativeReferenceFilePath, project->BasePath(), libraryDirectories);
        std::cout << "> " << referenceFilePath << "\n";
        std::u32string libraryFileContent = ToUtf32(ReadFile(referenceFilePath));
        libraryFileGrammar->Parse(&libraryFileContent[0], &libraryFileContent[0] + libraryFileContent.length(), i, referenceFilePath, parsingDomain.get());
    }
    std::cout << "Parsing source files...\n";
    std::vector<std::unique_ptr<ParserFileContent>> parserFiles;
    const std::vector<std::string>& sourceFiles = project->SourceFiles();
    soul::parsing::SetFileInfo(sourceFiles);
    int n = int(sourceFiles.size());
    for (int i = 0; i < n; ++i)
    {
        std::string sourceFilePath = sourceFiles[i];
        std::cout << "> " << sourceFilePath << "\n";
        std::u32string sourceFileContent = ToUtf32(ReadFile(sourceFilePath));
        std::unique_ptr<ParserFileContent> parserFileContent(parserFileGrammar->Parse(&sourceFileContent[0], &sourceFileContent[0] + sourceFileContent.length(), i, sourceFilePath, i, parsingDomain.get()));
        parserFiles.push_back(std::move(parserFileContent));
    }
    std::cout << "Linking...\n";
    soul::parsing::Link(parsingDomain.get());
    std::cout << "Expanding code...\n";
    soul::parsing::ExpandCode(parsingDomain.get());
    std::cout << "Generating source code...\n";
    GenerateCode(parserFiles);
    std::cout << "Generating library file...\n";
    GenerateLibraryFile(project.get(), parsingDomain.get());
}

} } // namespace soul::syntax
