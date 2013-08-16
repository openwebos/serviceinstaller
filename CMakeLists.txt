# @@@LICENSE
#
# Copyright (c) 2012-2013 LG Electronics, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# LICENSE@@@

cmake_minimum_required(VERSION 2.8.7)

project(serviceinstaller CXX)

include(webOS/webOS)
webos_modules_init(1 0 0 QUALIFIER RC4)
webos_component(2 0 0)

include(FindPkgConfig)

# -- check for glib 2.0
pkg_check_modules(GLIB2 REQUIRED glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${GLIB2_CFLAGS_OTHER})

# -- check for pbnjson_c and pbnjson_cpp
pkg_check_modules(PBNJSON_C REQUIRED pbnjson_c)
include_directories(${PBNJSON_C_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${PBNJSON_C_CFLAGS_OTHER})

pkg_check_modules(PBNJSON_CPP REQUIRED pbnjson_cpp)
include_directories(${PBNJSON_CPP_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${PBNJSON_CPP_CFLAGS_OTHER})

# -- check for rolegen 
pkg_check_modules(ROLEGEN REQUIRED rolegen)
include_directories(ROLEGEN_INCLUDE_DIRS)
webos_add_compiler_flags(ALL ${ROLEGEN_CFLAGS_OTHER})

# -- check for LS2 (internally depends on pmloglib)
pkg_check_modules(LS2 REQUIRED luna-service2)
include_directories(LS2_INCLUDE_DIRS)
webos_add_compiler_flags(ALL ${LS2_CFLAGS_OTHER})

webos_build_pkgconfig()

# -- no way to disable warn_unused_result right now.
webos_add_compiler_flags(ALL -g -O3 -Wall -Wno-unused-but-set-variable -Wno-unused-variable -fno-exceptions -fno-rtti)
webos_add_linker_options(ALL --no-undefined)

# -- add local include paths
include_directories(include/public)

# Define paths used in the source code
add_definitions(-DWEBOS_INSTALL_BINDIR="${WEBOS_INSTALL_BINDIR}")
add_definitions(-DWEBOS_INSTALL_SBINDIR="${WEBOS_INSTALL_SBINDIR}")
add_definitions(-DWEBOS_INSTALL_WEBOS_SERVICESDIR="${WEBOS_INSTALL_WEBOS_SERVICESDIR}")
add_definitions(-DWEBOS_INSTALL_SYSBUS_DYNPRVROLESDIR="${WEBOS_INSTALL_SYSBUS_DYNPRVROLESDIR}")
add_definitions(-DWEBOS_INSTALL_SYSBUS_DYNPUBROLESDIR="${WEBOS_INSTALL_SYSBUS_DYNPUBROLESDIR}")
add_definitions(-DWEBOS_INSTALL_SYSBUS_DYNPUBSERVICESDIR="${WEBOS_INSTALL_SYSBUS_DYNPUBSERVICESDIR}")
add_definitions(-DWEBOS_INSTALL_SYSBUS_DYNPRVSERVICESDIR="${WEBOS_INSTALL_SYSBUS_DYNPRVSERVICESDIR}")

# -- static library
add_library(serviceinstaller serviceinstall.cpp)
target_link_libraries(serviceinstaller 
                      ${GLIB2_LDFLAGS} 
                      ${GTHREAD2_LDFLAGS}
                      ${GXML2_LDFLAGS}
                      ${LS2_LDFLAGS}
                      ${PBNJSON_C_LDFLAGS}
                      ${PBNJSON_CPP_LDFLAGS}
                      ${ROLEGEN_LDFLAGS}
                      )

webos_build_library()
