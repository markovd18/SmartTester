#include <rtl/FilterLib.h>
#include "../DrawingFilterUnitTester.h"

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
        executeConfigTest(L"empty canvas size test",
                          FILTER_CONFIG + "\n\nCanvas_Width = \n\nCanvas_Height = ", E_FAIL);
        executeConfigTest(L"correct canvas size test",
                          FILTER_CONFIG + "\n\nCanvas_Width = 1200\n\nCanvas_Height = 800", S_OK);
    }
}
