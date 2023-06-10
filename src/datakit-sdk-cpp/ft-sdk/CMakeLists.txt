project(ft-sdk VERSION 1.0.0 LANGUAGES CXX C)

#include("../module.cmake")
#include_directories(${PROJECT_SOURCE_DIR}/../include)
#include ("${VCPKG_CMAKE_SHARE}/restclient-cpp/restclient-cppConfig.cmake")
#include ("${VCPKG_CMAKE_SHARE}/fmt/fmt-config.cmake")

message(STATUS "VCPKG_SHARE_DIR=" ${VCPKG_CMAKE_SHARE})
message(STATUS "libpath=" ${CMAKE_SYSTEM_LIBRARY_PATH})
if(DEFINED BUILD_FROM_VCPKG)
  message(STATUS "build from vcpkg!")
endif()

if(WIN32)
add_definitions(-DFTSDK_EXPORTS)
add_definitions(-D_USRDLL)
add_definitions(/EHa)
add_definitions(-D_WINDLL)
add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
add_definitions(/std:c++17)
endif()

set(DIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../datakit_sdk_redist/${CMAKE_BUILD_TYPE}")
set(CURL_DIR "${VCPKG_CMAKE_SHARE}/curl/")
find_package(CURL CONFIG REQUIRED)
set(stduuid_DIR "${VCPKG_CMAKE_SHARE}/stduuid/")
find_package(stduuid CONFIG REQUIRED)
set(fmt_DIR "${VCPKG_CMAKE_SHARE}/fmt/")
find_package(fmt CONFIG REQUIRED)
set(spdlog_DIR "${VCPKG_CMAKE_SHARE}/spdlog/")
find_package(spdlog CONFIG REQUIRED)
#find_package(nlohmann_json REQUIRED CONFIG PATHS "/usr/lib/cmake/")
set(nlohmann_json_DIR "${VCPKG_CMAKE_SHARE}/nlohmann_json/")
find_package(nlohmann_json REQUIRED CONFIG)
set(SQLiteCpp_DIR "${VCPKG_CMAKE_SHARE}/sqlitecpp/")
find_package(SQLiteCpp CONFIG REQUIRED)
set(restclient-cpp_DIR "${VCPKG_CMAKE_SHARE}/restclient-cpp/")
find_package(restclient-cpp CONFIG REQUIRED)

#find_package(Filesystem REQUIRED)

#file(GLOB PROJECT_SOURCE "*.cpp")
#file(GLOB PROJECT_HEADER "../include/*.h" "*.h")

message(STATUS "PROJECT_SOURCE_DIR=" ${PROJECT_SOURCE_DIR})
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR} ALL_CPPs)

add_library (${PROJECT_NAME} SHARED  ${ALL_CPPs})

if(WIN32)
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}d)
  endif()
endif()

if(UNIX)
add_library (${PROJECT_NAME}_static STATIC  ${ALL_CPPs})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

add_custom_target(
    auto_gen_app_uuid
    COMMAND /bin/sh ${CMAKE_CURRENT_SOURCE_DIR}/autogen.sh ${CMAKE_CURRENT_SOURCE_DIR}/Include/Datakit_UUID.h
)
add_dependencies(${PROJECT_NAME} auto_gen_app_uuid)
add_dependencies(${PROJECT_NAME}_static auto_gen_app_uuid)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "/usr/local/include/")
target_include_directories(${PROJECT_NAME} PRIVATE "/usr/include/")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include/)

install(TARGETS ${PROJECT_NAME} DESTINATION ${DIST_DIR}/lib/${ARCHITECTURE}/)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Include/ DESTINATION ${DIST_DIR}/include)

if(UNIX)
install(TARGETS ${PROJECT_NAME}_static DESTINATION ${DIST_DIR}/lib/${ARCHITECTURE}/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/install.sh DESTINATION ${DIST_DIR})
endif()

#target_link_libraries(${PROJECT_NAME} "/usr/local/lib/librestclient-cpp.a")
target_link_libraries(${PROJECT_NAME} PRIVATE stduuid SQLiteCpp)
target_link_libraries(${PROJECT_NAME} PRIVATE restclient-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
#target_link_libraries(${PROJECT_NAME}  PRIVATE std::filesystem)

if(UNIX)
target_link_libraries(${PROJECT_NAME}_static PRIVATE stduuid SQLiteCpp)
target_link_libraries(${PROJECT_NAME}_static PRIVATE restclient-cpp)
target_link_libraries(${PROJECT_NAME}_static PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME}_static PRIVATE nlohmann_json::nlohmann_json)
endif()

if (WIN32)
#find_library(VERSION_LIB version)
target_link_libraries(${PROJECT_NAME} PRIVATE version.lib)
#target_link_libraries(${PROJECT_NAME}_static PRIVATE version.lib)
endif()

if(UNIX)
  set(TARGET_FOLDER ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
elseif(WIN32)
  set(TARGET_FOLDER "$(OutDir)")  # Use MSVC's $(OutDir) for dynamic copying
endif()


