#include "TestHelper.h"
#include "../../datakit-sdk-cpp/ft-sdk/Include/FTSDKDataContracts.h"
#include "../../datakit-sdk-cpp/ft-sdk/FTSDKConstants.h"
#include <thread>

using namespace com::ft::sdk;

namespace _test::helper
{
	std::vector<std::shared_ptr<internal::Measurement>> getRumItemsByName(const std::string& type, const std::string& name, std::vector<std::shared_ptr<internal::Measurement>>& vtLine)
	{
		std::string measurementKeyName = "";

		if (type == constants::FT_MEASUREMENT_RUM_VIEW)
		{
			measurementKeyName = constants::KEY_RUM_VIEW_NAME;
		}
		else if (type == constants::FT_MEASUREMENT_RUM_ACTION)
		{
			measurementKeyName = constants::KEY_RUM_ACTION_NAME;
		}
		else if (type == constants::FT_MEASUREMENT_RUM_RESOURCE)
		{
			measurementKeyName = constants::KEY_RUM_RESOURCE_TYPE;
		}
		else if (type == constants::FT_MEASUREMENT_RUM_ERROR)
		{
			measurementKeyName = constants::KEY_RUM_ERROR_STACK;
		}
		else if (type == constants::FT_MEASUREMENT_RUM_LONG_TASK)
		{
			measurementKeyName = constants::KEY_RUM_LONG_TASK_STACK;
		}

		std::vector<std::shared_ptr<internal::Measurement>> vtView;
		for (auto item : vtLine)
		{
			if (item->measurementName == type && (item->tags[measurementKeyName] == name || name == _GET_ALL_RECORDS_))
			{
				vtView.push_back(item);
			}
		}

		return vtView;
	}

	void waitForCompleted()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}
}
