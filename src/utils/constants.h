#pragma once

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
#include <rtl/guid.h>
#include <rtl/hresult.h>

constexpr long MAX_EXEC_TIME = 1000; //ms

//C0E942B9-3928-4B81-9B43-A347668200BA
constexpr GUID LOG_GUID = { 0xc0e942b9, 0x3928, 0x4b81, {0x9b, 0x43, 0xa3, 0x47, 0x66, 0x82, 0x00, 0xba} };
//850a122c-8943-a211-c514-25baa9143574
constexpr GUID DRAWING_GUID = { 0x850a122c, 0x8943, 0xa211, {0xc5, 0x14, 0x25, 0xba, 0xa9, 0x14, 0x35, 0x74} };
//8FAB525C-5E86-AB81-12CB-D95B1588530A
constexpr GUID MAPPING_GUID = { 0x8fab525c, 0x5e86, 0xab81, {0x12, 0xcb, 0xd9, 0x5b, 0x15, 0x88, 0x53, 0x0a} };
//A1124C89-18A4-F4C1-28E8-A9471A58021E
constexpr GUID MASKING_GUID = { 0xa1124c89, 0x18a4, 0xf4c1, {0x28, 0xe8, 0xa9, 0x47, 0x1a, 0x58, 0x02, 0x1e} };

//correct guid format
constexpr wchar_t* GUID_FORMAT = L"XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";

#ifdef _WIN32
constexpr wchar_t* LIB_EXTENSION = L".dll";
constexpr wchar_t* SCGMS_LIB = L"scgms";
constexpr wchar_t* LOG_LIBRARY = L"filters/log";
constexpr wchar_t* DRAWING_LIBRARY = L"filters/drawing";
constexpr wchar_t* SIGNAL_LIBRARY = L"filters/signal";

#else

#ifdef __APPLE__
constexpr wchar_t* LIB_EXTENSION = L".dylib";
#else
constexpr wchar_t* LIB_EXTENSION = L".so";
#endif

constexpr wchar_t* SCGMS_LIB = L"libscgms";
constexpr wchar_t* LOG_LIBRARY = L"filters/liblog";
constexpr wchar_t* DRAWING_LIBRARY = L"filters/libdrawing";
constexpr wchar_t* SIGNAL_LIBRARY = L"filters/libsignal";

#endif //_WIN32
#endif _CONSTANTS_H_