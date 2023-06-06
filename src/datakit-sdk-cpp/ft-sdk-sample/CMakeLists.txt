project(ft-sdk-sample VERSION 1.0.0 LANGUAGES CXX C)

message(STATUS "PROJECT_SOURCE_DIR:" ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/../ft-sdk/Include)

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


file(GLOB PROJECT_SOURCE "*.cpp")
file(GLOB PROJECT_HEADER "../include/*.h" "*.h")

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_executable (${PROJECT_NAME} ${PROJECT_SOURCE} ${PROJECT_HEADER})
add_executable (${PROJECT_NAME}_static ${PROJECT_SOURCE} ${PROJECT_HEADER})

target_include_directories(${PROJECT_NAME} PRIVATE "/usr/local/include/")
target_include_directories(${PROJECT_NAME} PRIVATE "/usr/include/")
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../ft-sdk/Include")

target_link_libraries(${PROJECT_NAME} PRIVATE restclient-cpp ft-sdk)
target_link_libraries(${PROJECT_NAME}_static PRIVATE restclient-cpp ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/libft-sdk.a)
target_link_libraries(${PROJECT_NAME}_static PRIVATE stduuid SQLiteCpp)
target_link_libraries(${PROJECT_NAME}_static PRIVATE restclient-cpp)
target_link_libraries(${PROJECT_NAME}_static PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME}_static PRIVATE nlohmann_json::nlohmann_json)

if(UNIX)
  set(TARGET_FOLDER ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
elseif(WIN32)
  set(TARGET_FOLDER "$(OutDir)")  # Use MSVC's $(OutDir) for dynamic copying
endif()

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_SOURCE_DIR}/ft_sdk_config.json 
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)

message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY:" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY:" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

