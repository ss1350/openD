#  ___ _____ _   ___ _  _____ ___  ___  ___ ___
# / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
# \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
# |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
# embedded.connectivity.solutions.==============
#
# copyright  STACKFORCE GmbH, Heitersheim, Germany, http://www.stackforce.de
# author     Heiko Berger
# brief      openD DSPG FP frame.
#
# This work is dual-licensed under Apache 2.0 and GPL 2.0. You can choose between one of them if you use this work.
# For further details, please refer to the project home: https://github.com/opend-connect/openD
#
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only

project(dspg-cmbs-frame)

cmake_minimum_required(VERSION 3.6)

#---------------------------------------------------------------------------------------
# Build target
#---------------------------------------------------------------------------------------

file(GLOB ${PROJECT_NAME}_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/osl/linux/OsLiteTask.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/osl/linux/OsLiteNetwork.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/linux/cfr_uart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/linux/cfr_cmbs.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cfr_ie.c"
)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

#---------------------------------------------------------------------------------------
# Include
#---------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/osl/inc
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/linux
)

#---------------------------------------------------------------------------------------
# Link
#---------------------------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME}
    dspg-cmbs-lib
    dspg-cmbs-testLib
)
