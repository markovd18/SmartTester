//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_LOGUTILS_H
#define SMARTTESTER_LOGUTILS_H

#include <string>
#include <vector>
#include <rtl/referencedImpl.h>
#include <utils/string_utils.h>
#include "Logger.h"
#include "constants.h"

namespace log {

    /// Prints result information into the console and log
    void printResult(const HRESULT result);
    /// Error logs given line
    void errorLogLine(const std::vector<std::string>& line);
    /// Info logs given line
    void infoLogLine(const std::vector<std::string> &line);
    /// Info logs given lines
    void infoLogLines(const std::vector<std::vector<std::string>>& lines);
    /// Prints errors into the log and console and empties the container
    void printAndEmptyErrors(const refcnt::Swstr_list& errors);
    /**
     * Reads log file at given path and transforms it into individual tokens.
     *
     * @param logPath path to a log file
     * @return vector of vectors representing individual lines
     */
    std::vector<std::vector<std::string>> readLogFile(const std::string& logPath);
    /**
     * Compares given lines from result log and the reference log.
     *
     * @param resultLogLine line from the result log
     * @param referenceLogLine line from the reference log
     * @return true if the lines are identical, otherwise false
     */
    bool compareLines(const std::vector<std::string>& resultLogLine, const std::vector<std::string>& referenceLogLine);

}

#endif //SMARTTESTER_LOGUTILS_H
