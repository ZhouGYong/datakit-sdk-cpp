#include "pch.h"
#include "LineProtocol.h"
#include <iostream>
#include <chrono>
#include <memory>
#include <sstream>
#include "Utils.h"
#include "FTSDKConstants.h"

namespace com::ft::sdk::internal
{
    LineProtocol::LineProtocol(const std::string& measurementName) :
        mMeasurement(utils::escapeMeasurements(measurementName)), mTimestamp(LineProtocol::getCurrentTimestamp())
    {
        mTags = {};
        mFields = {};
    }

    LineProtocol&& LineProtocol::addField(const std::string& name, double value)
    {
        std::stringstream convert;
        if (!mFields.empty()) convert << ",";

        convert << utils::escapeTagAndKeyValue(name) << "=" << value;
        mFields += convert.str();
        return std::move(*this);
    }

    LineProtocol&& LineProtocol::addField(const std::string& name, int value)
    {
        std::stringstream convert;
        if (!mFields.empty()) convert << ",";

        convert << utils::escapeTagAndKeyValue(name) << "=" << value << 'i';
        mFields += convert.str();
        return std::move(*this);
    }

    LineProtocol&& LineProtocol::addField(const std::string& name, std::int64_t value)
    {
        std::stringstream convert;
        if (!mFields.empty()) convert << ",";

        convert << utils::escapeTagAndKeyValue(name) << "=" << value << 'i';
        mFields += convert.str();
        return std::move(*this);
    }

    LineProtocol&& LineProtocol::addField(const std::string& name, const std::string& value)
    {
        std::stringstream convert;
        if (!mFields.empty()) convert << ",";

        convert << utils::escapeTagAndKeyValue(name) << "=" << '"' << utils::escapeStringFieldValue(value) << '"';
        mFields += convert.str();
        return std::move(*this);
    }

    LineProtocol&& LineProtocol::addTag(const std::string& key, const std::string& value)
    {
        mTags += ",";
        mTags += utils::escapeTagAndKeyValue(key);
        mTags += "=";

        std::string convertedVal = value == "" ? constants::UNKNOWN : value;
        mTags += utils::escapeTagAndKeyValue(convertedVal);
        return std::move(*this);
    }

    LineProtocol&& LineProtocol::setTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp)
    {
        mTimestamp = timestamp;
        return std::move(*this);
    }

    auto LineProtocol::getCurrentTimestamp() -> decltype(std::chrono::system_clock::now())
    {
        return std::chrono::system_clock::now();
    }

    std::string LineProtocol::toLineProtocol() const
    {
        std::string strTime = std::to_string(
            std::chrono::duration_cast <std::chrono::nanoseconds>(mTimestamp.time_since_epoch()).count()
        );
        //std::cout << "time is :" << strTime << std::endl;
        return mMeasurement + mTags + " " + mFields + " " + strTime; 
    }

    std::string LineProtocol::getName() const
    {
        return mMeasurement;
    }

    std::chrono::time_point<std::chrono::system_clock> LineProtocol::getTimestamp() const
    {
        return mTimestamp;
    }

    std::string LineProtocol::getFields() const
    {
        return mFields;
    }

    std::string LineProtocol::getTags() const
    {
        return mTags.substr(1, mTags.size());
    }

}