#include <rtl/FilterLib.h>
#include <utils/string_utils.h>
#include "../DrawingFilterUnitTester.h"
#include "../../utils/UnitTestExecUtils.h"
#include "../FilterConfiguration.h"
#include "../../utils/scgmsLibUtils.h"
#include "../../utils/constants.h"

namespace tester {

    const std::string DrawingFilterUnitTester::FILTER_CONFIG = "[Filter_001_{850a122c-8943-a211-c514-25baa9143574}]";

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
    }

    HRESULT DrawingFilterUnitTester::imageGenerationTest() {
        if (!isFilterLoaded()) {
            std::wcerr << L"No filter loaded! Can't execute test.\n";
            Logger::getInstance().error(L"No filter loaded! Can't execute test!");
            return E_FAIL;
        }

        scgms::SPersistent_Filter_Chain_Configuration configuration;
        refcnt::Swstr_list errors;

        HRESULT result = configuration ? S_OK : E_FAIL;
        if (!Succeeded(result)) {
            Logger::getInstance().error(L"Error while creating configuration!");
            shutDownTest();
            return E_FAIL;
        }

        std::string dayImage = "dayImage.svg";
        std::string graphImage = "graphImage.svg";
        tester::DrawingFilterConfig config(1200, 800);
        config.setDayFilePath(dayImage);
        config.setGraphFilePath(graphImage);
        std::string memory = config.toString();

        configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
        Logger::getInstance().info(L"Loading configuration from memory...");

        scgms::IFilter_Configuration_Link** begin, ** end;
        configuration->get(&begin, &end);

        result = getTestedFilter()->Configure(begin[0], errors.get());
        Logger::getInstance().info(L"Configuring filter...");

        if (Succeeded(result)) {
            scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Level);
            if (!event) {
                std::wcerr << L"Error while creating " << describeEvent(scgms::NDevice_Event_Code::Level);
                Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
                result = E_FAIL;
            } else {
                getTestedFilter()->Execute(event);
                if (!filesystem::exists(dayImage) || !filesystem::exists(graphImage)) {
                    Logger::getInstance().error(L"Drawing filter did not create an image at given paths!");
                    Logger::getInstance().error(L"Given paths: Day_File_Path = " + Widen_String(dayImage) + L", Graph_File_Path = " + Widen_String(graphImage));
                    result = E_FAIL;
                } else {
                    Logger::getInstance().info(L"Images created successfully!");
                    result = S_OK;

                    moveToTmp(dayImage);
                    moveToTmp(graphImage);
                }

            }
        } else {
            Logger::getInstance().error(L"Failed to configure filter:!\n"
                                        L"(" + std::wstring(memory.begin(), memory.end()) + L")");
            Logger::getInstance().error(std::wstring(L"expected result: ") + Describe_Error(S_OK));
            Logger::getInstance().error(std::wstring(L"actual result: ") + Describe_Error(result));
            result = E_FAIL;
        }

        shutDownTest();
        return result;
    }
}
