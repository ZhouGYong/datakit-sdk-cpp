#include "pch.h"
#include <iostream>
#include <chrono>
#include <memory>
#include <sstream>

#include "LineProtocolBuilder.h"
#include "LineProtocol.h"
#include "LoggerManager.h"
#include "TrackNodeImpl.h"
#include "Utils.h"

namespace com::ft::sdk::internal
{
	void LineProtocolBuilder::init()
	{

	}
	
	std::string LineProtocolBuilder::encode(DataMsg& msg)
	{
		std::string multiLPs;
		for (auto& item : msg.vtLine)
		{
			LineProtocol lp(item.measurementName);

			for (auto& tg : item.tags)
			{
				lp.addTag(tg.first, tg.second);
			}

			for (auto& fld : item.fields)
			{
				std::string name = fld.first;
				std::string type = fld.second.type().name();
				if (type == typeid(int).name())
				{
					lp.addField(name, std::any_cast<int>(fld.second));
				}
				else if (type == typeid(std::int64_t).name())
				{
					lp.addField(name, std::any_cast<std::int64_t>(fld.second));
				}
				else if (type == typeid(std::uint64_t).name())
				{
					lp.addField(name, (std::int64_t)std::any_cast<std::uint64_t>(fld.second));
				}
				else if (type == typeid(double).name())
				{
					lp.addField(name, std::any_cast<double>(fld.second));
				}
				else if (type == typeid(std::string).name())
				{
					lp.addField(name, std::any_cast<std::string>(fld.second));
				}
				else if (type == typeid(bool).name())
				{
					lp.addField(name, std::any_cast<bool>(fld.second) ? "true" : "false");
				}
				else
				{
					throw std::exception();
				}
			}

			internal::LoggerManager::getInstance().logInfo("Converted data message to line protocol");

			// TODO: investigate if the lp will be recycled out of the function
			multiLPs.append(lp.toLineProtocol()).append("\n"); // check if the last break need to be removed
		}

		return multiLPs;
	}

	std::vector<std::shared_ptr<Measurement>> LineProtocolBuilder::decode(const std::string& line)
	{
		std::vector<std::shared_ptr<Measurement>> vtDataMsg;

		auto vtLines = utils::splitStringByChar(line, '\n');
		for (auto ln : vtLines)
		{
			auto pDM = std::make_shared<Measurement>();
			std::string::size_type pos = ln.find(',');
			std::string name = ln.substr(0, pos);
			std::string content = ln.substr(pos + 1);
			pos = findFirstNonEscapeChar(content, ' ');
			std::string tags = content.substr(0, pos);
			spitByNonEscapeChar(pDM->tags, tags, ',');
			content = content.substr(pos + 1);
			pos = content.find_last_of(' ');
			std::string fields = content.substr(0, pos);
			spitByNonEscapeChar(pDM->fields, fields, ',');
			std::string timestamp = content.substr(pos + 1);

			pDM->measurementName = name;

			vtDataMsg.push_back(pDM);
		}

		return vtDataMsg;
	}

	void LineProtocolBuilder::decodeOneTag(TagMap& tags, const std::string& str)
	{
		auto vtTag = utils::splitStringByChar(str, '=');
		if (vtTag.size() == 2)
		{
			tags[vtTag[0]] = vtTag[1];
		}
		else if (vtTag.size() == 1)
		{
			if (str.find('=') != std::string::npos)
			{
				tags[vtTag[0]] = "";
			}
			else
			{
				throw std::runtime_error("wrong size");
			}
		}
		else
		{
			throw std::runtime_error("wrong size");
		}
	}

	void LineProtocolBuilder::spitByNonEscapeChar(TagMap& tags, const std::string& str, const char ch)
	{
		std::string::size_type pos = 0;
		std::string::size_type prev = 0;

		bool lastRemain = false;
		while ((pos = findFirstNonEscapeChar(str.substr(prev), ch)) != std::string::npos)
		{
			std::string curTag = str.substr(prev, pos);
			decodeOneTag(tags, curTag);
			prev += pos + 1;

			lastRemain = true;
		}

		if (lastRemain || (str.size() > 1 && tags.size() == 0))
		{
			std::string curTag = str.substr(prev);
			decodeOneTag(tags, curTag);
		}
	}

	void LineProtocolBuilder::spitByNonEscapeChar(FieldMap& fields, const std::string& str, const char ch)
	{
		TagMap tags;
		spitByNonEscapeChar(tags, str, ch);

		for (auto tag : tags)
		{
			int len = tag.second.size();
			if (tag.second[len - 1] == 'i')
			{
				std::string numeric = tag.second.substr(0, len - 1);
				fields[tag.first] = (std::int64_t)std::stol(numeric);
			}
			else
			{
				fields[tag.first] = tag.second;
			}
		}
	}

	std::string::size_type LineProtocolBuilder::findFirstNonEscapeChar(const std::string& str, const char ch)
	{
		std::vector<std::string> strings;

		std::string::size_type pos = 0;
		std::string::size_type prev = 0;
		while ((pos = str.find(ch, prev)) != std::string::npos)
		{
			strings.push_back(str.substr(prev, pos - prev));
			if (str[pos - 1] != '\\')
			{
				break;
			}
			else
			{
				prev = pos + 1;
			}
		}

		return pos;
	}
}
