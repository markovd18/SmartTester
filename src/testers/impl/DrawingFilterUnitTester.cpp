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

    const char* DAY_IMAGE_SVG = "dayImage.svg";
    const char* GRAPH_IMAGE_SVG = "graphImage.svg";

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
        config.setGraphFilePath("testImage.svg");
        executeConfigTest(L"valid graph file path test", config, S_OK);

        config.setGraphFilePath("/");
        config.setDayFilePath("/");
        executeConfigTest(L"identical invalid paths test", config, E_FAIL);

        config.setGraphFilePath("testImage.svg");
        config.setDayFilePath("testImage.svg");
        executeConfigTest(L"identical valid paths test", config, E_FAIL);

        config.setDayFilePath("testImage2.svg");
        executeConfigTest(L"different valid paths test", config, S_OK);
        moveToTmp("testImage.svg");
        moveToTmp("testImage2.svg");

        /// Functional tests
        executeTest(L"image generation test", std::bind(&DrawingFilterUnitTester::imageGenerationTest, this));
        moveToTmp(DAY_IMAGE_SVG);
        moveToTmp(GRAPH_IMAGE_SVG);

// TODO markovd odkomentovat!
//        executeTest(L"Interface inspection test", std::bind(&DrawingFilterUnitTester::ifaceInspectionTest, this));
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

        HRESULT testResult = S_OK;
        scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Level);
        if (!event) {
            std::wcerr << L"Error while creating " << describeEvent(scgms::NDevice_Event_Code::Level);
            Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
            testResult = E_FAIL;
        } else {
            getTestedFilter()->Execute(event);
            if (!filesystem::exists(DAY_IMAGE_SVG) || !filesystem::exists(GRAPH_IMAGE_SVG)) {
                Logger::getInstance().error(L"Drawing filter did not create an image at given paths!");
                Logger::getInstance().error(L"Given paths: Day_File_Path = " + Widen_String(DAY_IMAGE_SVG) +
                L", Graph_File_Path = " + Widen_String(GRAPH_IMAGE_SVG));
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(L"Images created successfully!");
            }

        }

        return testResult;
    }

    HRESULT DrawingFilterUnitTester::ifaceInspectionTest() {
        if (!isFilterLoaded()) {
            std::wcerr << L"No filter loaded! Can't execute test.\n";
            Logger::getInstance().error(L"No filter loaded! Can't execute test!");
            return E_FAIL;
        }

        //TODO markovd nefunguje??? dořešit s Úblem
        scgms::IDrawing_Filter_Inspection *inspection;
        if (!Succeeded(getTestedFilter()->QueryInterface(&scgms::IID_Drawing_Filter,
                                                         reinterpret_cast<void **>(&inspection)))) {
            std::wcerr << L"Error while querying the interface of a Drawing filter!\n";
            Logger::getInstance().error(L"Error while querying the interface of a Drawing filter!");
            return E_FAIL;
        }

        refcnt::internal::CVector_Container<char> svg;
        bool result;
        GUID signalId = WString_To_GUID(L"AA402CE3-BA4A-457B-AA19-1B908B9B53C4", result);
        uint64_t segmentId = 0;

        refcnt::IVector_Container<GUID> *signalIds = refcnt::Create_Container(&signalId, &signalId + sizeof(signalId));
        refcnt::IVector_Container<uint64_t> *segmentIds = refcnt::Create_Container(&segmentId, &segmentId + sizeof(segmentId));

        if (!Succeeded(inspection->Draw(scgms::TDrawing_Image_Type::Graph, scgms::TDiagnosis::Type1, &svg, segmentIds, signalIds))) {
            std::wcerr << L"Error while retrieving the SVG!\n";
        }

        return S_OK;
    }
}
