#ifndef LibraryFile_hpp_4488
#define LibraryFile_hpp_4488

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/parsing/ParsingDomain.hpp>

namespace soul { namespace syntax {

class LibraryFileGrammar : public soul::parsing::Grammar
{
public:
    static LibraryFileGrammar* Create();
    static LibraryFileGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, soul::parsing::ParsingDomain* parsingDomain);
private:
    LibraryFileGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class LibraryFileRule;
    class NamespaceContentRule;
    class NamespaceRule;
    class GrammarRule;
    class GrammarContentRule;
    class RuleRule;
};

} } // namespace soul.syntax

#endif // LibraryFile_hpp_4488
