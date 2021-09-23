#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: path.hpp
//      Author: Ziming Li
//     Created: 2021-02-10
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_FILE_PATH_HPP
#define ZED_FILE_PATH_HPP

#include <vector>
#include "../platform_sdk.h"
#include "../string.hpp"
#ifdef _Z_OS_WINDOWS
#   include <ShlObj.h>
#   include "../win/hmodule.hpp"
#endif

namespace zed {

class path
{
public:
#ifdef _Z_OS_WINDOWS
    using string_t = std::wstring;
    using psz_t    = PCWSTR;
    static constexpr WCHAR splitter = L'\\';
#else
    using string_t = std::string;
    using psz_t    = const char *;
    static constexpr char splitter = '/';
#endif

    static bool create_directory(psz_t path_name);
    static bool exists(psz_t path_name);
    static string_t path_of_file(const string_t &file_name);
};

class sys_path
{
public:
    static path::string_t get_app_path(void);
#ifdef _Z_OS_WINDOWS
    static std::wstring get_known_folder_path(REFKNOWNFOLDERID rfid, DWORD flags = KF_FLAG_DEFAULT);
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

namespace detail {

template <typename CharT>
class path_combiner
{
public:
    void set_prefix(const CharT *ps, size_t length)
    {
        ZASSERT(m_prefix.empty());
        m_prefix.assign(ps, length);
    }

    std::basic_string<CharT> get_canonicalized_base(void) const { return m_prefix + m_base; }

    std::basic_string<CharT> combine(const std::basic_string<CharT> &relative) const
    {
        std::basic_string<CharT> ret(m_prefix);

        string_t path(m_base);
        append(path, relative);
        ret.append(path);

        return ret;
    }
protected:
    path_combiner(CharT splitter = path::splitter) : m_splitter(splitter) {}

    void initialize(const string_piece<CharT> &base)
    {
        append(m_base, base);
    }

    class string_t final : public std::basic_string<CharT>
    {
        using base_class = std::basic_string<CharT>;
    public:
        void append(const CharT *ps, size_t length)
        {
            base_class::append(ps, length);
            m_segments.push_back(length);
        }
        const CharT& back(void) const
        {
            ZASSERT(!base_class::empty());
            return base_class::back();
        }
        void clear(void)
        {
            base_class::clear();
            m_segments.clear();
        }
        void pop_segment(void)
        {
            ZASSERT(!m_segments.empty());

            size_t new_length = base_class::length() - m_segments.back();
            base_class::resize(new_length);
            m_segments.pop_back();
        }
    private:
        std::vector<size_t> m_segments;
    };
    void append(string_t &dst, const string_piece<CharT> &src) const
    {
        if (src.empty())
            return;

        if (m_splitter == *src.data())
            dst.clear(); // Root

        size_t p = src.find(m_splitter);
        size_t q = 0;
        while (string_piece<CharT>::npos != p)
        {
            bool processed = false;

            const CharT *pch = src.data() + q;
            size_t length = p - q + 1;

            if (2 == length && '.' == *pch)
            {
                processed = true;
            }
            else if (3 == length && '.' == *pch && '.' == *(pch + 1))
            {
                if (dst.empty())
                    set_as_root(dst);
                else
                    dst.pop_segment();
                processed = true;
            }

            if (!processed)
                dst.append(pch, length);
            q = p + 1;
            p = src.find(m_splitter, q);
        }

        size_t length = src.length() - q;
        if (length > 0)
            dst.append(src.data() + q, length);
    }

    void set_as_root(void)
    {
        set_as_root(m_base);
    }
    void set_as_root(string_t &dst) const
    {
        dst.clear();
        dst.append(&m_splitter, 1);
    }
private:
    const CharT m_splitter;
    std::basic_string<CharT> m_prefix;
    string_t m_base;
};

} // namespace detail

inline path::string_t path::path_of_file(const string_t &file_name)
{
    size_t p = file_name.rfind(splitter);
    return string_t::npos != p ? file_name.substr(0, p + 1) : string_t();
}

#ifdef _Z_OS_WINDOWS
inline bool path::create_directory(psz_t path_name)
{
    return ::CreateDirectoryW(path_name, nullptr);
}

inline bool path::exists(psz_t path_name)
{
    return INVALID_FILE_ATTRIBUTES != ::GetFileAttributesW(path_name);
}

inline path::string_t sys_path::get_app_path(void)
{
    std::wstring app_file = hmodule::get_file_name(nullptr);
    return path::path_of_file(app_file);
}

inline std::wstring sys_path::get_known_folder_path(REFKNOWNFOLDERID rfid, DWORD flags)
{
    std::wstring ret;

    PWSTR pp = nullptr;
    HRESULT hr = ::SHGetKnownFolderPath(rfid, flags, nullptr, &pp);
    if (SUCCEEDED(hr))
    {
        ret = pp;
        if (ret.back() != L'\\')
            ret.push_back(L'\\');
        ::CoTaskMemFree(pp);
    }

    return ret;
}
#endif // _Z_OS_WINDOWS

} // namespace zed

#endif // ZED_FILE_PATH_HPP
