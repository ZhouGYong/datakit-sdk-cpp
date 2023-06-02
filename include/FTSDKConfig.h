#pragma once
#ifndef _DATAKIT_SDK_CONFIG_H_
#define _DATAKIT_SDK_CONFIG_H_

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>
#include "datakit_exports.h"
#include "FTSDKDataContracts.h"

#define PROPERTY(_class, _type, _method_name, _property_name)                \
  _class&& set##_method_name(_type val) {   \
        _property_name = val;   \
        return std::move(*this);    \
    }   \
  _type get##_method_name() const { return _property_name; };  

namespace com::ft::sdk
{
    /**
     * 全局属性配置基类
     */
    template<typename T>
    class FTSDK_EXPORT ContextConfig
    {
    public:
        ContextConfig()
        {
        }

        /**
         * 添加全局属性
         * 
         * @param key
         * @param value
         * @return 
         */
        T&& addGlobalContext(const std::string& key, const std::string& value)
        {
            m_globalContext[key] = value;

            return std::move(*(static_cast<T*>(this)));
        }

        /**
         * 获取全局属性
         * 
         * @return 
         */
        std::map<std::string, std::string>& getGlobalContext()
        {
            return m_globalContext;
        }

    protected:
        //设置全局 tag
        std::map<std::string, std::string> m_globalContext;
    };

    /**
     * SDK 通用配置项
     */
    class FTSDK_EXPORT FTSDKConfig : public ContextConfig<FTSDKConfig>
    {
    public:
        /**
         * 设置datakit安装地址
         */
        PROPERTY(FTSDKConfig, std::string, ServerUrl, serverUrl)

        /**
         * 设置服务名称
         */
        PROPERTY(FTSDKConfig, std::string, ServiceName, serviceName)

        /**
         * 设置外部应用程序版本
         */
        PROPERTY(FTSDKConfig, std::string, AppVersion, appVersion)

        /**
         * 设置数据传输的环境
         */
        PROPERTY(FTSDKConfig, EnvType, Env, env)

        /**
         * 设置是否使用文件缓存
         */
        PROPERTY(FTSDKConfig, bool, EnableFileDBCache, enableFileDBCache)

        /**
         * 设置启动时是否清除缓存数据
         */
        PROPERTY(FTSDKConfig, bool, ClearDBWhenStartUp, clearDBWhenStartUp)

    private:
        //崩溃日志的环境
        EnvType env = EnvType::PROD;

        std::string appVersion = "";

        bool enableFileDBCache = false;
        bool clearDBWhenStartUp = true;

        // HttpConfig
        std::string serverUrl;

        std::string serviceName = "";

    };


    /**
     * 用户数据
     */
    class FTSDK_EXPORT UserData 
    {
    public:
        UserData() {}

        /**
         * 初始化用户数据
         * 
         * @param name
         * @param id
         * @param email
         * @return 
         */
        UserData&& init(const std::string& name, const std::string& id, const std::string& email) 
        {
            this->name = name;
            this->id = id;
            this->email = email;

            return std::move(*this);
        }

        /**
         * 重置用户数据
         * 
         */
        void reset()
        {
            this->name = "";
            this->id = "";
            this->email = "";

            this->exts.clear();
        }

        /**
         * 添加用户自定义字段
         * 
         * @param key
         * @param value
         * @return 
         */
        UserData&& addCustomizeItem(const std::string& key, const std::string& value)
        {
            exts[key] = value;

            return std::move(*this);
        }

        /**
         * 设置用户名
         */
        PROPERTY(UserData, std::string, Name, name);

        /**
         * 设置用户Id
         */
        PROPERTY(UserData, std::string, Id, id);

        /**
         * 设置用户邮件地址
         */
        PROPERTY(UserData, std::string, Email, email);


        std::map<std::string, std::string>& getExts()
        {
            return exts;
        }

    private:
        std::string name;
        std::string id;
        std::string email;
        std::map<std::string, std::string> exts;
    };

    /**
     * RUM 相关配置项
     */
    class FTSDK_EXPORT FTRUMConfig : public ContextConfig<FTRUMConfig>
    {
    public:
        FTRUMConfig() {}

        bool isRumEnable() 
        {
            return getRumAppId() != "";
        }

        /**
         * 设置RUM采样率
         */
        PROPERTY(FTRUMConfig, float, SamplingRate, samplingRate);

        /**
         * 设置应用Id
         */
        PROPERTY(FTRUMConfig, std::string, RumAppId, rumAppId);

    private:
        float samplingRate = 1.0f;

        std::string rumAppId = "";
        //设置是否需要采集崩溃日志
        bool enableTrackAppCrash = false;
        //设置是否检测 UI 卡顿
        bool enableTrackAppUIBlock = false;
        //设置是否检测 ANR
        bool enableTrackAppANR = false;
        //是否开启用户行为 action 追踪
        bool enableTraceUserAction = false;
        //是否开启用户行为 view 追踪
        bool enableTraceUserView = false;
        //是否开启用户欣慰 Resource 追踪
        bool enableTraceUserResource = false;
        //崩溃采集数据附加类型
        //ErrorMonitorType extraMonitorTypeWithError = ErrorMonitorType::NO_SET;

        //监控指标数据类型
        //DeviceMetricsMonitorType deviceMetricsMonitorType = DeviceMetricsMonitorType::NO_SET;
        //DetectFrequency deviceMetricsDetectFrequency = DetectFrequency::DEFAULT;

    };

    /**
     * Trace 相关配置项
     */
    class FTSDK_EXPORT FTTraceConfig
    {
    public:
        FTTraceConfig() {}

        /**
         * 设置Trace采样率
         */
        PROPERTY(FTTraceConfig, float, SamplingRate, samplingRate);

        /**
         * 设置Trace类型
         */
        PROPERTY(FTTraceConfig, TraceType, TraceType, traceType);

        /**
         * 设置是否关联RUM数据
         */
        PROPERTY(FTTraceConfig, bool, EnableLinkRUMData, enableLinkRUMData);

    private:
        float samplingRate = 1;
        TraceType traceType = TraceType::DDTRACE;
        bool enableWebTrace = false;
        bool enableAutoTrace = false;
        bool enableLinkRUMData = false;
    };

    /**
     * 日志相关配置项
     */
    class FTSDK_EXPORT FTLogConfig : public ContextConfig<FTLogConfig>
    {
    public:
        FTLogConfig() {}

        /**
         * 设置Log采样率
         */
        PROPERTY(FTLogConfig, float, SamplingRate, samplingRate);

        /**
         * 设置是否关联RUM数据
         */
        PROPERTY(FTLogConfig, bool, EnableLinkRumData, enableLinkRumData);

        /**
         * 设置是否上传自定义日志
         */
        PROPERTY(FTLogConfig, bool, EnableCustomLog, enableCustomLog);

    private:
        float samplingRate = 1;
        bool enableLinkRumData = false;
        bool enableConsoleLog = false;
        bool enableCustomLog = false;
        std::string logPrefix = "";
        std::vector<LogLevel> logLevelFilters;

    };
}

#endif // _DATAKIT_SDK_CONFIG_H_