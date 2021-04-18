#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: http_constants.hpp
//      Author: Ziming Li
//     Created: 2021-02-09
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_NET_HTTP_CONSTANTS_HPP
#define ZED_NET_HTTP_CONSTANTS_HPP

namespace zed {
namespace http {

class status_code {
public:
    enum {
        OK = 200,

        Found = 302,

        BadRequest = 400,
        Forbidden  = 403,
        NotFound   = 404,

        InternalServerError = 500
    };
};

namespace header_names {

constexpr const char AcceptLanguage[] = "Accept-Language";
constexpr const char ContentLength[]  = "Content-Length";
constexpr const char ContentType[]    = "Content-Type";
constexpr const char Cookie[]         = "Cookie";
constexpr const char ETag[]           = "ETag";
constexpr const char IfMatch[]        = "If-Match";
constexpr const char Referer[]        = "Referer";
constexpr const char UserAgent[]      = "User-Agent";

} // namespace header_names

} // namespace http
} // namespace zed

#endif // ZED_NET_HTTP_CONSTANTS_HPP
