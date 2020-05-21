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
  \sharedobject ml_bluetooth_adapter
  \{
*/

/*! The length of Bluetooth credential. */
#define LENGTH_CREDENTIAL 7

enum {
    MLResultAPIPrefix_BluetoothAdapter = MLRESULT_PREFIX(0x19b2)
};

/*!
  \brief Possible result codes for Bluetooth adapter.

  \apilevel 8

*/
typedef enum MLBluetoothAdapterResult {
  /*! The pin code for bonding is not valid. */
  MLBluetoothAdapterResult_BondInvalidPin = MLResultAPIPrefix_BluetoothAdapter,
  /*! The bond type is unknown. */
  MLBluetoothAdapterResult_BondUnknownType,
  /*! Indicates a general service error. */
  MLBluetoothAdapterResult_ServiceFailure,
  MLBluetoothAdapterResult_Ensure32Bits = 0x7FFFFFFF
} MLBluetoothAdapterResult;

/*!
  \brief Bluetooth adapter states.

  \apilevel 8

*/
typedef enum MLBluetoothAdapterState {
  MLBluetoothAdapterState_Off,
  MLBluetoothAdapterState_On,
  MLBluetoothAdapterState_Ensure32Bits = 0x7FFFFFFF
} MLBluetoothAdapterState;

/*!
  \brief Bond states.

  \apilevel 8

*/
typedef enum MLBluetoothBondState {
  MLBluetoothBondState_None = 0,
  MLBluetoothBondState_Bonding,
  MLBluetoothBondState_Bonded,
  MLBluetoothBondState_Ensure32Bits = 0x7FFFFFFF
} MLBluetoothBondState;

/*!
  \brief Bond types.

  \apilevel 8

*/
typedef enum MLBluetoothBondType {
  MLBluetoothBondType_ClassicPin = 0,
  MLBluetoothBondType_SspJustWorks,
  MLBluetoothBondType_SspNumericalComparison,
  MLBluetoothBondType_SspPasskeyEntry,
  MLBluetoothBondType_SspPasskeyNotification,
  MLBluetoothBondType_Ensure32Bits = 0x7FFFFFFF
} MLBluetoothBondType;

/*!
  \brief ACL states.

  \apilevel 8

*/
typedef enum MLBluetoothAclState {
  MLBluetoothAclState_Connected = 0,
  MLBluetoothAclState_Disconnected,
  MLBluetoothAclState_Ensure32Bits = 0x7FFFFFFF
} MLBluetoothAclState;

/*!
  \brief A structure containing callbacks for local bluetooth adapter events.
  Individual callbacks which are not required by the client can be NULL.

  \apilevel 8

*/
typedef struct MLBluetoothAdapterCallbacks {
  /*! Version of this structure. */
  uint32_t version;
  /*! This callback will be invoked when the adapter state is changed. */
  void (*on_adapter_state_changed)(MLBluetoothAdapterState state, void *context);
  /*! This callback will be invoked when the bond state is changed. */
  void (*on_bond_state_changed)(const MLBluetoothDevice *device,
                             MLBluetoothBondState state, void *context);
  /*! This callback will be invoked when the ACL state is changed. */
  void (*on_acl_state_changed)(const MLBluetoothDevice *device,
                            MLBluetoothAclState state, void *context);
} MLBluetoothAdapterCallbacks;

/*!
  \brief Intializes the default values for #MLBluetoothAdapterCallbacks.

  \apilevel 8

  \param[in,out] inout_cb The object to initialize with default values.
*/
ML_STATIC_INLINE void MLBluetoothAdapterCallbacksInit(MLBluetoothAdapterCallbacks *inout_cb) {
  if(NULL != inout_cb) {
    inout_cb->version = 1u;
    inout_cb->on_adapter_state_changed = NULL;
    inout_cb->on_bond_state_changed = NULL;
    inout_cb->on_acl_state_changed = NULL;
  }
}

/*!
  \brief Get local Bluetooth adpater name.

  \apilevel 8

  \param[out] out_bdname Friendly local Bluetooth name.
  \retval MLResult_Ok Current local Bluetooth name is retrieved successfully.
  \retval MLResult_InvalidParam The parameter is invalid.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothAdapterResult Specific adapter service error.

  \priv BluetoothAdapterUser
*/
ML_API MLResult ML_CALL MLBluetoothAdapterGetName(MLBluetoothName *out_bdname);

/*!
  \brief Get the state of local Bluetooth adpater.

  \apilevel 8

  \param[out] out_state The state of local Bluetooth adapter.
  \retval MLResult_Ok The status of local Bluetooth is retrieved successfully.
  \retval MLResult_InvalidParam The parameter is invalid.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothAdapterResult Specific adapter service error.

  \priv BluetoothAdapterUser
*/
ML_API MLResult ML_CALL MLBluetoothAdapterGetState(
    MLBluetoothAdapterState *out_state);

/*!
  \brief Initiates bonding request with remote device.

  \apilevel 8

  \param[in] bdaddr The address of remote device to bond with.
  \retval MLResult_Ok Bonding procedure will be started.
  \retval MLResult_InvalidParam The parameter is invalid.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothAdapterResult Specific adapter service error.

  \priv BluetoothAdapterExternalApp
*/
ML_API MLResult ML_CALL MLBluetoothAdapterCreateBond(const MLBluetoothAddr *bdaddr);

/*!
  \brief Registers local adapter callbacks.

  \apilevel 8

  \param[in] cb Adapter callbacks to receive the events from adapter.
  \param[in] context The caller can pass in a context that will be returned in the callback.
  \retval MLResult_Ok The adapter callbacks have been registered.
  \retval MLResult_InvalidParam The parameter is invalid.

  \priv None
*/
ML_API MLResult ML_CALL MLBluetoothAdapterSetCallbacks(
    MLBluetoothAdapterCallbacks *cb, void *context);

/*!
  \brief Returns an ASCII string for each result code.

  \apilevel 8

  \param[in] result_code MLResult to select the result code.
  \retval ASCII string containing readable version of result code.

  \priv None
*/
ML_API const char* ML_CALL MLBluetoothAdapterGetResultString(MLResult result_code);

/*! \} */

ML_EXTERN_C_END
