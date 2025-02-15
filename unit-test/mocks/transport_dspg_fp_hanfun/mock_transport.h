/**
 * @code
 *  ___ _____ _   ___ _  _____ ___  ___  ___ ___
 * / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 * \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 * |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 * embedded.connectivity.solutions.==============
 * @endcode
 *
 * @file       mock_transport.h
 * @copyright  STACKFORCE GmbH, Heitersheim, Germany, http://www.stackforce.de
 * @author     STACKFORCE
 * @details
 *
 * This work is dual-licensed under Apache 2.0 and GPL 2.0. You can choose between one of them if you use this work.
 * For further details, please refer to the project home: https://github.com/opend-connect/openD
 *
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only
 */
#include <gmock/gmock.h>

#include "hf_transport.h"
#include "opend_dataTypes.h"


/* Module class declaration. */
class Hf_transport
{
public:
   virtual ~Hf_transport() {}
   virtual openD_status_t initUleApp( void ) = 0;
   virtual void set_deviceEntryList( ST_IE_HAN_EXTENDED_DEVICE_ENTRIES *pst_HANExtendedDeviceEntry ) = 0;
};


/* Module class implementaion. */
class MockTransport : public Hf_transport
{
public:
   MOCK_METHOD0( initUleApp ,  openD_status_t( void ) );
   MOCK_METHOD1( initUleApp ,  void( ST_IE_HAN_EXTENDED_DEVICE_ENTRIES *pst_HANExtendedDeviceEntry ) );
};

void setMockTransportReference(Hf_transport *transportParam);
