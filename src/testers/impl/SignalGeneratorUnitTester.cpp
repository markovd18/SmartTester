//
// Author: markovd@students.zcu.cz
//

#include "../SignalGeneratorUnitTester.h"

constexpr GUID INVALID_MODEL_GUID = { 0x811122a6, 0xb4b2, 0x457d, {0xc0, 0x29, 0x62, 0x5c, 0xbd, 0xb6, 0x82, 0xef} };
constexpr GUID INVALID_SIGNAL_ID_GUID = {0xe1cd0700, 0xb079, 0x4911, {0xb7, 0x9b, 0xd2, 0x03, 0x48, 0x61, 0x01, 0xc8}};

tester::SignalGeneratorUnitTester::SignalGeneratorUnitTester() : FilterUnitTester(cnst::SIGNAL_GEN_GUID, EntityType::FILTER) {
    //
}

void tester::SignalGeneratorUnitTester::executeSpecificTests() {
    ///Configuration tests
    tester::SignalGeneratorConfig config;
    executeConfigTest(L"empty configuration test", config, E_INVALIDARG);

    config.setFeedbackName("fb1");
    config.setTimeSegmentId(1);
    config.setMaximumTime(0.5);
    config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});
    config.setStepping(0.035);
    config.setSynchronizeToSignal(true);
    config.setSynchronizationSignal(scgms::signal_BG);
    config.setModelId(INVALID_MODEL_GUID);
    executeConfigTest(L"invalid model id test", config, E_INVALIDARG);

    config.setModelId(cnst::BERGMAN_MODEL_GUID);
    executeConfigTest(L"bergman model id test", config, S_OK);
/// When signal synchronization set to false, crashes on std::invalid_logic exception thrown in the filter
//    config.setSynchronizeToSignal(false);
//    config.setSynchronizationSignal(Invalid_GUID);
//    executeConfigTest(L"no signal synchronization test", config, S_OK);
//
//    config.setSynchronizeToSignal(true);
//    executeConfigTest(L"invalid synchronization signal test", config, E_INVALIDARG);

    config.setSynchronizationSignal(INVALID_SIGNAL_ID_GUID);
    executeConfigTest(L"not existing synchronization signal test", config, E_INVALIDARG);

    config.setSynchronizationSignal(scgms::signal_BG);
    executeConfigTest(L"valid synchronization signal test", config, S_OK);

    config.setSynchronizationSignal(scgms::signal_All);
    executeConfigTest(L"all synchronization signal test", config, S_OK);

    config.setSynchronizationSignal(scgms::signal_Null);
    executeConfigTest(L"null synchronization signal test", config, S_OK);

    config.setStepping(-5);
    executeConfigTest(L"negative stepping test", config, E_INVALIDARG);

    config.setStepping(0);
    executeConfigTest(L"zero stepping test", config, S_OK);

    config.setStepping(0.0354);
    executeConfigTest(L"positive stepping test", config, S_OK);

    config.setMaximumTime(-5);
    executeConfigTest(L"negative maximum time test", config, E_INVALIDARG);

    config.setStepping(0);
    config.setMaximumTime(0);
    executeConfigTest(L"zero maximum time test", config, S_OK);

    config.setMaximumTime(1);
    executeConfigTest(L"positive maximum time test", config, S_OK);

    config.setStepping(0.0354);
    config.setMaximumTime(0);
    executeConfigTest(L"less than stepping maximum time test", config, E_INVALIDARG);

}
