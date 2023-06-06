/*****************************************************************//**
 * \file   Singleton.h
 * \brief  Singleton Base Class
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_SINGLETON_H_
#define _DATAKIT_SDK_SINGLETON_H_

namespace com::ft::sdk::internal
{
    template <typename T>
    class Singleton
    {
    public:
        static T& getInstance()
        {
            static T instance;
            return instance;
        }
    protected:
        Singleton() {}
        ~Singleton() {}
    public:
        Singleton(Singleton const&) = delete;
        Singleton& operator=(Singleton const&) = delete;
    };
}

#endif // !_DATAKIT_SDK_SINGLETON_H_