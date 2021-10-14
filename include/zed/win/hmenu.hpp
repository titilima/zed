#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: hmenu.hpp
//      Author: Ziming Li
//     Created: 2021-10-13
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_WIN_HMENU_HPP
#define ZED_WIN_HMENU_HPP

#include "../platform_sdk.h"
#include "./handled_resource.hpp"

namespace zed {

class unique_menu : public unique_resource<HMENU, default_resource_finalizer<HMENU>, menu_handle_traits>
{
    using base_class = unique_resource<HMENU, default_resource_finalizer<HMENU>, menu_handle_traits>;
public:
    unique_menu(void) : unique_menu(::CreateMenu()) {}
    unique_menu(UINT id, HINSTANCE inst = nullptr) : unique_menu(::LoadMenu(inst, MAKEINTRESOURCE(id))) {}

    static unique_menu create_popup(void);
    static unique_menu null(void) { return unique_menu(nullptr); }

    bool append(UINT_PTR id, PCSTR item, UINT flags = MF_ENABLED | MF_STRING);
    bool append(UINT_PTR id, PCWSTR item, UINT flags = MF_ENABLED | MF_STRING);
    bool append_separator(void) { return ::AppendMenu(get(), MF_SEPARATOR, 0, nullptr); }

    bool get_item_info_by_id(UINT id, MENUITEMINFOA &mii) { return ::GetMenuItemInfoA(get(), id, FALSE, &mii); }
    bool get_item_info_by_id(UINT id, MENUITEMINFOW &mii) { return ::GetMenuItemInfoW(get(), id, FALSE, &mii); }

    UINT track_popup(UINT flags, int x, int y, HWND h);
private:
    unique_menu(HMENU h) : base_class(h) {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

inline bool unique_menu::append(UINT_PTR id, PCSTR item, UINT flags)
{
    return ::AppendMenuA(get(), flags, id, item);
}

inline bool unique_menu::append(UINT_PTR id, PCWSTR item, UINT flags)
{
    return ::AppendMenuW(get(), flags, id, item);
}

inline unique_menu unique_menu::create_popup(void)
{
    return unique_menu(::CreatePopupMenu());
}

inline UINT unique_menu::track_popup(UINT flags, int x, int y, HWND h)
{
    return ::TrackPopupMenu(get(), flags, x, y, 0, h, nullptr);
}

} // namespace zed

#endif // ZED_WIN_HMENU_HPP
