// %BANNER_BEGIN%
// ---------------------------------------------------------------------
// %COPYRIGHT_BEGIN%
//
// Copyright (c) 2017 Magic Leap, Inc. (COMPANY) All Rights Reserved.
// Magic Leap, Inc. Confidential and Proprietary
//
// NOTICE: All information contained herein is, and remains the property
// of COMPANY. The intellectual and technical concepts contained herein
// are proprietary to COMPANY and may be covered by U.S. and Foreign
// Patents, patents in process, and are protected by trade secret or
// copyright law. Dissemination of this information or reproduction of
// this material is strictly forbidden unless prior written permission is
// obtained from COMPANY. Access to the source code contained herein is
// hereby forbidden to anyone except current COMPANY employees, managers
// or contractors who have executed Confidentiality and Non-disclosure
// agreements explicitly covering such access.
//
// The copyright notice above does not evidence any actual or intended
// publication or disclosure of this source code, which includes
// information that is confidential and/or proprietary, and is a trade
// secret, of COMPANY. ANY REPRODUCTION, MODIFICATION, DISTRIBUTION,
// PUBLIC PERFORMANCE, OR PUBLIC DISPLAY OF OR THROUGH USE OF THIS
// SOURCE CODE WITHOUT THE EXPRESS WRITTEN CONSENT OF COMPANY IS
// STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE LAWS AND
// INTERNATIONAL TREATIES. THE RECEIPT OR POSSESSION OF THIS SOURCE
// CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS
// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE,
// USE, OR SELL ANYTHING THAT IT MAY DESCRIBE, IN WHOLE OR IN PART.
//
// %COPYRIGHT_END%
// ---------------------------------------------------------------------
// %BANNER_END%

#pragma once

#include "ml_api.h"
#include "ml_types.h"

ML_EXTERN_C_BEGIN
/*!
  \addtogroup Bluetooth
  \sharedobject ml_bluetooth
  \brief APIs for Bluetooth system.
   Bluetooth APIs provide functionalities for.
   - Local Bluetooth adapter.
   - Communicating with a GATT server on a remote device.
   - Scanning advertisers.

  \{
*/

/*! Maximum length of Bluetooth address. */
#define LENGTH_BD_ADDR 17
/*! Maximum length of Bluetooth name. */
#define LENGTH_DEV_NAME 249

/*!
  \brief A structure for Bluetooth address.

  \apilevel 8

*/
typedef struct MLBluetoothAddr {
  char address[LENGTH_BD_ADDR + 1];
} MLBluetoothAddr;

/*!
  \brief A structure for Bluetooth name.

  \apilevel 8

*/
typedef struct MLBluetoothName {
  char name[LENGTH_DEV_NAME + 1];
} MLBluetoothName;

/*!
  \brief Device types.

  \apilevel 8

*/
typedef enum MLBluetoothDeviceType {
  MLBluetoothDeviceType_Unknown = 0,
  MLBluetoothDeviceType_LE = 2,
  MLBluetoothDeviceType_Ensure32Bits = 0x7FFFFFFF
} MLBluetoothDeviceType;

/*!
  \brief A structure for a remote Bluetooth device.

  \apilevel 8

*/
typedef struct MLBluetoothDevice {
  /*! Version of this structure. */
  uint32_t version;
  /*! Bluetooth device name. */
  MLBluetoothAddr bd_addr;
  /*! Bluetooth device address. */
  MLBluetoothName bd_name;
  /*! The RSSI. */
  uint8_t rssi;
  /*! Bluetooth device type. */
  MLBluetoothDeviceType device_type;
} MLBluetoothDevice;

/*!
  \brief Intializes the default values for #MLBluetoothDevice.

  \apilevel 8

  \param[in,out] inout_bt_device The object to initialize with default values.
*/
ML_STATIC_INLINE void MLBluetoothDeviceInit(MLBluetoothDevice *inout_bt_device) {
  if(NULL != inout_bt_device) {
    inout_bt_device->version = 1u;
    uint32_t i;
    for (i = 0; i <= LENGTH_BD_ADDR; i++) {
      inout_bt_device->bd_addr.address[i] = '\0';
    }
    for (i = 0; i <= LENGTH_DEV_NAME; i++) {
      inout_bt_device->bd_name.name[i] = '\0';
    }
    inout_bt_device->rssi = 0;
    inout_bt_device->device_type = MLBluetoothDeviceType_LE;
  }
}

/*! \} */

ML_EXTERN_C_END
