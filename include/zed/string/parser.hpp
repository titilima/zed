#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: parser.hpp
//      Author: Ziming Li
//     Created: 2021-03-04
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_STRING_PARSER_HPP
#define ZED_STRING_PARSER_HPP

#include <cstdio>
#include "../ctype.hpp"

namespace zed {

class parser_stream
{
public:
    virtual ~parser_stream(void) = default;

    virtual int current_char(void) = 0;
    int peek(const char *chars_to_skip = ascii_whitespace<char>::chars);

    int advance(void);
private:
    virtual void advance_internal(void) = 0;
};

class parser_psz_stream final : public parser_stream
{
public:
    explicit parser_psz_stream(const char *psz) : m_psz(psz) {}
private:
    int current_char(void) override;
    void advance_internal(void) override;

    const char *m_psz;
};

class parser_string_stream final : public parser_stream
{
public:
    template <class String>
    explicit parser_string_stream(const String &s) : m_ps(s.data()), m_left(s.length()) {}
private:
    int current_char(void) override { return m_left > 0 ? *m_ps : EOF; }
    void advance_internal(void) override;

    const char *m_ps;
    size_t m_left;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

inline int parser_stream::advance(void)
{
    int ch = current_char();
    if (EOF != ch)
    {
        advance_internal();
        ch = current_char();
    }
    return ch;
}

inline int parser_stream::peek(const char *chars_to_skip)
{
    int ch = current_char();
    while (EOF != ch && nullptr != strchr(chars_to_skip, ch))
        ch = advance();
    return ch;
}

inline int parser_psz_stream::current_char(void)
{
    char ch = *m_psz;
    return '\0' != ch ? ch : EOF;
}

inline void parser_psz_stream::advance_internal(void)
{
    ZASSERT('\0' != *m_psz);
    ++m_psz;
}

inline void parser_string_stream::advance_internal(void)
{
    ZASSERT(m_left > 0);
    ++m_ps; --m_left;
}

} // namespace zed

#endif // ZED_STRING_PARSER_HPP
