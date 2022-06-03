# Copyright (c) 2022 by Apex.AI Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)

# check if iceoryx is in CMAKE_PREFIX_PATH
find_package(iceoryx_hoofs QUIET)
find_package(iceoryx_posh QUIET)

# fetch iceoryx if not found
if(NOT iceoryx_hoofs_FOUND OR NOT iceoryx_posh_FOUND)
    if(iceoryx_hoofs_FOUND)
        message(FATAL_ERROR "iceoryx_hoofs was not found with 'find_package' but other parts were found!")
    endif()
    if(iceoryx_posh_FOUND)
        message(FATAL_ERROR "iceoryx_posh was not found with 'find_package' but other parts were found!")
    endif()

    include(FetchContent)
    FetchContent_Declare(
        iceoryx
        GIT_REPOSITORY https://github.com/ApexAI/iceoryx.git
        GIT_TAG automotive_soa_unix_domain_socket_comparison
    )
    FetchContent_GetProperties(iceoryx)
    if (NOT iceoryx_POPULATED)
        message(STATUS "updating: iceoryx" )
        FetchContent_Populate(iceoryx)
    endif()

    set(ICEORYX_WITH_FETCH_CONTENT true CACHE INTERNAL "")
    set(iceoryx_SOURCE_DIR ${iceoryx_SOURCE_DIR} CACHE INTERNAL "")
    set(iceoryx_BINARY_DIR ${iceoryx_BINARY_DIR} CACHE INTERNAL "")
endif()

if(ICEORYX_WITH_FETCH_CONTENT)
    add_subdirectory(${iceoryx_SOURCE_DIR}/iceoryx_hoofs ${iceoryx_BINARY_DIR}/iceoryx_hoofs)
    add_subdirectory(${iceoryx_SOURCE_DIR}/iceoryx_posh  ${iceoryx_BINARY_DIR}/iceoryx_posh)

    find_package(iceoryx_posh REQUIRED)
    find_package(iceoryx_hoofs REQUIRED)
endif()

if(ICEORYX_WITH_FETCH_CONTENT)
message("
#############################################################
The project was build by obtaining iceoryx with FetchContent.
If you want to use an existing installation use
'-DCMAKE_PREFIX_PATH=/path/to/installed/iceoryx'!
#############################################################
")
endif()

project(example_automotive_soa)

find_package(iceoryx_posh CONFIG REQUIRED)
find_package(iceoryx_hoofs CONFIG REQUIRED)

get_target_property(ICEORYX_CXX_STANDARD iceoryx_posh::iceoryx_posh CXX_STANDARD)

include(GNUInstallDirs)
include(IceoryxPlatform)


option(USE_UDS "Send Events via UNIX domain sockets" OFF)

add_executable(iox-cpp-automotive-skeleton  ./iox_automotive_skeleton.cpp
                                            ./src/minimal_skeleton.cpp
                                            ./src/runtime.cpp
                                            ./src/owl/kom/method_server.cpp
)
target_link_libraries(iox-cpp-automotive-skeleton
    iceoryx_posh::iceoryx_posh
)
target_compile_options(iox-cpp-automotive-skeleton PRIVATE ${ICEORYX_WARNINGS} ${ICEORYX_SANITIZER_FLAGS})

if(NOT USE_UDS)
  target_compile_definitions(iox-cpp-automotive-skeleton PUBLIC USED_IPC_MECHANISM_FOR_EVENTS=EventTransmission::IOX)
else()
  target_compile_definitions(iox-cpp-automotive-skeleton PUBLIC USED_IPC_MECHANISM_FOR_EVENTS=EventTransmission::UDS)
endif()

target_include_directories(iox-cpp-automotive-skeleton PRIVATE include)

add_executable(iox-cpp-automotive-proxy ./iox_automotive_proxy.cpp
                                        ./src/minimal_proxy.cpp
                                        ./src/runtime.cpp
                                        ./src/owl/kom/method_client.cpp
)
target_link_libraries(iox-cpp-automotive-proxy
    iceoryx_posh::iceoryx_posh
)
target_compile_options(iox-cpp-automotive-proxy PRIVATE ${ICEORYX_WARNINGS} ${ICEORYX_SANITIZER_FLAGS})

if(NOT USE_UDS)
  target_compile_definitions(iox-cpp-automotive-proxy PUBLIC USED_IPC_MECHANISM_FOR_EVENTS=EventTransmission::IOX)
else()
  target_compile_definitions(iox-cpp-automotive-proxy PUBLIC USED_IPC_MECHANISM_FOR_EVENTS=EventTransmission::UDS)
endif()

target_include_directories(iox-cpp-automotive-proxy PRIVATE include)

set_target_properties(iox-cpp-automotive-proxy
                      iox-cpp-automotive-skeleton
                      PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD ${ICEORYX_CXX_STANDARD}
    POSITION_INDEPENDENT_CODE ON
)

install(TARGETS iox-cpp-automotive-skeleton
                iox-cpp-automotive-proxy
        RUNTIME DESTINATION bin)
