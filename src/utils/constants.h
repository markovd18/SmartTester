#pragma once

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
#include <rtl/guid.h>
#include <rtl/hresult.h>

namespace cnst {
    //maximum execution time of each test in milliseconds
    constexpr long MAX_EXEC_TIME = 1000;
    //expected name of tested log file
    static const wchar_t* LOG_FILE = L"log.csv";
    //expected name of imported configuration file
    static const wchar_t* CONFIG_FILE = L"config.ini";
    //temp directory name
    static const wchar_t* TMP_DIR = L"tmp";
    //regression log in temp directory
    static const wchar_t* TMP_LOG_FILE = L"tmp/log.csv";

    constexpr int firstComparedIndex = 2;
    constexpr int firstNumberValueIndex = 4;
    constexpr int lastNumberValueIndex = 6;

    //C0E942B9-3928-4B81-9B43-A347668200BA
    constexpr GUID LOG_GUID = { 0xc0e942b9, 0x3928, 0x4b81, {0x9b, 0x43, 0xa3, 0x47, 0x66, 0x82, 0x00, 0xba} };
    //850a122c-8943-a211-c514-25baa9143574
    constexpr GUID DRAWING_GUID = { 0x850a122c, 0x8943, 0xa211, {0xc5, 0x14, 0x25, 0xba, 0xa9, 0x14, 0x35, 0x74} };
    //8FAB525C-5E86-AB81-12CB-D95B1588530A
    constexpr GUID MAPPING_GUID = { 0x8fab525c, 0x5e86, 0xab81, {0x12, 0xcb, 0xd9, 0x5b, 0x15, 0x88, 0x53, 0x0a} };
    //A1124C89-18A4-F4C1-28E8-A9471A58021E
    constexpr GUID MASKING_GUID = { 0xa1124c89, 0x18a4, 0xf4c1, {0x28, 0xe8, 0xa9, 0x47, 0x1a, 0x58, 0x02, 0x1e} };
    //172EA814-9DF1-657C-1289-C71893F1D085
    constexpr GUID LOG_REPLAY_GUID = { 0x172ea814, 0x9df1, 0x657c, { 0x12, 0x89, 0xc7, 0x18, 0x93, 0xf1, 0xd0, 0x85} };
    //9EEB3451-2A9D-49C1-BA37-2EC0B00E5E6D
    constexpr GUID SIGNAL_GEN_GUID = { 0x9eeb3451, 0x2a9d, 0x49c1, {0xba, 0x37, 0x2e, 0xc0, 0xb0, 0x0e, 0x5e, 0x6d} };

    //correct guid format
    static const wchar_t* GUID_FORMAT = L"XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";
    //guid representing a model
    constexpr GUID MODEL_GUID = { 1, 1, 1, { 1, 1, 1, 1, 1, 1, 1, 1} };

#ifdef _WIN32
    static const wchar_t* LIB_EXTENSION = L".dll";
    static const wchar_t* SCGMS_LIB = L"scgms";
    static const wchar_t* LOG_LIBRARY = L"filters/log";
    static const wchar_t* DRAWING_LIBRARY = L"filters/drawing";
    static const wchar_t* SIGNAL_LIBRARY = L"filters/signal";
    static const wchar_t* MODEL_LIBRARY = L"filters/model";
    static const wchar_t* CONTROLLERS_LIBRARY = L"filters/controllers";

#else

#ifdef __APPLE__
    constexpr wchar_t* LIB_EXTENSION = L".dylib";
#else
    static const wchar_t* LIB_EXTENSION = L".so";
#endif

    static const wchar_t* SCGMS_LIB = L"./libscgms";
    static const wchar_t* LOG_LIBRARY = L"./filters/liblog";
    static const wchar_t* DRAWING_LIBRARY = L"./filters/libdrawing";
    static const wchar_t* SIGNAL_LIBRARY = L"./filters/libsignal";
    static const wchar_t* MODEL_LIBRARY = L"./filters/libmodel";
    static const wchar_t* CONTROLLERS_LIBRARY = L"./filters/libcontrollers";



#endif //_WIN32
#endif //_CONSTANTS_H_

}