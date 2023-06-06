#include "pch.h"
#include "TraceIDGenerator.h"
#include <stduuid/uuid.h>
#include <random>
#include <cmath>
#include <algorithm>
#include <regex>
#include <thread>
#include <iostream>
#include "Utils.h"
#include "FTSDKConstants.h"


namespace com::ft::sdk::internal
{
	static std::shared_ptr<std::mt19937_64> random64Generator(new std::mt19937_64((std::chrono::system_clock::now().time_since_epoch().count() << 32) + std::random_device()()));

	//std::atomic<int> TraceIDGenerator::m_increasingInt = -1;
	//std::atomic<std::int64_t> TraceIDGenerator::m_increasingLong = 0;
	//std::string TraceIDGenerator::m_skyWalkingTraceUUID;
	//std::string TraceIDGenerator::m_skyWalkingParentServiceUUID;

#define DEBUG_STR(x) std::cout << x << std::endl;

	TraceIDGenerator::TraceIDGenerator()
	{
		//DEBUG_STR("init TraceIDGenerator...");

		m_skyWalkingTraceUUID = utils::generateRandomUUID();
		m_skyWalkingParentServiceUUID = utils::generateRandomUUID();
	}

	/// <summary>
	/// UUID.randomUUID().toString().replace("-", "").toLowerCase();
	/// </summary>
	/// <returns></returns>
	std::string TraceIDGenerator::getZipKinNewTraceId()
	{
		return utils::generateRandomUUID();
	}

	/// <summary>
	/// Utils.getGUID_16()
	/// </summary>
	/// <returns></returns>
	std::string TraceIDGenerator::getZipKinNewSpanId()
	{
		return convertNumberToId(generate64BitsRandomNumber());
	}

	/// <summary>
	/// 64 bits integer, Utils.getDDtraceNewId()
	/// </summary>
	/// <returns></returns>
	std::string TraceIDGenerator::getDDtraceNewTraceId()
	{
		return std::to_string(generate64BitsRandomNumber());
	}

	std::string TraceIDGenerator::getSkyWalkingNewTraceId()
	{
		return convertNumberToId(generate64BitsRandomNumber());
	}

	std::string TraceIDGenerator::getSkyWalkingNewSpanId()
	{
		return "";
	}

	std::string TraceIDGenerator::getSkyWalkingSW(SkyWalkingVersion version, const std::string& sampled, std::int64_t requestTime, HttpUrl url)
	{
		std::string swTraceID;

		// lock
		{
			if (m_increasingInt < INCREASING_NUMBER_MAX)
			{
				m_increasingInt += 2;
			}
			else
			{
				m_increasingInt = 1;
			}

			if (version == SkyWalkingVersion::V3) 
			{
				swTraceID = createSw8Head(sampled, requestTime, url);
			}
			else if (version == SkyWalkingVersion::V2) 
			{
				m_increasingLong += 1;
				swTraceID = createSw6Head(sampled, requestTime, url);
			}
		}

		return swTraceID;
	}

	std::string TraceIDGenerator::createSw8Head(std::string sampled, std::int64_t requestTime, HttpUrl url)
	{
		//DEBUG_STR("service uuid=" + m_skyWalkingParentServiceUUID);
		std::string newParentTraceId = m_skyWalkingTraceUUID + "." + std::to_string(utils::getCurrentThreadID()) + "." + std::to_string(requestTime) + utils::formatInt("%04d", m_increasingInt - 1);
		//DEBUG_STR("parent trace id=" + newParentTraceId);
		std::string newTraceId = m_skyWalkingTraceUUID + "." + std::to_string(utils::getCurrentThreadID()) + "." + std::to_string(requestTime) + utils::formatInt("%04d", m_increasingInt);
		//DEBUG_STR("trace id=" + newTraceId);

		std::string sw8 = sampled + "-" +
			utils::base64Encode(newTraceId) + "-" +
			utils::base64Encode(newParentTraceId) + "-" +
			"0-" +
			utils::base64Encode(constants::DEFAULT_LOG_SERVICE_NAME + "") + "-" +
			utils::base64Encode(m_skyWalkingParentServiceUUID + "@" + utils::getLocalIPAddr()) + "-" +
			utils::base64Encode(url.getPath()) + "-" +
			utils::base64Encode(url.getHost() + ":" + std::to_string(url.getPort()));

		return sw8;
	}

	std::string TraceIDGenerator::createSw6Head(std::string sampled, std::int64_t requestTime, HttpUrl url)
	{
		std::string newParentTraceId = std::to_string(m_increasingLong) + "." + std::to_string(utils::getCurrentThreadID()) + "." + std::to_string(requestTime) + utils::formatInt("%04d", m_increasingInt - 1);
		std::string newTraceId = std::to_string(m_increasingLong) + "." + std::to_string(utils::getCurrentThreadID()) + "." + std::to_string(requestTime) + utils::formatInt("%04d", m_increasingInt);
		std::string sw6 = sampled + "-" +
			utils::base64Encode(newTraceId) + "-" +
			utils::base64Encode(newParentTraceId) + "-" +
			"0-" +
			std::to_string(m_increasingLong) + "-" + std::to_string(m_increasingLong) + "-" +
			utils::base64Encode("#" + url.getHost() + ":" + std::to_string(url.getPort())) + "-" +
			utils::base64Encode("-1") + "-" + utils::base64Encode("-1");

		return sw6;
	}


	std::uint64_t TraceIDGenerator::generate64BitsRandomNumber()
	{
		std::mt19937_64& rand_gen = *random64Generator.get();

		return rand_gen();
	}

	std::string TraceIDGenerator::convertNumberToId(std::uint64_t num)
	{
		char str[64];
		
		return std::string(str, snprintf(str, sizeof(str), ZIPKIN_SPAN_ID_FMT, num));
	}

}