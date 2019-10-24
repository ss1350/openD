#  ___ _____ _   ___ _  _____ ___  ___  ___ ___
# / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
# \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
# |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
# embedded.connectivity.solutions.==============
#
# copyright  STACKFORCE GmbH, Heitersheim, Germany, http://www.stackforce.de
# author     Heiko Berger
# brief      openD DSPG FP cmbs.
#
# This work is dual-licensed under Apache 2.0 and GPL 2.0. You can choose between one of them if you use this work.
# For further details, please refer to the project home: https://github.com/opend-connect/openD
#
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only

project(dspg-cmbs-lib)

cmake_minimum_required(VERSION 3.6)

#---------------------------------------------------------------------------------------
# Build target
#---------------------------------------------------------------------------------------

file(GLOB ${PROJECT_NAME}_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_int.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_api.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_dcm.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_dsr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_dee.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_dem.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_ie.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_cmd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_suota.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_rtp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_util.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_fifo.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_dbg.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_han.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_han_ie.c"
)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

#---------------------------------------------------------------------------------------
# Include
#---------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#---------------------------------------------------------------------------------------
# Link
#---------------------------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME}
    dspg-cmbs-frame
)

#---------------------------------------------------------------------------------------
# Set macros
#---------------------------------------------------------------------------------------

target_compile_definitions(${PROJECT_NAME} PUBLIC
    -DCMBS_API
    -DDNA_TAG
    -DCHECKSUM_SUPPORT
    -D_CONSOLE
    -DCMBS_BUF_SIZE=2560
    -DCATIQ_2_1
    -DSUOTA_SDU_CHOPPING
    -DSUOTA_APP
    -DTCXLOG_FILE
    -DOS_64_BIT
    -DSUOTA_LOCAL_FILE
    -DCMBS_MSG_DEBUG
    -DPNCAP
    -D__linux__
)
