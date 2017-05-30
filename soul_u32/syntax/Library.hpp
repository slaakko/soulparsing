/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef SOUL_SYNTAX_LIBRARY_INCLUDED
#define SOUL_SYNTAX_LIBRARY_INCLUDED

#include <soul_u32/syntax/Project.hpp>
#include <soul_u32/parsing/ParsingDomain.hpp>

namespace soul { namespace syntax {

void GenerateLibraryFile(Project* project, soul::parsing::ParsingDomain* parsingDomain);

} } // namespace cpg::syntax

#endif // SOUL_SYNTAX_LIBRARY_INCLUDED
