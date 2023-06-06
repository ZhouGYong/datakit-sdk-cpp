// ft-sdk-sample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "FTSDKFactory.h"
#include <vector>

#include "restclient-cpp/restclient.h"
#include "restclient-cpp/connection.h"

#include "Helper.h"

using namespace com::ft::sdk;


int main()
{
    std::string TEST_FAKE_URL = "https://baike.baidu.com/calendar/?param=1";
    std::string TEST_FAKE_URL2 = "http://www.guance.com/test.html?param=2";

    std::string CUSTOM_KEY = "custom_key";
    std::string CUSTOM_VALUE = "custom_value";

    std::string TRACK_ID = "null";

    // 1. initialzie SDK environment
    auto sdk = FTSDKFactory::get("ft_sdk_config.json");
    sdk->init();

    // 2. prepare the settings for connection
    FTSDKConfig gc;
    gc.setServerUrl("http://172.25.248.216:9529")
        .setEnv(EnvType::PROD)
        .setEnableFileDBCache(true);
#ifdef __linux__
    // there is no similiar file version concept in linux,
    // the app has to pass its own version to sdk.
    gc.setAppVersion("0.6.2");
#endif // __linux__

    gc.addGlobalContext(CUSTOM_KEY, CUSTOM_VALUE);

    FTRUMConfig rc;
    rc.setRumAppId("my_test_app");
    rc.addGlobalContext("track_id", TRACK_ID);
    rc.addGlobalContext("custom_tag", "any tags");


    FTTraceConfig tc;
    tc.setTraceType(TraceType::DDTRACE)
        .setEnableLinkRUMData(true);

    FTLogConfig lpc;
    lpc.setEnableCustomLog(true)
        .setEnableLinkRumData(true);

    sdk->install(gc)
        .initRUMWithConfig(rc)
        .initTraceWithConfig(tc)
        .initLogWithConfig(lpc);

    UserData uc;
    uc.init("zgy", "1001", "zhougy2009@gmail.com");
    uc.addCustomizeItem("ft_key", "ft_value");
    sdk->bindUserData(uc);

    // 3. simulate a user defined event
    //std::vector<TrackNode> vtTrackNodes;
    //vtTrackNodes.push_back(TrackNodeBuilder::build(constants::KEY_RUM_LONG_TASK_DURATION, "5000" ));
    //vtTrackNodes.push_back(TrackNodeBuilder::build(constants::KEY_RUM_LONG_TASK_STACK, "just4test" ));
    //sdk->addCustomTrack(constants::FT_MEASUREMENT_RUM_LONG_TASK, vtTrackNodes);

    //auto tn = TrackNodeBuilder::build(constants::FT_MEASUREMENT_RUM_LONG_TASK);
    //tn->addTrackProperty(constants::KEY_RUM_LONG_TASK_DURATION, 5000).addTrackProperty(constants::KEY_RUM_LONG_TASK_STACK, "Handler (android.view.ViewRootImpl$ViewRootHandler) {2f6fcc3} android.view.View$PerformClick@15cad97: 0");
    //sdk->addCustomTrack(tn);

    // 4. generate trace header base on the trace config
    auto header = sdk->generateTraceHeader("https://www.example.com/test.html?param=1");

    // 5. -- start testing rum entities
    sdk->startView("TEST_VIEW_ONE");

    sdk->startAction("just4test", "click");
    sdk->addLongTask("test long task", 100010);
    sdk->addError("test error 1", "first error", RUMErrorType::NATIVE_CRASH, AppState::UNKNOWN);
    sdk->addError("test error 2", "second error", RUMErrorType::NETWORK_ERROR, AppState::UNKNOWN);


    // 6. generate trace header linked to RUM resource
    std::string resId = "resource id 1111";
    NetStatus status;
    ResourceParams params;
    request(TEST_FAKE_URL, resId, params, status, sdk);
    sdk->addResource(resId, params, status);

    sdk->stopAction();
    sdk->stopView();

    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    sdk->startView("TEST_VIEW_TWO");

    //std::string resId2 = "resource id 2222";
    //sdk->startResource(resId2);
    //sdk->stopResource(resId2);

    //// 6. generate trace header linked to RUM resource
    //NetStatus status2;
    //ResourceParams params2;
    //request(TEST_FAKE_URL2, resId2, params2, status2, sdk);
    //sdk->addResource(resId2, params2, status2);

    sdk->startView("TEST_VIEW_THREE");
    // 6. upload the user log
    sdk->addLog("this\\is a \"test\" log", LogLevel::INFO);
    sdk->stopView();    //测试
    
    // 7. wait a litter longer so that the background thread has enough time to upload the event to datakit agent
    std::cout << "Press any key to exit...";
    char ch = std::cin.get();

    // 8. shutdown the SDK
    sdk->deinit();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
