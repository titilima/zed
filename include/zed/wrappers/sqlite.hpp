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
    ~sqlite_stmt(void) = default;

    void reset(void) { ::sqlite3_reset(get()); }
    int step(void) { return ::sqlite3_step(get()); }

    template <class String>
    bool bind(int one_based_index, const String &s, bool transient = false) { return bind(one_based_index, s.data(), s.length(), transient); }
    bool bind(int one_based_index, const char *psz, bool transient = false) { return bind(one_based_index, psz, -1, transient); }

    int get_column_int(int zero_based_index) { return ::sqlite3_column_int(get(), zero_based_index); }
    std::string get_column_text(int zero_based_index);
private:
    friend class sqlite;
    explicit sqlite_stmt(sqlite3_stmt *stmt);

    bool bind(int one_based_index, const char *ps, int l, bool transient);
};

class sqlite : unique_resource<sqlite3 *>
{
public:
    sqlite(void) = default;

    bool open(file::path_t db_path);
    void close(void) { reset(); }

    bool exec(const char *sql);

    template <class String>
    sqlite_stmt prepare(const String &sql) { return sqlite_stmt(prepare_internal(sql.data(), sql.length())); }
    sqlite_stmt prepare(const char *sql) { return sqlite_stmt(prepare_internal(sql, -1)); }

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

inline sqlite_stmt::sqlite_stmt(sqlite3_stmt *stmt) : unique_resource(stmt)
{
    ZASSERT(is_valid(get()));
}

inline bool sqlite_stmt::bind(int one_based_index, const char *ps, int l, bool transient)
{
    int r = ::sqlite3_bind_text(get(), one_based_index, ps, l, transient ? SQLITE_TRANSIENT : SQLITE_STATIC);
    return SQLITE_OK == r;
}

inline std::string sqlite_stmt::get_column_text(int zero_based_index)
{
    const char *ps = reinterpret_cast<const char *>(::sqlite3_column_text(get(), zero_based_index));
    int l = ::sqlite3_column_bytes(get(), zero_based_index);
    return std::string(ps, l);
}

} // namespace zed

#endif // ZED_WRAPPERS_SQLITE_HPP
