#include "FTSDKNode.h"
#include "TrackNodeImpl.h"
#include <typeinfo>
#include <type_traits>

namespace com::ft::sdk
{

	TrackNode::TrackNode(const std::string& trackName)
	{
	}

	//const std::string& TrackNode::getTrackName()
	TrackNode::~TrackNode()
	{

	}

	TrackNode&& TrackNode::addTrackProperty(const std::string& property_name, int value)
	{
		return std::move(*this);
	}

	TrackNode&& TrackNode::addTrackProperty(const std::string& property_name, std::int64_t value)
	{
		return std::move(*this);
	}

	TrackNode&& TrackNode::addTrackProperty(const std::string& property_name, const std::string& value)
	{
		return std::move(*this);
	}

	TrackNode&& TrackNode::addTrackProperty(const std::string& property_name, double value)
	{
		return std::move(*this);
	}

	//template<typename T>
	//TrackNode&& TrackNode::addTrackProperty(const std::string& property_name, T& value)
	//{
	//	m_trackPropertyList[property_name] = std::any(value);
	//}

	internal::TrackNodeImpl::TrackNodeImpl(const std::string& trackName) : TrackNode(trackName)
	{
		m_trackName = trackName;
	}

	TrackNode&& internal::TrackNodeImpl::addTrackProperty(const std::string& property_name, int value)
	{
		m_trackPropertyList[property_name] = std::any(value);

		return std::move(*this);
	}

	TrackNode&& internal::TrackNodeImpl::addTrackProperty(const std::string& property_name, std::int64_t value)
	{
		m_trackPropertyList[property_name] = std::any(value);

		return std::move(*this);
	}

	TrackNode&& internal::TrackNodeImpl::addTrackProperty(const std::string& property_name, const std::string& value)
	{
		m_trackPropertyList[property_name] = std::any(value);

		return std::move(*this);
	}

	TrackNode&& internal::TrackNodeImpl::addTrackProperty(const std::string& property_name, double value)
	{
		m_trackPropertyList[property_name] = std::any(value);

		return std::move(*this);
	}

	const std::string& internal::TrackNodeImpl::getTrackName()
	{
		return m_trackName;
	}

	std::map<std::string, std::any>& internal::TrackNodeImpl::getTrackPropertyList()
	{
		return m_trackPropertyList;
	}

	std::shared_ptr<TrackNode> TrackNodeBuilder::build(const std::string& trackName)
	{
		internal::TrackNodeImpl* tni = new internal::TrackNodeImpl(trackName);
		return std::shared_ptr<TrackNode>(tni);
	}
}