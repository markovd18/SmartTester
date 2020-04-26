#ifndef SMARTTESTER_REGGRESSIONTESTER_H
#define SMARTTESTER_REGGRESSIONTESTER_H

#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"

class ReggressionTester {
private:
    CDynamic_Library* library;
    std::wstring config_filepath;
    void loadLibrary();
public:
    ReggressionTester(CDynamic_Library* library, std::wstring config_filepath);
    void compareLogs();
  
    //...

};
#endif //SMARTTESTER_UNITTESTER_H
