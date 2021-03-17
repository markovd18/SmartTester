//
// Author: markovd@students.zcu.cz
//

#pragma once

#ifndef _MAPPING_FILTER_UNIT_TESTER_H_
#define _MAPPING_FILTER_UNIT_TESTER_H_
#include <rtl/hresult.h>
#include "../testers/GenericUnitTester.h"

namespace tester {
    /**
     * Derived class from GenericUnitTester responsible for the testing of Mapping filter.
     */
    class MappingFilterUnitTester : public FilterUnitTester {
    public: // public methods
        MappingFilterUnitTester();
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
        /**
         * MappingFilter should map only the configured signal source id to the configured signal destination id. This test checks that event with
         * signal id that is not configured as the source signal id is not mapped, because it should not be.
         * @return S_OK if the signal_id of executed event was not mapped and no other attribute was modified, otherwise false
         */
        HRESULT notConfiguredSignalMappingTest();
        /**
         * When destination signal id is configured to @a scgms::signal_Null, all executed events with signal_id equal to configured
         * source signal id should be thrown away - they should not arrive to the next filter.
         * This test checks, if the event is really thrown away.
         * @return S_OK if the event did not arrive to the appended filter, otherwise E_FAIL
         */
        HRESULT nullDestinationIdTest();
        /**
         * When the source id of executed event does not match the configured source signal id and the destination signal id is @a scgms::signal:Null,
         * the event should not be thrown away and should arrive to the appended filter.
         * This test checks, if the event really not thrown away.
         * @return S_OK if the event arrived to the appended filter, otherwise E_FAIL
         */
        HRESULT differentSrcIdNullDestinationIdTest();
        /**
         * When the configured source id is @a scgms::signal_All, every with any signal_id should be mapped to the configured destination id.
         * This test checks, if every executed event's signal id is mapped to the configured destination id.
         * @return S_OK if every executed event was correctly mapped, otherwise E_FAIL
         */
        HRESULT allSourceIdTest();

    private: // private methods
        HRESULT eventMappingTest(const tester::MappingFilterConfig &config, scgms::NDevice_Event_Code eventCode, const GUID& signalId = Invalid_GUID);
    };
}
#endif // !_MAPPING_FILTER_UNIT_TESTER_H_
