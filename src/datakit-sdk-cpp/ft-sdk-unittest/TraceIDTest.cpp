#include "ft-sdk-unittest.h"
#include "../../datakit-sdk-cpp/ft-sdk/TraceIDGenerator.h"
#include "../../datakit-sdk-cpp/ft-sdk/TraceManager.h"
#include <string>
#include <iostream>

using namespace com::ft::sdk;
using namespace com::ft::sdk::internal;

class TraceIDTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::cout << "\nSetUp..." << std::endl;
		
		
		//tc.setEnableAutoTrace(true);
		tc.setSamplingRate(1000.0f);
	}

	void TearDown() override {
		std::cout << "TearDown...\n" << std::endl;
	}

	std::string urlStr = "https://www.example.com/test.html?param=1";
	HttpUrl url{ "host", "path", 1234 };
	FTTraceConfig tc;
};

TEST_F(TraceIDTest, TestZipKinTraceID) {
	std::cout << "generate ZipKin Trace ID:" << std::endl;
	std::string uuid = TraceIDGenerator::getInstance().getZipKinNewTraceId();
	std::cout << uuid << std::endl;
	EXPECT_TRUE(uuid.length() == 32);

}

TEST_F(TraceIDTest, TestZipKinSpanID) {
	std::cout << "generate ZipKin Span ID:" << std::endl;
	std::string uuid = TraceIDGenerator::getInstance().getZipKinNewSpanId();
	std::cout << uuid << std::endl;
	EXPECT_TRUE(uuid.length() == 16);

}

TEST_F(TraceIDTest, TestDDtraceTraceID) {
	std::cout << "generate DDTrace Trace ID:" << std::endl;
	std::string uuid = TraceIDGenerator::getInstance().getDDtraceNewTraceId();
	std::cout << uuid << std::endl;
	EXPECT_TRUE(uuid.length() < 32);

}

TEST_F(TraceIDTest, TestSkyWalkingTraceID_V3) {
	std::cout << "generate SkyWaling V3 Trace ID:" << std::endl;
	std::string uuid = TraceIDGenerator::getInstance().getSkyWalkingSW(SkyWalkingVersion::V3, "1", 121212121212l, url);
	std::cout << uuid << std::endl;
	EXPECT_TRUE(uuid.length() > 32);

}

TEST_F(TraceIDTest, TestSkyWalkingTraceID_V2) {
	std::cout << "generate SkyWaling V2 Trace ID:" << std::endl;
	std::string uuid = TraceIDGenerator::getInstance().getSkyWalkingSW(SkyWalkingVersion::V2, "1", 121212121212l, url);
	std::cout << uuid << std::endl;
	EXPECT_TRUE(uuid.length() > 32);

}

TEST_F(TraceIDTest, TestTraceHeader_DDTRACE) {
	std::cout << "generate trace header for ddtrace:" << std::endl;
	tc.setTraceType(TraceType::DDTRACE);
	TraceManager::getInstance().initialize(tc);

	auto header = TraceManager::getInstance().getTraceHeader(urlStr);
	for (auto& item : header)
	{
		std::cout << item.first << " = " << item.second << std::endl;
	}

	EXPECT_TRUE(header.size() > 0);

}


TEST_F(TraceIDTest, TestTraceHeader_SKYWALKING) {
	std::cout << "generate trace header for skywalking:" << std::endl;
	tc.setTraceType(TraceType::SKYWALKING);
	TraceManager::getInstance().initialize(tc);

	auto header = TraceManager::getInstance().getTraceHeader(urlStr);
	for (auto& item : header)
	{
		std::cout << item.first << " = " << item.second << std::endl;
	}

	EXPECT_TRUE(header.size() > 0);

}

TEST_F(TraceIDTest, TestTraceHeader_ZIPKIN) {
	std::cout << "generate trace header for zipkin:" << std::endl;
	tc.setTraceType(TraceType::ZIPKIN_MULTI_HEADER);
	TraceManager::getInstance().initialize(tc);

	auto header = TraceManager::getInstance().getTraceHeader(urlStr);
	for (auto& item : header)
	{
		std::cout << item.first << " = " << item.second << std::endl;
	}

	EXPECT_TRUE(header.size() > 0);

}
