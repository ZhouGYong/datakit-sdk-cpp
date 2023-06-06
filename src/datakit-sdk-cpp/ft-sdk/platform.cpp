#include "pch.h"
#include "Platform.h"
#if defined(WIN32)
#include <iostream>
#include <Windows.h>
#include <iphlpapi.h>
#include <cfgmgr32.h>
#include <tchar.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Cfgmgr32.lib")
#elif __linux__
#include <ifaddrs.h>
#include <netdb.h>
#include <cstring>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <csignal>
#endif
#include <vector>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "LoggerManager.h"
#include "Utils.h"

namespace com::ft::sdk::internal::platform
{
    //creates a static variable to convert Bytes to Megabytes
    #define MB 1048576

    std::string getOSName() 
    {
#ifdef _WIN32
        std::string osName = "Windows";

        HKEY hKey;
        DWORD dwType;
        DWORD dwSize = MAX_PATH;
        TCHAR szProductType[MAX_PATH];

        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
        {
            if (RegGetValue(hKey, nullptr, TEXT("ProductName"), RRF_RT_REG_SZ, &dwType, szProductType, &dwSize) == ERROR_SUCCESS)
            {
                utils::Wchar_tToString(osName, (wchar_t*)szProductType);
            }

            RegCloseKey(hKey);
        }
        return osName;
#elif __linux__
        // todo : use lsb_release -a
        /**
         * root@iZbp12igqv5c7jywq6l2nmZ:~/dev/libs# lsb_release -a
            LSB Version:	core-11.1.0ubuntu2-noarch:security-11.1.0ubuntu2-noarch
            Distributor ID:	Ubuntu
            Description:	Ubuntu 20.04.6 LTS
            Release:	20.04
            Codename:	focal
         * 
         * \return 
         */
        std::string osName = "Linux";

        //struct utsname buf;

        //if (uname(&buf) != -1) {
        //    osName = buf.sysname;
        //}

        std::string osNameReg = "sudo lsb_release -a | grep -Po 'Distributor ID:	\\K[^\"]*'";
        osName = utils::trimRet(utils::execShellCmd(osNameReg));
        if (osName == "")
        {
            osName = "Linux";
        }

        return osName;
#else
        return "Unknown";
#endif
    }

    std::string getOSVersion() 
    {
#ifdef _WIN32
        const auto system = L"kernel32.dll";
        DWORD dummy = 0;
        const auto cbInfo = ::GetFileVersionInfoSizeExW(FILE_VER_GET_NEUTRAL, system, &dummy);
        std::vector<char> buffer(cbInfo);

        if (TRUE == ::GetFileVersionInfoExW(FILE_VER_GET_NEUTRAL, system, dummy, buffer.size(), &buffer[0]))
        {
            void* p = nullptr;
            UINT size = 0;
            ::VerQueryValueW(buffer.data(), L"\\", &p, &size);
            assert(size >= sizeof(VS_FIXEDFILEINFO));
            assert(p != nullptr);
            auto pFixed = static_cast<const VS_FIXEDFILEINFO*>(p);
            //std::cout << HIWORD(pFixed->dwFileVersionMS) << '.'
            //    << LOWORD(pFixed->dwFileVersionMS) << '.'
            //    << HIWORD(pFixed->dwFileVersionLS) << '.'
            //    << LOWORD(pFixed->dwFileVersionLS) << '\n';
            return std::to_string(HIWORD(pFixed->dwFileVersionMS)) +
                "." + std::to_string(LOWORD(pFixed->dwFileVersionMS)) +
                "." + std::to_string(HIWORD(pFixed->dwFileVersionLS)) +
                "." + std::to_string(LOWORD(pFixed->dwFileVersionLS));
        }
        else
        {
            return "0.0.0.0";
        }
        //DWORD majorVersion, minorVersion, buildNumber;
        //// Get the version information from Windows API
        //GetProductInfo(
        //    /* dwOSMajorVersion */ majorVersion,
        //    /* dwOSMinorVersion */ minorVersion,
        //    /* dwSpMajorVersion */ NULL,
        //    /* dwSpMinorVersion */ NULL,
        //    /* pdwReturnedProductType */ NULL);

        //buildNumber = 0;
        //// Return the version number as a string
        //return std::to_string(majorVersion) + "." + std::to_string(minorVersion) + "." + std::to_string(buildNumber);
#elif __linux__
        // Get the version information from the /etc/os-release file
        std::ifstream osReleaseFile("/etc/os-release");
        std::string line;
        std::string version;
        while (std::getline(osReleaseFile, line)) {
            if (line.find("VERSION_ID=") != std::string::npos) {
                version = line.substr(line.find("=") + 1);
                //version.erase(std::remove(version.begin(), version.end(), '"'), version.end());
                break;
            }
        }

        // Return the version number as a string
        version = utils::replaceString(version, "\"", "");
        return version;
#else
        return "Unknown";
#endif
    }

    std::string getMajorVersion(const std::string& versionString) 
    {
        size_t pos = versionString.find('.');
        if (pos == std::string::npos) {
            return "-1"; // Invalid version string
        }
        std::string majorVersionString = versionString.substr(0, pos);
        return majorVersionString;
        //return std::stoi(majorVersionString);
    }

    /// <summary>
    /// to get computer's product uuid
    /// </summary>
    /// <returns></returns>
    std::string getDeviceUUID()
    {
        std::string deviceUuid = "Unknown Device UUID";

#ifdef _WIN32
        //HW_PROFILE_INFO hwProfileInfo;
        //if (GetCurrentHwProfile(&hwProfileInfo))
        //{
        //    utils::Wchar_tToString(deviceUuid, hwProfileInfo.szHwProfileGuid);
        //}

        DWORD bufsize = 0;
        BYTE buf[65536] = { 0 };
        int ret = 0;
        RawSMBIOSData* Smbios;
        dmi_header* h = NULL;
        int flag = 1;

        ret = GetSystemFirmwareTable('RSMB', 0, 0, 0);
        if (!ret)
        {
            //printf("Function failed!\n");
            return deviceUuid;
        }

        //printf("get buffer size is %d\n", ret);
        bufsize = ret;

        ret = GetSystemFirmwareTable('RSMB', 0, buf, bufsize);

        if (!ret)
        {
            //printf("Function failed!\n");
            return deviceUuid;
        }



        Smbios = (RawSMBIOSData*)buf;
        BYTE* p = Smbios->SMBIOSTableData;

        if (Smbios->Length != bufsize - 8)
        {
            //printf("Smbios length error\n");
            return deviceUuid;
        }

        for (int i = 0; i < Smbios->Length; i++) {
            h = (dmi_header*)p;

            if (h->type == 1) 
            {
                if (dmi_system_uuid(p + 0x8, Smbios->SMBIOSMajorVersion * 0x100 + Smbios->SMBIOSMinorVersion, &deviceUuid))
                {
                    LoggerManager::getInstance().logDebug("Got the device uuid: {}", deviceUuid);
                }
                else
                {
                    LoggerManager::getInstance().logDebug("Failed to get the device uuid: {}", deviceUuid);
                }
                break;
                //printf("\nType %02d - [System Information]\n", h->type);
                //printf("\tManufacturer: %s\n", dmi_string(h, p[0x4]));
                //printf("\tProduct Name: %s\n", dmi_string(h, p[0x5]));
                //printf("\tVersion: %s\n", dmi_string(h, p[0x6]));
                //printf("\tSerial Number: %s\n", dmi_string(h, p[0x7]));
                //printf("\tUUID: "); dmi_system_uuid(p + 0x8, Smbios->SMBIOSMajorVersion * 0x100 + Smbios->SMBIOSMinorVersion);
                //printf("\tSKU Number: %s\n", dmi_string(h, p[0x19]));
                //printf("\tFamily: %s\n", dmi_string(h, p[0x1a]));
            }
            p += h->length;
            while ((*(WORD*)p) != 0) p++;
            p += 2;
        }


#elif __linux__
        //std::ifstream ifs("/sys/class/dmi/id/product_uuid");
        //std::string uuid((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        //deviceUuid = uuid;
        std::string deviceUuidReg = "sudo dmidecode -t1 | grep -Po 'UUID: \\K[^\"]*'";
        deviceUuid = utils::trimRet(utils::execShellCmd(deviceUuidReg));

        if (deviceUuid == "")
        {
            deviceUuid = "require root privilege to get device uuid in Linux!";
        }

#endif

        return deviceUuid;
    }

    float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
    {
        static unsigned long long _previousTotalTicks = 0;
        static unsigned long long _previousIdleTicks = 0;

        unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
        unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

        float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

        _previousTotalTicks = totalTicks;
        _previousIdleTicks = idleTicks;
        return ret;
    }

#ifdef _WIN32
    unsigned long long FileTimeToInt64(const FILETIME& ft) 
    { 
        return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime); 
    }
#endif

    // Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
    // You'll need to call this at regular intervals, since it measures the load between
    // the previous call and the current one.  Returns -1.0 on error.
    float GetCPULoad()
    {
#ifdef _WIN32
        FILETIME idleTime, kernelTime, userTime;
        return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
#elif __linux__        
        std::string coreReg = "lscpu | grep -Po 'CPU\\(s\\):      +\\K[^\"]*'";
        int coreNum = std::stoi(utils::trimRet(utils::execShellCmd(coreReg)));
        //printf("CPU CORE NUM=%d\n\n", coreNum);

        std::string loadReg = "cat /proc/loadavg | grep -Eo '^[^ ]+'";
        float loadIn1Min = std::stof(utils::trimRet(utils::execShellCmd(coreReg)));
        //printf("LOAD AVG=%f\n\n", loadIn1Min);

        return loadIn1Min/(float)coreNum;
#endif
    }

    float getMemoryLoad()
    {
#ifdef _WIN32
        //Code block intiialization for the memory referenced in the Kernell
        MEMORYSTATUSEX memStat;
        memStat.dwLength = sizeof(memStat);
        GlobalMemoryStatusEx(&memStat);

        return (float)memStat.dwMemoryLoad / (float)memStat.ullTotalPhys;
#elif __linux__
        /* Obtain system statistics. */
        struct sysinfo si;
        sysinfo(&si);
        return (float)(si.totalram - si.freeram) / (float)si.totalram;
#endif
    }

    std::string getNetworkType()
    {
        const std::string UNKNOWN_NETWORK_TYPE = "Unknown Type";
        std::string networkType = UNKNOWN_NETWORK_TYPE;
#ifdef _WIN32
        PIP_ADAPTER_INFO pAdapterInfo;
        PIP_ADAPTER_INFO pAdapter = NULL;
        DWORD dwRetVal = 0;
        ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
        if (pAdapterInfo == NULL) {
            LoggerManager::getInstance().logError("Error allocating memory needed to call GetAdaptersinfo");
            return networkType;
        }
        if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
            free(pAdapterInfo);
            pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
            if (pAdapterInfo == NULL) {
                LoggerManager::getInstance().logError("Error allocating memory needed to call GetAdaptersinfo");
                return networkType;
            }
        }
        if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != NO_ERROR) {
            LoggerManager::getInstance().logError("GetAdaptersInfo failed with error: " + std::to_string(dwRetVal));
            return networkType;
        }
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            //cout << "Network Adapter Name: " << pAdapter->Description << endl;
            //cout << "Network Adapter Type: ";
            switch (pAdapter->Type) {
            case MIB_IF_TYPE_OTHER:
                networkType = "Other";
                break;
            case MIB_IF_TYPE_ETHERNET:
                networkType = "Ethernet";
                break;
            case MIB_IF_TYPE_TOKENRING:
                networkType = "Token Ring";
                break;
            case MIB_IF_TYPE_FDDI:
                networkType = "FDDI";
                break;
            case MIB_IF_TYPE_PPP:
                networkType = "PPP";
                break;
            case MIB_IF_TYPE_LOOPBACK:
                networkType = "Lookback";
                break;
            case MIB_IF_TYPE_SLIP:
                networkType = "Slip";
                break;
            default:
                networkType = UNKNOWN_NETWORK_TYPE;
                break;
            }

            if (networkType != UNKNOWN_NETWORK_TYPE)
            {
                break;
            }
            pAdapter = pAdapter->Next;
        }
        if (pAdapterInfo)
            free(pAdapterInfo);

#elif __linux__
        struct ifaddrs* ifaddr;
        struct ifaddrs* ifa;
        int n;

        if (getifaddrs(&ifaddr) == -1) 
        {
            std::cerr << "Error getting network interfaces" << std::endl;
            return networkType;
        }

        for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) 
        {
            if (ifa->ifa_addr == NULL)
                continue;

            int family = ifa->ifa_addr->sa_family;

            // Only interested in AF_PACKET (i.e., Ethernet) and AF_INET (i.e., IPv4)
            if (family == AF_PACKET || family == AF_INET) 
            {
                //std::cout << "Interface: " << ifa->ifa_name << std::endl;

                // Check for Wi-Fi interface
                if (strstr(ifa->ifa_name, "wlan") != NULL) 
                {
                    //std::cout << "Network Type: Wi-Fi" << std::endl;
                    networkType = "Wi-Fi";
                }
                // Check for Ethernet interface
                else if (strstr(ifa->ifa_name, "eth") != NULL)
                {
                    //std::cout << "Network Type: Ethernet" << std::endl;
                    networkType = "Ethernet";
                }
            }

            if (networkType != UNKNOWN_NETWORK_TYPE)
            {
                break;
            }
        }

        freeifaddrs(ifaddr);
#endif

        return networkType;
    }

    std::string getAppVersionName()
    {
        std::string version = "0.0.0";

#ifdef _WIN32
        TCHAR szExePath[MAX_PATH];
        GetModuleFileName(NULL, szExePath, MAX_PATH);

        DWORD dwHandle;
        DWORD dwLen = GetFileVersionInfoSize(szExePath, &dwHandle);

        if (dwLen > 0)
        {
            LPBYTE lpData = new BYTE[dwLen];

            if (GetFileVersionInfo(szExePath, dwHandle, dwLen, lpData))
            {
                VS_FIXEDFILEINFO* pFileInfo;
                UINT uLen;

                if (VerQueryValue(lpData, _T("\\"), (LPVOID*)&pFileInfo, &uLen))
                {
                    DWORD dwFileVersionMS = pFileInfo->dwFileVersionMS;
                    DWORD dwFileVersionLS = pFileInfo->dwFileVersionLS;

                    version = std::to_string(HIWORD(dwFileVersionMS)) +
                        "." + std::to_string(LOWORD(dwFileVersionMS)) +
                        "." + std::to_string(HIWORD(dwFileVersionLS)) +
                        "." + std::to_string(LOWORD(dwFileVersionLS));
                }
            }

            delete[] lpData;
        }
#elif __linux__

#endif // 0

        return version;
    }

    DeviceInfo getDeviceInfo()
    {
        DeviceInfo devInfo;
#ifdef _WIN32
        HKEY hKey;
        DWORD dwType = REG_SZ;
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            char szValue[MAX_PATH] = { 0 };
            DWORD dwSize = sizeof(szValue);
            if (RegQueryValueEx(hKey, L"SystemManufacturer", NULL, &dwType, (LPBYTE)szValue, &dwSize) == ERROR_SUCCESS)
            {
                utils::Wchar_tToString(devInfo.band, (wchar_t*)szValue);
            }

            char szValue2[1024] = { 0 };
            DWORD dwSize2 = sizeof(szValue2);
            if (RegQueryValueEx(hKey, L"SystemProductName", NULL, &dwType, (LPBYTE)szValue2, &dwSize2) == ERROR_SUCCESS)
            {
                utils::Wchar_tToString(devInfo.model, (wchar_t*)szValue2);
            }
            RegCloseKey(hKey);
        }

        int width = 1920;
        int height = 1080;
        //int width = GetSystemMetrics(SM_CXSCREEN);
        //int height = GetSystemMetrics(SM_CYSCREEN);

        DEVMODE dm;
        dm.dmSize = sizeof(dm);
        if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm) != 0)
        {
            width = dm.dmPelsWidth;
            height = dm.dmPelsHeight;
            //std::cout << "Width: " << dm.dmPelsWidth << std::endl;
            //std::cout << "Height: " << dm.dmPelsHeight << std::endl;
        }
        devInfo.screen_size = std::to_string(width) + "*" + std::to_string(height);
#elif __linux__
        std::string manufacturerReg = "sudo dmidecode -t1 | grep -Po 'Manufacturer: \\K[^\"]*'";
        devInfo.band = utils::trimRet(utils::execShellCmd(manufacturerReg));
        if (devInfo.band == "")
        {
            devInfo.band = "require root privilege to get this info in Linux!";
        }
        std::string productNameReg = "sudo dmidecode -t1 | grep -Po 'Product Name: \\K[^\"]*'";
        devInfo.model = utils::trimRet(utils::execShellCmd(productNameReg));
        if (devInfo.model == "")
        {
            devInfo.model = "require root privilege to get this info in Linux!";
        }

        devInfo.screen_size = "1920*1080";
#endif
        return devInfo;
    }

#ifdef _WIN32
    bool dmi_system_uuid(const BYTE* p, short ver, std::string* pStrUuid)
    {
        int only0xFF = 1, only0x00 = 1;
        int i;

        for (i = 0; i < 16 && (only0x00 || only0xFF); i++)
        {
            if (p[i] != 0x00)
            {
                only0x00 = 0;
            }
            if (p[i] != 0xFF)
            {
                only0xFF = 0;
            }
        }

        if (only0xFF)
        {
            //printf("Not Present");
            return false;
        }

        if (only0x00)
        {
            //printf("Not Settable");
            return false;
        }

        char hareware_uuid[MAX_PATH];
        std::memset(hareware_uuid, 0, sizeof(hareware_uuid));
        if (ver >= 0x0206)
        {
            sprintf(hareware_uuid, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                p[3], p[2], p[1], p[0], p[5], p[4], p[7], p[6],
                p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
        }
        else
        {
            sprintf(hareware_uuid, "-%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
                p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
        }

        *pStrUuid = hareware_uuid;

        return true;
    }


    const char* dmi_string(const dmi_header* dm, BYTE s)
    {
        char* bp = (char*)dm;
        size_t i, len;

        if (s == 0)
            return "Not Specified";

        bp += dm->length;

        while (s > 1 && *bp)
        {
            bp += strlen(bp);
            bp++;
            s--;
        }
        if (!*bp)
            return "BAD_INDEX";

        /* ASCII filtering */
        len = strlen(bp);
        for (i = 0; i < len; i++)
            if (bp[i] < 32 || bp[i] == 127)
                bp[i] = '.';
        return bp;
    }
#endif

#ifdef _WIN32
    void se_translator(unsigned int u, EXCEPTION_POINTERS* pExp)
#elif __linux__
    void sig_handler(int sig)
#endif // _WIN32
    {
        throw std::runtime_error("Access violation");
    }


    void registerSystemExceptionHandler()
    {
#ifdef _WIN32
        _set_se_translator(se_translator);
#elif __linux__
        signal(SIGFPE, sig_handler);
#endif // _WIN32

    }
}