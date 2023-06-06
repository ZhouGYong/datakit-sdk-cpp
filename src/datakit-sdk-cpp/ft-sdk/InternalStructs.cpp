#include "pch.h"
#include "InternalStructs.h"

namespace com::ft::sdk::internal
{
    std::string _trim(const std::string& str)
    {
        size_t start = str.find_first_not_of(" \n\r\t");
        size_t until = str.find_last_not_of(" \n\r\t");
        std::string::const_iterator i = start == std::string::npos ? str.begin() : str.begin() + start;
        std::string::const_iterator x = until == std::string::npos ? str.end() : str.begin() + until + 1;
        return std::string(i, x);
    }

    PropagationUrl::PropagationUrl()
    {
        //typedef std::string::const_iterator iterator_t;
        //typedef std::string::size_type position_t;

        //if (uri.length() == 0)
        //    return ;

        //iterator_t uriEnd = uri.end();

        //position_t queryStart = uri.find('?');
        //// get query start
        //iterator_t queryStart = std::find(nullptr, uri.begin(), uriEnd, L'?');

        //// protocol
        //iterator_t protocolStart = uri.begin();
        //iterator_t protocolEnd = std::find(protocolStart, uriEnd, ':');            //"://");

        //if (protocolEnd != uriEnd)
        //{
        //    std::string prot = &*(protocolEnd);
        //    if ((prot.length() > 3) && (prot.substr(0, 3) == "://"))
        //    {
        //        this->protocol = std::string(protocolStart, protocolEnd);
        //        protocolEnd += 3;   //      ://
        //    }
        //    else
        //        protocolEnd = uri.begin();  // no protocol
        //}
        //else
        //    protocolEnd = uri.begin();  // no protocol

        //// domain
        //iterator_t hostStart = protocolEnd;
        //iterator_t pathStart = std::find(hostStart, uriEnd, L'/');  // get pathStart

        //iterator_t hostEnd = std::find(protocolEnd,
        //    (pathStart != uriEnd) ? pathStart : queryStart,
        //    ':');  // check for port

        //this->domain = std::string(hostStart, hostEnd);

        //// port
        //if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == L':'))  // we have a port
        //{
        //    hostEnd++;
        //    iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
        //    this->port = std::stoi(std::string(hostEnd, portEnd));
        //}

        //// path
        //if (pathStart != uriEnd)
        //{
        //    this->path = std::string(pathStart, queryStart);
        //}

        //// query
        //if (queryStart != uriEnd)
        //{
        //    this->query = std::string(queryStart, uri.end());
        //}
    }

    PropagationUrl PropagationUrl::parse(const std::string& url)
    {
        PropagationUrl proUrl;

        std::string trimedUrl;
        int offset = 0;
        size_t pos1, pos2, pos3, pos4;

        trimedUrl = _trim(url);
        offset = offset == 0 && trimedUrl.compare(0, 8, "https://") == 0 ? 8 : offset;
        offset = offset == 0 && trimedUrl.compare(0, 7, "http://") == 0 ? 7 : offset;
        pos1 = trimedUrl.find_first_of('/', offset + 1);
        proUrl.path = pos1 == std::string::npos ? "" : trimedUrl.substr(pos1);
        proUrl.domain = std::string(trimedUrl.begin() + offset, pos1 != std::string::npos ? trimedUrl.begin() + pos1 : trimedUrl.end());
        proUrl.path = (pos2 = proUrl.path.find("#")) != std::string::npos ? proUrl.path.substr(0, pos2) : proUrl.path;
        proUrl.port = std::stoi((pos3 = proUrl.domain.find(":")) != std::string::npos ? proUrl.domain.substr(pos3 + 1) : "80");
        proUrl.domain = proUrl.domain.substr(0, pos3 != std::string::npos ? pos3 : proUrl.domain.length());
        proUrl.protocol = offset > 0 ? trimedUrl.substr(0, offset - 3) : "";
        proUrl.query = (pos4 = proUrl.path.find("?")) != std::string::npos ? proUrl.path.substr(pos4 + 1) : "";
        proUrl.path = pos4 != std::string::npos ? proUrl.path.substr(0, pos4) : proUrl.path;

        return proUrl;
    }

    long NetStatusHelper::getTcpTime(NetStatus& netStatus)
    {
        if (netStatus.tcpTime > 0)
        {
            return netStatus.tcpTime;
        }

        if (netStatus.tcpEndTime > netStatus.tcpStartTime)
        {
            return netStatus.tcpEndTime - netStatus.tcpStartTime;
        }
        return 0;
    }

    long NetStatusHelper::getDNSTime(NetStatus& netStatus)
    {
        if (netStatus.dnsTime > 0)
        {
            return netStatus.dnsTime;
        }

        if (netStatus.dnsEndTime > netStatus.dnsStartTime)
        {
            return netStatus.dnsEndTime - netStatus.dnsStartTime;
        }
        return 0;
    }

    long NetStatusHelper::getResponseTime(NetStatus& netStatus)
    {
        if (netStatus.responseTime > 0)
        {
            return netStatus.responseTime;
        }

        if (netStatus.responseEndTime > netStatus.responseStartTime)
        {
            return netStatus.responseEndTime - netStatus.responseStartTime;
        }
        return 0;
    }

    long NetStatusHelper::getTTFB(NetStatus& netStatus)
    {
        if (netStatus.ttfb > 0)
        {
            return netStatus.ttfb;
        }

        if (netStatus.responseStartTime > netStatus.fetchStartTime)
        {
            return netStatus.responseStartTime - netStatus.fetchStartTime;
        }
        return 0;
    }

    long NetStatusHelper::getFirstByteTime(NetStatus& netStatus)
    {
        if (netStatus.firstByteTime > 0)
        {
            return netStatus.firstByteTime;
        }

        if (netStatus.responseStartTime > netStatus.dnsStartTime)
        {
            if (netStatus.dnsStartTime > 0)
            {
                return netStatus.responseStartTime - netStatus.dnsStartTime;
            }
            else {
                return netStatus.responseStartTime - netStatus.fetchStartTime;
            }
        }
        return 0;
    }

    long NetStatusHelper::getHoleRequestTime(NetStatus& netStatus) 
    {
        if (netStatus.responseEndTime > netStatus.fetchStartTime) {
            return netStatus.responseEndTime - netStatus.fetchStartTime;
        }
        return 0;
    }

    long NetStatusHelper::getSSLTime(NetStatus& netStatus)
    {
        if (netStatus.sslTime > 0)
        {
            return netStatus.sslTime;
        }

        if (netStatus.sslEndTime > netStatus.sslStartTime)
        {
            return netStatus.sslEndTime - netStatus.sslStartTime;
        }
        return 0;
    }

    void NetStatusHelper::reset(NetStatus& netStatus) 
    {
        netStatus.fetchStartTime = -1;
        netStatus.tcpStartTime = -1;
        netStatus.tcpEndTime = -1;
        netStatus.dnsStartTime = -1;
        netStatus.dnsEndTime = -1;
        netStatus.responseStartTime = -1;
        netStatus.responseEndTime = -1;
        netStatus.sslStartTime = -1;
        netStatus.sslEndTime = -1;

        netStatus.tcpTime = -1;
        netStatus.dnsTime = -1;
        netStatus.sslTime = -1;
        netStatus.firstByteTime = -1;
        netStatus.ttfb = -1;
        netStatus.responseTime = -1;
    }

}
