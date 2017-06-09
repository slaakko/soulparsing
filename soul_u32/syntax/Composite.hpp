#ifndef Composite_hpp_4488
#define Composite_hpp_4488

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/parsing/Scope.hpp>
#include <soul_u32/parsing/Parser.hpp>

namespace soul { namespace syntax {

class CompositeGrammar : public soul::parsing::Grammar
{
public:
    static CompositeGrammar* Create();
    static CompositeGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
    soul::parsing::Parser* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, soul::parsing::Scope* enclosingScope);
private:
    CompositeGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class AlternativeRule;
    class SequenceRule;
    class DifferenceRule;
    class ExclusiveOrRule;
    class IntersectionRule;
    class ListRule;
    class PostfixRule;
};

} } // namespace soul.syntax

#endif // Composite_hpp_4488
