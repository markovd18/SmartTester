#ifndef SMARTTESTER_REGRESSIONTESTER_H
#define SMARTTESTER_REGRESSIONTESTER_H

#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"

class RegressionTester {
private:
    CDynamic_Library* library;
    std::wstring config_filepath;
    void loadLibrary();
    std::vector<std::vector<std::string>> setLogVector(std::string cLog);
    bool compareLines(std::vector<std::string> log, std::vector<std::string> result);
public:
    RegressionTester(CDynamic_Library* library, std::wstring config_filepath);
    HRESULT compareLogs(std::string cLog, std::string rLog);

  
    //...

};
#endif //SMARTTESTER_UNITTESTER_H
