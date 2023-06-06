/*****************************************************************//**
 * \file   LineProtocol.h
 * \brief  Line Protocol Serialization
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/

#pragma once
#ifndef _DATAKIT_SDK_LINE_PROTOCOL_H_
#define _DATAKIT_SDK_LINE_PROTOCOL_H_

#include <string>
#include <chrono>
#include <variant>

namespace com::ft::sdk::internal
{
	class LineProtocol
	{
		//std::string measurementName;
		//std::string tags; // JSONObject tags;
		//std::string fields; // JSONObject fields;
		//long timeNano;

    public:
        /// Constructs LineProtocol based on measurementName name
        LineProtocol(const std::string& measurementName);

        /// Default destructor
        ~LineProtocol() = default;

        /// Adds a tags
        LineProtocol&& addTag(const std::string & key, const std::string & value);

        /// Adds filed
        LineProtocol&& addField(const std::string & name, int value);
        LineProtocol&& addField(const std::string & name, std::int64_t value);
        LineProtocol&& addField(const std::string & name, const std::string & value);
        LineProtocol&& addField(const std::string & name, double value);

        /// Generetes current timestamp
        static auto getCurrentTimestamp() -> decltype(std::chrono::system_clock::now());

        /// Converts LineProtocol to Influx Line Protocol
        std::string toLineProtocol() const;

        /// Sets custom timestamp
        LineProtocol&& setTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp);

        /// Name getter
        std::string getName() const;

        /// Timestamp getter
        std::chrono::time_point<std::chrono::system_clock> getTimestamp() const;

        /// Fields getter
        std::string getFields() const;

        /// Tags getter
        std::string getTags() const;

    protected:
        /// A value
        //std::variant<long long int, std::string, double> mValue;

        /// A name
        std::string mMeasurement;

        /// A timestamp
        std::chrono::time_point<std::chrono::system_clock> mTimestamp;

        /// Tags
        std::string mTags;

        /// Fields
        std::string mFields;


	};
}

#endif // !_DATAKIT_SDK_LINE_PROTOCOL_H_
