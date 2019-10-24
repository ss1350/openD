#  ___ _____ _   ___ _  _____ ___  ___  ___ ___
# / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
# \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
# |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
# embedded.connectivity.solutions.==============
#
# copyright  STACKFORCE GmbH, Heitersheim, Germany, http://www.stackforce.de
# author     Heiko Berger
# brief      openD DSPG FP test.
#
# This work is dual-licensed under Apache 2.0 and GPL 2.0. You can choose between one of them if you use this work.
# For further details, please refer to the project home: https://github.com/opend-connect/openD
#
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only

project(dspg-cmbs-testLib)

cmake_minimum_required(VERSION 3.6)

#---------------------------------------------------------------------------------------
# Build target
#---------------------------------------------------------------------------------------

file(GLOB ${PROJECT_NAME}_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/ext_src/pncap/src/etp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext_src/pncap/src/pie_write.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext_src/pncap/src/utils.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/suota/suota.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/suota/gmep-us.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/han/HAN_Server/hanServer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/han/HAN_Server/hanServerCM.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/han/HAN_Server/hanServerDB.c"


    "${CMAKE_CURRENT_SOURCE_DIR}/tcx_eep.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/tcx_log.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/tcx_hostlog.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/tcx_util.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/keyb_srv.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/keyb_call.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/keyb_facility.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/keyb_suota.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_voipline.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmbs_event.c"


    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appswup.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appdata.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appfun.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appcmbs.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appsrv.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appcall.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appCallRouter.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appfacility.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/applistacc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appla2.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appsuota.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appsuotafile.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appmsgparser.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/apprtp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/applog.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/apphan.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appHanMw.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/apphanDb.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/appstreambuffer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/UDPStorageServer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/appcmbs/apphanvoicecallroutingtable.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ListAccess/sqlite3.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ListAccess/SQLiteWrapper.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ListAccess/ListsApp.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ListAccess/LASessionMgr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ListAccess/ListChangeNotif.c"
)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

#---------------------------------------------------------------------------------------
# Include
#---------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/appcmbs
    ${CMAKE_CURRENT_SOURCE_DIR}/ListAccess
    ${CMAKE_CURRENT_SOURCE_DIR}/han/HAN_Client/src
    ${CMAKE_CURRENT_SOURCE_DIR}/osl/linux
    ${CMAKE_CURRENT_SOURCE_DIR}/suota
    ${CMAKE_CURRENT_SOURCE_DIR}/ext_src/pncap/src
    $<TARGET_PROPERTY:dspg-cmbs,INTERFACE_INCLUDE_DIRECTORIES>
)

#---------------------------------------------------------------------------------------
# Link
#---------------------------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME}
    dl
    # cmbs_host_lnx
    pthread
    rt
    dspg-cmbs-lib
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    -DPNCAP
)
