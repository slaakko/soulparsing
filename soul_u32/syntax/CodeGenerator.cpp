// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/syntax/CodeGenerator.hpp>
#include <soul_u32/syntax/CodeGeneratorVisitor.hpp>
#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Namespace.hpp>
#include <soul_u32/parsing/Visitor.hpp>
#include <soul_u32/parsing/Scope.hpp>
#include <soul_u32/util/TextUtils.hpp>
#include <soul_u32/util/Util.hpp>
#include <soul_u32/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <stdlib.h>

namespace soul { namespace syntax {

using namespace soul::parsing;
using namespace soul::util;
using namespace soul::unicode;
using boost::filesystem::path;

class NamespaceContent
{
public:
    void AddUsingObject(UsingObject* usingObject)
    {
        usingObjects.push_back(usingObject);
    }
    void AddGrammar(Grammar* grammar)
    {
        grammars.push_back(grammar);
    }
    const std::vector<UsingObject*>& UsingObjects() const { return usingObjects; }
    const std::vector<Grammar*>& Grammars() const { return grammars; }
private:
    std::vector<UsingObject*> usingObjects;
    std::vector<Grammar*> grammars;
};

typedef std::unordered_map<Namespace*, NamespaceContent> NamespaceContentMap;
typedef NamespaceContentMap::const_iterator NamespaceContentMapIt;

class ObjectSelectorVisitor : public Visitor
{
public:
    ObjectSelectorVisitor(int id_): id(id_)
    {
    }
    virtual void Visit(UsingObject& object) 
    {
        if (object.Owner() == id)
        {
            Namespace* ns = object.EnclosingScope()->Ns();
            NamespaceContent& nsContent = nsContentMap[ns];
            nsContent.AddUsingObject(static_cast<UsingObject*>(&object));
        }
    }
    virtual void BeginVisit(Grammar& grammar) 
    {
        if (grammar.Owner() == id)
        {
            Namespace* ns = grammar.Ns();
            NamespaceContent& nsContent = nsContentMap[ns];
            nsContent.AddGrammar(&grammar);
        }
    }
    const NamespaceContentMap& NsContentMap() const { return nsContentMap; }
private:
    int id;
    NamespaceContentMap nsContentMap;
};

bool Changed(const std::string& parserFilePath, const std::string& cppFilePath, const std::string& hppFilePath)
{
    if (!boost::filesystem::exists(cppFilePath)) return true;
    if (!boost::filesystem::exists(hppFilePath)) return true;
    if ((boost::filesystem::last_write_time(cppFilePath) < boost::filesystem::last_write_time(parserFilePath)) || 
        (boost::filesystem::last_write_time(hppFilePath) < boost::filesystem::last_write_time(parserFilePath)))
    {
        return true;
    }
    return false;
}

bool forceCodeGen = false;

void SetForceCodeGen(bool force)
{
    forceCodeGen = force;
}

void GenerateCode(ParserFileContent* parserFile)
{
    const std::string& filePath = parserFile->FilePath();
    std::string cppFilePath = boost::filesystem::path(filePath).replace_extension(".cpp").generic_string();
    std::string hppFilePath = boost::filesystem::path(filePath).replace_extension(".hpp").generic_string();
    if (!forceCodeGen && !Changed(filePath, cppFilePath, hppFilePath))
    {
        return;
    }
    std::ofstream cppFile(cppFilePath.c_str());
    CodeFormatter cppFormatter(cppFile);
    std::ofstream hppFile(hppFilePath.c_str());
    CodeFormatter hppFormatter(hppFile);
    path hppPath(hppFilePath);
    std::string hppFileName = hppPath.filename().string();
#pragma warning(disable: 4244)
    srand(time(0));
#pragma warning(default: 4244)
    int r = rand();
    std::string random = std::to_string(r);
    std::string includeGuard = Replace(hppFileName, '.', '_') + "_" + random;
    hppFormatter.WriteLine("#ifndef " + includeGuard);
    hppFormatter.WriteLine("#define " + includeGuard);
    hppFormatter.WriteLine();
    hppFormatter.WriteLine("#include <soul_u32/parsing/Grammar.hpp>");
    hppFormatter.WriteLine("#include <soul_u32/parsing/Keyword.hpp>");
    if (hppFileName != "StdLib.hpp")
    {
        cppFormatter.WriteLine("#include \"" + hppFileName + "\"");
    }
    cppFormatter.WriteLine("#include <soul_u32/parsing/Action.hpp>");
    cppFormatter.WriteLine("#include <soul_u32/parsing/Rule.hpp>");
    cppFormatter.WriteLine("#include <soul_u32/parsing/ParsingDomain.hpp>");
    cppFormatter.WriteLine("#include <soul_u32/parsing/Primitive.hpp>");
    cppFormatter.WriteLine("#include <soul_u32/parsing/Composite.hpp>");
    cppFormatter.WriteLine("#include <soul_u32/parsing/Nonterminal.hpp>");
    cppFormatter.WriteLine("#include <soul_u32/parsing/Exception.hpp>");
    cppFormatter.WriteLine("#include <soul_u32/parsing/StdLib.hpp>");
    cppFormatter.WriteLine("#include <soul_u32/parsing/XmlLog.hpp>");
    cppFormatter.WriteLine("#include <soul_u32/util/Unicode.hpp>");

    int n = int(parserFile->IncludeDirectives().size());
    for (int i = 0; i < n; ++i)
    {
        const std::pair<std::string, std::string>& includeDirective = parserFile->IncludeDirectives()[i];
        const std::string& includeFileName = includeDirective.first;
        const std::string& fileAttribute = includeDirective.second;
        CodeFormatter& formatter = fileAttribute == "hpp" ? hppFormatter : cppFormatter;
        formatter.WriteLine("#include " + includeFileName);
    }
    cppFormatter.WriteLine();
    hppFormatter.WriteLine();

    ObjectSelectorVisitor selectorVisitor(parserFile->Id());
    int m = int(parserFile->UsingObjects().size());
    for (int i = 0; i < m; ++i)
    {
        UsingObject* usingObject = parserFile->UsingObjects()[i].get();
        selectorVisitor.Visit(*usingObject);
    }
    parserFile->ParsingDomain()->GlobalNamespace()->Accept(selectorVisitor);
    const NamespaceContentMap& nsContent = selectorVisitor.NsContentMap();
    NamespaceContentMapIt e = nsContent.end();
    for (NamespaceContentMapIt i = nsContent.begin(); i != e; ++i)
    {
        Namespace* ns = i->first;
        std::u32string nsFullName = ns->FullName();
        std::vector<std::u32string> nsComponents;
        if (!nsFullName.empty())
        {
            nsComponents = Split(nsFullName, '.');
            int n = int(nsComponents.size());
            for (int i = 0; i < n; ++i)
            {
                if (i > 0)
                {
                    cppFormatter.Write(" ");
                    hppFormatter.Write(" ");
                }
                cppFormatter.Write("namespace " + ToUtf8(nsComponents[i]) + " {");
                hppFormatter.Write("namespace " + ToUtf8(nsComponents[i]) + " {");
            }
            cppFormatter.WriteLine();
            cppFormatter.WriteLine();
            hppFormatter.WriteLine();
            hppFormatter.WriteLine();
        }
        const NamespaceContent& content = i->second;
        if (!content.UsingObjects().empty())
        {
            int n = int(content.UsingObjects().size());
            for (int i = 0; i < n; ++i)
            {
                UsingObject* usingObject = content.UsingObjects()[i];
                soul::codedom::UsingObject* subject = usingObject->Subject();
                if (subject->FileAttr() == U"hpp")
                {
                    subject->Print(hppFormatter);
                }
                else
                {
                    subject->Print(cppFormatter);
                }
            }
        }
        cppFormatter.WriteLine("using namespace soul::parsing;");
        cppFormatter.WriteLine("using namespace soul::util;");
        cppFormatter.WriteLine("using namespace soul::unicode;");
        cppFormatter.WriteLine();
        if (!content.Grammars().empty())
        {
            int n = int(content.Grammars().size());
            for (int i = 0; i < n; ++i)
            {
                Grammar* grammar = content.Grammars()[i];
                CodeGeneratorVisitor visitor(cppFormatter, hppFormatter);
                grammar->Accept(visitor);
            }
        }
        if (!nsFullName.empty())
        {
            int n = int(nsComponents.size());
            for (int i = 0; i < n; ++i)
            {
                if (i > 0)
                {
                    cppFormatter.Write(" ");
                    hppFormatter.Write(" ");
                }
                cppFormatter.Write("}");
                hppFormatter.Write("}");
            }
            cppFormatter.WriteLine(" // namespace " + ToUtf8(nsFullName));
            hppFormatter.WriteLine(" // namespace " + ToUtf8(nsFullName));
            hppFormatter.WriteLine();
        }
    }
    hppFormatter.WriteLine("#endif // " + includeGuard);
    std::cout << "=> " << cppFilePath << std::endl;
    std::cout << "=> " << hppFilePath << std::endl;
}

void GenerateCode(const std::vector<std::unique_ptr<ParserFileContent>>& parserFiles)
{
    int n = int(parserFiles.size());
    for (int i = 0; i < n; ++i)
    {
        ParserFileContent* parserFile = parserFiles[i].get();
        GenerateCode(parserFile);
    }
}

} } // namespace soul::syntax
