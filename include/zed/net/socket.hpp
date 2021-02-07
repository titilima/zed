#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: socket.hpp
//      Author: Ziming Li
//     Created: 2021-02-05
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_NET_SOCKET_HPP
#define ZED_NET_SOCKET_HPP

#include "../build_macros.h"
#include "../memory.hpp"

#if defined(_Z_OS_WINDOWS)
#   include <WinSock2.h>
#endif

namespace zed {

#ifdef _Z_OS_WINDOWS
using socket_t = SOCKET;
#else
using socket_t = int;
#endif

template <>
struct default_resource_finalizer<socket_t>
{
    void operator()(socket_t s) const { close(s); }
#ifdef _Z_OS_WINDOWS
private:
    static void close(SOCKET socket) { ::closesocket(socket); }
#endif
};

template <>
struct default_resource_traits<socket_t>
{
#ifdef _Z_OS_WINDOWS
    static constexpr socket_t invalid_value = INVALID_SOCKET;
#else
    static constexpr socket_t invalid_value = -1;
#endif
};

class socket : unique_resource<socket_t>
{
public:
    socket(void) = default;
    socket(socket_t s) : unique_resource(s) {}
    socket(socket &&r) : socket(r.release()) {}

    using unique_resource::operator bool;
    operator socket_t() const { return get(); }

    bool open(int af, int type, int protocol)
    {
        reset(::socket(af, type, protocol));
        return is_valid(get());
    }
    void close(void) { reset(); }

    bool bind(const sockaddr_in &addr_in)
    {
        sockaddr *addr = reinterpret_cast<sockaddr *>(const_cast<sockaddr_in *>(&addr_in));
        return ::bind(get(), addr, sizeof(sockaddr_in)) != SOCKET_ERROR;
    }
private:
#ifndef _Z_OS_WINDOWS
    static constexpr int SOCKET_ERROR = -1;
#endif
};

} // namespace zed

#endif // ZED_NET_SOCKET_HPP
