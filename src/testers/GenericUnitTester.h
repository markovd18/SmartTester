//
// Author: markovd@students.zcu.cz
//

#pragma once

#ifndef _GENERIC_UNIT_TESTER_H_
#define _GENERIC_UNIT_TESTER_H_

#include <map>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <iface/UIIface.h>
#include <rtl/Dynamic_Library.h>
#include <rtl/hresult.h>
#include <utils/string_utils.h>
#include "../utils/TestFilter.h"
#include "../utils/Logger.h"
#include "FilterConfiguration.h"
#include "../utils/constants.h"
#include "../mappers/GuidFileMapper.h"
#include "../utils/EntityUtils.h"

namespace tester {
    /**
     * Contains methods, for running entity tests.
     */
    class TestRunner {
    protected: // protected attributes
        std::mutex m_testMutex;
        std::condition_variable m_testCv;
        HRESULT m_lastTestResult = S_OK;
    public: // public methods
        /**
            Invokes test method passed as a parameter. Invoked method has to take in zero parameters and return HRESULT as a return value.
            @param testName name of the test which will be displayed in logs
            @param test method to be invoked by this method
        */
        void executeTest(const std::wstring& testName, const std::function<HRESULT(void)>& test);

    protected:
        /// Every derived class has to override the test shutdown mechanism
        virtual HRESULT shutDownTest() = 0;
    private: // private methods
        HRESULT runTestInThread(const std::function<HRESULT(void)>& test);
        virtual void runTest(const std::function<HRESULT(void)>& test);
    };

    /**
     * Contains generic tests for any module.
     */
    class ModuleUnitTester : public TestRunner {
    private:    // private attributes
        /// Vector of already tested modules
        static std::vector<std::string> s_testedModules;
        /// Mapped do_get_*_descriptors methods to methods which need to be implemented along with them
        static std::map<std::string, std::string> s_descriptorsToFactories;
        /// Mapped do_get_*_descriptors methods to factory tests of their factory methods
        std::map<std::string, std::function<HRESULT(void)>> m_descriptorsToTests {
                {"do_get_filter_descriptors", std::bind(&ModuleUnitTester::filterCreationTest, this) },
                {"do_get_signal_descriptors", std::bind(&ModuleUnitTester::signalCreationTest, this) },
                {"do_get_metric_descriptors", std::bind(&ModuleUnitTester::metricCreationTest, this) },
                {"do_get_solver_descriptors", std::bind(&ModuleUnitTester::solverCreationTest, this) },
                {"do_get_approximator_descriptors", std::bind(&ModuleUnitTester::approxCreationTest, this) }
        };
        /// Mapped do_get_*_descriptors methods to tests of their input parameters validation
        std::map<std::string, std::function<HRESULT(const std::string&)>> m_descriptorsToParamsTests {
                {"do_get_filter_descriptors", std::bind(&ModuleUnitTester::descriptorsParamsTest<scgms::TGet_Filter_Descriptors , scgms::TFilter_Descriptor>, this, std::placeholders::_1) },
                {"do_get_signal_descriptors", std::bind(&ModuleUnitTester::descriptorsParamsTest<scgms::TGet_Signal_Descriptors , scgms::TSignal_Descriptor>, this, std::placeholders::_1) },
                {"do_get_metric_descriptors", std::bind(&ModuleUnitTester::descriptorsParamsTest<scgms::TGet_Metric_Descriptors , scgms::TMetric_Descriptor>, this, std::placeholders::_1) },
                {"do_get_solver_descriptors", std::bind(&ModuleUnitTester::descriptorsParamsTest<scgms::TGet_Solver_Descriptors , scgms::TSolver_Descriptor>, this, std::placeholders::_1) },
                {"do_get_approximator_descriptors", std::bind(&ModuleUnitTester::descriptorsParamsTest<scgms::TGet_Approx_Descriptors , scgms::TApprox_Descriptor>, this, std::placeholders::_1) },
        };
        /// Path to the tested module
        std::string m_modulePath;
        /// Dynamic library of the tested module
        CDynamic_Library m_moduleLibrary;
        /// Vector of do_get_*_descriptors methods found in the tested module
        std::vector<std::string> m_foundDescriptorsMethods;
    public:     // public methods
        ModuleUnitTester() = default;
        /// Executes all tests for the loaded module
        void executeModuleTests();
        /**
         * Every module has to implement at least one do_get_*_descriptors method.
         * This test checks if tested module implements one.
         * @return S_OK if at least one descriptors method is found, otherwise E_FAIL
         */
        HRESULT descriptorsMethodsTest();
        /**
         * This test checks whether the do_get_*_descriptors methods validate their input parameters properly.
         * @return S_OK if all input parameters were correctly validated, otherwise E_FAIL
         */
        HRESULT descriptorsMethodParamsTest();
        /**
         * When module implements a do_get_*_descriptors method, it also has to implement corresponding
         * do_create_* factory method.
         * This test checks if tested module implements one.
         * @return S_OK if all factory methods are found, otherwise false
         */
        HRESULT factoryMethodsTest();
        /**
         * When module implements do_create_* factory method, a valid object should be constructed when this method is called.
         * This test checks if every factory method is able to create a valid object.
         * @return S_OK if valid objects were successfully created, otherwise false
         */
        HRESULT entityCreationTest();
        /**
         * Loads given module for future test execution.
         * @param modulePath path to the module
         * @return true, if successfully loaded, otherwise false
         */
        bool loadModule(const std::string& modulePath);
        /**
         * Checks if module on given path has already been tested.
         * @param module module to check
         * @return true if the module was already tested, otherwise false
         */
        static bool isModuleTested(const std::string& module) ;

    private:
        HRESULT shutDownTest() override;

        template<typename T, typename D>
        HRESULT descriptorsParamsTest(const std::string& symbolName);
        HRESULT filterCreationTest();
        HRESULT signalCreationTest();
        HRESULT metricCreationTest();
        HRESULT solverCreationTest();
        HRESULT approxCreationTest();
    };

    /**
     * Generic class containing information about any tested entity.
     * @tparam T Type of the entity we are testing
     */
    template<typename T>
    class EntityUnitTester : public TestRunner {
    private:    // private attributes
        /// GUID of tested entity
        const GUID m_entityGuid;
        /// Tested entity itself
        T* m_testedEntity = nullptr;
        /// Dynamic library of the tested entity
        CDynamic_Library m_entityLibrary;
        /// Path to the entity library
        std::wstring m_libraryPath;

    public:     // public methods
        explicit EntityUnitTester(const GUID& entityGuid) : m_entityGuid(entityGuid) {}
        virtual ~EntityUnitTester() = default;
        /// Every derived class has to override this method to execute all tests
        virtual void executeAllTests() = 0;

        /**
         * Checks whether the entity is loaded or not.
         * @return @a true if the entity is loaded, otherwise @a false
         */
        bool isEntityLoaded() const noexcept {
            return m_testedEntity != nullptr;
        }

        /**
         * Returns the wrapper of the entity library.
         * @return entity library wrapper
         */
        CDynamic_Library& getEntityLib() noexcept {
            return m_entityLibrary;
        }

        /**
         * Returns the entity @a GUID.
         * @return entity @a GUID
         */
        const GUID& getEntityGuid() const noexcept {
            return m_entityGuid;
        }

        /**
         * Loads library of tested entity.
         */
        void loadEntityLibrary() {
            if (m_libraryPath.empty()) {
                const wchar_t* file_name = GuidFileMapper::GetInstance().getFileName(m_entityGuid);
                m_libraryPath = std::wstring(file_name) + cnst::LIB_EXTENSION;
            }

            m_entityLibrary.Load(m_libraryPath);

            if (!m_entityLibrary.Is_Loaded()) {
                std::wcerr << L"Couldn't load " << m_libraryPath << " library!\n";
                Logger::getInstance().error(L"Couldn't load " + std::wstring(m_libraryPath) + L" library.");
            }
        }

        /// Loads tested entity. Has to be implemented by derived classes.
        virtual void loadEntity() = 0;

        template<typename C, typename D>
        const wchar_t* getEntityName(const std::string& symbolName) {
            if (!getEntityLib().Is_Loaded()) {
                loadEntityLibrary();
            }

            D* descriptor = tester::getEntityDescriptor<C, D>(m_entityLibrary, m_entityGuid, symbolName);
            if (!descriptor) {
                return nullptr;
            }

            return descriptor->description;
        }
    protected:  // protected methods

        void setEntityLib(const std::wstring& libPath) {
            if (m_entityLibrary.Is_Loaded()) {
                Logger::getInstance().error(L"Attempt to override loaded library! Forbidding...");
                return;
            }

            if (m_entityLibrary.Load(libPath)) {
                m_libraryPath = libPath;
            } else {
                Logger::getInstance().error(std::wstring(L"Error while loading library ") + libPath);
            }
        }

        void setLibraryPath(const std::wstring& libraryPath) noexcept {
            m_libraryPath = libraryPath;
        }

        const std::wstring& getLibraryPath() const {
            return m_libraryPath;
        }

        T* getTestedEntity() noexcept {
            return m_testedEntity;
        }

        void setTestedEntity(T *entity) {
            if (m_testedEntity) {
                m_testedEntity->Release();
            }

            m_testedEntity = entity;
        }

        /// We need special behavior of the test execution sequence, so we override this method
        void runTest(const std::function<HRESULT ()> &test) override {
            if (!m_entityLibrary.Is_Loaded()) {
                loadEntityLibrary();
            }

            if (!isEntityLoaded()) {
                loadEntity();
            }

            if (isEntityLoaded()) {
                m_lastTestResult = test();
                shutDownTest();     /// Need to check, because filter will be unloaded in case of TIMEOUT
            } else {
                Logger::getInstance().error(L"Filter is not loaded! Test will not be executed.");
                m_lastTestResult = E_FAIL;
            }

            m_testCv.notify_all();
        }
    };

    /**
     * @a scgms::IFilter interface is implemented by multiple entities. This enumeration indicates which entities can
     * implement the interface.
     */
    enum class EntityType : int16_t {
        FILTER = 0,
        MODEL
    };
    /**
     * Contains generic tests for any object implementing @a scgms::IFilter.
     */
class FilterUnitTester : public EntityUnitTester<scgms::IFilter> {
    private:    // private attributes
        /// What entity, implementing the @a scgms::IFilter interface, are we testing
        EntityType m_entityType;
        /// Our custom filter for testing
        TestFilter m_testFilter;

    public:     // public methods
        FilterUnitTester(GUID guid, const EntityType& type);
        virtual ~FilterUnitTester() = default;
        /**
         * If any filter is created, executes an info event upon it. Tested filter should send the info event to
         * the output filter, which will be TestFilter. If the event is not received by TestFilter, test ends with an error.
         */
        HRESULT infoEventTest();
        /**
         * If any filter is created, executes a warning event upon it. Tested filter should send the warning event to
         * the output filter, which will be TestFilter. If the event is not received by TestFilter, test ends with an error.
         */
        HRESULT warningEventTest();
        /**
         * If any filter is created, executes an error event upon it. Tested filter should send the error event to
         * the output filter, which will be TestFilter. If the event is not received by TestFilter, test ends with an error.
         */
        HRESULT errorEventTest();
        /**
         * If any filter is created, executes warm reset event upon it. Tested filter should send the warm reset event to
         * the output filter, which will be TestFilter. If the event is not received by TestFilter, test ends with an error.
         */
        HRESULT warmResetEventTest();
        /**
         * If any filter is created, executes shut down event upon it. Tested filter should send the shut down event to
         * the output filter, which will be TestFilter. If the event is not received by TestFilter, test ends with an error.
         * Shut down event ends filter's execution routine and it should not accept any other event after shutting down - it's Execute
         * method should return an error. If not, the test fails.
         */
        HRESULT shutDownEventTest();
        /**
         * Invokes test method passed as a parameter. Invoked method has to take in two parameters and return HRESULT as a return value.
         * The first parameter of invoked method is string and second one is HRESULT. Main purpose of this method is to invoke
         * configuration test method which takes in configuration string as first parameter and expected result as second parameter.
         * @param testName name of the test which will be displayed in logs
         * @param test method to be invoked by this method
         */
        void executeConfigTest(const std::wstring& testName, const tester::FilterConfig& configuration, HRESULT expectedResult);
        void executeAllTests() override;
        void executeGenericTests();
        /// Executes all tests for a specific filter. Needs to be implemented by derived class.
        virtual void executeSpecificTests() = 0;
    protected:
        /**
         * Executes configuration test based on given parameters. Every filter should be successfully configured only if all parts
         * of the configuration are provided. Configuration test will be evaluated as successful and return S_OK only if the result of configuration
         * is identical with given expectedResult parameter, otherwise will return E_FAIL.
         *
         * @param memory configuration string
         * @param expectedResult expected result of this configuration
         * @return S_OK only if the result of given configuration is identical with given expectedResult parameter, otherwise E_FAIL
         */
        HRESULT configurationTest(const tester::FilterConfig& config, HRESULT expectedResult);

        /// Creates shut down event and executes it with tested filter
        HRESULT shutDownTest() override;
        /**
         * Configures tested filter with given configuration and returns the result.
         * @param configuration filter configuration
         * @return configuration result
         */
        HRESULT configureFilter(const tester::FilterConfig& configuration);
        TestFilter& getTestFilter();
    private: // private methods
        HRESULT informativeEventsTest(const scgms::NDevice_Event_Code& eventCode);
        void loadEntity() override;

        HRESULT runConfigTestInThread(const tester::FilterConfig& configuration, HRESULT expectedResult);
        void runConfigTest(const tester::FilterConfig& configuration, HRESULT expectedResult);
    };

    /**
     * Executes module tests of a module on given path
     * @param modulePath path to the module to test
     */
    void executeModuleTests(const std::string& modulePath);
}


#endif // !_GENERIC_UNIT_TESTER_H_

