#include "ft-sdk-unittest.h"
#include "../../datakit-sdk-cpp/ft-sdk/Include/FTSDKDataContracts.h"
#include "../../datakit-sdk-cpp/ft-sdk/LoggerManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/DataSyncManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/FTSDKConfigManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/LineDBManager.h"
#include "../../datakit-sdk-cpp/ft-sdk/LineProtocolBuilder.h"
#include "../../datakit-sdk-cpp/ft-sdk/LogPipeManager.h"
#include "TestHelper.h"
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

// Turn off iterator debugging as it makes the compiler very slow on large methods in debug builds
//#define _HAS_ITERATOR_DEBUGGING 0

using namespace com::ft::sdk;
using namespace _test::helper;

#define VERIFY_ENCODED_LINE_PROTOCOL(encoded, expected) EXPECT_TRUE(removeTimestampe(encoded) == removeTimestampe(expected))

class LineProtocolTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::cout << "\nSetUp..." << std::endl;
		internal::LoggerManager::getInstance().init();
		internal::LineProtocolBuilder::getInstance().init();
	}

	void TearDown() override {
		std::cout << "TearDown...\n" << std::endl;

	}

	std::string removeTimestampe(const std::string& line)
	{
		std::string ret = line;
		std::string::size_type pos = line.find_last_of(' ');
		if (pos != std::string::npos)
		{
			ret = line.substr(0, pos);
		}

		return ret;
	}
};

TEST_F(LineProtocolTest, TesLineProtocolRegular) {
	internal::DataMsg dm;

	internal::Measurement sampleMeas;
	sampleMeas.measurementName = "Test_Measurement_Name";
	sampleMeas.tags["tag1"] = "tag1_value";
	sampleMeas.tags["tag2"] = "tag2_value";
	sampleMeas.fields["field1"] = std::string("field1_value");
	sampleMeas.fields["field2"] = std::string("field2_value");
	dm.vtLine.push_back(sampleMeas);

	std::string line = internal::LineProtocolBuilder::getInstance().encode(dm);

	EXPECT_TRUE(line.size() > 0);
	if (line.size() > 0)
	{
		const std::string expected = "Test_Measurement_Name,tag1=tag1_value,tag2=tag2_value field1=\"field1_value\",field2=\"field2_value\" 1684129497870634800\n";
		VERIFY_ENCODED_LINE_PROTOCOL(line, expected);
	}
}


TEST_F(LineProtocolTest, TesLineProtocolWithSpecialChars) {
	internal::DataMsg dm;

	internal::Measurement sampleMeas;
	sampleMeas.measurementName = "Test,Measurement Name";
	sampleMeas.tags["tag,1"] = "tag1,value";
	sampleMeas.tags["tag=2"] = "tag2 value";
	sampleMeas.tags["tag 3"] = "tag3=value";
	sampleMeas.fields["field=1"] = std::string("field1\"value");
	sampleMeas.fields["field 2"] = std::string("field2\\value");
	sampleMeas.fields["field,3"] = 10;
	dm.vtLine.push_back(sampleMeas);

	std::string line = internal::LineProtocolBuilder::getInstance().encode(dm);

	EXPECT_TRUE(line.size() > 0);
	if (line.size() > 0)
	{
		const std::string expected = "Test\\,Measurement\\ Name,tag\\ 3=tag3\\=value,tag\\,1=tag1\\,value,tag\\=2=tag2\\ value field\\ 2=\"field2\\\\value\",field\\,3=10i,field\\=1=\"field1\\\"value\" 1684130584126441700\n";
		VERIFY_ENCODED_LINE_PROTOCOL(line, expected);
	}
}