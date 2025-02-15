/**
 * @code
 *  ___ _____ _   ___ _  _____ ___  ___  ___ ___
 * / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 * \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 * |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 * embedded.connectivity.solutions.==============
 * @endcode
 *
 * @file       ut_opend_mgmt_confirmation.cpp
 * @copyright  STACKFORCE GmbH, Heitersheim, Germany, http://www.stackforce.de
 * @author     Heiko Berger (STACKFORCE)
 * @author     Mario Behringer (STACKFORCE)
 * @details
 *
 * This work is dual-licensed under Apache 2.0 and GPL 2.0. You can choose between one of them if you use this work.
 * For further details, please refer to the project home: https://github.com/opend-connect/openD
 *
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only
 */

/** Include files */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdio.h>
#include <string.h>

extern "C"
{
/** Resource under test */
#include "opend_mgmt.h"
#include "opend_mgmt_api.h"
}

/** Namespaces */
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;

/*!
 * Class definition
 */
class ut_dialog_fp_openD_mgmt_confirmation: public testing::Test
{
    public:

        void SetUp()
        {
        }
        void TearDown()
        {
        }
};

/*
 * Unit tests
 * */

/*!
 * \brief Tests the function openD_mgmt_confirmation.
 *
 * \details Verifies if the function works as expected
 *          when a null pointer is passed.
 */
TEST_F( ut_dialog_fp_openD_mgmt_confirmation, openD_mgmt_confirmation )
{
    openD_mgmtApiPrimitives_t primitives;
    primitives.openD_mgmtApiCfm = NULL;

    openD_mgmtApi_init(&primitives);

    openD_mgmt_confirmation(NULL);
}
