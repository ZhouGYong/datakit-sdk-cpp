/*****************************************************************//**
 * \file   FTSDKConstants.h
 * \brief  Global Constant Definition
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_CONST_H_
#define _DATAKIT_SDK_CONST_H_

#include <string>

namespace com::ft::sdk::constants
{
	const std::string USER_AGENT = "datakit-cpp-sdk";
	const std::string CONTENT_TYPE = "text/plain";
	const std::string CHARSET = "UTF-8";
	const std::string DEFAULT_APP_VERSION = "0.0.0";
	const std::string DATAKIT_SDK_VERSION = "0.8.2";

	const std::string URL_MODEL_TRACING = "/v1/write/tracing";//链路上传
	const std::string URL_MODEL_LOG = "/v1/write/logging";//日志数据上传路径
	const std::string URL_MODEL_RUM = "/v1/write/rum";

	const std::string FT_MEASUREMENT_RUM_VIEW = "view";
	const std::string FT_MEASUREMENT_RUM_ERROR = "error";
	const std::string FT_MEASUREMENT_RUM_LONG_TASK = "long_task";
	const std::string FT_MEASUREMENT_RUM_RESOURCE = "resource";
	const std::string FT_MEASUREMENT_RUM_ACTION = "action";

	const std::string KEY_RUM_IS_SIGN_IN = "is_signin";
	const std::string KEY_RUM_USER_ID = "userid";
	const std::string KEY_RUM_USER_NAME = "user_name";
	const std::string KEY_RUM_USER_EMAIL = "user_email";
	const std::string KEY_RUM_APP_ID = "app_id";
	const std::string KEY_RUM_CUSTOM_KEYS = "custom_keys";

	const std::string KEY_RUM_RESOURCE_URL = "resource_url";
	const std::string KEY_RUM_RESOURCE_URL_HOST = "resource_url_host";
	const std::string KEY_RUM_RESOURCE_TYPE = "resource_type";
	const std::string KEY_RUM_RESPONSE_CONNECTION = "response_connection";
	const std::string KEY_RUM_RESPONSE_CONTENT_TYPE = "response_content_type";
	const std::string KEY_RUM_RESPONSE_CONTENT_ENCODING = "response_content_encoding";
	const std::string KEY_RUM_RESOURCE_METHOD = "resource_method";
	const std::string KEY_RUM_RESPONSE_HEADER = "response_header";
	const std::string KEY_RUM_REQUEST_HEADER = "request_header";
	const std::string KEY_RUM_SDK_PACKAGE_AGENT = "sdk_package_agent";
	const std::string KEY_RUM_SDK_PACKAGE_TRACK = "sdk_package_track";
	const std::string KEY_RUM_SDK_PACKAGE_NATIVE = "sdk_package_native";
	const std::string KEY_SDK_VERSION = "sdk_version";

	const std::string KEY_RUM_RESOURCE_STATUS = "resource_status";
	const std::string KEY_RUM_RESOURCE_STATUS_GROUP = "resource_status_group";
	const std::string KEY_RUM_RESOURCE_SIZE = "resource_size";
	const std::string KEY_RUM_RESOURCE_DURATION = "duration";
	const std::string KEY_RUM_RESOURCE_DNS = "resource_dns";
	const std::string KEY_RUM_RESOURCE_TCP = "resource_tcp";
	const std::string KEY_RUM_RESOURCE_SSL = "resource_ssl";
	const std::string KEY_RUM_RESOURCE_TTFB = "resource_ttfb";
	const std::string KEY_RUM_RESOURCE_FIRST_BYTE = "resource_first_byte";
	const std::string KEY_RUM_RESOURCE_SPAN_ID = "span_id";
	const std::string KEY_RUM_RESOURCE_TRACE_ID = "trace_id";
	const std::string KEY_RUM_RESOURCE_TRANS = "resource_trans";
	const std::string KEY_RUM_RESOURCE_URL_PATH = "resource_url_path";
	const std::string KEY_RUM_RESOURCE_URL_PATH_GROUP = "resource_url_path_group";
	const std::string KEY_RUM_ERROR_MESSAGE = "error_message";
	const std::string KEY_RUM_ERROR_STACK = "error_stack";
	const std::string KEY_RUM_ERROR_SOURCE = "error_source";
	const std::string KEY_RUM_ERROR_TYPE = "error_type";
	const std::string KEY_RUM_ERROR_SITUATION = "error_situation";
	const std::string KEY_RUM_LONG_TASK_DURATION = "duration";
	const std::string KEY_RUM_LONG_TASK_STACK = "long_task_stack";
	const std::string KEY_RUM_NETWORK_TYPE = "network_type";

	const std::string KEY_RUM_SESSION_ID = "session_id";
	const std::string KEY_RUM_SESSION_TYPE = "session_type";
	const std::string KEY_RUM_VIEW_ID = "view_id";
	const std::string KEY_RUM_VIEW_REFERRER = "view_referrer";
	const std::string KEY_RUM_VIEW_NAME = "view_name";
	const std::string KEY_RUM_VIEW_LOAD = "loading_time";
	const std::string KEY_RUM_VIEW_LONG_TASK_COUNT = "view_long_task_count";
	const std::string KEY_RUM_VIEW_RESOURCE_COUNT = "view_resource_count";
	const std::string KEY_RUM_VIEW_ERROR_COUNT = "view_error_count";
	const std::string KEY_RUM_VIEW_ACTION_COUNT = "view_action_count";
	const std::string KEY_RUM_VIEW_TIME_SPENT = "time_spent";
	const std::string KEY_RUM_VIEW_IS_ACTIVE = "is_active";
	const std::string KEY_RUM_VIEW_IS_WEB_VIEW = "is_web_view";

	const std::string KEY_RUM_ACTION_ID = "action_id";
	const std::string KEY_RUM_ACTION_NAME = "action_name";
	const std::string KEY_RUM_ACTION_TYPE = "action_type";
	const std::string KEY_RUM_ACTION_LONG_TASK_COUNT = "action_long_task_count";
	const std::string KEY_RUM_ACTION_RESOURCE_COUNT = "action_resource_count";
	const std::string KEY_RUM_ACTION_ERROR_COUNT = "action_error_count";
	const std::string KEY_RUM_ACTION_DURATION = "duration";

	const std::string KEY_DEVICE_UUID = "device_uuid";
	const std::string KEY_DEVICE_OS = "os";
	const std::string KEY_DEVICE_OS_VERSION = "os_version";
	const std::string KEY_DEVICE_OS_VERSION_MAJOR = "os_version_major";
	const std::string KEY_DEVICE_DEVICE_BAND = "device";
	const std::string KEY_DEVICE_DEVICE_MODEL = "model";
	const std::string KEY_DEVICE_DISPLAY = "screen_size";
	const std::string KEY_DEVICE_CARRIER = "carrier";
	const std::string KEY_DEVICE_LOCALE = "locale";

#ifdef _WIN32
	const std::string FT_LOG_DEFAULT_MEASUREMENT = "df_rum_windows_log";
#elif __linux__
	const std::string FT_LOG_DEFAULT_MEASUREMENT = "df_rum_linux_log";
#endif // __WIN32

	const std::string DEFAULT_OBJECT_CLASS = "Mobile_Device";//默认的对象名
#ifdef _WIN32
	const std::string DEFAULT_LOG_SERVICE_NAME = "df_rum_windows";
#elif __linux__
	const std::string DEFAULT_LOG_SERVICE_NAME = "df_rum_linux";
#endif // __WIN32
	const int MAX_DB_CACHE_NUM = 5000;//数据库最大缓存容量

	const std::string KEY_TIME_COST_DURATION = "duration";
	const std::string KEY_STATUS = "status";

	const std::string KEY_BATTERY_TOTAL = "battery_total";
	const std::string KEY_BATTERY_CHARGE_TYPE = "battery_charge_type";
	const std::string KEY_BATTERY_STATUS = "battery_status";
	const std::string KEY_BATTERY_USE = "battery_use";

	const std::string KEY_MEMORY_TOTAL = "memory_total";
	const std::string KEY_MEMORY_USE = "memory_use";
	const std::string KEY_CPU_USE = "cpu_use";

	const std::string KEY_CPU_TICK_COUNT = "cpu_tick_count";
	const std::string KEY_CPU_TICK_COUNT_PER_SECOND = "cpu_tick_count_per_second";
	const std::string KEY_MEMORY_AVG = "memory_avg";
	const std::string KEY_MEMORY_MAX = "memory_max";
	const std::string KEY_FPS_MINI = "fps_mini";
	const std::string KEY_FPS_AVG = "fps_avg";
	const std::string KEY_BATTERY_CURRENT_AVG = "battery_current_avg";
	const std::string KEY_BATTERY_CURRENT_MAX = "battery_current_max";

	const std::string FT_KEY_VALUE_NULL = "null";
	const std::string UNKNOWN = "N/A";

	const std::string KEY_SERVICE = "service";
	const std::string KEY_ENV = "env";
	const std::string KEY_APPLICATION_UUID = "application_uuid";
	const std::string KEY_APP_VERSION_NAME = "version";
	const std::string KEY_SDK_NAME = "sdk_name";
	const std::string KEY_MESSAGE = "message";
	const std::string KEY_BACKENDSAMPLE = "backend_sample";
}

#endif // !_DATAKIT_SDK_CONST_H_

