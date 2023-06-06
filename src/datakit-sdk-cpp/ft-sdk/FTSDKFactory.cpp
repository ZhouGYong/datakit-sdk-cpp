
#include "Include/FTSDKFactory.h"
#include "FTSDKError.h"

namespace com::ft::sdk
{
    std::unique_ptr<FTSDK> FTSDKFactory::get(const std::string& configJson)
    {
        FTSDK_CHECK_RETURN(std::unique_ptr<FTSDK>, std::unique_ptr<FTSDK>(new FTSDK(configJson)));
    }
}