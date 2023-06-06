#include "Helper.h"


#include <vector>

#include "restclient-cpp/restclient.h"
#include "restclient-cpp/connection.h"

using namespace com::ft::sdk;

const long ns_factor = 1000 * 1000 * 1000;

void request(const std::string& url, const std::string& resId, ResourceParams& params, NetStatus& status, std::unique_ptr<FTSDK>& pSDK)
{
    // initialize RestClient
    RestClient::init();

    // get a connection object
    RestClient::Connection* conn = new RestClient::Connection(url);

    // set connection timeout to 5s
    conn->SetTimeout(5);

    // set custom user agent
    // (this will result in the UA "foo/cool restclient-cpp/VERSION")
    conn->SetUserAgent("foo/cool");

    // enable following of redirects (default is off)
    conn->FollowRedirects(true);
    // and limit the number of redirects (default is -1, unlimited)
    conn->FollowRedirects(true, 3);

    // set headers
    RestClient::HeaderFields headers;
    headers["Accept"] = "application/json";

    auto headerWithRes = pSDK->generateTraceHeader(resId, url);
    for (auto& hd : headerWithRes)
    {
        headers[hd.first] = hd.second;
    }
    conn->SetHeaders(headers);

    pSDK->startResource(resId);
    RestClient::Response r = conn->get("/get");
    pSDK->stopResource(resId);

    RestClient::Connection::Info info = conn->GetInfo();

    params.resourceMethod = "GET";
    params.requestHeader = convert(headers);
    params.responseHeader = convert(r.headers);

    params.responseBody = r.body;
    params.responseConnection = "Keep-Alive";
    params.responseContentEncoding = "UTF-8";
    params.responseContentType = r.headers["Content-Type"];
    params.url = url;
    params.resourceStatus = r.code;

    status.dnsTime = info.lastRequest.nameLookupTime * ns_factor;
    status.tcpTime = (info.lastRequest.connectTime - info.lastRequest.nameLookupTime ) * ns_factor;
    status.sslTime = (info.lastRequest.appConnectTime - info.lastRequest.connectTime) * ns_factor;
    status.ttfb = (info.lastRequest.startTransferTime - info.lastRequest.preTransferTime) * ns_factor;
    status.responseTime = (info.lastRequest.totalTime -info.lastRequest.startTransferTime) * ns_factor;
    status.firstByteTime = info.lastRequest.startTransferTime * ns_factor;
    
    // deinit RestClient. After calling this you have to call RestClient::init()
    // again before you can use it
    RestClient::disable();
}

std::string convert(RestClient::HeaderFields& header)
{
    std::string strHdr = "{";
    for (auto& item : header)
    {
        strHdr.append(item.first + "=" + item.second + ", ");
    }
    strHdr = strHdr.substr(0, strHdr.size() - 2);
    strHdr.append("}");

    return strHdr;
}