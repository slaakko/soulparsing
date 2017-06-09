#ifndef StdLib_hpp_2042
#define StdLib_hpp_2042

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <stdint.h>

namespace soul { namespace parsing {

class stdlib : public soul::parsing::Grammar
{
public:
    static stdlib* Create();
    static stdlib* Create(soul::parsing::ParsingDomain* parsingDomain);
private:
    stdlib(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class intRule;
    class uintRule;
    class longRule;
    class ulongRule;
    class hexuintRule;
    class hexRule;
    class hex_literalRule;
    class realRule;
    class urealRule;
    class numRule;
    class boolRule;
    class identifierRule;
    class qualified_idRule;
    class escapeRule;
    class charRule;
    class stringRule;
};

} } // namespace soul.parsing

#endif // StdLib_hpp_2042
