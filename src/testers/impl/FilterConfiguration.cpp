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
        return getHeader() + "Signal = " + (m_signalId == Invalid_GUID ? "" : Narrow_WString(GUID_To_WString(m_signalId))) + getParamSeparator() +
                            "Bitmask = " + m_bitmask;
    }

    LogReplayFilterConfig::LogReplayFilterConfig(std::string logFile)
            : FilterConfig(cnst::LOG_REPLAY_GUID), m_logFile(std::move(logFile)) {
        //
    }

    const std::string &LogReplayFilterConfig::getLogFile() const {
        return m_logFile;
    }

    void LogReplayFilterConfig::setLogFile(const std::string &logFile) {
        m_logFile = logFile;
    }

    bool LogReplayFilterConfig::isEmitShutdown() const {
        return m_emitShutdown;
    }

    void LogReplayFilterConfig::setEmitShutdown(bool emitShutdown) {
        m_emitShutdown = emitShutdown;
    }

    bool LogReplayFilterConfig::isFilenameAsSegmentId() const {
        return m_filenameAsSegmentId;
    }

    void LogReplayFilterConfig::setFilenameAsSegmentId(bool filenameAsSegmentId) {
        m_filenameAsSegmentId = filenameAsSegmentId;
    }

    std::string LogReplayFilterConfig::toString() const {
        return getHeader() + "Log_File = " + m_logFile + getParamSeparator() +
                            "Emit_Shutdown = " + (isEmitShutdown() ? "true" : "false") + getParamSeparator() +
                            "Filename_as_segment_id = " + (isFilenameAsSegmentId() ? "true" : "false");
    }

    SignalGeneratorConfig::SignalGeneratorConfig(const GUID &modelId) : FilterConfig(cnst::SIGNAL_GEN_GUID), m_modelId(modelId) {
        //
    }

    const GUID &SignalGeneratorConfig::getModelId() const {
        return m_modelId;
    }

    void SignalGeneratorConfig::setModelId(const GUID &modelId) {
        m_modelId = modelId;
    }

    const std::string &SignalGeneratorConfig::getFeedbackName() const {
        return m_feedbackName;
    }

    void SignalGeneratorConfig::setFeedbackName(const std::string &feedbackName) {
        m_feedbackName = feedbackName;
    }

    bool SignalGeneratorConfig::isSynchronizeToSignal() const {
        return m_synchronizeToSignal;
    }

    void SignalGeneratorConfig::setSynchronizeToSignal(bool synchronizeToSignal) {
        m_synchronizeToSignal = synchronizeToSignal;
    }

    const GUID &SignalGeneratorConfig::getSynchronizationSignal() const {
        return m_synchronizationSignal;
    }

    void SignalGeneratorConfig::setSynchronizationSignal(const GUID &synchronizationSignal) {
        m_synchronizationSignal = synchronizationSignal;
    }

    int64_t SignalGeneratorConfig::getTimeSegmentId() const {
        return m_timeSegmentId;
    }

    void SignalGeneratorConfig::setTimeSegmentId(int64_t timeSegmentId) {
        m_timeSegmentId = timeSegmentId;
    }

    double SignalGeneratorConfig::getStepping() const {
        return m_stepping;
    }

    void SignalGeneratorConfig::setStepping(double stepping) {
        m_stepping = stepping;
    }

    double SignalGeneratorConfig::getMaximumTime() const {
        return m_maximumTime;
    }

    void SignalGeneratorConfig::setMaximumTime(double maximumTime) {
        m_maximumTime = maximumTime;
    }

    bool SignalGeneratorConfig::isShutDownAfterLast() const {
        return m_shutDownAfterLast;
    }

    void SignalGeneratorConfig::setShutDownAfterLast(bool mShutDownAfterLast) {
        m_shutDownAfterLast = mShutDownAfterLast;
    }

    bool SignalGeneratorConfig::isEchoDefaultParametersAsEvent() const {
        return m_echoDefaultParametersAsEvent;
    }

    void SignalGeneratorConfig::setEchoDefaultParametersAsEvent(bool mEchoDefaultParametersAsEvent) {
        m_echoDefaultParametersAsEvent = mEchoDefaultParametersAsEvent;
    }

    const std::vector<double> &SignalGeneratorConfig::getParameters() const {
        return m_parameters;
    }

    void SignalGeneratorConfig::setParameters(const std::vector<double> &parameters) {
        m_parameters = parameters;
    }

    std::string SignalGeneratorConfig::toString() const {
        return getHeader() + "Model = " + Narrow_WString(GUID_To_WString(m_modelId)) + getParamSeparator() +
                            "Feedback_Name = " + m_feedbackName + getParamSeparator() +
                            "Synchronize_To_Signal = " + (m_synchronizeToSignal ? "true" : "false") + getParamSeparator() +
                            "Synchronization_Signal = " + Narrow_WString(GUID_To_WString(m_synchronizationSignal)) + getParamSeparator() +
                            "Time_Segment_Id = " + ((m_timeSegmentId == -1) ? "" : std::to_string(m_timeSegmentId)) + getParamSeparator() +
                            "Stepping = " + ((m_stepping == -1) ? "" : std::to_string(m_stepping)) + getParamSeparator() +
                            "Maximum_Time = " + ((m_maximumTime == -1) ? "" : std::to_string(m_maximumTime)) + getParamSeparator() +
                            "Shutdown_After_Last = " + (m_shutDownAfterLast ? "true" : "false") + getParamSeparator() +
                            "Echo_Default_Parameters_As_Event = " + (m_echoDefaultParametersAsEvent ? "true" : "false") + getParamSeparator() +
                            "Parameters =" + parametersToString();  /// Purposefully left out space after =
    }

    std::string SignalGeneratorConfig::parametersToString() const {
        std::string string;
        for (const auto &param : m_parameters) {
            string += ' ' + std::to_string(param);
        }
        return string;
    }
}

