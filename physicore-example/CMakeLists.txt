cmake_minimum_required(VERSION 3.13)

project(physicore_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(physicore_example main.cpp)

set(PHYSICORE_VERSION "1.3.3")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(PHYSICORE_PACKAGE_URL
      https://github.com/bsc-life/PhysiCore/releases/download/v${PHYSICORE_VERSION}/physicore-linux-x64-v${PHYSICORE_VERSION}.tar.gz
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(PHYSICORE_PACKAGE_URL
      https://github.com/bsc-life/PhysiCore/releases/download/v${PHYSICORE_VERSION}/physicore-macos-arm64-v${PHYSICORE_VERSION}.tar.gz
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(PHYSICORE_PACKAGE_URL
      https://github.com/bsc-life/PhysiCore/releases/download/v${PHYSICORE_VERSION}/physicore-windows-x64-v${PHYSICORE_VERSION}.zip
  )
else()
  message(FATAL_ERROR "Unsupported OS: ${CMAKE_SYSTEM_NAME}")
endif()

include(FetchContent)
FetchContent_Declare(
  physicore
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  URL ${PHYSICORE_PACKAGE_URL})
FetchContent_MakeAvailable(physicore)

set(BioFVM_DIR ${physicore_SOURCE_DIR}/lib/cmake/BioFVM)
message(STATUS "Setting BioFVM_DIR to ${BioFVM_DIR}")

find_package(BioFVM CONFIG REQUIRED)

find_package(OpenMP)
target_link_libraries(physicore_example PUBLIC OpenMP::OpenMP_CXX PhysiCore::reactions-diffusion.biofvm)
