#.rst:
# AndroidToolchain
# ----------------
#
# Enable easy compilation of cmake projects on Android.
#
# By using this android toolchain, the projects will be set up to compile the
# specified project targeting an Android platform, depending on its input.
# Furthermore, if desired, an APK can be directly generated by using the
# `androiddeployqt <http://doc.qt.io/qt-5/deployment-android.html>`_ tool.
#
# CMake upstream has Android support now. This module will still give us some
# useful features offering androiddeployqt integration and adequate executables
# format for our Android applications.
#
# Since we are using CMake Android support, any information from CMake documentation
# still applies:
# https://cmake.org/cmake/help/v3.7/manual/cmake-toolchains.7.html#cross-compiling-for-android
#
# .. note::
#
#   This module requires CMake 3.7.
#
# Since 1.7.0.
#
# Usage
# =====
#
# To use this file, you need to set the ``CMAKE_TOOLCHAIN_FILE`` to point to
# ``AndroidToolchain.cmake`` on the command line::
#
#   cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/ECM/toolchain/AndroidToolchain.cmake
#
# You will also need to provide the locations of the Android NDK and SDK. This
# can be done on the commandline or with environment variables; in either case
# the variable names are:
#
# ``CMAKE_ANDROID_NDK``
#     The NDK root path.
# ``ANDROID_SDK_ROOT``
#     The SDK root path.
#
# Additional options are specified as cache variables (eg: on the command line):
#
# ``ANDROID_TOOLCHAIN``
#     The toolchain to use. See the ``toolchains`` directory of the NDK.
#     Default: ``arm-linux-androideabi``.
# ``ANDROID_COMPILER_PREFIX``
#     The prefix for the gcc and g++ executables. See the ``toolchains/$ANDROID_TOOLCHAIN/prebuilt/*/bin/*-gcc`` in the NDK.
#     Default: ``arm-linux-androideabi``.
# ``ANDROID_ABI``
#     The ABI to use. See the ``sources/cxx-stl/gnu-libstdc++/*/libs``
#     directories in the NDK. Default: ``armeabi-v7a``.
# ``ANDROID_SDK_BUILD_TOOLS_REVISION``
#     The build tools version to use. Default: ``21.1.1``.
#
# For integrating other libraries which are not part of the Android toolchain,
# like Qt5, and installed to a separate prefix on the host system, the install
# prefixes of those libraries would be passed as alternative roots as list via
# ``ECM_ADDITIONAL_FIND_ROOT_PATH``. Since 5.30.0.
#
# For example, for integrating a Qt5 for Android with armv7 target present at
# ``/opt/android/Qt5/5.7/android_armv7`` and some other libraries installed to
# the prefix ``/opt/android/foo``, you would use::
#
#   cmake \
#     -DCMAKE_TOOLCHAIN_FILE=/usr/share/ECM/toolchain/AndroidToolchain.cmake \
#     -DECM_ADDITIONAL_FIND_ROOT_PATH="/opt/android/Qt5/5.7/android_armv7;/opt/android/foo"
#
# If your project uses ``find_package()`` to locate build tools on the host
# system, make sure to pass ``CMAKE_FIND_ROOT_PATH_BOTH`` or
# ``NO_CMAKE_FIND_ROOT_PATH`` as argument in the call. See the
# ``find_package()`` documentation for more details.
#
# Deploying Qt Applications
# =========================
#
# After building the application, you will need to generate an APK that can be
# deployed to an Android device. This module integrates androiddeployqt support
# to help with this for Qt-based projects. To enable this, set the
# ``QTANDROID_EXPORTED_TARGET`` variable to the targets you wish to export as an
# APK (in a ;-separed list), as well as ``ANDROID_APK_DIR`` to a directory
# containing some basic information. This will create a ``create-apk-<target>``
# target that will generate the APK file.  See the `Qt on Android deployment
# documentation <http://doc.qt.io/qt-5/deployment-android.html>`_ for more
# information.
#
# For example, you could do::
#
#   cmake \
#     -DCMAKE_TOOLCHAIN_FILE=/usr/share/ECM/toolchain/AndroidToolchain.cmake \
#     -DQTANDROID_EXPORTED_TARGET=myapp \
#     -DANDROID_APK_DIR=myapp-apk
#   make
#   make create-apk-myapp
#
# The APK would then be found in ``myapp_build_apk/bin`` in the build directory.
#
# The create-apk-myapp target will be able to take an ARGS parameter with further
# arguments for androiddeployqt. For example, one can use::
#
#   make create-apk-myapp ARGS="--install"
#
# To install the apk to test. To generate a signed apk, one can do it with the
# following syntax::
#
#   make create-apk-myapp ARGS="--sign ~/my.keystore alias_name"
#
# See Android documentation on how to create a keystore to use

# =============================================================================
# Copyright 2014 Aleix Pol i Gonzalez <aleixpol@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION "3.7")

macro(set_deprecated_variable actual_variable deprecated_variable default_value)
    set(${deprecated_variable} "${default_value}" CACHE STRING "Deprecated. Use ${actual_variable}")
    if (NOT DEFINED ${${actual_variable}})
        set(${actual_variable} ${${deprecated_variable}})
    endif()
endmacro()

set_deprecated_variable(CMAKE_ANDROID_NDK ANDROID_NDK "$ENV{ANDROID_NDK}")
set_deprecated_variable(CMAKE_ANDROID_ARCH ANDROID_ARCHITECTURE "arm")
set_deprecated_variable(CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION ANDROID_GCC_VERSION "4.9")
set_deprecated_variable(CMAKE_ANDROID_ARCH_ABI ANDROID_ABI "armeabi-v7a")
set_deprecated_variable(CMAKE_ANDROID_API ANDROID_API_LEVEL "14")

set(ANDROID_SDK_ROOT "$ENV{ANDROID_SDK_ROOT}" CACHE path "Android SDK path")
set(ANDROID_TOOLCHAIN "arm-linux-androideabi" CACHE string "Used SDK")
set(ANDROID_COMPILER_PREFIX "arm-linux-androideabi" CACHE string "Prefix for gcc/g++/etc")
set(ANDROID_SDK_BUILD_TOOLS_REVISION "21.1.1" CACHE string "Android API Level")

set(CMAKE_SYSTEM_VERSION ${CMAKE_ANDROID_API})
set(CMAKE_SYSTEM_NAME Android)
if (NOT CMAKE_ANDROID_STL_TYPE)
    set(CMAKE_ANDROID_STL_TYPE gnustl_shared)
endif()

include(${CMAKE_ROOT}/Modules/Platform/Android-GNU.cmake REQUIRED)
include(${CMAKE_ROOT}/Modules/Platform/Android-Initialize.cmake REQUIRED)

if (NOT DEFINED ECM_ADDITIONAL_FIND_ROOT_PATH)
    SET(ECM_ADDITIONAL_FIND_ROOT_PATH ${CMAKE_PREFIX_PATH})
endif()

SET(CMAKE_FIND_ROOT_PATH ${CMAKE_ANDROID_NDK} ${CMAKE_ANDROID_NDK}/sysroot ${CMAKE_SYSROOT} ${ECM_ADDITIONAL_FIND_ROOT_PATH})
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

#we want executables to be shared libraries, hooks will invoke the exported cmake function
set(CMAKE_CXX_LINK_EXECUTABLE
    "<CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <SONAME_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>"
)

set(ECM_DIR "${CMAKE_CURRENT_LIST_DIR}/../cmake" CACHE STRING "")

######### generation

# Need to ensure we only get in here once, as this file is included twice:
# from CMakeDetermineSystem.cmake and from CMakeSystem.cmake generated within the
# build directory.
if(DEFINED QTANDROID_EXPORTED_TARGET AND NOT TARGET "create-apk")
    get_filename_component(_CMAKE_ANDROID_DIR "${CMAKE_TOOLCHAIN_FILE}" PATH)
    list(LENGTH QTANDROID_EXPORTED_TARGET targetsCount)
    include(${_CMAKE_ANDROID_DIR}/ECMAndroidDeployQt.cmake)

    math(EXPR last "${targetsCount}-1")
    foreach(idx RANGE 0 ${last})
        list(GET QTANDROID_EXPORTED_TARGET ${idx} exportedTarget)
        list(GET ANDROID_APK_DIR ${idx} APK_DIR)
        if(APK_DIR AND NOT EXISTS "${APK_DIR}/AndroidManifest.xml")
            message(FATAL_ERROR "Cannot find ${APK_DIR}/AndroidManifest.xml according to ANDROID_APK_DIR. ${ANDROID_APK_DIR} ${exportedTarget}")
        endif()
        ecm_androiddeployqt("${exportedTarget}" "${ECM_ADDITIONAL_FIND_ROOT_PATH}" "${APK_DIR}")
    endforeach()
else()
    message(STATUS "You can export a target by specifying -DQTANDROID_EXPORTED_TARGET=<targetname> and -DANDROID_APK_DIR=<paths>")
endif()
