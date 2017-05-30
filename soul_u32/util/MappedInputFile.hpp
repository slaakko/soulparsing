// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_UTIL_MAPPED_INPUT_FILE_INCLUDED
#define SOUL_UTIL_MAPPED_INPUT_FILE_INCLUDED
#include <string>
#include <stdint.h>

namespace soul { namespace util {

class MappedInputFileImpl;

class MappedInputFile
{
public:
    MappedInputFile(const std::string& fileName_);
    ~MappedInputFile();
    const char* Begin() const;
    const char* End() const;
private:
    MappedInputFileImpl* impl;
};

std::string ReadFile(const std::string& fileName);

} } // namespace soul::util

#endif // SOUL_UTIL_MAPPED_INPUT_FILE_INCLUDED
