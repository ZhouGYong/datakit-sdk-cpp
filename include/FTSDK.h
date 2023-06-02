/*****************************************************************//**
 * \file   FTSDK.h
 * \brief  Datakit SDK C++ interface.
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/

#pragma once
#ifndef _DATAKIT_SDK_H_
#define _DATAKIT_SDK_H_

#include <string>
#include <vector>
#include "datakit_exports.h"
#include "FTSDKConfig.h"
#include "FTSDKDataContracts.h"

namespace com::ft::sdk
{
	class FTSDKFactory;
	typedef std::map<std::string, std::string> PropagationHeader;

	/**
	 * Datakit SDK 接口类, 所有操作通过此接口访问。
	 */
	class FTSDK_EXPORT FTSDK
	{
	public:
		/// Disable copy constructor
		FTSDK& operator=(const FTSDK&) = delete;

		/// Disable copy constructor
		FTSDK(const FTSDK&) = delete;

		/**
		 * 初始化SDK
		 * 
		 */
		void init();

		/**
		 * 关闭SDK，执行相关资源清理操作
		 * 
		 */
		void deinit();

		/**
		 * 获取SDK版本
		 * 
		 * @return SDK版本号
		 */
		std::string getVersionString();

		/**
		 * 配置全局通用参数
		 * 
		 * @param config
		 * @return 
		 */
		FTSDK&& install(FTSDKConfig& config);

		/**
		 * 配置RUM参数
		 * 
		 * @param config
		 * @return 
		 */
		FTSDK&& initRUMWithConfig(FTRUMConfig& config);
		
		/**
		 * 配置Trace参数
		 * 
		 * @param config
		 * @return 
		 */
		FTSDK&& initTraceWithConfig(FTTraceConfig& config);
		
		/**
		 * 配置Log参数
		 * 
		 * @param config
		 * @return 
		 */
		FTSDK&& initLogWithConfig(FTLogConfig& config);

		/**
		 * 绑定用户数据
		 * 
		 * @param config	用户数据
		 * @return
		 */
		FTSDK&& bindUserData(UserData& config);		

		/**
		 * 解绑用户数据
		 * 
		 */
		void unbindUserData();

		/**
		 * 按配置生成trace数据
		 * 
		 * @param url	网络地址
		 * @return		trace数据
		 */
		PropagationHeader generateTraceHeader(const std::string& url);

		/**
		 * 按配置生成trace数据
		 * 
		 * @param resourceId	关联资源 id
		 * @param url			网络地址
		 * @return				trace数据
		 */
		PropagationHeader generateTraceHeader(const std::string& resourceId, const std::string& url);

		/**
		 * 上传用户日志到datakit
		 * 
		 * @param content	日志内容
		 * @param level		日志级别
		 */
		void addLog(std::string content, LogLevel level);

		// ---RUM interface
		/**
		 * 添加长耗时任务
		 * 
		 * @param log		日志
		 * @param duration	持续时间(ns)
		 */
		void addLongTask(std::string log, long duration);

		/**
		 * 添加错误信息
		 * 
		 * @param log		日志
		 * @param message	消息
		 * @param errorType	错误类型
		 * @param state		程序运行状态
		 */
		void addError(std::string log, std::string message, RUMErrorType errorType, AppState state);

		/**
		 * 设置网络传输内容
		 * 
		 * @param resourceId		资源 Id
		 * @param params			网络传输参数
		 * @param netStatusBean		网络状态统计
		 */
		void addResource(std::string resourceId, ResourceParams params, NetStatus netStatusBean);

		/**
		 * resource 起始
		 * 
		 * @param resourceId		资源 Id
		 */
		void startResource(std::string resourceId);

		/**
		 * resource 终止
		 * 
		 * @param resourceId
		 */
		void stopResource(std::string resourceId);

		void addAction(std::string actionName, std::string actionType, long duration, long startTime);

		/**
		 * action 开始
		 * 
		 * @param actionName action 名称
		 * @param actionType action 类型
		 */
		void startAction(std::string actionName, std::string actionType);

		/**
		 * action结束
		 * 
		 */
		void stopAction();

		/**
		 * view 开始.
		 * 
		 * @param viewName 当前页面名称
		 */
		void startView(std::string viewName);

		/**
		 * view 结束.
		 * 
		 */
		void stopView();

		// ---end RUM interface

	private:
		/// Constructor required config json file
		explicit FTSDK(const std::string& configJson);

		friend class FTSDKFactory;
	};

}

#endif // _DATAKIT_SDK_H_