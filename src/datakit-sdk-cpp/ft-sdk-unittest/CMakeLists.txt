project(ft-sdk-unittest VERSION 1.0.0 LANGUAGES CXX C)

add_definitions(-DFMT_HEADER_ONLY)
include_directories(${PROJECT_SOURCE_DIR}/../ft-sdk/Include)
set(CURL_DIR "~/github/vcpkg/installed/x64-linux/share/curl/")
find_package(CURL CONFIG REQUIRED)

set(restclient-cpp_DIR "~/github/vcpkg/installed/x64-linux/share/restclient-cpp/")
find_package(restclient-cpp CONFIG REQUIRED)

set(restclient-cpp_DIR "~/github/vcpkg/installed/x64-linux/share/gtest/")
find_package(GTest CONFIG REQUIRED)


file(GLOB PROJECT_SOURCE "*.cpp")
file(GLOB PROJECT_HEADER "../include/*.h" "*.h")

enable_testing()

add_executable (${PROJECT_NAME} ${PROJECT_SOURCE} ${PROJECT_HEADER})

target_include_directories(${PROJECT_NAME} PRIVATE "/usr/local/include/")
target_include_directories(${PROJECT_NAME} PRIVATE "/usr/include/")

target_link_libraries(${PROJECT_NAME} PRIVATE restclient-cpp ft-sdk)
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

add_test(AllTestsInMain main)

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

