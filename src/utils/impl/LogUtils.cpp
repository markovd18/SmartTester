//
// Author: markovd@students.zcu.cz
//

#include "../LogUtils.h"

namespace logs {

    void printResult(const HRESULT result) {
        switch (result) {
            case S_OK:
                std::wcout << "OK!\n";
                Logger::getInstance().info(L"Test result: OK!");
                break;
            case S_FALSE:
                std::wcout << "FAIL!\n";
                Logger::getInstance().error(L"Test result: FAIL!");
                break;
            case E_FAIL:
                std::wcout << "ERROR!\n";
                Logger::getInstance().error(L"Test result: ERROR!");
                break;
            default:
                std::wcerr << "UNKNOWN!\n";
                Logger::getInstance().info(L"Test result: UNKNOWN!");
                break;
        }
    }

    void printTestStartInfo(const std::wstring& testName) {
        Logger::getInstance().info(L"----------------------------------------");
        Logger::getInstance().info(L"Executing " + testName + L"...");
        Logger::getInstance().info(L"----------------------------------------");
        std::wcout << "Executing " << testName << "... ";
    }

    void errorLogLine(const std::vector<std::string>& line) {
        std::string printedLine;
        for (const auto & token : line) {
            printedLine += (token + "; ");
        }

        Logger::getInstance().error(Widen_Char(printedLine.c_str()));
    }

    void infoLogLine(const std::vector<std::string> &line) {
        std::string printedLine;
        for (const auto & token : line) {
            printedLine += (token + "; ");
        }

        Logger::getInstance().info(Widen_Char(printedLine.c_str()));
    }

    void infoLogLines(const std::vector<std::vector<std::string>>& lines) {
        for (auto & line : lines) {
            infoLogLine(line);
        }
    }

    void printAndEmptyErrors(const refcnt::Swstr_list& errors) {
        refcnt::wstr_container* wstr;
        if (errors->empty() != S_OK) {
            std::wcerr << "Error description: " << std::endl;
            while (errors->pop(&wstr) == S_OK) {
                std::wcerr << refcnt::WChar_Container_To_WString(wstr) << std::endl;
                Logger::getInstance().error(refcnt::WChar_Container_To_WString(wstr));
                wstr->Release();
            }
        }
    }

    void logConfigurationError(const tester::FilterConfig &config, HRESULT expected, HRESULT result) {
        Logger::getInstance().error(L"Provided configuration:\n"
                                    L"(" + Widen_String(config.toString()) + L")");
        Logger::getInstance().error(std::wstring(L"expected configuration result: ") + Describe_Error(expected));
        Logger::getInstance().error(std::wstring(L"actual configuration result: ") + Describe_Error(result));
    }

    std::vector<std::vector<std::string>> readLogFile(const std::string& logPath) {
        std::string line;
        std::string delimiter = ";";

        std::ifstream logFile(logPath);
        if (!logFile) {
            throw std::runtime_error("Error while opening log file!");
        }

        std::vector<std::vector<std::string>> loggedData;

        while (std::getline(logFile, line)) {
            size_t pos;
            std::vector<std::string> tmpVec;
            while ((pos = line.find(delimiter)) != std::string::npos) {
                tmpVec.push_back(line.substr(0, pos));
                line.erase(0, pos + delimiter.length() + 1);
            }

            loggedData.push_back(tmpVec);
        }

        return loggedData;
    }

    bool compareLines(const std::vector<std::string>& resultLogLine, const std::vector<std::string>& referenceLogLine) {
        for (size_t i = cnst::firstComparedIndex; i < referenceLogLine.size(); i++) {

            if ((i >= cnst::firstNumberValueIndex) && (i <= cnst::lastNumberValueIndex)) {
                std::string expectedField = referenceLogLine[i];
                std::string actualField = resultLogLine[i];

                if (expectedField.empty() && actualField.empty()) {
                    continue;
                } else if (expectedField.empty() || actualField.empty()) {
                    return false;
                }

                double expected = std::stod(expectedField);
                double actual = std::stod(actualField);
                double lambda = 0.0001;

                if (abs(expected - actual) > lambda) {
                    return false;
                }
            } else {
                if (referenceLogLine[i] != resultLogLine[i]) {
                    return false;
                }
            }
        }

        return true;
    }

    std::size_t getLoggedLinesCount(const std::string &logFilePath) {
        std::ifstream file(logFilePath);
        if (!file) {
            return 0;
        }

        std::string line;
        std::size_t loggedLinesCount = 0;
        std::getline(file, line);   /// Skipping the header line

        while (std::getline(file, line)) {
            loggedLinesCount++;
        }

        return loggedLinesCount;
    }
}