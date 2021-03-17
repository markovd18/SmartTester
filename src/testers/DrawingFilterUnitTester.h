//
// Author: markovd@students.zcu.cz
//

#ifndef _DRAWING_FILTER_UNIT_TESTER_H_
#define _DRAWING_FILTER_UNIT_TESTER_H_

#include "GenericUnitTester.h"
#include <rtl/hresult.h>

namespace tester {
    /**
     * Derived class from GenericUnitTester responsible for unit testing of Drawing filter.
     */
    class DrawingFilterUnitTester : public FilterUnitTester {
    public:

        DrawingFilterUnitTester();
        void executeSpecificTests() override;
        /**
         * After successful configuration and executing an event upon the DrawingFilter, there should be an image created
         * on every given path in the configuration.
         * This test configures the DrawingFilter with graph image path and day image path and executes level event
         * upon it. There should be images created at both given paths.
         * @return S_OK if the images were successfully created, otherwise E_FAIL
         */
        HRESULT imageGenerationTest();
        /**
         * After any event is executed upon the DrawingFilter, executing the Draw method should return the svg,
         * describing the executed event.
         * This test executes level event upon the DrawingFilter and retrieves the svg using the Draw method.
         * @return S_OK if the svg was successfully retrieved, otherwise E_FAIL
         */
        HRESULT svgRetrievingTest();
        /**
         * After any event is executed upon the DrawingFilter, executing the New_Data_Available should return true,
         * meaning that there is new data available to be drawn. Then the method should return false until new event
         * is executed.
         * This test executes level event upon the DrawingFilter and calls the New_Data_Available method, which should
         * return true. Then, the method is called again, which should return false.
         * @return S_OK if the tested method returns right values, otherwise E_FAIL
         */
        HRESULT newDataAvailableTest();
    };
}
#endif // !_DRAWING_FILTER_UNIT_TESTER_H_
