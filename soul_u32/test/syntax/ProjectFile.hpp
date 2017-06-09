#ifndef ProjectFile_hpp_13455
#define ProjectFile_hpp_13455

#include <soul_u32/parsing/Grammar.hpp>
#include <soul_u32/parsing/Keyword.hpp>
#include <soul_u32/syntax/Project.hpp>

namespace soul { namespace syntax {

class ProjectFileGrammar : public soul::parsing::Grammar
{
public:
    static ProjectFileGrammar* Create();
    static ProjectFileGrammar* Create(soul::parsing::ParsingDomain* parsingDomain);
    Project* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    ProjectFileGrammar(soul::parsing::ParsingDomain* parsingDomain_);
    virtual void CreateRules();
    virtual void GetReferencedGrammars();
    class ProjectFileRule;
    class ProjectFileContentRule;
    class SourceRule;
    class ReferenceRule;
    class FilePathRule;
};

} } // namespace soul.syntax

#endif // ProjectFile_hpp_13455
