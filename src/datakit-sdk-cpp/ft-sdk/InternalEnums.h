#pragma once
#ifndef _DATAKIT_SDK_CPP_INTERNAL_ENUMS_H_
#define _DATAKIT_SDK_CPP_INTERNAL_ENUMS_H_

namespace com::ft::sdk
{
    /**
	 * 上传的数据类型
	 */
    enum class DataType : char
    {
        RUM_APP = 0x0,
        RUM_WEBVIEW,
        LOG,
        TRACE
    };

	//-- below three enums will be moved to interface once they are ready
    enum class ErrorMonitorType : unsigned int {
        ALL = 0xFFFFFFFF,
        BATTERY = 1 << 1,
        MEMORY = 1 << 2,
        CPU = 1 << 3,
        NO_SET = 0
    };

    enum class DeviceMetricsMonitorType : unsigned int {
        ALL = 0xFFFFFFFF,
        BATTERY = 1 << 1,
        MEMORY = 1 << 2,
        CPU = 1 << 3,
        FPS = 1 << 4,
        NO_SET = 0
    };

    enum class DetectFrequency : unsigned short {
        DEFAULT = 500,
        FREQUENT = 100,
        RARE = 1000
    };

	enum HTTP_STATUS
	{
		HTTP_OK = 200,
		HTTP_CREATED = 201,
		HTTP_ACCEPTED = 202,
		HTTP_NOT_AUTHORITATIVE = 203,
		HTTP_NO_CONTENT = 204,
		HTTP_RESET = 205,
		HTTP_PARTIAL = 206,
		HTTP_MULT_CHOICE = 300,
		HTTP_MOVED_PERM = 301,
		HTTP_MOVED_TEMP = 302,
		HTTP_SEE_OTHER = 303,
		HTTP_NOT_MODIFIED = 304,
		HTTP_USE_PROXY = 305,
		HTTP_BAD_REQUEST = 400,
		HTTP_UNAUTHORIZED = 401,
		HTTP_PAYMENT_REQUIRED = 402,
		HTTP_FORBIDDEN = 403,
		HTTP_NOT_FOUND = 404,
		HTTP_BAD_METHOD = 405,
		HTTP_NOT_ACCEPTABLE = 406,
		HTTP_PROXY_AUTH = 407,
		HTTP_CLIENT_TIMEOUT = 408,
		HTTP_CONFLICT = 409,
		HTTP_GONE = 410,
		HTTP_LENGTH_REQUIRED = 411,
		HTTP_PRECON_FAILED = 412,
		HTTP_ENTITY_TOO_LARGE = 413,
		HTTP_REQ_TOO_LONG = 414,
		HTTP_UNSUPPORTED_TYPE = 415,

		/** @deprecated */
		HTTP_SERVER_ERROR = 500,
		HTTP_INTERNAL_ERROR = 500,
		HTTP_NOT_IMPLEMENTED = 501,
		HTTP_BAD_GATEWAY = 502,
		HTTP_UNAVAILABLE = 503,
		HTTP_GATEWAY_TIMEOUT = 504,
		HTTP_VERSION = 505,
	};
}

#endif // !_DATAKIT_SDK_CPP_INTERNAL_ENUMS_H_

