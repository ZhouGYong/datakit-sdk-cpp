#include "ft-sdk-unittest.h"
#include "../../datakit-sdk-cpp/ft-sdk/Include/FTSDKDataContracts.h"
#include "../../datakit-sdk-cpp/ft-sdk/LoggerManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/RUMManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/FTSDKConfigManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/LineDBManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/LineProtocolBuilder.h"
#include "../../datakit-sdk-cpp/ft-sdk/TraceManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/InternalStructs.h"
#include "../../datakit-sdk-cpp/ft-sdk/FTSDKConstants.h"
#include "TestHelper.h"
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

// Turn off iterator debugging as it makes the compiler very slow on large methods in debug builds
//#define _HAS_ITERATOR_DEBUGGING 0

using namespace com::ft::sdk;
using namespace _test::helper;

class RUMManagerTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::cout << "\nSetUp..." << std::endl;
		internal::LoggerManager::getInstance().init();
		internal::FTSDKConfigManager::getInstance().enableOfflineMode();
		internal::FTSDKConfigManager::getInstance().getGeneralConfig().setEnableFileDBCache(true);
		internal::FTSDKConfigManager::getInstance().getRUMConfig().setSamplingRate(1.0f);

		internal::LineDBManager::getInstance().init();
	}

	void TearDown() override {
		std::cout << "TearDown...\n" << std::endl;
	}

	void generateSimpleView()
	{
		internal::RUMManager::getInstance().startView(FIRST_VIEW);
		internal::RUMManager::getInstance().stopView();

		waitForCompleted();
	}
};

TEST_F(RUMManagerTest, TestViewGeneration) 
{
	internal::RUMManager::getInstance().startView(FIRST_VIEW);
	
	internal::RUMManager::getInstance().startAction("just4test", "click");
	internal::RUMManager::getInstance().addLongTask("test long task", 100010);
	internal::RUMManager::getInstance().addError("test error", "first error", RUMErrorType::NATIVE_CRASH, AppState::UNKNOWN);
	internal::RUMManager::getInstance().addError("test error", "second error", RUMErrorType::NETWORK_ERROR, AppState::UNKNOWN);
	internal::RUMManager::getInstance().stopAction();

	internal::RUMManager::getInstance().stopView();

	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	internal::RUMManager::getInstance().startView(SECOND_VIEW);
	internal::RUMManager::getInstance().stopView();

	waitForCompleted();

	auto runRecs = internal::LineDBManager::getInstance().queryLineFromDB(DataType::RUM_APP);

	EXPECT_TRUE(runRecs.size() == 1);
	EXPECT_TRUE(runRecs[0]->dataType == DataType::RUM_APP);

	auto vtMeas = internal::LineProtocolBuilder::getInstance().decode(runRecs[0]->rawLine);
	EXPECT_TRUE(vtMeas.size() == 6);

	auto vtViews = getRumItemsByName(constants::FT_MEASUREMENT_RUM_VIEW, FIRST_VIEW, vtMeas);
	EXPECT_TRUE(vtViews.size() == 1);

	if (vtViews.size() > 0)
	{
		//EXPECT_TRUE((std::any_cast<std::int64_t>(vtViews[0]->fields[constants::KEY_RUM_VIEW_ACTION_COUNT])), 1);
		VERIFY_RUM_FIELD_INT((vtViews[0]), (constants::KEY_RUM_VIEW_ACTION_COUNT), 1);
		VERIFY_RUM_FIELD_INT((vtViews[0]), (constants::KEY_RUM_VIEW_LONG_TASK_COUNT), 1);
		//auto cnt = std::any_cast<std::int64_t>(vtViews[0]->fields[constants::KEY_RUM_VIEW_RESOURCE_COUNT]);
		VERIFY_RUM_FIELD_INT((vtViews[0]), (constants::KEY_RUM_VIEW_RESOURCE_COUNT), 0);
		VERIFY_RUM_FIELD_INT((vtViews[0]), (constants::KEY_RUM_VIEW_ERROR_COUNT), 2);
	}

	auto vtViewsSecond = getRumItemsByName(constants::FT_MEASUREMENT_RUM_VIEW, SECOND_VIEW, vtMeas);
	EXPECT_TRUE(vtViewsSecond.size() == 1);
	if (vtViewsSecond.size() > 0)
	{
		VERIFY_RUM_FIELD_INT((vtViewsSecond[0]), (constants::KEY_RUM_VIEW_ACTION_COUNT), 0);
		VERIFY_RUM_FIELD_INT((vtViewsSecond[0]), (constants::KEY_RUM_VIEW_LONG_TASK_COUNT), 0);
		VERIFY_RUM_FIELD_INT((vtViewsSecond[0]), (constants::KEY_RUM_VIEW_RESOURCE_COUNT), 0);
		VERIFY_RUM_FIELD_INT((vtViewsSecond[0]), (constants::KEY_RUM_VIEW_ERROR_COUNT), 0);
	}

	const std::string firstViewID = std::any_cast<std::string>(vtViews[0]->tags[constants::KEY_RUM_VIEW_ID]);
	const std::string secondViewID = std::any_cast<std::string>(vtViewsSecond[0]->tags[constants::KEY_RUM_VIEW_ID]);
	EXPECT_TRUE(firstViewID != secondViewID);

	// view map test
	const std::string firstViewReferrer = std::any_cast<std::string>(vtViews[0]->tags[constants::KEY_RUM_VIEW_REFERRER]);
	const std::string secondViewReferrer = std::any_cast<std::string>(vtViewsSecond[0]->tags[constants::KEY_RUM_VIEW_REFERRER]);
	EXPECT_TRUE(firstViewReferrer == ROOT_RUM_VIEW_ID);
	EXPECT_TRUE(secondViewReferrer == FIRST_VIEW);

	auto vtActs = getRumItemsByName(constants::FT_MEASUREMENT_RUM_ACTION, "just4test", vtMeas);
	EXPECT_TRUE(vtActs.size() == 1);
	if (vtActs.size() > 0)
	{
		VERIFY_RUM_FIELD_INT((vtActs[0]), (constants::KEY_RUM_ACTION_LONG_TASK_COUNT), 1);
		VERIFY_RUM_FIELD_INT((vtActs[0]), (constants::KEY_RUM_ACTION_RESOURCE_COUNT), 0);
		VERIFY_RUM_FIELD_INT((vtActs[0]), (constants::KEY_RUM_ACTION_ERROR_COUNT), 2);
	}

	auto vtErrs = getRumItemsByName(constants::FT_MEASUREMENT_RUM_ERROR, _GET_ALL_RECORDS_, vtMeas);
	EXPECT_TRUE(vtErrs.size() == 2);

}

TEST_F(RUMManagerTest, TestViewGenerationWithSampleRateZero)
{
	internal::FTSDKConfigManager::getInstance().getRUMConfig().setSamplingRate(0.0f);
	internal::RUMManager::getInstance().init();

	generateSimpleView();

	auto runRecs = internal::LineDBManager::getInstance().queryLineFromDB(DataType::RUM_APP);
	EXPECT_TRUE(runRecs.size() == 0);
}

TEST_F(RUMManagerTest, TestViewGenerationWithSampleRate100)
{
	internal::FTSDKConfigManager::getInstance().getRUMConfig().setSamplingRate(100.0f);
	internal::FTSDKConfigManager::getInstance().unbindUserData();
	internal::RUMManager::getInstance().init();

	generateSimpleView();

	auto runRecs = internal::LineDBManager::getInstance().queryLineFromDB(DataType::RUM_APP);
	EXPECT_TRUE(runRecs.size() > 0);
	if (runRecs.size() > 0)
	{
		EXPECT_TRUE(runRecs[0]->dataType == DataType::RUM_APP);
	}

	auto vtMeas = internal::LineProtocolBuilder::getInstance().decode(runRecs[0]->rawLine);
	EXPECT_TRUE(vtMeas.size() == 1);

	auto vtViews = getRumItemsByName(constants::FT_MEASUREMENT_RUM_VIEW, FIRST_VIEW, vtMeas);
	EXPECT_TRUE(vtViews.size() == 1);

	if (vtViews.size() > 0)
	{
		// not user-binded
		EXPECT_TRUE(vtViews[0]->tags.find(constants::KEY_RUM_USER_ID) == vtViews[0]->tags.end());
	}
}

TEST_F(RUMManagerTest, TestViewGenerationWithUserBinded)
{
	internal::FTSDKConfigManager::getInstance().getRUMConfig().setSamplingRate(100);
	//internal::FTSDKConfigManager::getInstance().enableRUMUserBinding(true);

	UserData ud;
	ud.setId("test_user_id");
	ud.setName("test_user_name");
	internal::FTSDKConfigManager::getInstance().bindUserData(ud);

	internal::RUMManager::getInstance().init();

	generateSimpleView();

	auto runRecs = internal::LineDBManager::getInstance().queryLineFromDB(DataType::RUM_APP);
	EXPECT_TRUE(runRecs.size() == 1);
	EXPECT_TRUE(runRecs[0]->dataType == DataType::RUM_APP);

	auto vtMeas = internal::LineProtocolBuilder::getInstance().decode(runRecs[0]->rawLine);
	EXPECT_TRUE(vtMeas.size() == 1);

	auto vtViews = getRumItemsByName(constants::FT_MEASUREMENT_RUM_VIEW, FIRST_VIEW, vtMeas);
	EXPECT_TRUE(vtViews.size() == 1);

	if (vtViews.size() > 0)
	{
		// user-binded
		EXPECT_TRUE(vtViews[0]->tags.find(constants::KEY_RUM_USER_ID) != vtViews[0]->tags.end());
	}
}


TEST_F(RUMManagerTest, TestResourceGeneration)
{
	internal::FTSDKConfigManager::getInstance().getRUMConfig().setSamplingRate(1.0f);
	internal::RUMManager::getInstance().init();

	internal::FTSDKConfigManager::getInstance().getTraceConfig().setSamplingRate(1.0f);
	internal::TraceManager::getInstance().initialize(internal::FTSDKConfigManager::getInstance().getTraceConfig());

	internal::RUMManager::getInstance().startView(FIRST_VIEW);

	std::string resId = "resource id 1111";
	internal::RUMManager::getInstance().startResource(resId);
	internal::RUMManager::getInstance().stopResource(resId);
	NetStatus status;
	status.dnsStartTime = 0;
	status.dnsEndTime = 100002;
	status.tcpStartTime = status.dnsEndTime;
	status.tcpEndTime = 300023;
	status.sslStartTime = status.tcpEndTime;
	status.sslEndTime = 3232122;
	status.fetchStartTime = 4232122;
	status.responseStartTime = 5632122;
	status.responseEndTime = 13632122;

	ResourceParams params;
	params.resourceMethod = "GET";
	params.resourceStatus = HTTP_STATUS::HTTP_OK;
	params.responseConnection = "Keep-Alive";
	params.responseContentEncoding = "UTF-8";

	internal::PropagationUrl url = internal::PropagationUrl::parse("https://www.example.com/test.html?param=1");
	internal::HttpUrl internalUrl{ url.getHost(), url.getPath(), url.getPort() };
	auto traceHdr = internal::TraceManager::getInstance().getTraceHeader(resId, "https://www.example.com/test.html?param=1");

	internal::RUMManager::getInstance().addResource(resId, params, status);

	internal::RUMManager::getInstance().stopView();

	waitForCompleted();

	auto runRecs = internal::LineDBManager::getInstance().queryLineFromDB(DataType::RUM_APP);
	EXPECT_TRUE(runRecs.size() > 0);
	if (runRecs.size() > 0)
	{
		EXPECT_TRUE(runRecs[0]->dataType == DataType::RUM_APP);
	}

	auto vtMeas = internal::LineProtocolBuilder::getInstance().decode(runRecs[0]->rawLine);
	EXPECT_TRUE(vtMeas.size() == 2);

	auto vtViews = getRumItemsByName(constants::FT_MEASUREMENT_RUM_VIEW, FIRST_VIEW, vtMeas);
	EXPECT_TRUE(vtViews.size() == 1);

	auto vtRess = getRumItemsByName(constants::FT_MEASUREMENT_RUM_RESOURCE, _GET_ALL_RECORDS_, vtMeas);
	EXPECT_TRUE(vtRess.size() == 1);
	if (vtRess.size() > 0)
	{
		VERIFY_RUM_FIELD_INT((vtRess[0]), (constants::KEY_RUM_RESOURCE_DNS), (status.dnsEndTime - status.dnsStartTime));
	}
}