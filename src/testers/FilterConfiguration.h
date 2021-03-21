//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_FILTERCONFIGURATION_H
#define SMARTTESTER_FILTERCONFIGURATION_H

#include <string>
#include <rtl/guid.h>
#include <vector>

namespace tester {

    class FilterConfig {
    private:
        static inline const char* PARAM_SEPARATOR = "\n\n";
        static inline const char* HEADER = "[Filter_001_";

        const GUID m_filterId;
    public:
        explicit FilterConfig(const GUID& filterId);
        [[nodiscard]] virtual std::string toString() const = 0;
        static const char* getParamSeparator();
        std::string getHeader() const;
        const GUID & getFilterID() const;
    };

    class LogFilterConfig : public FilterConfig {
    private:
        std::string m_logFile;
    public:
        explicit LogFilterConfig(std::string logFile = "");
        std::string toString() const override;
        const std::string& getLogFile() const;
        void setLogFile(std::string logFile);
    };

    class DrawingFilterConfig : public FilterConfig {
    private:
        int32_t m_canvasWidth;
        int32_t m_canvasHeight;
        std::string m_graphFilePath;
        std::string m_dayFilePath;
        std::string m_agpFilePath;
        std::string m_parkesFilePath;
        std::string m_clarkFilePath;
        std::string m_ecdfFilePath;
    public:
        DrawingFilterConfig(int32_t canvasWidth = 0, int32_t canvasHeight = 0);
        std::string toString() const override;

        int32_t getCanvasWidth() const;
        void setCanvasWidth(int32_t canvasWidth);
        int32_t getCanvasHeight() const;
        void setCanvasHeight(int32_t canvasHeight);
        const std::string &getGraphFilePath() const;
        void setGraphFilePath(std::string graphFilePath);
        const std::string &getDayFilePath() const;
        void setDayFilePath(std::string dayFilePath);
        const std::string &getAgpFilePath() const;
        void setAgpFilePath(std::string agpFilePath);
        const std::string &getParkesFilePath() const;
        void setParkesFilePath(std::string parkesFilePath);
        const std::string &getClarkFilePath() const;
        void setClarkFilePath(std::string clarkFilePath);
        const std::string &getEcdfFilePath() const;
        void setEcdfFilePath(std::string ecdfFilePath);
    };

    class MappingFilterConfig : public FilterConfig {
    private:
        GUID m_signalSrcId;
        GUID m_signalDstId;
    public:
        MappingFilterConfig(const GUID& signalSrcId = Invalid_GUID, const GUID& signalDstId = Invalid_GUID);

        std::string toString() const override;
        const GUID &getSignalSrcId() const;
        void setSignalSrcId(const GUID &signalSrcId);
        const GUID &getSignalDstId() const;
        void setSignalDstId(const GUID &signalDstId);
    };

    class MaskingFilterConfig : public FilterConfig {
    private:
        GUID m_signalId;
        std::string m_bitmask;
    public:
        MaskingFilterConfig(const GUID& signalId = Invalid_GUID, std::string bitmask = "");

        std::string toString() const override;
        const GUID &getSignalId() const;
        void setSignalId(const GUID &signalId);
        const std::string &getBitmask() const;
        void setBitmask(const std::string &bitmask);
    };

    class LogReplayFilterConfig : public FilterConfig {
    private:
        std::string m_logFile;
        bool m_emitShutdown = false;
        bool m_filenameAsSegmentId = false;
    public:
        LogReplayFilterConfig(std::string logFile = "");

        std::string toString() const override;
        const std::string &getLogFile() const;
        void setLogFile(const std::string &logFile);
        bool isEmitShutdown() const;
        void setEmitShutdown(bool emitShutdown);
        bool isFilenameAsSegmentId() const;
        void setFilenameAsSegmentId(bool filenameAsSegmentId);
    };
  
    class SignalGeneratorConfig : public FilterConfig {
    private:
        GUID m_modelId;
        std::string m_feedbackName;
        bool m_synchronizeToSignal = false;
        GUID m_synchronizationSignal{};
        int64_t m_timeSegmentId = -1;
        double m_stepping = -1.0;
        double m_maximumTime = -1.0;
        bool m_shutDownAfterLast = false;
        bool m_echoDefaultParametersAsEvent = false;
        std::vector<double> m_parameters;
    public:
        SignalGeneratorConfig(const GUID& modelId = Invalid_GUID);

        std::string toString() const override;
        const GUID &getModelId() const;
        void setModelId(const GUID &modelId);
        const std::string &getFeedbackName() const;
        void setFeedbackName(const std::string &feedbackName);
        bool isSynchronizeToSignal() const;
        void setSynchronizeToSignal(bool synchronizeToSignal);
        const GUID &getSynchronizationSignal() const;
        void setSynchronizationSignal(const GUID &synchronizationSignal);
        int64_t getTimeSegmentId() const;
        void setTimeSegmentId(int64_t timeSegmentId);
        double getStepping() const;
        void setStepping(double stepping);
        double getMaximumTime() const;
        void setMaximumTime(double maximumTime);
        bool isShutDownAfterLast() const;
        void setShutDownAfterLast(bool mShutDownAfterLast);
        bool isEchoDefaultParametersAsEvent() const;
        void setEchoDefaultParametersAsEvent(bool mEchoDefaultParametersAsEvent);
        const std::vector<double> &getParameters() const;
        void setParameters(const std::vector<double> &parameters);

    private:
        std::string parametersToString() const;
    };
}
#endif //SMARTTESTER_FILTERCONFIGURATION_H
