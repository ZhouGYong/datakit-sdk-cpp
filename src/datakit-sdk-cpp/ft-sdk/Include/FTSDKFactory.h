/*
  Author: Zhou Guangyong
  Datakit SDK C++ interface.
*/

#pragma once
#ifndef _DATAKIT_SDK_FACTORY_H_
#define _DATAKIT_SDK_FACTORY_H_

#include <memory>
#include "datakit_exports.h"
#include "FTSDK.h"

namespace com::ft::sdk
{
	/// <summary>
	/// FTSDK factory
	/// </summary>
	class FTSDK_EXPORT FTSDKFactory
	{
	public:
		/// Disables copy constructor
		FTSDKFactory& operator=(const FTSDKFactory&) = delete;

		/// Disables copy constructor
		FTSDKFactory(const FTSDKFactory&) = delete;

		/**
		 * 创建SDK实例
		 * 
		 * @param configJson 全局配置文件
		 * @return FTSDK实例指针
		 */
		static std::unique_ptr<FTSDK> get(const std::string& configJson = "ft_sdk_config.json") noexcept(false);

	private:
		/// Private constructor disallows to create instance of Factory
		FTSDKFactory() = default;
	};

}

#endif // !_DATAKIT_SDK_FACTORY_H_

