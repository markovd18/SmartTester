#pragma once

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
#include "../../smartcgms/src/common/rtl/guid.h"
#include "../../smartcgms/src/common/rtl/hresult.h"

constexpr long MAX_EXEC_TIME = 1000; //ms

//C0E942B9-3928-4B81-9B43-A347668200BA
constexpr GUID LOG_GUID = { 0xc0e942b9, 0x3928, 0x4b81, {0x9b, 0x43, 0xa3, 0x47, 0x66, 0x82, 0x00, 0xba} };
//850a122c-8943-a211-c514-25baa9143574
constexpr GUID DRAWING_GUID = { 0x850a122c, 0x8943, 0xa211, {0xc5, 0x14, 0x25, 0xba, 0xa9, 0x14, 0x35, 0x74} };

//correct guid format
constexpr wchar_t* GUID_FORMAT = L"XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";

#ifdef _WIN32
constexpr wchar_t* SCGMS_LIB = L"scgms";
constexpr wchar_t* LOG_LIBRARY = L"filters/log";
constexpr wchar_t* DRAWING_LIBRARY = L"filters/drawing";

#else

constexpr wchar_t* SCGMS_LIB = L"libscgms";
constexpr wchar_t* LOG_LIBRARY = L"filters/liblog";
constexpr wchar_t* DRAWING_LIBRARY = L"filters/libdrawing";

#endif //_WIN32
#endif _CONSTANTS_H_