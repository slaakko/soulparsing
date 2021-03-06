#include <soul_u32/xml/XmlParser.hpp>
#include <soul_u32/parsing/InitDone.hpp>
#include <soul_u32/parsing/Utility.hpp>
#include <soul_u32/codedom/InitDone.hpp>
#include <soul_u32/util/InitDone.hpp>
#include <soul_u32/util/Unicode.hpp>
#include <soul_u32/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <stdint.h>
#include <set>
#include <map>

using namespace soul::xml;
using namespace soul::unicode;
using namespace soul::parsing;

class UnicodeCharacterDatabaseContentHandler : public XmlContentHandler
{
public:
    UnicodeCharacterDatabaseContentHandler();
    void StartElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName, const Attributes& attributes) override;
    void EndElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName) override;
private:
    char32_t codePoint;
    ExtendedCharacterInfo* extendedCharacterInfo;
};

UnicodeCharacterDatabaseContentHandler::UnicodeCharacterDatabaseContentHandler() : codePoint(-1), extendedCharacterInfo(nullptr)
{
}

std::set<std::u32string> elementNames;
std::set<std::u32string> attributeNames;
std::set<std::u32string> booleanAttributeNames;
std::set<std::string> unseenPropertyNames;
std::vector<std::unique_ptr<Block>> blocks;
std::map<std::string, Block*>  blockMap;
std::ofstream iso88591;

uint32_t FromHex(const std::string& hex)
{
    uint32_t c = 0;
    std::stringstream s;
    s.str(hex);
    s >> std::hex >> c;
    return c;
}

void UnicodeCharacterDatabaseContentHandler::StartElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName, const Attributes& attributes)
{
    auto it = elementNames.find(qualifiedName);
    if (it == elementNames.cend())
    {
        elementNames.insert(qualifiedName);
    }
    if (qualifiedName == U"char")
    {
        const std::u32string* cp = attributes.GetAttributeValue(U"cp");
        CharacterInfo* characterInfo = nullptr;
        extendedCharacterInfo = nullptr;
        uint32_t c = 0;
        if (cp)
        {
            std::string hex = ToUtf8(*cp);
            c = FromHex(hex);
            codePoint = c;
            characterInfo = &CreateCharacterInfo(c);
            extendedCharacterInfo = &CreateExtendedCharacterInfo(c);
/*
            if (c > 0 && c < 256)
            {
                iso88591 << "<" << ToUtf8(qualifiedName) << " ";
                for (const Attribute& attribute : attributes)
                {
                    iso88591 << ToUtf8(attribute.QualifiedName()) << "=\"" << XmlEscape(ToUtf8(attribute.Value())) << "\" ";
                }
                iso88591 << "/>" << std::endl;
            }
*/
        }
        for (const Attribute& attribute : attributes)
        {
            auto it = attributeNames.find(attribute.QualifiedName());
            if (it == attributeNames.cend())
            {
                attributeNames.insert(attribute.QualifiedName());
            }
            if (cp && characterInfo && extendedCharacterInfo)
            {
                std::string propertyName = ToUtf8(attribute.QualifiedName());
                std::string attributeValue = ToUtf8(attribute.Value());
                if (BinaryPropertyTable::Instance().IsBinaryProperty(propertyName))
                {
                    const BinaryProperty& property = BinaryPropertyTable::Instance().GetBinaryPropertyByShortName(propertyName);
                    BinaryPropertyId id = property.Id();
                    bool value = false;
                    if (attributeValue == "Y")
                    {
                        value = true;
                    }
                    else if (attributeValue != "N")
                    {
                        throw std::runtime_error("binary property value not Y/N");
                    }
                    characterInfo->SetBinaryPropery(id, value);
                }
                else if (propertyName == "age")
                {
                    const Age& age = AgeTable::Instance().GetAge(attributeValue);
                    characterInfo->SetAge(age.Id());
                }
                else if (propertyName == "gc")
                {
                    const GeneralCategory& generalCategory = GeneralCategoryTable::Instance().GetGeneralCategoryByShortName(attributeValue);
                    characterInfo->SetGeneralCategory(generalCategory.Id());
                }
                else if (propertyName == "blk")
                {
                    const Block& block = BlockTable::Instance().GetBlockByShortName(attributeValue);
                    characterInfo->SetBlock(block.Id());
                }
                else if (propertyName == "suc")
                {
                    if (attributeValue == "#")
                    {
                        characterInfo->SetUpper(c);
                    }
                    else
                    {
                        uint32_t upper = FromHex(attributeValue);
                        characterInfo->SetUpper(upper);
                    }
                }
                else if (propertyName == "slc")
                {
                    if (attributeValue == "#")
                    {
                        characterInfo->SetLower(c);
                    }
                    else
                    {
                        uint32_t lower = FromHex(attributeValue);
                        characterInfo->SetLower(lower);
                    }
                }
                else if (propertyName == "stc")
                {
                    if (attributeValue == "#")
                    {
                        characterInfo->SetTitle(c);
                    }
                    else
                    {
                        uint32_t title = FromHex(attributeValue);
                        characterInfo->SetTitle(title);
                    }
                }
                else if (propertyName == "scf")
                {
                    if (attributeValue == "#")
                    {
                        characterInfo->SetFolding(c);
                    }
                    else
                    {
                        uint32_t folding = FromHex(attributeValue);
                        characterInfo->SetFolding(folding);
                    }
                }
                else if (propertyName == "sc")
                {
                    ScriptId script = GetScriptByShortName(attributeValue).Id();
                    characterInfo->SetScript(script);
                }
                else if (propertyName == "bc")
                {
                    BidiClassId bidiClass = GetBidiClassByShortName(attributeValue).Id();
                    extendedCharacterInfo->SetBidiClass(bidiClass);
                }
                else if (propertyName == "uc")
                {
                    if (attributeValue == "#")
                    {
                        extendedCharacterInfo->FullUpper().append(1, c);
                    }
                    else
                    {
                        std::vector<std::string> v = Split(attributeValue, ' ');
                        for (const std::string& s : v)
                        {
                            uint32_t u = FromHex(s);
                            extendedCharacterInfo->FullUpper().append(1, u);
                        }
                    }
                }
                else if (propertyName == "lc")
                {
                    if (attributeValue == "#")
                    {
                        extendedCharacterInfo->FullLower().append(1, c);
                    }
                    else
                    {
                        std::vector<std::string> v = Split(attributeValue, ' ');
                        for (const std::string& s : v)
                        {
                            uint32_t u = FromHex(s);
                            extendedCharacterInfo->FullLower().append(1, u);
                        }
                    }
                }
                else if (propertyName == "tc")
                {
                    if (attributeValue == "#")
                    {
                        extendedCharacterInfo->FullTitle().append(1, c);
                    }
                    else
                    {
                        std::vector<std::string> v = Split(attributeValue, ' ');
                        for (const std::string& s : v)
                        {
                            uint32_t u = FromHex(s);
                            extendedCharacterInfo->FullTitle().append(1, u);
                        }
                    }
                }
                else if (propertyName == "cf")
                {
                    if (attributeValue == "#")
                    {
                        extendedCharacterInfo->FullFolding().append(1, c);
                    }
                    else
                    {
                        std::vector<std::string> v = Split(attributeValue, ' ');
                        for (const std::string& s : v)
                        {
                            uint32_t u = FromHex(s);
                            extendedCharacterInfo->FullFolding().append(1, u);
                        }
                    }
                }
                else if (propertyName == "ccc")
                {
                    int value = boost::lexical_cast<int>(attributeValue);
                    if (value < 0 || value > 254)
                    {
                        throw std::runtime_error("invalid ccc attribute value");
                    }
                    extendedCharacterInfo->SetCanonicalCombiningClass(static_cast<uint8_t>(value));
                }
                else if (propertyName == "na")
                {
                    extendedCharacterInfo->SetCharacterName(attributeValue);
                }
                else if (propertyName == "na1")
                {
                    extendedCharacterInfo->SetUnicode1Name(attributeValue);
                }
                else if (propertyName == "nv")
                {
                    if (attributeValue != "NaN")
                    {
                        extendedCharacterInfo->SetNumericValue(attributeValue);
                    }
                }
                else if (propertyName == "nt")
                {
                    extendedCharacterInfo->SetNumericType(GetNumericTypeByShortName(attributeValue).Id());
                }
                else if (propertyName == "bmg")
                {
                    uint32_t bmg = FromHex(attributeValue);
                    extendedCharacterInfo->SetBidiMirroringGlyph(bmg);
                }
                else if (propertyName == "bpt")
                {
                    extendedCharacterInfo->SetBidiPairedBracketType(GetBidiPairedBracketTypeByShortName(attributeValue).Id());
                }
                else if (propertyName == "bpb")
                {
                    uint32_t bpb = FromHex(attributeValue);
                    extendedCharacterInfo->SetBidiPairedBracket(bpb);
                }
                else
                {
                    unseenPropertyNames.insert(propertyName);
                }
            }
        }
    }
    else if (qualifiedName == U"name-alias" && codePoint != -1 && extendedCharacterInfo != nullptr)
    {
        std::string alias;
        std::string type;
        for (const Attribute& attribute : attributes)
        {
            std::string attributeName = ToUtf8(attribute.QualifiedName());
            if (attributeName == "alias")
            {
                alias = ToUtf8(attribute.Value());
            }
            else if (attributeName == "type")
            {
                type = ToUtf8(attribute.Value());
            }
        }
        const AliasType& aliasType = AliasTypeTable::Instance().GetAliasType(type);
        extendedCharacterInfo->Aliases().push_back(Alias(aliasType.Id(), alias));
    }
}

void UnicodeCharacterDatabaseContentHandler::EndElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName)
{
    if (qualifiedName == U"char")
    {
        codePoint = -1;
        extendedCharacterInfo = nullptr;
    }
}

struct InitDone
{
    InitDone()
    {
        soul::util::Init();
        soul::parsing::Init();
        soul::codedom::Init();
    }
    ~InitDone()
    {
        soul::codedom::Done();
        soul::parsing::Done();
        soul::util::Done();
    }
};

int main(int argc, const char** argv)
{
    try
    {
        InitDone initDone;
        std::string soul_u32_root;
        const char* soul_u32_root_env = getenv("SOUL_U32_ROOT");
        if (soul_u32_root_env)
        {
            soul_u32_root = soul_u32_root_env;
        }
        if (soul_u32_root.empty())
        {
            std::cerr << "please set 'SOUL_U32_ROOT' environment variable to contain /path/to/soul_u32 directory." << std::endl;
            return 2;
        }
/*/
        std::string iso88591fileName = (boost::filesystem::path(soul_u32_root) / boost::filesystem::path("unicode") / boost::filesystem::path("iso88591.xml")).generic_string();
        iso88591.open(iso88591fileName);
        iso88591 << "<ucd>" << std::endl;
*/
        std::string xmlFileName = (boost::filesystem::path(soul_u32_root) / boost::filesystem::path("unicode") / boost::filesystem::path("ucd.all.flat.xml")).generic_string();
        std::cout << "processing " << xmlFileName << "..." << std::endl;
        UnicodeCharacterDatabaseContentHandler contentHandler;
        ParseXmlFile(xmlFileName, &contentHandler);
/*
        iso88591 << "</ucd>" << std::endl;
*/
        CharacterTable::Instance().Write();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}