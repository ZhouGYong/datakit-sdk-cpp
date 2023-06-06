#pragma once
#ifndef _TEST_HELPER_H_
#define _TEST_HELPER_H_

#include "../../datakit-sdk-cpp/ft-sdk/DataSyncManager.h"

#define VERIFY_RUM_FIELD_INT(item, prop, count)	 EXPECT_TRUE(std::any_cast<std::int64_t>((item)->fields[prop]) == count)
#define VERIFY_RUM_FIELD_STR(item, prop, val)	 EXPECT_TRUE(std::any_cast<std::string>(item->fields[prop]) == val)
#define VERIFY_RUM_TAG(item, prop, val)	 EXPECT_TRUE(item->tags[prop] == val)

namespace _test::helper
{

	std::vector<std::shared_ptr<com::ft::sdk::internal::Measurement>> getRumItemsByName(const std::string& type, const std::string& name, std::vector<std::shared_ptr<com::ft::sdk::internal::Measurement>>& vtLine);
	void waitForCompleted();

	const std::string _GET_ALL_RECORDS_ = "__LP__GET__ALL__";

	const std::string FIRST_VIEW = "FirstView";
	const std::string SECOND_VIEW = "SecondView";
	const std::string ROOT = "root";
	const std::string ACTION_NAME = "action";
	const std::int64_t DURATION = 1000L;
	const std::string ACTION_TYPE_NAME = "action test";
	const std::string ANY_ACTION = "AnyAction";
	const std::string LONG_TASK = "longTask";
	const std::string _ERROR = "error";
	const std::string ERROR_MESSAGE = "error message";
	const std::string RESOURCE_REQUEST_HEADER = "{x-datadog-parent-id=73566521391796532, x-datadog-sampling-priority=1,\
		ft-dio-key=a44e0ab0-232f-4f93-a6fd-b7a45cf8d20c, x-datadog-origin=rum, x-datadog-trace-id=123622877354441421}";
	const std::string RESOURCE_RESPONSE_HEADER = "{date=[Fri, 26 Nov 2021 06:08:47 GMT], server=[sffe], content-length=[1437],\
		 expires=[Fri, 01 Jan 1990 00:00:00 GMT], vary=[Accept-Encoding], content-encoding=[gzip],\
		 pragma=[no-cache], last-modified=[Fri, 19 Jun 2020 10:30:00 GMT], x-xss-protection=[0],\
		x-content-type-options=[nosniff], content-type=[text/html],\
		 content-security-policy-report-only=[script-src 'nonce-e-zcQTJtEfSCU-KEHJAWqw'\
		 'report-sample' 'strict-dynamic' 'unsafe-eval' 'unsafe-inline' http: https:;\
		 object-src 'none'; report-uri https://csp.withgoogle.com/csp/static-on-bigtable;\
		base-uri 'none'],\
		 report-to=[{\"group\":\"static-on-bigtable\",\"max_age\":2592000,\"endpoints\"\
		:[{\"url\":\"https://csp.withgoogle.com/csp/report-to/static-on-bigtable\"}]}],\
		 cross-origin-resource-policy=[cross-origin], cache-control=[no-cache, must-revalidate],\
		 accept-ranges=[bytes], cross-origin-opener-policy-report-only=[same-origin; report-to=\"static-on-bigtable\"]}";
	const std::string BODY_CONTENT = "body content";

}

#endif // !_TEST_HELPER_H_

