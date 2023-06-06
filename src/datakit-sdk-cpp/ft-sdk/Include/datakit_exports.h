#pragma once
#ifndef _DATAKIT_CPP_EXPORT_H_
#define _DATAKIT_CPP_EXPORT_H_

#if __linux__
#define FTSDK_STATIC_DEFINE
#endif

#ifdef FTSDK_STATIC_DEFINE
#  define FTSDK_EXPORT
#  define FTSDK_NO_EXPORT
#else
#  ifndef FTSDK_EXPORT
#    ifdef FTSDK_EXPORTS
/* We are building this library */
#      define FTSDK_EXPORT __declspec(dllexport)
#    else
/* We are using this library */
#      define FTSDK_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef FTSDK_NO_EXPORT
#    define FTSDK_NO_EXPORT 
#  endif
#endif

#ifndef FTSDK_DEPRECATED
#  define FTSDK_DEPRECATED __declspec(deprecated)
#endif

#ifndef FTSDK_DEPRECATED_EXPORT
#  define FTSDK_DEPRECATED_EXPORT FTSDK_EXPORT FTSDK_DEPRECATED
#endif

#ifndef FTSDK_DEPRECATED_NO_EXPORT
#  define FTSDK_DEPRECATED_NO_EXPORT FTSDK_NO_EXPORT FTSDK_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef FTSDK_NO_DEPRECATED
#    define FTSDK_NO_DEPRECATED
#  endif
#endif

#endif // !_DATAKIT_CPP_EXPORT_H_
