#ifndef SMARTTESTER_REGRESSIONTESTER_H
#define SMARTTESTER_REGRESSIONTESTER_H

#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"

class RegressionTester {
private:
    CDynamic_Library* library;
    std::wstring config_filepath;
    void loadLibrary();
public:
    RegressionTester(CDynamic_Library* library, std::wstring config_filepath);
    void compareLogs();
  
    //...

};
#endif //SMARTTESTER_UNITTESTER_H
