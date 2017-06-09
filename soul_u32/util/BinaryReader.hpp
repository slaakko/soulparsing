// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_UTIL_BINARY_READER_INCLUDED
#define SOUL_UTIL_BINARY_READER_INCLUDED
#include <soul_u32/util/MappedInputFile.hpp>

namespace soul { namespace util {

class BinaryReader
{
public:
    BinaryReader(const std::string& fileName_);
    virtual ~BinaryReader();
    bool ReadBool();
    uint8_t ReadByte();
    int8_t ReadSByte();
    uint16_t ReadUShort();
    int16_t ReadShort();
    uint32_t ReadUInt();
    int32_t ReadInt();
    uint64_t ReadULong();
    int64_t ReadLong();
    float ReadFloat();
    double ReadDouble();
    char32_t ReadChar();
    std::string ReadUtf8String();
    std::u32string ReadUtf32String();
    uint32_t ReadEncodedUInt();
    uint32_t Pos() const { return pos; }
    void Skip(uint32_t size);
private:
    std::string fileName;
    MappedInputFile file;
    const uint8_t* begin;
    const uint8_t* end;
    uint32_t pos;
    void CheckEof();
};

} } // namespace soul::util

#endif // SOUL_UTIL_BINARY_READER_INCLUDED
