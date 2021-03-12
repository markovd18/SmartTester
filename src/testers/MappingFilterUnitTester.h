//
// Author: markovd@students.zcu.cz
//

#pragma once

#ifndef _CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
#define _CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
#include <rtl/hresult.h>
#include "../testers/GenericUnitTester.h"

namespace tester {
    /**
     * Derived class from GenericUnitTester responsible for the testing of Mapping filter.
     */
    class MappingFilterUnitTester : public GenericUnitTester {
    public: // public methods
        MappingFilterUnitTester(const GUID& guid);
        void executeSpecificTests() override;
        /**
         * When the MappingFilter is correctly configured, executed level event's signal_id should be mapped to the configured destination signal_id.
         * This test executes level event upon the MappingFilter and checks, if the signal_id was correctly mapped.
         * @return S_OK if the signal_id of executed was correctly mapped and no other attribute was modified, otherwise false
         */
        HRESULT levelEventMappingTest();
        /**
         * When the MappingFilter is correctly configured, executed info event's signal_id should be mapped to the configured destination signal_id.
         * This test executes info event upon the MappingFilter and checks, if the signal_id was correctly mapped.
         * @return S_OK if the signal_id of executed was correctly mapped and no other attribute was modified, otherwise false
         */
        HRESULT infoEventMappingTest();
        /**
         * When the MappingFilter is correctly configured, executed parameter event's signal_id should be mapped to the configured destination signal_id.
         * This test executes parameter event upon the MappingFilter and checks, if the signal_id was correctly mapped.
         * @return S_OK if the signal_id of executed was correctly mapped and no other attribute was modified, otherwise false
         */
        HRESULT parametersEventMappingTest();

    private: // private methods
        HRESULT eventMappingTest(const tester::MappingFilterConfig &config, scgms::NDevice_Event_Code eventCode);
    };
}
#endif // !_CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
