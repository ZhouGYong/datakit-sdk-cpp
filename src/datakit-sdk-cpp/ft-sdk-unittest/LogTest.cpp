#include "ft-sdk-unittest.h"
#include "../../datakit-sdk-cpp/ft-sdk/Include/FTSDKDataContracts.h"
#include "../../datakit-sdk-cpp/ft-sdk/LoggerManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/RUMManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/FTSDKConfigManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/LineDBManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/LineProtocolBuilder.h"
#include "../../datakit-sdk-cpp/ft-sdk/LogPipeManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/FTSDKConstants.h"
#include "../../datakit-sdk-cpp/ft-sdk/Utils.h"
#include "TestHelper.h"
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

// Turn off iterator debugging as it makes the compiler very slow on large methods in debug builds
//#define _HAS_ITERATOR_DEBUGGING 0

using namespace com::ft::sdk;
using namespace _test::helper;

class UserLogTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::cout << "\nSetUp..." << std::endl;
		internal::LoggerManager::getInstance().init();
		internal::FTSDKConfigManager::getInstance().enableOfflineMode();
		internal::FTSDKConfigManager::getInstance().getGeneralConfig().setEnableFileDBCache(true);

		internal::LineDBManager::getInstance().init();
	}

	void TearDown() override {
		std::cout << "TearDown...\n" << std::endl;

		internal::RUMManager::getInstance().clear();
	}

};

TEST_F(UserLogTest, TesUserLogWithSampleRateZero) {
	internal::FTSDKConfigManager::getInstance().getLogPipeConfig().setEnableCustomLog(true).setSamplingRate(0);
	internal::LogPipeManager::getInstance().init(internal::FTSDKConfigManager::getInstance().getLogPipeConfig());

	std::string logContent = "----logInsertDataTest----";
	internal::LogPipeManager::getInstance().addLog(logContent, LogLevel::INFO);

	waitForCompleted();

	auto runRecs = internal::LineDBManager::getInstance().queryLineFromDB(DataType::LOG);

	EXPECT_TRUE(runRecs.size() == 0);
}

TEST_F(UserLogTest, TesUserLogWithoutRum) {
	internal::FTSDKConfigManager::getInstance().getLogPipeConfig().setEnableCustomLog(true).setSamplingRate(1);
	internal::LogPipeManager::getInstance().init(internal::FTSDKConfigManager::getInstance().getLogPipeConfig());

	std::string logContent = "----logInsertDataTest----";
	LogLevel level = LogLevel::ERR;
	internal::LogPipeManager::getInstance().addLog(logContent, level);

	waitForCompleted();

	auto runRecs = internal::LineDBManager::getInstance().queryLineFromDB(DataType::LOG);

	EXPECT_TRUE(runRecs.size() == 1);
	if (runRecs.size() > 0)
	{
		EXPECT_TRUE(runRecs[0]->dataType == DataType::LOG);
	}

	auto vtMeas = internal::LineProtocolBuilder::getInstance().decode(runRecs[0]->rawLine);
	EXPECT_TRUE(vtMeas.size() == 1);

	auto vtLogs = getRumItemsByName(constants::FT_LOG_DEFAULT_MEASUREMENT, _GET_ALL_RECORDS_, vtMeas);
	EXPECT_TRUE(vtLogs.size() == 1);

	if (vtLogs.size() > 0)
	{
		VERIFY_RUM_FIELD_STR((vtLogs[0]), (constants::KEY_MESSAGE), "\"" + logContent + "\"");
		VERIFY_RUM_TAG((vtLogs[0]), (constants::KEY_STATUS), internal::utils::convertToLowerCase(EnumToString(level)));

		EXPECT_TRUE(vtLogs[0]->tags.find(constants::KEY_RUM_VIEW_NAME) == vtLogs[0]->tags.end());
	}
}

TEST_F(UserLogTest, TesUserLogWithRum) {
	internal::FTSDKConfigManager::getInstance().getLogPipeConfig().setEnableCustomLog(true).setEnableLinkRumData(true).setSamplingRate(1);
	internal::LogPipeManager::getInstance().init(internal::FTSDKConfigManager::getInstance().getLogPipeConfig());

	internal::RUMManager::getInstance().startView(FIRST_VIEW);

	std::string logContent = "----logInsertDataTest----";
	internal::LogPipeManager::getInstance().addLog(logContent, LogLevel::INFO);

	internal::RUMManager::getInstance().stopView();
	waitForCompleted();

	auto runRecs = internal::LineDBManager::getInstance().queryLineFromDB(DataType::LOG);

	EXPECT_TRUE(runRecs.size() == 1);
	EXPECT_TRUE(runRecs[0]->dataType == DataType::LOG);

	auto vtMeas = internal::LineProtocolBuilder::getInstance().decode(runRecs[0]->rawLine);
	EXPECT_TRUE(vtMeas.size() == 1);

	auto vtLogs = getRumItemsByName(constants::FT_LOG_DEFAULT_MEASUREMENT, _GET_ALL_RECORDS_, vtMeas);
	EXPECT_TRUE(vtLogs.size() == 1);

	if (vtLogs.size() > 0)
	{
		VERIFY_RUM_FIELD_STR((vtLogs[0]), (constants::KEY_MESSAGE), "\"" + logContent + "\"");

		VERIFY_RUM_TAG((vtLogs[0]), (constants::KEY_RUM_VIEW_NAME), FIRST_VIEW);
	}
}
