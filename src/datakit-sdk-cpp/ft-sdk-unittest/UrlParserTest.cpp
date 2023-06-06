#include "ft-sdk-unittest.h"
#include "../../datakit-sdk-cpp/ft-sdk/InternalStructs.h"
#include <string>
#include <iostream>

using namespace com::ft::sdk;

class UrlParserTest : public ::testing::Test {
protected:
	void SetUp() override {
		std::cout << "\nSetUp..." << std::endl;
	}

	void TearDown() override {
		std::cout << "TearDown...\n" << std::endl;
	}
};

TEST_F(UrlParserTest, TestUrlParse) {
	std::string url = "https://www.example.com:8000/index.html?test=1&test2=3";
	std::cout << "input url:" << url << std::endl;
	internal::PropagationUrl result = internal::PropagationUrl::parse(url);
	std::cout << "protocol=" << result.protocol << ";domain=" << result.domain << ";port=" << result.port << ";path=" << result.path << ";query=" << result.query << std::endl;
	
	EXPECT_TRUE(result.protocol == "https");
	EXPECT_TRUE(result.domain == "www.example.com");
	EXPECT_TRUE(result.port == 8000);
	EXPECT_TRUE(result.path == "/index.html");
	EXPECT_TRUE(result.query == "test=1&test2=3");
}

TEST_F(UrlParserTest, TestUrlParse2) {
	std::string url = "http://www.example.com:8000/index.html?test=1&test2=3";
	std::cout << "input url:" << url << std::endl;
	internal::PropagationUrl result = internal::PropagationUrl::parse(url);
	std::cout << "protocol=" << result.protocol << ";domain=" << result.domain << ";port=" << result.port << ";path=" << result.path << ";query=" << result.query << std::endl;

	EXPECT_TRUE(result.protocol == "http");
	EXPECT_TRUE(result.domain == "www.example.com");
	EXPECT_TRUE(result.port == 8000);
	EXPECT_TRUE(result.path == "/index.html");
	EXPECT_TRUE(result.query == "test=1&test2=3");
}

TEST_F(UrlParserTest, TestUrlParse3) {
	std::string url = "http://www.example.com/index.html?test=1&test2=3";
	std::cout << "input url:" << url << std::endl;
	internal::PropagationUrl result = internal::PropagationUrl::parse(url);
	std::cout << "protocol=" << result.protocol << ";domain=" << result.domain << ";port=" << result.port << ";path=" << result.path << ";query=" << result.query << std::endl;

	EXPECT_TRUE(result.protocol == "http");
	EXPECT_TRUE(result.domain == "www.example.com");
	EXPECT_TRUE(result.port == 80);
	EXPECT_TRUE(result.path == "/index.html");
	EXPECT_TRUE(result.query == "test=1&test2=3");
}

TEST_F(UrlParserTest, TestUrlParse4) {
	std::string url = "http://www.example.com/index.html";
	std::cout << "input url:" << url << std::endl;
	internal::PropagationUrl result = internal::PropagationUrl::parse(url);
	std::cout << "protocol=" << result.protocol << ";domain=" << result.domain << ";port=" << result.port << ";path=" << result.path << ";query=" << result.query << std::endl;

	EXPECT_TRUE(result.protocol == "http");
	EXPECT_TRUE(result.domain == "www.example.com");
	EXPECT_TRUE(result.port == 80);
	EXPECT_TRUE(result.path == "/index.html");
	EXPECT_TRUE(result.query == "");
}