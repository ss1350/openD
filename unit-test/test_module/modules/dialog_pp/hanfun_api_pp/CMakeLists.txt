#  ___ _____ _   ___ _  _____ ___  ___  ___ ___
# / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
# \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
# |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
# embedded.connectivity.solutions.==============
#
# copyright  STACKFORCE GmbH, Heitersheim, Germany, http://www.stackforce.de
# author     Heiko Berger (STACKFORCE), Patrick Weber (STACKFORCE)
# brief
#
# This work is dual-licensed under Apache 2.0 and GPL 2.0. You can choose between one of them if you use this work.
# For further details, please refer to the project home: https://github.com/opend-connect/openD
#
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only

cmake_minimum_required(VERSION 2.8)


##
## PROJECT
##
project(dspg_hanfun_api_pp)

enable_testing()

SET(MOCKED_MODULES_SOURCE_DIR ../../../../../src)

##
## Includes
##
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${MOCKED_MODULES_SOURCE_DIR}/opend/dialog/pp_hanfun/hanfun_api
    ${MOCKED_MODULES_SOURCE_DIR}/opend/dialog/pp_hanfun/iwu
    ${MOCKED_MODULES_SOURCE_DIR}/opend/dialog/pp_hanfun/include
    ${MOCKED_MODULES_SOURCE_DIR}/opend/dialog/pp_hanfun/include/Stack/Include
    ${MOCKED_MODULES_SOURCE_DIR}/opend/dialog/pp_hanfun
    ${MOCKED_MODULES_SOURCE_DIR}/opend
    ${MOCKED_MODULES_SOURCE_DIR}/opend/prv
    ${MOCKED_MODULES_SOURCE_DIR}/hardware/low-level-api
    ${MOCKED_MODULES_SOURCE_DIR}/hardware/boards/dialogNucleo
    ${MOCKED_MODULES_SOURCE_DIR}/hanfun_lib
    ${MOCKED_MODULES_SOURCE_DIR}/hanfun_lib/hanfun_lib/inc
)

##
## Add targets
##
add_executable(ut_dialog_openD_hanfunApi_pp_init ut_dialog_openD_hanfunApi_pp_init.cpp)
add_executable(ut_dialog_openD_hanfunApi_pp_devMgmtRequest ut_dialog_openD_hanfunApi_pp_devMgmtRequest.cpp)
add_executable(ut_dialog_openD_hanfunApi_pp_profileRequest ut_dialog_openD_hanfunApi_pp_profileRequest.cpp)

##
## Link targets
##
target_link_libraries(ut_dialog_openD_hanfunApi_pp_init libgmock pthread libApp libMockDialogIwu libTestHanFun mockLowLevelApi)
target_compile_definitions(ut_dialog_openD_hanfunApi_pp_init PUBLIC -DHF_APP_EXT_REG -DPAYLOADDATASIZE=496)

target_link_libraries(ut_dialog_openD_hanfunApi_pp_devMgmtRequest libgmock pthread libApp libMockDialogIwu libTestHanFun mockLowLevelApi)
target_compile_definitions(ut_dialog_openD_hanfunApi_pp_devMgmtRequest PUBLIC -DHF_APP_EXT_REG -DPAYLOADDATASIZE=496)

target_link_libraries(ut_dialog_openD_hanfunApi_pp_profileRequest libgmock pthread libApp libMockDialogIwu libTestHanFun mockLowLevelApi)
target_compile_definitions(ut_dialog_openD_hanfunApi_pp_profileRequest PUBLIC -DHF_APP_EXT_REG -DPAYLOADDATASIZE=496)

##
## Add tests
##
add_test(NAME ut_dialog_openD_hanfunApi_pp_init
         COMMAND ut_dialog_openD_hanfunApi_pp_init --gtest_output=xml:ut_dialog_openD_hanfunApi_pp_init.xml)
add_test(NAME ut_dialog_openD_hanfunApi_pp_devMgmtRequest
         COMMAND ut_dialog_openD_hanfunApi_pp_devMgmtRequest --gtest_output=xml:ut_dialog_openD_hanfunApi_pp_devMgmtRequest.xml)
add_test(NAME ut_dialog_openD_hanfunApi_pp_profileRequest
         COMMAND ut_dialog_openD_hanfunApi_pp_profileRequest --gtest_output=xml:ut_dialog_openD_hanfunApi_pp_profileRequest.xml)
