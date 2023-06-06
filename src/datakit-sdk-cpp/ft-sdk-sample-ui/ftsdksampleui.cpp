#include "ftsdksampleui.h"
#include <QMessageBox>
#include <windows.h>

#include "Helper.h"
#include "restclient-cpp/restclient.h"
#include "restclient-cpp/connection.h"

const std::string CUSTOM_KEY = "custom_key";
const std::string CUSTOM_VALUE = "custom_value";
const std::string TRACK_ID = "null";
const std::string TEST_VIEW_ONE = "TEST_VIEW_ONE";
const std::string TEST_VIEW_TWO = "TEST_VIEW_TWO";
const std::string TEST_VIEW_THREE = "TEST_VIEW_THREE";

#define CHECK_INITED()  \
    if (!m_isInited) {      \
        QMessageBox::information(this, "INFO", "Please initialized first!");    \
        return ;        \
    }

ftsdksampleui::ftsdksampleui(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    m_pFTSDK = FTSDKFactory::get("ft_sdk_config.json");
    m_pFTSDK->init();
}

ftsdksampleui::~ftsdksampleui()
{
    m_pFTSDK->deinit();
}


void ftsdksampleui::on_init_clicked()
{
    try
    {
        ui.firstViewBtn->setChecked(false);
        ui.secondViewBtn->setChecked(false);
        ui.thirdViewBtn->setChecked(false);

        std::string datakitUrl = ui.datakitUrlText->text().toStdString();
        FTSDKConfig gc;
        gc.setServerUrl(datakitUrl)
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

        m_pFTSDK->install(gc)
            .initRUMWithConfig(rc)
            .initTraceWithConfig(tc)
            .initLogWithConfig(lpc);

        UserData uc;
        uc.init("zgy", "1001", "zhougy2009@gmail.com");
        uc.addCustomizeItem("ft_key", "ft_value");
        m_pFTSDK->bindUserData(uc);

        m_isInited = true;
        ui.initBtn->setDisabled(true);

        QMessageBox::information(this, "INFO", "Initialization is done!");
    }
    catch (const std::exception& ex)
    {
        QMessageBox::critical(this, "Error", "Failed to initialize£¡");
    }
}

void ftsdksampleui::on_startAction_clicked()
{
    CHECK_INITED();
    m_pFTSDK->startAction("simulate_click_button", "click");
}

void ftsdksampleui::on_endAction_clicked()
{
    CHECK_INITED();
    m_pFTSDK->stopAction();
}

void ftsdksampleui::on_firstView_clicked()
{
    CHECK_INITED();
    ui.secondViewBtn->setChecked(false);
    ui.thirdViewBtn->setChecked(false);

    if (m_viewIdx == 0)
    {
        m_pFTSDK->startView(TEST_VIEW_ONE);
        m_viewIdx = 1;
    }
    else if (m_viewIdx == 1)
    {
        //if (!ui.firstViewBtn->isChecked())
        {
            m_pFTSDK->stopView();
            m_viewIdx = 0;
        }
    }
    else
    {
        m_pFTSDK->stopView();
        m_pFTSDK->startView(TEST_VIEW_ONE);
        m_viewIdx = 1;
    }

}

void ftsdksampleui::on_secondView_clicked()
{
    CHECK_INITED();
    ui.firstViewBtn->setChecked(false);
    ui.thirdViewBtn->setChecked(false);

    if (m_viewIdx == 0)
    {
        m_pFTSDK->startView(TEST_VIEW_TWO);
        m_viewIdx = 2;
    }
    else if (m_viewIdx == 2)
    {
        //if (!ui.secondViewBtn->isChecked())
        {
            m_pFTSDK->stopView();
            m_viewIdx = 0;
        }
    }
    else
    {
        m_pFTSDK->stopView();
        m_pFTSDK->startView(TEST_VIEW_TWO);
        m_viewIdx = 2;
    }
}

void ftsdksampleui::on_thirdView_clicked()
{
    CHECK_INITED();
    ui.firstViewBtn->setChecked(false);
    ui.secondViewBtn->setChecked(false);

    if (m_viewIdx == 0)
    {
        m_pFTSDK->startView(TEST_VIEW_THREE);
        m_viewIdx = 3;
    }
    else if (m_viewIdx == 3)
    {
        //if (!ui.thirdViewBtn->isChecked())
        {
            m_pFTSDK->stopView();
            m_viewIdx = 0;
        }
    }
    else
    {
        m_pFTSDK->stopView();
        m_pFTSDK->startView(TEST_VIEW_THREE);
        m_viewIdx = 3;
    }
}

void ftsdksampleui::on_simulateStuck_clicked()
{
    CHECK_INITED();
    m_pFTSDK->addLongTask("simulate long task", 100010);
}

void ftsdksampleui::on_simulateCrash_clicked()
{
    CHECK_INITED();
    m_pFTSDK->addError("simulate crash", "first error", RUMErrorType::NATIVE_CRASH, AppState::UNKNOWN);
}

void ftsdksampleui::on_addLog_clicked()
{
    CHECK_INITED();
    m_pFTSDK->addLog(ui.logText->text().toStdString(), LogLevel::INFO);
}

void ftsdksampleui::on_simulateResource_clicked()
{
    CHECK_INITED();
    std::string resId = "resource id 1111";
    NetStatus status;
    ResourceParams params;
    request(ui.resourceUrl->text().toStdString(), resId, params, status, m_pFTSDK);
    m_pFTSDK->addResource(resId, params, status);
}
