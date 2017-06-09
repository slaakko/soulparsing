#ifndef Declarator_hpp_13426
#define Declarator_hpp_13426

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/codedom/Declaration.hpp>

namespace soul { namespace code {

class DeclaratorGrammar : public soul::parsing::Grammar
{
public:
    static DeclaratorGrammar* Create();
    static DeclaratorGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
    soul::codedom::InitDeclaratorList* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    DeclaratorGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class InitDeclaratorListRule;
    class InitDeclaratorRule;
    class DeclaratorRule;
    class DirectDeclaratorRule;
    class DeclaratorIdRule;
    class TypeIdRule;
    class TypeRule;
    class TypeSpecifierSeqRule;
    class AbstractDeclaratorRule;
    class DirectAbstractDeclaratorRule;
    class CVQualifierSeqRule;
    class InitializerRule;
    class InitializerClauseRule;
    class InitializerListRule;
};

} } // namespace soul.code

#endif // Declarator_hpp_13426
