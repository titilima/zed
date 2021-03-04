#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: ini.hpp
//      Author: Ziming Li
//     Created: 2021-03-04
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_PARSERS_INI_HPP
#define ZED_PARSERS_INI_HPP

#include <unordered_map>
#include "../string/algorithm.hpp"
#include "../string/parser.hpp"

namespace zed {

class ini_data
{
public:
    static ini_data parse_cstr(const char *psz);
    static ini_data parse_stream(parser_stream &stream);

    int get_int(const char *sec, const char *name, int def) const;
    std::string get_string(const char *sec, const char *name, const char *def = "") const;

    using section = std::unordered_map<std::string, std::string>;
    const section* get_section(const char *sec) const;
private:
    ini_data(void) = default;

    const std::string* get_value(const char *sec, const char *name) const;

    std::unordered_map<std::string, section> m_sections;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

namespace detail {

struct ini_token
{
    enum token_type { unexpected = -1, finished = 0, comment, section, key, value };

    token_type type;
    std::string data;

    ini_token(token_type t = unexpected) : type(t) {}
    bool is_finished(void) const { return finished == type; }
};

class ini_tokenizer
{
public:
    ini_tokenizer(parser_stream &stream) : m_stream(stream) {}

    ini_token get(void)
    {
        ini_token ret;
        int ch = m_stream.peek();
        switch (ch)
        {
            case ';':
                ret.type = ini_token::comment;
                skip_line();
                break;
            case '[':
                parse_section_header(ret);
                break;
            case EOF:
                ret.type = ini_token::finished;
                break;
            default:
                parse_key(ch, ret);
        }
        return ret;
    }
    ini_token get_value(void)
    {
        ini_token ret;
        int ch = m_stream.peek(" \t");
        if ('\'' == ch || '"' == ch)
            parse_quoted_value(ch, ret);
        else
            parse_value(ch, ret);
        return ret;
    }
private:
    void parse_section_header(ini_token &dst)
    {
        int ch;
        while ((ch = m_stream.advance()) != EOF)
        {
            if ('\n' == ch || ']' == ch)
                break;
            dst.data.push_back(ch);
        }

        if (']' == ch)
        {
            dst.type = ini_token::section;
            zed::trim(&dst.data);
            m_stream.advance();
        }
        else
        {
            dst.type = ini_token::unexpected;
        }
    }
    void parse_key(int ch, ini_token &dst)
    {
        do {
            dst.data.push_back(ch);
            ch = m_stream.advance();
        } while (EOF != ch && '=' != ch && '\n' != ch);
        if ('=' == ch)
        {
            dst.type = ini_token::key;
            zed::trim_right(&dst.data);
        }
    }
    void parse_value(int ch, ini_token &dst)
    {
        do {
            dst.data.push_back(ch);
            ch = m_stream.advance();
        } while (EOF != ch && '\n' != ch && ';' != ch);
        dst.type = ini_token::value;
        zed::trim_right(&dst.data);
    }
    void parse_quoted_value(char q, ini_token &dst)
    {
        int ch;
        while ((ch = m_stream.advance()) != q)
        {
            if (EOF == ch || '\n' == ch)
                return;

            if ('\\' == ch)
            {
                ch = m_stream.advance();
                if (EOF == ch)
                    return;

                switch (ch)
                {
                    case 't': ch = '\t'; break;
                    case 'n': ch = '\n'; break;
                    case 'r': ch = '\r'; break;
                    case '\\': case '"': case '\'':
                        break;
                    default:
                        if ('\n' != ch)
                            skip_line();
                        return;
                }
            }

            dst.data.push_back(ch);
        }
        m_stream.advance();
        dst.type = ini_token::value;
    }
    void skip_line(void)
    {
        int ch;
        do {
            ch = m_stream.advance();
        } while ('\n' != ch && EOF != ch);
    }

    parser_stream &m_stream;
};

} // namespace detail

inline int ini_data::get_int(const char *sec, const char *name, int def) const
{
    int ret = def;
    if (const std::string *v = get_value(sec, name))
    {
        try
        {
            ret = std::stoi(*v);
        }
        catch (...)
        {
        }
    }
    return ret;
}

const ini_data::section* ini_data::get_section(const char *sec) const
{
    auto it = m_sections.find(sec);
    return m_sections.end() != it ? &(it->second) : nullptr;
}

std::string ini_data::get_string(const char *sec, const char *name, const char *def) const
{
    if (const std::string *v = get_value(sec, name))
        return *v;
    return std::string(def);
}

const std::string* ini_data::get_value(const char *sec, const char *name) const
{
    if (const section *s = get_section(sec))
    {
        auto it = s->find(name);
        if (s->end() != it)
            return &(it->second);
    }
    return nullptr;
}

inline ini_data ini_data::parse_cstr(const char *psz)
{
    parser_psz_stream stream(psz);
    return parse_stream(stream);
}

inline ini_data ini_data::parse_stream(parser_stream &stream)
{
    ini_data ret;
    using namespace detail;

    ini_tokenizer tokenizer(stream);
    ini_token t(ini_token::unexpected);
    ini_data::section *cur_sec = nullptr;
    do {
        t = tokenizer.get();
        switch (t.type)
        {
            case ini_token::key:
            {
                stream.advance();
                ini_token v = tokenizer.get_value();
                if (ini_token::value == v.type && nullptr != cur_sec)
                    cur_sec->emplace(t.data, v.data);
                break;
            }
            case ini_token::section:
            {
                section &sec = ret.m_sections[t.data];
                cur_sec = &sec;
                break;
            }
        }
    } while (!t.is_finished());
    return ret;
}

} // namespace zed

#endif // ZED_PARSERS_INI_HPP
