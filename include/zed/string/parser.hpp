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

    virtual int current_char(void) const = 0;
    int peek(const char *chars_to_skip = ascii_whitespace<char>::chars);

    int advance(void);

    size_t parsed_count(void) const { return m_current - m_start; }
protected:
    parser_stream(const char *start) : m_start(start), m_current(start) {}

    const char *m_current;
private:
    virtual void advance_internal(void) = 0;

    const char *m_start;
};

class parser_psz_stream final : public parser_stream
{
public:
    explicit parser_psz_stream(const char *psz) : parser_stream(psz) {}
private:
    int current_char(void) const override;
    void advance_internal(void) override;
};

class parser_string_stream final : public parser_stream
{
public:
    template <class String>
    explicit parser_string_stream(const String &s) : parser_stream(s.data()), m_end(s.data() + s.length()) {}
private:
    int current_char(void) const override { return m_current < m_end ? *m_current : EOF; }
    void advance_internal(void) override;

    const char *m_end;
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

inline int parser_psz_stream::current_char(void) const
{
    char ch = *m_current;
    return '\0' != ch ? ch : EOF;
}

inline void parser_psz_stream::advance_internal(void)
{
    ZASSERT('\0' != *m_current);
    ++m_current;
}

inline void parser_string_stream::advance_internal(void)
{
    ZASSERT(m_current < m_end);
    ++m_current;
}

} // namespace zed

#endif // ZED_STRING_PARSER_HPP
