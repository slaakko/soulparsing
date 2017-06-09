// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/xml/XmlParser.hpp>
#include <soul_u32/xml/XmlGrammar.hpp>
#include <soul_u32/util/Unicode.hpp>
#include <soul_u32/util/MappedInputFile.hpp>

namespace soul { namespace xml {

using namespace soul::util;
using namespace soul::unicode;

void ParseXmlFile(const std::string& xmlFileName, XmlContentHandler* contentHandler)
{
    std::string xmlContent = ReadFile(xmlFileName);
    ParseXmlContent(xmlContent, xmlFileName, contentHandler);
}

void ParseXmlContent(const std::string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler)
{
    ParseXmlContent(ToUtf32(xmlContent), systemId, contentHandler);
}

XmlGrammar* xmlGrammar = nullptr;

void ParseXmlContent(const std::u32string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler)
{
    const char32_t* contentStart = &xmlContent[0];
    const char32_t* contentEnd = &xmlContent[0] + xmlContent.length();
    XmlProcessor xmlProcessor(contentStart, contentEnd, contentHandler);
    if (xmlGrammar == nullptr)
    {
        xmlGrammar = XmlGrammar::Create();
    }
    xmlGrammar->Parse(contentStart, contentEnd, 0, systemId, &xmlProcessor);
}

} } // namespace soul::xml
