// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soul_u32/util/CodeFormatter.hpp>
#include <string>

namespace soul { namespace util {

void CodeFormatter::Write(const std::string& text)
{
    if (atBeginningOfLine)
    {
        if (indent != 0)
        {
            stream << std::string(indentSize * indent, ' ');
            atBeginningOfLine = false;
        }
    }
    stream << text;
}

void CodeFormatter::WriteLine(const std::string& text)
{
    Write(text);
    NewLine();
}

void CodeFormatter::NewLine()
{
    stream << "\n";
    atBeginningOfLine = true;
    ++line;
}

} } // namespace soul::util
