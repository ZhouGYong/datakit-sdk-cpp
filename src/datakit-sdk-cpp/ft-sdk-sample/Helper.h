#pragma once

#include "FTSDKFactory.h"
#include "restclient-cpp/restclient.h"

using namespace com::ft::sdk;

std::string convert(RestClient::HeaderFields& header);
void request(const std::string& url, const std::string& resId, ResourceParams& params, NetStatus& status, std::unique_ptr<FTSDK>& pSDK);


