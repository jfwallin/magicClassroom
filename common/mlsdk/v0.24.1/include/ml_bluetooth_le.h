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
#include "ml_bluetooth.h"

ML_EXTERN_C_BEGIN
/*!
  \addtogroup Bluetooth
  \sharedobject ml_bluetooth_le
  \{
*/

enum {
    MLResultAPIPrefix_BluetoothLe = MLRESULT_PREFIX(0x6f8a)
};

/*!
  \brief Possible result codes for Bluetooth Le.

  \apilevel 8

*/
typedef enum MLBluetoothLeResult {
  /*! Indicates a general service error. */
  MLBluetoothLeResult_ServiceFailure = MLResultAPIPrefix_BluetoothLe,
  MLBluetoothLeResult_Ensure32Bits = 0x7FFFFFFF
} MLBluetoothLeResult;

/*!
  \brief A structure containing the result of BLE scanning.

  \apilevel 8

*/
typedef struct MLBluetoothLeScanResult {
  MLBluetoothDevice device;
} MLBluetoothLeScanResult;

/*!
  \brief A structure containing callbacks for BLE scanner events.
  Individual callbacks which are not required by the client can be NULL.

  \apilevel 8

*/
typedef struct MLBluetoothLeScannerCallbacks {
  /*! Version of this structure. */
  uint32_t version;
  /*! This callback will be invoked when a BLE advertisement has been found. */
  void (*on_scan_result)(MLBluetoothLeScanResult *result, void *context);
} MLBluetoothLeScannerCallbacks;

/*!
  \brief Intializes the default values for #MLBluetoothLeScannerCallbacks.

  \apilevel 8

  \param[in,out] inout_cb The object to initialize with default values.
*/
ML_STATIC_INLINE void MLBluetoothLeScannerCallbacksInit(MLBluetoothLeScannerCallbacks *inout_cb) {
  if(NULL != inout_cb) {
    inout_cb->version = 1u;
    inout_cb->on_scan_result = NULL;
  }
}

/*!
  \brief Registers BLE scanner callbacks.

  \apilevel 8

  \param[in] cb Scanner callbacks to receive the events from BLE scanner.
  \param[in] context The caller can pass in a context that will be returned in the callback.
  \retval MLResult_Ok The scanner callbacks have been registered.
  \retval MLResult_InvalidParam the parameter is invalid.

  \priv None
*/
ML_API MLResult ML_CALL MLBluetoothLeSetScannerCallbacks(
    MLBluetoothLeScannerCallbacks *cb, void *context);

/*!
  \brief Starts Bluetooth LE scan.

  The results of this call is delivered by the on_scan_result callback.

  \apilevel 8

  \retval MLResult_Ok The LE scan has been started.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothLeResult Specific LE service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothLeStartScan();

/*!
  \brief Stops Bluetooth LE scan in progress.

  \apilevel 8

  \retval MLResult_Ok The LE scan has been stopped.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothLeResult Specific LE service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothLeStopScan();

/*!
  \brief Returns an ASCII string for each result code.

  \apilevel 8

  \param[in] result_code MLResult to select the result code.
  \retval ASCII string containing readable version of result code.

  \priv None
*/
ML_API const char* ML_CALL MLBluetoothLeGetResultString(MLResult result_code);

/*! \} */

ML_EXTERN_C_END
