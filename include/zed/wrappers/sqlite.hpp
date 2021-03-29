#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: sqlite.hpp
//      Author: Ziming Li
//     Created: 2021-02-22
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_WRAPPERS_SQLITE_HPP
#define ZED_WRAPPERS_SQLITE_HPP

#include <sqlite3.h>
#include "../assert.h"
#include "../file/file.hpp"
#include "../memory.hpp"
#ifdef _Z_OS_WINDOWS
#   include "../string/conv.hpp"
#endif

namespace zed {

template <>
struct default_resource_finalizer<sqlite3 *>
{
    void operator()(sqlite3 *db) const { ::sqlite3_close(db); }
};

template <>
struct default_resource_finalizer<sqlite3_stmt *>
{
    void operator()(sqlite3_stmt *stmt) const { ::sqlite3_finalize(stmt); }
};

class sqlite_stmt : unique_resource<sqlite3_stmt *>
{
public:
    sqlite_stmt(void) : unique_resource() {}
    sqlite_stmt(sqlite_stmt &&o) : unique_resource(std::move(o)) {}
    ~sqlite_stmt(void) = default;

    using unique_resource::operator bool;
    using unique_resource::swap;

    void reset(void) { ::sqlite3_reset(get()); }
    int step(void) { return ::sqlite3_step(get()); }

    bool bind(int one_based_index, int n);
    template <class String>
    bool bind(int one_based_index, const String &s, bool transient = false) { return bind(one_based_index, s.data(), s.length(), transient); }
    bool bind(int one_based_index, const char *psz, bool transient = false) { return bind(one_based_index, psz, -1, transient); }
    template <class Container>
    bool bind_blob(int one_based_index, const Container &c, bool transient = false) { return bind_blob(one_based_index, c.data(), c.size(), transient); }
    bool bind_blob(int one_based_index, const void *data, size_t size, bool transient = false);
    template <typename E, typename = std::enable_if<std::is_enum<E>::value>>
    bool bind_enum(int one_based_index, E e) { return bind(one_based_index, static_cast<int>(e)); }
    bool bind_int64(int one_based_index, sqlite_int64 n);
    bool bind_null(int one_based_index);
    bool bind_string_or_null(int one_based_index, const char *psz, bool transient = false);
    template <class String>
    bool bind_string_or_null(int one_based_index, const String &s, bool transient = false);

    int get_column_int(int zero_based_index) { return ::sqlite3_column_int(get(), zero_based_index); }
    sqlite_int64 get_column_int64(int zero_based_index) { return ::sqlite3_column_int64(get(), zero_based_index); }
    template <typename E, typename = std::enable_if<std::is_enum<E>::value>>
    E get_column_enum(int zero_based_index) { return static_cast<E>(get_column_int(zero_based_index)); }
    std::string get_column_text(int zero_based_index);
#ifdef _Z_OS_WINDOWS
    std::wstring get_column_text16(int zero_based_index);
#endif
    template <typename CharT>
    void get_column_text(int zero_based_index, std::basic_string<CharT> &dst);
    void get_column_blob(int zero_based_index, std::vector<unsigned char> &dst);
private:
    friend class sqlite;
    friend class sqlite_qstream;
    explicit sqlite_stmt(sqlite3_stmt *stmt);

    bool bind(int one_based_index, const char *ps, int l, bool transient);
#ifdef _Z_OS_WINDOWS
    bool bind(int one_based_index, const wchar_t *ps, int l, bool transient);
#endif
};

class sqlite_rstream
{
public:
    operator bool() const { return SQLITE_ROW == m_ret; }

    int next(void);
private:
    friend class sqlite_qstream;
    friend const sqlite_rstream& operator>>(const sqlite_rstream &rs, int &n);
    template <typename E, typename>
    friend const sqlite_rstream& operator>>(const sqlite_rstream &rs, E &e);
    friend const sqlite_rstream& operator>>(const sqlite_rstream &rs, std::string &s);
    friend const sqlite_rstream& operator>>(const sqlite_rstream &rs, std::vector<unsigned char> &b);
    explicit sqlite_rstream(sqlite_stmt &stmt);

    int index(void) const;

    int m_ret = SQLITE_DONE;
    mutable sqlite_stmt m_stmt;
    mutable int m_index = 0;
};

class sqlite_qstream
{
public:
    ~sqlite_qstream(void);

    operator sqlite_rstream() const { return sqlite_rstream(m_stmt); }

    int flush(void);
    void restore(sqlite_rstream &rst);
private:
    friend class sqlite;
    friend const sqlite_qstream& operator<<(const sqlite_qstream &qs, int n);
    template <typename E, typename>
    friend const sqlite_qstream& operator<<(const sqlite_qstream &qs, E e);
    friend const sqlite_qstream& operator<<(const sqlite_qstream &qs, const char *psz);
    friend const sqlite_qstream& operator<<(const sqlite_qstream &qs, const std::string &s);
    explicit sqlite_qstream(sqlite3_stmt *stmt);

    int index(void) const;

    mutable sqlite_stmt m_stmt;
    mutable int m_index = 1;
};

class sqlite : unique_resource<sqlite3 *>
{
public:
    sqlite(void) = default;

    bool open(file::path_t db_path);
    void close(void) { reset(); }

    bool exec(const char *sql);
    sqlite_qstream query(const char *sql);

    template <class String>
    sqlite_stmt prepare(const String &sql) { return sqlite_stmt(prepare_internal(sql.data(), sql.length())); }
    sqlite_stmt prepare(const char *sql) { return sqlite_stmt(prepare_internal(sql, -1)); }

    auto last_insert_rowid(void) { return ::sqlite3_last_insert_rowid(get()); }

    class transaction;
private:
    static sqlite3* open_internal(file::path_t db_path);
    sqlite3_stmt* prepare_internal(const char *sql, int l);
};

class sqlite::transaction
{
public:
    transaction(sqlite &db, const char *name = nullptr);
    ~transaction(void);

    void rollback(void);
private:
    void exec(const char *command);

    sqlite &m_db;
    std::string m_name;
    bool m_processed = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

inline bool sqlite::exec(const char *sql)
{
    ZASSERT(is_valid(get()));
    int r = ::sqlite3_exec(get(), sql, nullptr, nullptr, nullptr);
    return SQLITE_OK == r || SQLITE_DONE == r || SQLITE_ROW == r;
}

inline bool sqlite::open(file::path_t db_path)
{
    reset(open_internal(db_path));
    return is_valid(get());
}

inline sqlite3* sqlite::open_internal(file::path_t db_path)
{
    sqlite3 *db = nullptr;
#ifdef _Z_OS_WINDOWS
    sqlite3_open16(db_path, &db);
#else
    sqlite3_open(db_path, &db);
#endif
    return db;
}

inline sqlite3_stmt* sqlite::prepare_internal(const char *sql, int l)
{
    sqlite3_stmt *ret = nullptr;
    ZASSERT(is_valid(get()));
    sqlite3_prepare(get(), sql, l, &ret, nullptr);
    return ret;
}

inline sqlite_qstream sqlite::query(const char *sql)
{
    return sqlite_qstream(prepare_internal(sql, -1));
}

inline sqlite::transaction::transaction(sqlite &db, const char *name) : m_db(db)
{
    if (nullptr != name && '\0' != *name)
    {
        m_name.push_back(' ');
        m_name.append(name);
    }
    exec("BEGIN");
}

inline sqlite::transaction::~transaction(void)
{
    if (!m_processed)
        exec("COMMIT");
}

inline void sqlite::transaction::exec(const char *command)
{
    std::string sql(command);
    sql.append(" TRANSACTION");
    sql.append(m_name);
    m_db.exec(sql.c_str());
}

inline void sqlite::transaction::rollback(void)
{
    ZASSERT(!m_processed);
    exec("ROLLBACK");
    m_processed = true;
}

inline sqlite_qstream::sqlite_qstream(sqlite3_stmt *stmt) : m_stmt(stmt)
{
}

inline sqlite_qstream::~sqlite_qstream(void)
{
    if (m_stmt && m_index > 1)
        m_stmt.step();
}

inline int sqlite_qstream::flush(void)
{
    if (m_stmt && m_index > 1)
    {
        m_index = 0;
        return m_stmt.step();
    }
    return SQLITE_ERROR;
}

inline int sqlite_qstream::index(void) const
{
    if (0 == m_index)
    {
        m_stmt.reset();
        ++m_index;
    }
    return m_index++;
}

inline void sqlite_qstream::restore(sqlite_rstream &rst)
{
    m_index = 0;
    m_stmt.swap(rst.m_stmt);
}

inline const sqlite_qstream& operator<<(const sqlite_qstream &qs, int n)
{
    qs.m_stmt.bind(qs.index(), n);
    return qs;
}

template <typename E, typename = std::enable_if<std::is_enum<E>::value>>
inline const sqlite_qstream& operator<<(const sqlite_qstream &qs, E e)
{
    qs.m_stmt.bind_enum<E>(qs.index(), e);
    return qs;
}

inline const sqlite_qstream& operator<<(const sqlite_qstream &qs, const char *psz)
{
    qs.m_stmt.bind_string_or_null(qs.index(), psz);
    return qs;
}

inline const sqlite_qstream& operator<<(const sqlite_qstream &qs, const std::string &s)
{
    qs.m_stmt.bind_string_or_null(qs.index(), s);
    return qs;
}

inline sqlite_rstream::sqlite_rstream(sqlite_stmt &stmt) : m_stmt(std::move(stmt))
{
    m_ret = m_stmt.step();
}

inline int sqlite_rstream::index(void) const
{
    return m_index++;
}

inline int sqlite_rstream::next(void)
{
    m_index = 0;
    return m_ret = m_stmt.step();
}

inline const sqlite_rstream& operator>>(const sqlite_rstream &rs, int &n)
{
    n = rs.m_stmt.get_column_int(rs.index());
    return rs;
}

template <typename E, typename = std::enable_if<std::is_enum<E>::value>>
inline const sqlite_rstream& operator>>(const sqlite_rstream &rs, E &e)
{
    e = rs.m_stmt.get_column_enum<E>(rs.index());
    return rs;
}

inline const sqlite_rstream& operator>>(const sqlite_rstream &rs, std::string &s)
{
    s = rs.m_stmt.get_column_text(rs.index());
    return rs;
}

inline const sqlite_rstream& operator>>(const sqlite_rstream &rs, std::vector<unsigned char> &b)
{
    rs.m_stmt.get_column_blob(rs.index(), b);
    return rs;
}

inline sqlite_stmt::sqlite_stmt(sqlite3_stmt *stmt) : unique_resource(stmt)
{
    ZASSERT(is_valid(get()));
}

inline bool sqlite_stmt::bind(int one_based_index, int n)
{
    int r = ::sqlite3_bind_int(get(), one_based_index, n);
    return SQLITE_OK == r;
}

inline bool sqlite_stmt::bind(int one_based_index, const char *ps, int l, bool transient)
{
    int r = ::sqlite3_bind_text(get(), one_based_index, ps, l, transient ? SQLITE_TRANSIENT : SQLITE_STATIC);
    return SQLITE_OK == r;
}

#ifdef _Z_OS_WINDOWS
inline bool sqlite_stmt::bind(int one_based_index, const wchar_t *ps, int l, bool transient)
{
    int r = ::sqlite3_bind_text16(get(), one_based_index, ps, l, transient ? SQLITE_TRANSIENT : SQLITE_STATIC);
    return SQLITE_OK == r;
}
#endif

inline bool sqlite_stmt::bind_blob(int one_based_index, const void *data, size_t size, bool transient)
{
    int r = ::sqlite3_bind_blob(get(), one_based_index, data, size, transient ? SQLITE_TRANSIENT : SQLITE_STATIC);
    return SQLITE_OK == r;
}

inline bool sqlite_stmt::bind_int64(int one_based_index, sqlite_int64 n)
{
    int r = ::sqlite3_bind_int64(get(), one_based_index, n);
    return SQLITE_OK == r;
}

inline bool sqlite_stmt::bind_null(int one_based_index)
{
    int r = ::sqlite3_bind_null(get(), one_based_index);
    return SQLITE_OK == r;
}

inline bool sqlite_stmt::bind_string_or_null(int one_based_index, const char *psz, bool transient)
{
    if ('\0' == *psz)
        return bind_null(one_based_index);
    else
        return bind(one_based_index, psz, -1, transient);
}

template <class String>
bool sqlite_stmt::bind_string_or_null(int one_based_index, const String &s, bool transient)
{
    if (s.empty())
        return bind_null(one_based_index);
    else
        return bind<String>(one_based_index, s, transient);
}

inline void sqlite_stmt::get_column_blob(int zero_based_index, std::vector<unsigned char> &dst)
{
    int size = ::sqlite3_column_bytes(get(), zero_based_index);
    dst.resize(size);
    if (size > 0)
        ::memcpy(dst.data(), ::sqlite3_column_blob(get(), zero_based_index), size);
}

inline std::string sqlite_stmt::get_column_text(int zero_based_index)
{
    const char *ps = reinterpret_cast<const char *>(::sqlite3_column_text(get(), zero_based_index));
    int l = ::sqlite3_column_bytes(get(), zero_based_index);
    return std::string(ps, l);
}

template <>
inline void sqlite_stmt::get_column_text<char>(int zero_based_index, std::string &dst)
{
    dst = get_column_text(zero_based_index);
}

#ifdef _Z_OS_WINDOWS
template <>
inline void sqlite_stmt::get_column_text<wchar_t>(int zero_based_index, std::wstring &dst)
{
    dst = get_column_text16(zero_based_index);
}

inline std::wstring sqlite_stmt::get_column_text16(int zero_based_index)
{
    std::string text = get_column_text(zero_based_index);
    return multi_byte_to_wide_string(text, CP_UTF8);
}
#endif

} // namespace zed

#endif // ZED_WRAPPERS_SQLITE_HPP
