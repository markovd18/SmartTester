#pragma once

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
#include "../../smartcgms/src/common/rtl/guid.h"

extern const long MAX_EXEC_TIME = 1000; //ms
extern const wchar_t* LOG_LIBRARY = L"log";
extern const wchar_t* DRAWING_LIBRARY = L"drawing";

extern const GUID LOG_GUID = { 0xc0e942b9, 0x3928, 0x4b81, {0x9b, 0x43, 0xa3, 0x47, 0x66, 0x82, 0x00, 0xba} };
//C0E942B9-3928-4B81-9B43-A347668200BA

#endif _CONSTANTS_H_