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

class sqlite : unique_resource<sqlite3 *>
{
public:
    sqlite(void) = default;

    bool open(file::path_t db_path);
    void close(void) { reset(); }

    bool exec(const char *sql);

    class transaction;
private:
    static sqlite3* open_internal(file::path_t db_path);
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

} // namespace zed

#endif // ZED_WRAPPERS_SQLITE_HPP
