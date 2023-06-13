cmake_minimum_required(VERSION 3.0)

project(datakit_sdk)
#set(MAIN_PROJECT_NAME ${PROJECT_NAME} CACHE INTERNAL "shared main project name")
#link_directories( $ENV{LD_LIBRARY_PATH} )
############################################################
#if (UNIX)
#  ###add rpath limitation  
#  set(INSTALL_LIB_DIR "${PROJECT_BINARY_DIR}/lib") #
#  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)  
#  set(CMAKE_INSTALL_RPATH "${PROJECT_BINARY_DIR}/lib")  
#  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#endif()
if(UNIX)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,-defs,-rpath='./../lib'")
endif()

if(UNIX)
    add_definitions(-DFMT_HEADER_ONLY)
    add_definitions(-fPIC -g -Werror=return-type)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O1 -ftree-vectorize -ffast-math ")
  message(UNIX "CMAKE_CXX_FLAGS:" ${CMAKE_CXX_FLAGS})

elseif(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
  add_definitions(-D_WINDOWS)
  add_definitions(-D_WIN32_WINNT=0x0600)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4200) # warning C4200: nonstandard extension used: zero-sized array in struct/union
endif()

message(STATUS "main project dir: " ${PROJECT_SOURCE_DIR})


# Force cmake to generate all output into /bin
if(UNIX)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
elseif(WIN32)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


if(UNIX)
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
if( ${ARCHITECTURE} STREQUAL "x86_64" )
  set(VCPKG_TARGET_TRIPLET "x64-linux"
      CACHE STRING "default vcpkg triplet")
elseif (${ARCHITECTURE} STREQUAL "aarch64")
  set(VCPKG_TARGET_TRIPLET "arm64-linux"
      CACHE STRING "default vcpkg triplet")
endif()
endif()


if(DEFINED ENV{VCPKG_ROOT})
if (EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    include ("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
  set(VCPKG_CMAKE_SHARE "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/share"
      CACHE STRING "TEST")
endif ()
else ()
message(STATUS "please set the system environment variable : VCPKG_ROOT" $ENV{VCPKG_ROOT})
endif ()


set(CMAKE_CXX_STANDARD 17)
if (CMAKE_CROSSCOMPILING)
	set(THREADS_PTHREAD_ARG
		"PLEASE_FILL_OUT-FAILED_TO_RUN"
		CACHE STRING "Result from TRY_RUN" FORCE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

#if(DEFINED BUILD_FROM_VCPKG)
#message(STATUS "got vcpkg!")
#endif()

if(DEFINED BUILD_FROM_VCPKG)
add_subdirectory(datakit-sdk-cpp/ft-sdk)
else ()
message(STATUS "NORMAL BUILD...")
add_subdirectory(datakit-sdk-cpp/ft-sdk)
add_subdirectory(datakit-sdk-cpp/ft-sdk-sample)
add_subdirectory(datakit-sdk-cpp/ft-sdk-unittest)
endif ()

