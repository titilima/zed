#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: format.hpp
//      Author: Ziming Li
//     Created: 2021-02-17
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_STRING_FORMAT_HPP
#define ZED_STRING_FORMAT_HPP

#include <functional>
#include <utility>
#include <vector>
#include "../string.hpp"

namespace zed {

class args_collector final : std::vector<std::string>
{
public:
    template <typename... Args>
    args_collector(const Args&... args) { collect_from<Args...>(args...); }

    using std::vector<std::string>::at;
    using std::vector<std::string>::size;
private:
    template <typename T>
    void collect_from(const T &arg) { push(arg); }
    template <typename Arg, typename... Args>
    void collect_from(const Arg &arg, const Args&... args);

    template <typename T>
    void push(const T &arg) { emplace_back(std::to_string(arg)); }
    void push(const char *arg) { emplace_back(arg); }
};

template <typename CharT>
class formatter_impl
{
protected:
    template <typename S>
    formatter_impl(const S &format);

    using part_formatter = std::function<std::basic_string<CharT>(const std::basic_string<CharT> &)>;
    std::basic_string<CharT> format(const part_formatter &formatter) const;
private:
    struct part {
        enum type { raw, placeholder } m_type = raw;
        std::basic_string<CharT> m_content;
    };
    std::vector<part> m_parts;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

template <typename Arg, typename... Args>
void args_collector::collect_from(const Arg &arg, const Args&... args)
{
    push(arg);
    collect_from<Args...>(args...);
}

template <>
inline void args_collector::push<std::string>(const std::string &arg) { emplace_back(arg); }

template <typename CharT>
template <typename S>
formatter_impl<CharT>::formatter_impl(const S &format)
{
    detail::char_iterator it(format);

    part p;
    typename part::type next_type = part::placeholder;
    CharT flag_char = '{', next_flag_char = '}';
    while (!it.reach_the_end())
    {
        auto ch = *it;
        if (ch == flag_char)
        {
            if (!p.m_content.empty() || part::placeholder == p.m_type)
                m_parts.push_back(p);
            std::swap(flag_char, next_flag_char);
            std::swap(p.m_type, next_type);
            p.m_content.clear();
        }
        else
        {
            p.m_content.push_back(ch);
        }
        ++it;
    }

    if ('}' == flag_char)
    {
        // Incomplete placeholder, treat it as a raw string.
        p.m_type = part::raw;
        p.m_content.insert(p.m_content.begin(), '{');
    }

    if (!p.m_content.empty() || part::placeholder == p.m_type)
        m_parts.push_back(p);
}

template <typename CharT>
std::basic_string<CharT> formatter_impl<CharT>::format(const part_formatter &formatter) const
{
    std::basic_string<CharT> ret;
    for (const part &p : m_parts)
    {
        if (p.m_type == part::raw)
            ret.append(p.m_content);
        else
            ret.append(formatter(p.m_content));
    }
    return ret;
}

} // namespace zed

#endif // ZED_STRING_FORMAT_HPP
