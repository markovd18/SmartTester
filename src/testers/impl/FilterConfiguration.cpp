//
// Author: markovd@students.zcu.cz
//

#include "../FilterConfiguration.h"
#include "../../utils/constants.h"
#include <utility>
#include <utils/string_utils.h>

namespace tester {

    FilterConfig::FilterConfig(const GUID &filterId) : m_filterId(filterId){
        //
    }

    const char* FilterConfig::getParamSeparator() {
        return PARAM_SEPARATOR;
    }

    const GUID & FilterConfig::getFilterID() const {
        return m_filterId;
    }

    std::string FilterConfig::getHeader() const {
        return std::string(HEADER) + Narrow_WString(GUID_To_WString(getFilterID())) + "]" + getParamSeparator();
    }

    LogFilterConfig::LogFilterConfig(std::string logFile) : FilterConfig(cnst::LOG_GUID), m_logFile(std::move(logFile)) {
        //
    }

    std::string LogFilterConfig::toString() const {
        return getHeader() + "Log_File = " + m_logFile;
    }

    const std::string & LogFilterConfig::getLogFile() const {
        return m_logFile;
    }

    void LogFilterConfig::setLogFile(std::string logFile) {
        m_logFile = std::move(logFile);
    }


    DrawingFilterConfig::DrawingFilterConfig(const int32_t canvasWidth, const int32_t canvasHeight)
            : FilterConfig(cnst::DRAWING_GUID), m_canvasWidth(canvasWidth), m_canvasHeight(canvasHeight){
        //
    }

    int32_t DrawingFilterConfig::getCanvasWidth() const {
        return m_canvasWidth;
    }

    void DrawingFilterConfig::setCanvasWidth(const int32_t canvasWidth) {
        m_canvasWidth = canvasWidth;
    }

    int32_t DrawingFilterConfig::getCanvasHeight() const {
        return m_canvasHeight;
    }

    void DrawingFilterConfig::setCanvasHeight(const int32_t canvasHeight) {
        m_canvasHeight = canvasHeight;
    }

    const std::string &DrawingFilterConfig::getGraphFilePath() const {
        return m_graphFilePath;
    }

    void DrawingFilterConfig::setGraphFilePath(std::string graphFilePath) {
        m_graphFilePath = std::move(graphFilePath);
    }

    const std::string &DrawingFilterConfig::getDayFilePath() const {
        return m_dayFilePath;
    }

    void DrawingFilterConfig::setDayFilePath(std::string dayFilePath) {
        m_dayFilePath = std::move(dayFilePath);
    }

    const std::string &DrawingFilterConfig::getAgpFilePath() const {
        return m_agpFilePath;
    }

    void DrawingFilterConfig::setAgpFilePath(std::string agpFilePath) {
        m_agpFilePath = std::move(agpFilePath);
    }

    const std::string &DrawingFilterConfig::getParkesFilePath() const {
        return m_parkesFilePath;
    }

    void DrawingFilterConfig::setParkesFilePath(std::string parkesFilePath) {
        m_parkesFilePath = std::move(parkesFilePath);
    }

    const std::string &DrawingFilterConfig::getClarkFilePath() const {
        return m_clarkFilePath;
    }

    void DrawingFilterConfig::setClarkFilePath(std::string clarkFilePath) {
        m_clarkFilePath = std::move(clarkFilePath);
    }

    const std::string &DrawingFilterConfig::getEcdfFilePath() const {
        return m_ecdfFilePath;
    }

    void DrawingFilterConfig::setEcdfFilePath(std::string ecdfFilePath) {
        m_ecdfFilePath = std::move(ecdfFilePath);
    }

    std::string DrawingFilterConfig::toString() const {
        return getHeader() + "Canvas_Width = " + std::to_string(m_canvasWidth) + getParamSeparator() +
                            "Canvas_Height = " + std::to_string(m_canvasHeight) + getParamSeparator() +
                            "Graph_File_Path = " + m_graphFilePath + getParamSeparator() +
                            "Day_File_Path = " + m_dayFilePath + getParamSeparator() +
                            "AGP_File_Path = " + m_agpFilePath + getParamSeparator() +
                            "Parkes_File_Path = " + m_parkesFilePath + getParamSeparator() +
                            "Clark_FilePath = " + m_clarkFilePath + getParamSeparator() +
                            "ECDF_File_Path = " + m_ecdfFilePath;
    }


    MappingFilterConfig::MappingFilterConfig(const GUID &signalSrcId, const GUID &signalDstId)
            : FilterConfig(cnst::MAPPING_GUID), m_signalSrcId(signalSrcId), m_signalDstId(signalDstId) {
        //
    }

    const GUID &MappingFilterConfig::getSignalSrcId() const {
        return m_signalSrcId;
    }

    void MappingFilterConfig::setSignalSrcId(const GUID &signalSrcId) {
        m_signalSrcId = signalSrcId;
    }

    const GUID &MappingFilterConfig::getSignalDstId() const {
        return m_signalDstId;
    }

    void MappingFilterConfig::setSignalDstId(const GUID &signalDstId) {
        m_signalDstId = signalDstId;
    }

    std::string MappingFilterConfig::toString() const {
        return getHeader() + "Signal_Src_Id = " + (m_signalSrcId == Invalid_GUID ? "" : Narrow_WString(GUID_To_WString(m_signalSrcId))) + getParamSeparator() +
                            "Signal_Dst_Id = " + (m_signalDstId == Invalid_GUID ? "" : Narrow_WString(GUID_To_WString(m_signalDstId)));
    }


    MaskingFilterConfig::MaskingFilterConfig(const GUID &signalId, std::string bitmask)
            : FilterConfig(cnst::MASKING_GUID), m_signalId(signalId), m_bitmask(std::move(bitmask)) {
        //
    }

    const GUID &MaskingFilterConfig::getSignalId() const {
        return m_signalId;
    }

    void MaskingFilterConfig::setSignalId(const GUID &signalId) {
        m_signalId = signalId;
    }

    const std::string &MaskingFilterConfig::getBitmask() const {
        return m_bitmask;
    }

    void MaskingFilterConfig::setBitmask(const std::string &bitmask) {
        m_bitmask = bitmask;
    }

    std::string MaskingFilterConfig::toString() const {
        return getHeader() + "Signal_Id = " + (m_signalId == Invalid_GUID ? "" : Narrow_WString(GUID_To_WString(m_signalId))) + getParamSeparator() +
                            "Bitmask = " + m_bitmask;
    }
}

