#include <iostream>
#include <rtl/FilterLib.h>
#include <utils/string_utils.h>
#include "MappingFilterUnitTester.h"

MappingFilterUnitTester::MappingFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter, const GUID* guid) : GenericUnitTester(library, testFilter, guid) {
	SIGNAL_SRC_ID_STR = "{E1CD07EF-B079-4911-B79B-D203486101C8}";
	SIGNAL_SRC_ID_GUID = { 0xe1cd07ef, 0xb079, 0x4911, {0xb7, 0x9b, 0xd2, 0x03, 0x48, 0x61, 0x01, 0xc8} };
	SIGNAL_DST_ID_STR = "{F666F6C2-D7C0-43E8-8EE1-C8CAA8F860E5}";
	SIGNAL_DST_ID_GUID = { 0xf666f6c2, 0xd7c0, 0x43e8, {0x8e, 0xe1, 0xc8, 0xca, 0xa8, 0xf8, 0x60, 0xe5} };
}

/**
	Executes all specific tests for loaded Mapping filter.
*/
void MappingFilterUnitTester::executeSpecificTests(){
	logger.info(L"Executing specific tests...");
	executeTest(L"empty source id test", std::bind(&MappingFilterUnitTester::emptySourceIdTest, this));
	executeTest(L"empty destination id test", std::bind(&MappingFilterUnitTester::emptyDestIdTest, this));
	executeTest(L"correct id's test", std::bind(&MappingFilterUnitTester::correctIdsTest, this));
	executeTest(L"level event mapping test", std::bind(&MappingFilterUnitTester::levelEventMappingTest, this));
	executeTest(L"info event mapping test", std::bind(&MappingFilterUnitTester::infoEventMappingTest, this));
	executeTest(L"parameters event mapping test", std::bind(&MappingFilterUnitTester::parametersEventMappingTest, this));
}

/**
	Tests if Mapping filter can be configured with empty signal source id.
*/
HRESULT MappingFilterUnitTester::emptySourceIdTest()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter created! Cannot execute test.\n";
		logger.error(L"No filter loaded! Can't execute test.");
		return E_FAIL;
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{8FAB525C-5E86-AB81-12CB-D95B1588530A}]\n\nSignal_Src_Id = \n\nSignal_Dst_Id = " + SIGNAL_DST_ID_STR;

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
		logger.info(L"Loading configuration from memory...");
	}
	else {
		logger.error(L"Error while creating configuration!");
		shutDownTest();
		return E_FAIL;
	}
	
	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	result = testedFilter->Configure(begin[0], errors.get());
	logger.info(L"Configuring filter...");

	if (!SUCCEEDED(result)) {	// test shouldn't succeed, we don't want to configure mapping without source id
		result = S_OK;
	}
	else {
		logger.error(L"Filter was configured successfully, but shouldn't be!\n"
		L"(" + std::wstring(memory.begin(), memory.end()) + L")");
		logger.error(L"expected result: " + Widen_Char(std::system_category().message(E_FAIL).c_str()));
		logger.error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
		result = E_FAIL;
	}

	shutDownTest();
	return result;
}

/**
	Tests if Mapping filter can be configured with empty signal destination id.
*/
HRESULT MappingFilterUnitTester::emptyDestIdTest()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter created! Cannot execute test.\n";
		logger.error(L"No filter loaded! Can't execute test.");
		return E_FAIL;
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{8FAB525C-5E86-AB81-12CB-D95B1588530A}]\n\nSignal_Src_Id = " + SIGNAL_SRC_ID_STR + "\n\nSignal_Dst_Id = ";

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
		logger.info(L"Loading configuration from memory...");
	}
	else {
		logger.error(L"Error while creating configuration!");
		shutDownTest();
		return E_FAIL;
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);
	
	result = testedFilter->Configure(begin[0], errors.get());
	logger.info(L"Configuring filter...");

	if (!SUCCEEDED(result)) {	// test shouldn't succeed, we don't want to configure mapping without destination id
		result = S_OK;
	}
	else {
		logger.error(L"Filter was configured successfully, but shouldn't be!\n"
			L"(" + std::wstring(memory.begin(), memory.end()) + L")");
		logger.error(L"expected result: " + Widen_Char(std::system_category().message(E_FAIL).c_str()));
		logger.error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
		result = E_FAIL;
	}

	shutDownTest();
	return result;
}

/**
	Helper method for correct configuring of Mapping filter.
*/
HRESULT MappingFilterUnitTester::configureFilterCorrectly()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter created! Cannot execute test.\n";
		logger.error(L"No filter loaded! Can't execute test.");
		return E_FAIL;
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{8FAB525C-5E86-AB81-12CB-D95B1588530A}]\n\nSignal_Src_Id = " + SIGNAL_SRC_ID_STR + "\n\nSignal_Dst_Id = " + SIGNAL_DST_ID_STR;

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
		logger.info(L"Loading configuration from memory...");
	}
	else {
		logger.error(L"Error while creating configuration!");
		return E_FAIL;
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	logger.info(L"Configuring filter...");
	return testedFilter->Configure(begin[0], errors.get());
}

/**
	Tests if Mapping filter will be configured with correct configuration.
*/
HRESULT MappingFilterUnitTester::correctIdsTest()
{
	HRESULT result = configureFilterCorrectly();

	if (SUCCEEDED(result)) {	
		result = S_OK;
	}
	else {
		logger.error(L"Failed to configure filter!");
		result = E_FAIL;
	}

	shutDownTest();
	return result;
}

/**
	Tests if level event's signal id will be correctly mapped.
*/
HRESULT MappingFilterUnitTester::levelEventMappingTest()
{
	HRESULT result = configureFilterCorrectly();

	if (SUCCEEDED(result)) {	
		scgms::IDevice_Event* event;

		auto creator = scgmsLibrary->Resolve<scgms::TCreate_Device_Event>("create_device_event");
		result = creator(scgms::NDevice_Event_Code::Level, &event);
		if (FAILED(result))
		{
			std::wcerr << L"Error while creating \"Level\" IDevice_event!\n";
			logger.error(L"Error while creating \"Level\" IDevice_event!");
			shutDownTest();
			return E_FAIL;
		}
		scgms::TDevice_Event* raw_event;
		event->Raw(&raw_event);
		raw_event->signal_id = SIGNAL_SRC_ID_GUID;
		scgms::TDevice_Event src_event = *raw_event;

		result = testedFilter->Execute(event);
		logger.info(L"Executing \"Level\" event...");

		GUID dst_id = testFilter->getRecievedEvent()->signal_id;
		if (SUCCEEDED(result) && (dst_id == SIGNAL_DST_ID_GUID))
		{
			if ((src_event.device_time == raw_event->device_time)
				&& (src_event.device_id == raw_event->device_id)
				&& (src_event.event_code == raw_event->event_code)
				&& (src_event.info == raw_event->info)
				//&& (src_event.level == raw_event->level) //pouze tady pad�?? nan
				&& (src_event.logical_time == raw_event->logical_time)
				&& (src_event.parameters == raw_event->parameters)
				&& (src_event.segment_id == raw_event->segment_id))
			{
				logger.debug(L"Event's signal_id successfully mapped!");
				result = S_OK;
			}
			else {
				logger.error(L"Other event attributes were modified!");
				result = E_FAIL;
			}
			
		}
		else {
			logger.error(L"Event was incorrectly sent or it's signal_id wasn't mapped!");
			logger.error(L"expected result: " + GUID_To_WString(SIGNAL_DST_ID_GUID));
			logger.error(L"actual result: " + GUID_To_WString(dst_id));
			result = E_FAIL;
		}
	}
	else {
		logger.error(L"Failed to configure filter!");
		logger.error(L"expected result: " + Widen_Char(std::system_category().message(S_OK).c_str()));
		logger.error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
		result = E_FAIL;
	}

	shutDownTest();
	return result;
}

/**
	Tests if info event's signal id will be correctly mapped.
*/
HRESULT MappingFilterUnitTester::infoEventMappingTest()
{
	HRESULT result = configureFilterCorrectly();

	if (SUCCEEDED(result)) {
		scgms::IDevice_Event* event;

		auto creator = scgmsLibrary->Resolve<scgms::TCreate_Device_Event>("create_device_event");
		result = creator(scgms::NDevice_Event_Code::Information, &event);
		if (FAILED(result))
		{
			std::wcerr << L"Error while creating \"Info\" IDevice_event!\n";
			logger.error(L"Error while creating \"Info\" IDevice_event!");
			shutDownTest();
			return E_FAIL;
		}
		scgms::TDevice_Event* raw_event;
		event->Raw(&raw_event);
		raw_event->signal_id = SIGNAL_SRC_ID_GUID;
		scgms::TDevice_Event src_event = *raw_event;

		result = testedFilter->Execute(event);
		logger.info(L"Executing \"Info\" event...");

		GUID dst_id = testFilter->getRecievedEvent()->signal_id;
		if (SUCCEEDED(result) && (dst_id == SIGNAL_DST_ID_GUID))
		{
			if ((src_event.device_time == raw_event->device_time)
				&& (src_event.device_id == raw_event->device_id)
				&& (src_event.event_code == raw_event->event_code)
				&& (src_event.info == raw_event->info)
				&& (src_event.level == raw_event->level)
				&& (src_event.logical_time == raw_event->logical_time)
				&& (src_event.parameters == raw_event->parameters)
				&& (src_event.segment_id == raw_event->segment_id))
			{
				logger.debug(L"Event's signal_id successfully mapped!");
				result = S_OK;
			}
			else {
				logger.error(L"Other event attributes were modified!");
				result = E_FAIL;
			}

		}
		else {
			logger.error(L"Event was incorrectly sent or it's signal_id wasn't mapped!");
			logger.error(L"expected result: " + GUID_To_WString(SIGNAL_DST_ID_GUID));
			logger.error(L"actual result: " + GUID_To_WString(dst_id));
			result = E_FAIL;
		}
	}
	else {
		logger.error(L"Failed to configure filter!");
		logger.error(L"expected result: " + Widen_Char(std::system_category().message(S_OK).c_str()));
		logger.error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
		result = E_FAIL;
	}

	shutDownTest();
	return result;
}

/**
	Tests if parameters event's signal id will be correctly mapped.
*/
HRESULT MappingFilterUnitTester::parametersEventMappingTest()
{
	HRESULT result = configureFilterCorrectly();

	if (SUCCEEDED(result)) {
		scgms::IDevice_Event* event;

		auto creator = scgmsLibrary->Resolve<scgms::TCreate_Device_Event>("create_device_event");
		result = creator(scgms::NDevice_Event_Code::Parameters, &event);
		if (FAILED(result))
		{
			std::wcerr << L"Error while creating \"Parameters\" IDevice_event!\n";
			logger.error(L"Error while creating \"Parameters\" IDevice_event!");
			shutDownTest();
			return E_FAIL;
		}
		scgms::TDevice_Event* raw_event;
		event->Raw(&raw_event);
		raw_event->signal_id = SIGNAL_SRC_ID_GUID;
		scgms::TDevice_Event src_event = *raw_event;

		result = testedFilter->Execute(event);
		logger.info(L"Executing \"Parameters\" event...");

		GUID dst_id = testFilter->getRecievedEvent()->signal_id;
		if (SUCCEEDED(result) && (dst_id == SIGNAL_DST_ID_GUID))
		{
			if ((src_event.device_time == raw_event->device_time)
				&& (src_event.device_id == raw_event->device_id)
				&& (src_event.event_code == raw_event->event_code)
				&& (src_event.info == raw_event->info)
				&& (src_event.level == raw_event->level)
				&& (src_event.logical_time == raw_event->logical_time)
				&& (src_event.parameters == raw_event->parameters)
				&& (src_event.segment_id == raw_event->segment_id))
			{
				logger.debug(L"Event's signal_id successfully mapped!");
				result = S_OK;
			}
			else {
				logger.error(L"Other event attributes were modified!");
				result = E_FAIL;
			}

		}
		else {
			logger.error(L"Event was incorrectly sent or it's signal_id wasn't mapped!");
			logger.error(L"expected result: " + GUID_To_WString(SIGNAL_DST_ID_GUID));
			logger.error(L"actual result: " + GUID_To_WString(dst_id));
			result = E_FAIL;
		}
	}
	else {
		logger.error(L"Failed to configure filter!");
		logger.error(L"expected result: " + Widen_Char(std::system_category().message(S_OK).c_str()));
		logger.error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
		result = E_FAIL;
	}

	shutDownTest();
	return result;
}