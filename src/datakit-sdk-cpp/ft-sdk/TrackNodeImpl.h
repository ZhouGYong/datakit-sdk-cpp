/*****************************************************************//**
 * \file   TrackNodeImpl.h
 * \brief  Handle the user custom track node
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/
#pragma once
#ifndef _TRACK_NODE_IMPL_H_
#define _TRACK_NODE_IMPL_H_

#include "FTSDKNode.h"
#include <any>

namespace com::ft::sdk::internal
{
	//template<typename T>
	class TrackNodeImpl : public TrackNode
	{
	public:
		TrackNodeImpl(const std::string& trackName);

		TrackNode&& addTrackProperty(const std::string& property_name, int value);
		TrackNode&& addTrackProperty(const std::string& property_name, std::int64_t value);
		TrackNode&& addTrackProperty(const std::string& property_name, const std::string& value);
		TrackNode&& addTrackProperty(const std::string& property_name, double value);

		const std::string& getTrackName();
		std::map<std::string, std::any>& getTrackPropertyList();

	private:
		std::string m_trackName;
		std::map<std::string, std::any> m_trackPropertyList;

	};

}
#endif // !_TRACK_NODE_IMPL_H_

