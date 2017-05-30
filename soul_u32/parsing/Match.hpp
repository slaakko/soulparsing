// =================================
// Copyright (c) 2017 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOUL_PARSING_MATCH_INCLUDED
#define SOUL_PARSING_MATCH_INCLUDED

namespace soul { namespace parsing {

class Match
{
public:
    Match(bool hit_, int length_): hit(hit_), length(length_) {}
    bool Hit() const { return hit; }
    int Length() const { return length; }
    void Concatenate(const Match& that)
    {
        length += that.length;
    }
    static Match Empty() 
    {
        return Match(true, 0);
    }
    static Match One()
    {
        return Match(true, 1);
    }
    static Match Nothing()
    {
        return Match(false, -1);
    }
private:
    bool hit;
    int length;
};

} } // namespace soul::parsing

#endif // SOUL_PARSING_MATCH_INCLUDED

