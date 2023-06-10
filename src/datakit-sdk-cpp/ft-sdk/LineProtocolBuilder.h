/*****************************************************************//**
 * \file   LineProtocolBuilder.h
 * \brief  Factory class to encode/decode line protocol message
 * 
 * \author Zhou Guangyong
 * \date   Dec 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_LINE_PROTOCOL_BUILD_H_
#define _DATAKIT_SDK_LINE_PROTOCOL_BUILD_H_

#include "Singleton.h"
#include "DataSyncManager.h"

namespace com::ft::sdk::internal
{
	class LineProtocolBuilder : public Singleton<LineProtocolBuilder>
	{
	public:
		void init();
		std::string encode(DataMsg& msg);
		std::vector<std::shared_ptr<Measurement>> decode(const std::string& line);

	private:
		LineProtocolBuilder() {}
		std::string::size_type findFirstNonEscapeChar(const std::string& str, const char ch);
		void spitByNonEscapeChar(TagMap& tags, const std::string& str, const char ch);
		void spitByNonEscapeChar(FieldMap& fields, const std::string& str, const char ch);
		void decodeOneTag(TagMap& tags, const std::string& str);

		ENABLE_SINGLETON();
	};
}

#endif // !_DATAKIT_SDK_LINE_PROTOCOL_BUILD_H_
