/*****************************************************************//**
 * \file   FTSDKNode.h
 * \brief  User Custom Node
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_NODE_H_
#define _DATAKIT_SDK_NODE_H_

#include <string>
#include <vector>
#include <map>
#include <memory>
//#include "datakit_exports.h"

namespace com::ft::sdk
{
	//template<typename T>
	class /*FTSDK_EXPORT*/ TrackNode
	{
	public:
		TrackNode(const std::string& trackName);
		virtual ~TrackNode();
		//virtual const std::string& getTrackName()

		//template<typename T>
		//TrackNode&& addTrackProperty(const std::string& property_name, T& value);


		virtual TrackNode&& addTrackProperty(const std::string& property_name, int value);
		virtual TrackNode&& addTrackProperty(const std::string& property_name, std::int64_t value);
		virtual TrackNode&& addTrackProperty(const std::string& property_name, const std::string& value);
		virtual TrackNode&& addTrackProperty(const std::string& property_name, double value);
	};

	class /*FTSDK_EXPORT*/ TrackNodeBuilder
	{
	public:
		static std::shared_ptr<TrackNode> build(const std::string& trackName);
	};
}


#endif // !_DATAKIT_SDK_NODE_H_

