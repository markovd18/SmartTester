#include <rtl/FilterLib.h>
#include <iface/FilterIface.h>
#include <utils/string_utils.h>
#include "../DrawingFilterUnitTester.h"
#include "../../utils/UnitTestExecUtils.h"
#include "../FilterConfiguration.h"
#include "../../utils/scgmsLibUtils.h"
#include "../../utils/constants.h"
#include "../../utils/LogUtils.h"

namespace tester {

    const char* TEST_IMAGE_SVG = "testImage.svg";
    const char* TEST_IMAGE_2_SVG = "testImage2.svg";
    const char* DAY_IMAGE_SVG = "dayImage.svg";
    const char* GRAPH_IMAGE_SVG = "graphImage.svg";
    const char* SVG_RETRIEVING_TEST_SVG = "svgRetrievingTest.svg";
    const char* NEW_DATA_AVAILABLE_TEST_SVG = "newDataAvailableTest.svg";

    DrawingFilterUnitTester::DrawingFilterUnitTester(const GUID& guid)
            : GenericUnitTester(guid) {
        //
    }

    /**
        Executes all tests specific to filter tested by this UnitTester.
    */
    void DrawingFilterUnitTester::executeSpecificTests() {
        Logger::getInstance().info(L"Executing specific tests...");

        /// Configuration tests
        tester::DrawingFilterConfig config;
        executeConfigTest(L"empty canvas size test", config, E_FAIL);

        config.setCanvasHeight(800);
        executeConfigTest(L"empty canvas width test", config, E_FAIL);

        config.setCanvasHeight(0);
        config.setCanvasWidth(1200);
        executeConfigTest(L"empty canvas height test", config, E_FAIL);

        config.setCanvasWidth(-100);
        config.setCanvasHeight(-100);
        executeConfigTest(L"negative canvas size test", config, E_FAIL);

        config.setCanvasWidth(1200);
        config.setCanvasHeight(800);
        executeConfigTest(L"correct canvas size test", config, S_OK);

        config.setGraphFilePath("/");
        executeConfigTest(L"invalid graph file path test", config, E_FAIL);

        config.setGraphFilePath("");
        config.setDayFilePath("/home");
        executeConfigTest(L"invalid day file path test", config, E_FAIL);

        config.setDayFilePath("");
        config.setAgpFilePath("/usr");
        executeConfigTest(L"invalid AGP file path test", config, E_FAIL);

        config.setAgpFilePath("");
        config.setParkesFilePath("/etc");
        executeConfigTest(L"invalid Parkes file path test", config, E_FAIL);

        config.setParkesFilePath("");
        config.setClarkFilePath("/lib");
        executeConfigTest(L"invalid Clark file path test", config, E_FAIL);

        config.setClarkFilePath("");
        config.setEcdfFilePath("/home/lost+found");
        executeConfigTest(L"invalid EDCF file path test", config, E_FAIL);

        config.setEcdfFilePath("");
        config.setGraphFilePath(TEST_IMAGE_SVG);
        executeConfigTest(L"valid graph file path test", config, S_OK);
        moveToTmp(TEST_IMAGE_SVG);

        config.setGraphFilePath("/");
        config.setDayFilePath("/");
        executeConfigTest(L"identical invalid paths test", config, E_FAIL);

        config.setGraphFilePath(TEST_IMAGE_SVG);
        config.setDayFilePath(TEST_IMAGE_SVG);
        executeConfigTest(L"identical valid paths test", config, E_FAIL);
        moveToTmp(TEST_IMAGE_SVG);

        config.setDayFilePath(TEST_IMAGE_2_SVG);
        executeConfigTest(L"different valid paths test", config, S_OK);
        moveToTmp(TEST_IMAGE_SVG);
        moveToTmp(TEST_IMAGE_2_SVG);

        /// Functional tests
        executeTest(L"image generation test", std::bind(&DrawingFilterUnitTester::imageGenerationTest, this));
        moveToTmp(DAY_IMAGE_SVG);
        moveToTmp(GRAPH_IMAGE_SVG);

        executeTest(L"svg retrieving test", std::bind(&DrawingFilterUnitTester::svgRetrievingTest, this));
        moveToTmp(SVG_RETRIEVING_TEST_SVG);

        executeTest(L"new data available test", std::bind(&DrawingFilterUnitTester::newDataAvailableTest, this));
        moveToTmp(NEW_DATA_AVAILABLE_TEST_SVG);
    }

    HRESULT DrawingFilterUnitTester::imageGenerationTest() {
        tester::DrawingFilterConfig config(1200, 800);
        config.setDayFilePath(DAY_IMAGE_SVG);
        config.setGraphFilePath(GRAPH_IMAGE_SVG);

        HRESULT configResult = configureFilter(config);
        if (!Succeeded(configResult)) {
            log::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Level);
        if (!event) {
            Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
            return E_FAIL;
        }

        Logger::getInstance().debug(L"Executing " + describeEvent(scgms::NDevice_Event_Code::Level));
        HRESULT execResult = getTestedFilter()->Execute(event);
        if (!Succeeded(execResult)) {
            Logger::getInstance().error(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Level));
            return E_FAIL;
        }

        if (!filesystem::exists(DAY_IMAGE_SVG) || !filesystem::exists(GRAPH_IMAGE_SVG)) {
            Logger::getInstance().error(L"Drawing filter did not create an image at given paths!");
            Logger::getInstance().error(L"Given paths: Day_File_Path = " + Widen_String(DAY_IMAGE_SVG) +
            L", Graph_File_Path = " + Widen_String(GRAPH_IMAGE_SVG));
            return E_FAIL;
        }

        Logger::getInstance().info(L"Images created successfully!");
        return S_OK;
    }

    HRESULT DrawingFilterUnitTester::svgRetrievingTest() {
        tester::DrawingFilterConfig config(1200, 800);
        config.setGraphFilePath(SVG_RETRIEVING_TEST_SVG);
        HRESULT configResult = configureFilter(config);
        if (!Succeeded(configResult)) {
            log::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Level);
        if (!event) {
            Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
            return E_FAIL;
        }

        Logger::getInstance().debug(L"Executing " + describeEvent(scgms::NDevice_Event_Code::Level));
        HRESULT execResult = getTestedFilter()->Execute(event);
        event->Release();
        if (!Succeeded(execResult)) {
            Logger::getInstance().error(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Level));
            return E_FAIL;
        }

        scgms::SDrawing_Filter_Inspection inspection(getTestedFilter());
        if (!inspection) {
            Logger::getInstance().error(L"Error while creating drawing filter inspection!");
            return E_FAIL;
        }

        refcnt::internal::CVector_Container<char> svg;
        HRESULT result = inspection->Draw(scgms::TDrawing_Image_Type::Graph, scgms::TDiagnosis::Type1, &svg,
                                          nullptr,nullptr);
        if (!Succeeded(result)) {
            Logger::getInstance().error(L"Error while retrieving the svg from the drawing filter!");
            return E_FAIL;
        }

        if (svg.empty() == S_OK) {
            Logger::getInstance().error(L"The retrieved svg is empty!");
            return E_FAIL;
        }

        Logger::getInstance().info(L"Svg successfully retrieved.");
        return S_OK;
    }

    HRESULT DrawingFilterUnitTester::newDataAvailableTest() {
        tester::DrawingFilterConfig config(1200, 800);
        config.setGraphFilePath(NEW_DATA_AVAILABLE_TEST_SVG);

        HRESULT configResult = configureFilter(config);
        if (!Succeeded(configResult)) {
            log::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Level);
        if (!event) {
            Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
            return E_FAIL;
        }

        Logger::getInstance().debug(L"Executing " + describeEvent(scgms::NDevice_Event_Code::Level));
        HRESULT execResult = getTestedFilter()->Execute(event);
        event->Release();
        if (!Succeeded(execResult)) {
            Logger::getInstance().error(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Level));
            return E_FAIL;
        }

        scgms::SDrawing_Filter_Inspection inspection(getTestedFilter());
        if (!inspection) {
            Logger::getInstance().error(L"Error while creating drawing filter inspection!");
            return E_FAIL;
        }

        HRESULT inspectResult = inspection->New_Data_Available();
        if (inspectResult != S_OK) {
            Logger::getInstance().error(L"Drawing filter did not recognize the executed event!");
            return E_FAIL;
        }

        inspectResult = inspection->New_Data_Available();
        if (inspectResult == S_OK) {
            Logger::getInstance().error(L"Drawing filter reported the same event twice!");
            return E_FAIL;
        }

        Logger::getInstance().info(L"Drawing filter reported the executed event correctly.");
        return S_OK;
    }


}
