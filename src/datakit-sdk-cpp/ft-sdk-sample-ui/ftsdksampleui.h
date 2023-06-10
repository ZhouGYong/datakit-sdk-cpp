#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ftsdksampleui.h"
#include "datakit-sdk-cpp/FTSDKFactory.h"

using namespace com::ft::sdk;

class ftsdksampleui : public QMainWindow
{
    Q_OBJECT

public:
    ftsdksampleui(QWidget *parent = nullptr);
    ~ftsdksampleui();

public slots:
    void on_startAction_clicked();
    void on_endAction_clicked();
    void on_firstView_clicked();
    void on_secondView_clicked();
    void on_thirdView_clicked();
    void on_simulateStuck_clicked();
    void on_simulateCrash_clicked();
    void on_addLog_clicked();
    void on_simulateResource_clicked();
    void on_init_clicked();

private:
    Ui::ftsdksampleuiClass ui;
    std::unique_ptr<FTSDK> m_pFTSDK;

    int m_viewIdx = 0;
    bool m_isInited = false;
};
