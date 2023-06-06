/*****************************************************************//**
 * \file   AbstractManager.h
 * \brief  Abstract class for all managers
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_ABSTRACT_MANAGER_H_
#define _DATAKIT_SDK_ABSTRACT_MANAGER_H_

#include <string>

namespace com::ft::sdk::internal
{
	class AbstractManager
	{
	public:
		AbstractManager(const char* name)
		{
			m_name = name;
		}

	protected:
		std::string m_name;
	};

}
#endif // !_DATAKIT_SDK_ABSTRACT_MANAGER_H_



