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
  \sharedobject ml_bluetooth_gatt
  \{
*/

/*! Maximum length of UUID. */
#define LENGTH_UUID 36
/*! Maximum length of buffer. */
#define MAX_VALUE_BUFFER 600
/*! Maximum MTU size. */
#define MAX_MTU_SIZE 256
/*! Minimum MTU size. */
#define MIN_MTU_SIZE 23

enum {
    MLResultAPIPrefix_BluetoothGatt = MLRESULT_PREFIX(0xa718)
};

/*!
  \brief Possible result codes for Bluetooth GATT.

  \apilevel 8

*/
typedef enum MLBluetoothGattResult {
  /*! Service list is not founded. */
  MLBluetoothGattResult_NoServiceRecord = MLResultAPIPrefix_BluetoothGatt,
  /*! An invalid attribute is spcified. */
  MLBluetoothGattResult_InvalidAttribute,
  /*! GATT read is not permitted. */
  MLBluetoothGattResult_ReadNotPermitted,
  /*! GATT write is not permitted. */
  MLBluetoothGattResult_WriteNotPermitted,
  /*! Insufficient authentication for a given operation. */
  MLBluetoothGattResult_AuthenticationFailed,
  /*! The given operation is not supported. */
  MLBluetoothGattResult_NotSupported,
  /*! GATT internal error. */
  MLBluetoothGattResult_InternalError,
  /*! Indicates a general service error. */
  MLBluetoothGattResult_ServiceFailure,
  MLBluetoothGattResult_Ensure32Bits = 0x7FFFFFFF
} MLBluetoothGattResult;

/*!
  \brief Definition of attribute permissions.

  \apilevel 8

*/
enum {
  MLBluetoothGattAttrPerm_Read = (1 << 0),
  MLBluetoothGattAttrPerm_ReadEncrypted = (1 << 1),
  MLBluetoothGattAttrPerm_ReadEncryptedMITM = (1 << 2),
  MLBluetoothGattAttrPerm_Write = (1 << 4),
  MLBluetoothGattAttrPerm_WriteEncrypted = (1 << 5),
  MLBluetoothGattAttrPerm_WriteEncryptedMITM = (1 << 6),
  MLBluetoothGattAttrPerm_WriteSigned = (1 << 7),
  MLBluetoothGattAttrPerm_WriteSignedMITM = (1 << 8)
};
/*!
  \brief GATT attribute.

  \apilevel 8

*/
typedef int32_t MLBluetoothGattAttrPerm;

/*!
  \brief Definition of characteristic properties.

  \apilevel 8

*/
enum {
  MLBluetoothGattCharProp_Broadcast = (1 << 0),
  MLBluetoothGattCharProp_Read = (1 << 1),
  MLBluetoothGattCharProp_WriteNoRes = (1 << 2),
  MLBluetoothGattCharProp_Write = (1 << 3),
  MLBluetoothGattCharProp_Notify = (1 << 4),
  MLBluetoothGattCharProp_Indicate = (1 << 5),
  MLBluetoothGattCharProp_SignedWrite = (1 << 6),
  MLBluetoothGattCharProp_ExtProps = (1 << 7)
};
/*!
  \brief GATT characteristic property.

  \apilevel 8

*/
typedef int32_t MLBluetoothGattCharProp;

/*!
  \brief Definition of write types.

  \apilevel 8

*/
enum {
  MLBluetoothGattCharWriteType_NoResponse = (1 << 0),
  MLBluetoothGattCharWriteType_Default = (1 << 1),
  MLBluetoothGattCharWriteType_Signed = (1 << 2 )
};
/*!
  \brief GATT characteristic write type.

  \apilevel 8

*/
typedef int32_t MLBluetoothGattCharWriteType;

/*!
  \brief Status values of GATT operation.

  \apilevel 8

*/
typedef enum MLBluetoothGattStatus {
  /*! An operation is completed successfully. */
  MLBluetoothGattStatus_Success = 0,
  /*! GATT read operation is not permitted. */
  MLBluetoothGattStatus_Read_Not_Permitted = 0x02,
  /*! GATT write operation is not permitted. */
  MLBluetoothGattStatus_Write_Not_Permitted = 0x03,
  /*! Insufficient authentication for a given operation. */
  MLBluetoothGattStatus_Insufficient_Authentication = 0x05,
  /*! The given request is not supported. */
  MLBluetoothGattStatus_Request_Not_Supported = 0x06,
  /*! A read or write operation was requested with an invalid offset. */
  MLBluetoothGattStatus_Invalid_Offset = 0x07,
  /*! A write operation exceeds the maximum length of the attribute. */
  MLBluetoothGattStatus_Invalid_Attribute_Length = 0x0d,
  /*! Insufficient encryption for a given operation. */
  MLBluetoothGattStatus_Insufficient_Encryption = 0x0f,
  /*! A remote device connection is congested. */
  MLBluetoothGattStatus_Connection_Congested = 0x8f,
  /*! Generic error. */
  MLBluetoothGattStatus_Error = 0x85,
  /*! An operation failed. */
  MLBluetoothGattStatus_Failure = 0x101,
  MLBluetoothGattStatus_Ensure32Bits = 0x7FFFFFFF
} MLBluetoothGattStatus;

/*!
  \brief GATT Connection states.

  \apilevel 8

*/
typedef enum MLBluetoothGattConnectionState {
  MLBluetoothGattConnectionState_Disconnected = 0,
  MLBluetoothGattConnectionState_Connected,
  MLBluetoothGattConnectionState_Ensure32Bits = 0x7FFFFFFF
} MLBluetoothGattConnectionState;

/*!
  \brief Connection interval.

  \apilevel 8

*/
typedef enum MLBluetoothGattConnectionPriority {
  MLBluetoothGattConnectionPriority_Balanced = 0,
  MLBluetoothGattConnectionPriority_High,
  MLBluetoothGattConnectionPriority_Low_Power,
  MLBluetoothGattConnectionPriority_Ensure32Bits = 0x7FFFFFFF
} MLBluetoothGattConnectionPriority;

/*!
  \brief A structure for Bluetooth UUID.

  \apilevel 8

*/
typedef struct MLBluetoothGattUUID {
  char uuid[LENGTH_UUID + 1];
} MLBluetoothGattUUID;

/*!
  \brief A structure containing the contents of a GATT descriptor.

  \apilevel 8

*/
typedef struct MLBluetoothGattDescriptor {
  /*! Version of this structure. */
  uint32_t version;
  /*! UUID for the descriptor. */
  MLBluetoothGattUUID uuid;
  /*! Instance ID. */
  int32_t instance_id;
  /*! Permission. */
  MLBluetoothGattAttrPerm permissions;
  /*! Value for the descriptor. */
  uint8_t value[MAX_VALUE_BUFFER];
  /*! The size of value. */
  int32_t size;
} MLBluetoothGattDescriptor;

/*!
  \brief A structure for a node in the list of GATT descriptors.

  \apilevel 8

*/
typedef struct MLBluetoothGattDescriptorNode {
  /*! A descriptor. */
  MLBluetoothGattDescriptor descriptor;
  /*! The pointer of next descriptor. */
  struct MLBluetoothGattDescriptorNode *next;
} MLBluetoothGattDescriptorNode;

/*!
  \brief A structure containing the contents of a GATT characteristic.

  \apilevel 8

*/
typedef struct MLBluetoothGattCharacteristic {
  /*! Version of this structure. */
  uint32_t version;
  /*! UUID for the characteristic. */
  MLBluetoothGattUUID uuid;
  /*! Instance ID. */
  int32_t instance_id;
  /*! Permission. */
  MLBluetoothGattAttrPerm permissions;
  /*! Properties. */
  MLBluetoothGattCharProp properties;
  /*! Write type. */
  MLBluetoothGattCharWriteType write_type;
  /*! Value for the characteristic. */
  uint8_t value[MAX_VALUE_BUFFER];
  /*! The size of value. */
  int32_t size;
  /*! The pointer of descriptor included. */
  MLBluetoothGattDescriptorNode *descriptors;
} MLBluetoothGattCharacteristic;

/*!
  \brief A structure for a node in the list of GATT characteristices.

  \apilevel 8

*/
typedef struct MLBluetoothGattCharacteristicNode {
  /*! A characteristic. */
  MLBluetoothGattCharacteristic characteristic;
  /*! The pointer of next characteristic. */
  struct MLBluetoothGattCharacteristicNode *next;
} MLBluetoothGattCharacteristicNode;

/*!
  \brief A structure containing the contents of a GATT include.

  \apilevel 8

*/
typedef struct MLBluetoothGattIncludedService {
  /*! UUID for the service. */
  MLBluetoothGattUUID uuid;
  /*! Instance ID. */
  int32_t instance_id;
  /*! Service type. */
  int32_t service_type;
} MLBluetoothGattIncludedService;

/*!
  \brief A structure for a node in the list of GATT includes.

  \apilevel 8

*/
typedef struct MLBluetoothGattIncludedServiceNode {
  /*! A service. */
  MLBluetoothGattIncludedService inc_service;
  /*! The pointer of next service. */
  struct MLBluetoothGattIncludedServiceNode *next;
} MLBluetoothGattIncludedServiceNode;

/*!
  \brief A structure containing the contents of a GATT service.

  \apilevel 8

*/
typedef struct MLBluetoothGattService {
  /*! Instance ID. */
  int32_t instance_id;
  /*! Service type. */
  int32_t service_type;
  /*! UUID for the service. */
  MLBluetoothGattUUID uuid;
  /*! The pointer of included service. */
  MLBluetoothGattIncludedServiceNode *inc_services;
  /*! The pointer of included characteristic. */
  MLBluetoothGattCharacteristicNode *characteristics;
} MLBluetoothGattService;

/*!
  \brief A structure for a node in the list of GATT services.

  \apilevel 8

*/
typedef struct MLBluetoothGattServiceNode {
  /*! A service. */
  MLBluetoothGattService service;
  /*! The pointer of next service. */
  struct MLBluetoothGattServiceNode *next;
} MLBluetoothGattServiceNode;

/*!
  \brief A structure for the list of GATT services.

  \apilevel 8

*/
typedef struct MLBluetoothGattServiceList {
  /*! Version of this structure. */
  uint32_t version;
  /*! The pointer of service list. */
  MLBluetoothGattService *services;
  /*! The number of GATT services. */
  size_t count;
} MLBluetoothGattServiceList;

/*!
  \brief A structure containing callbacks for GATT client events.
  Individual callbacks which are not required by the client can be NULL.

  \apilevel 8

*/
typedef struct MLBluetoothLeGattClientCallbacks {
  /*! Version of this structure. */
  uint32_t version;
  /*! This callback will be invoked when GATT connection state is changed. */
  void (*on_gatt_connection_state_changed)(
      MLBluetoothGattStatus status, MLBluetoothGattConnectionState new_state,
      void *context);
  /*! This callback will be invoked when GATT service discovery is completed. */
  void (*on_gatt_services_discovered)(MLBluetoothGattStatus status, void *context);
  /*! This callback will be invoked when the RSSI value of remote has been read. */
  void (*on_gatt_read_remote_rssi)(int32_t rssi, MLBluetoothGattStatus status, void *context);
  /*! This callback will be invoked when the remote characteristic has been read. */
  void (*on_gatt_characteristic_read)(
      const MLBluetoothGattCharacteristic *characteristic,
      MLBluetoothGattStatus status, void *context);
  /*!
    \brief This callback will be invoked when the characteristic has been written to
     the remote device.
  */
  void (*on_gatt_characteristic_write)(const MLBluetoothGattCharacteristic *characteristic,
                                    MLBluetoothGattStatus status, void *context);
  /*! This callback will be invoked when the remote descriptor has been read. */
  void (*on_gatt_descriptor_read)(const MLBluetoothGattDescriptor *descriptor,
                               MLBluetoothGattStatus status, void *context);
  /*!
    \brief This callback will be invoked when the descriptor has been written to the
     remote device.
  */
  void (*on_gatt_descriptor_write)(const MLBluetoothGattDescriptor *descriptor,
                                MLBluetoothGattStatus status, void *context);
  /*! This callback will be invoked when the remote characteristic has been changed. */
  void (*on_gatt_notify)(const MLBluetoothGattCharacteristic *characteristic,
                       void *context);
  /*! This callback will be invoked when the connection interval has been updated. */
  void (*on_gatt_connection_parameters_updated)(int32_t interval,
          MLBluetoothGattStatus status, void *context);
  /*! This callback will be invoked when the MTU size has been changed. */
  void (*on_gatt_mtu_changed)(int32_t mtu, MLBluetoothGattStatus status, void *context);
} MLBluetoothLeGattClientCallbacks;

/*!
  \brief Intializes the default values for #MLBluetoothGattCharacteristic.

  \apilevel 8

  \param[in,out] inout_char The object to initialize with default values.
*/
ML_STATIC_INLINE void MLBluetoothGattCharacteristicInit(MLBluetoothGattCharacteristic *inout_char) {
  if(NULL != inout_char) {
    inout_char->version = 1u;
    uint32_t i;
    for (i = 0; i <= LENGTH_UUID; i++) {
      inout_char->uuid.uuid[i] = '\0';
    }
    inout_char->instance_id = 0;
    inout_char->permissions = 0;
    inout_char->properties = 0;
    inout_char->write_type = 0;
    for (i = 0; i < 600 ; i++) {
      inout_char->value[i] = 0;
    }
    inout_char->size = 0;
    inout_char->descriptors = NULL;
  }
}

/*!
  \brief Intializes the default values for #MLBluetoothGattDescriptor.

  \apilevel 8

  \param[in,out] inout_desc The object to initialize with default values.
*/
ML_STATIC_INLINE void MLBluetoothGattDescriptorInit(MLBluetoothGattDescriptor *inout_desc) {
  if(NULL != inout_desc) {
    inout_desc->version = 1u;
    uint32_t i;
    for (i = 0; i <= LENGTH_UUID; i++) {
      inout_desc->uuid.uuid[i] = '\0';
    }
    inout_desc->instance_id = 0;
    inout_desc->permissions = 0;
    for (i = 0; i < 600 ; i++) {
      inout_desc->value[i] = 0;
    }
    inout_desc->size = 0;
  }
}

/*!
  \brief Intializes the default values for #MLBluetoothGattServiceList.

  \apilevel 8

  \param[in,out] inout_srv_list The object to initialize with default values.
*/
ML_STATIC_INLINE void MLBluetoothGattServiceListInit(MLBluetoothGattServiceList *inout_srv_list) {
  if(NULL != inout_srv_list) {
    inout_srv_list->version = 1u;
    inout_srv_list->services = NULL;;
    inout_srv_list->count = 0;
  }
}

/*!
  \brief Intializes the default values for #MLBluetoothLeGattClientCallbacks.

  \apilevel 8

  \param[in,out] inout_cb The object to initialize with default values.
*/
ML_STATIC_INLINE void MLBluetoothLeGattClientCallbacksInit(MLBluetoothLeGattClientCallbacks
                                                                    *inout_cb) {
  if(NULL != inout_cb) {
    inout_cb->version = 1u;
    inout_cb->on_gatt_connection_state_changed = NULL;
    inout_cb->on_gatt_services_discovered = NULL;
    inout_cb->on_gatt_read_remote_rssi = NULL;
    inout_cb->on_gatt_characteristic_read = NULL;
    inout_cb->on_gatt_characteristic_write = NULL;
    inout_cb->on_gatt_descriptor_read = NULL;
    inout_cb->on_gatt_descriptor_write = NULL;
    inout_cb->on_gatt_notify = NULL;
    inout_cb->on_gatt_connection_parameters_updated = NULL;
    inout_cb->on_gatt_mtu_changed = NULL;
  }
}

/*!
  \brief Initiates a connection to a Bluetooth GATT capable device.

  The connection state is delivered by the on_gatt_connection_state_changed callback.

  \apilevel 8

  \param[in] bdaddr The Bluetooth address of device to connect to.
  \retval MLResult_Ok The GATT connection attempt has been initiated.
  \retval MLResult_InvalidParam the parameter is invalid.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothGattConnect(const MLBluetoothAddr *bdaddr);

/*!
  \brief Disconnects an established connection, or cancel a connection attempt.

  The connection state is delivered by the on_gatt_connection_state_changed callback.

  \apilevel 8

  \retval MLResult_Ok The GATT disconnection attempt has been initiated.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothGattDisconnect();

/*!
  \brief Discovers GATT services offered by a remote device.

  The results of this call are delivered by the on_gatt_services_discovered callback.

  \apilevel 8

  \retval MLResult_Ok The remote service discovery has been started.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothGattDiscoverServices();

/*!
  \brief Reads the RSSI for a connected remote device.

  The results of this call are delivered by the on_gatt_read_remote_rssi callback.

  \apilevel 8

  \retval MLResult_Ok The RSSI value has been requested successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothGattReadRemoteRssi();

/*!
  \brief Gets a list of GATT services offered by the remote devices.

  \apilevel 8

  \param[out] out_srv_list The list of services on the remote devices.
  \retval MLResult_Ok The remote service discovery has been completed and
  \                   the services have been stored in local device.
  \retval MLResult_InvalidParam The parameter is invalid.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv None
*/
ML_API MLResult ML_CALL MLBluetoothGattGetServiceRecord(
    MLBluetoothGattServiceList *out_srv_list);

/*!
  \brief Release GATT service list.

  \apilevel 8

  \param[in] srv_list The service list to be released.
  \retval MLResult_Ok The service list has been released.
  \retval MLResult_InvalidParam The parameter is invalid.

  \priv None
*/
ML_API MLResult ML_CALL MLBluetoothGattReleaseServiceList(
        MLBluetoothGattServiceList *srv_list);

/*!
  \brief Reads the requested characteristic from the connected remote device.

  The results of this call are delivered by the on_gatt_characteristic_read
  callback. A client must wait for the callback to be called before calling
  read/write again. Otherwise the API call may fail and return an
  MLResult_UnspecifiedError.

  \apilevel 8

  \param[in] characteristic The characteristic to read from the remote device.
  \retval MLResult_Ok The read operation has been initiatied successfully.
  \retval MLResult_InvalidParam The parameter is invalid.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothGattReadCharacteristic(
    const MLBluetoothGattCharacteristic *characteristic);

/*!
  \brief Writes a given characteristic and its value to the connected remote device.

  The results of this call are delivered by the on_gatt_characteristic_write
  callback. A client must wait for the callback to be called before calling
  read/write again. Otherwise the API call may fail and return an
  MLResult_UnspecifiedError.

  \apilevel 8

  \param[in] characteristic The characteristic to write on the remote device.
  \retval MLResult_Ok The write operation has been completed.
  \retval MLResult_InvalidParam The parameter is invalid.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothGattWriteCharacteristic(
    const MLBluetoothGattCharacteristic *characteristic);

/*!
  \brief Reads the requested descriptor from the connected remote device.

  The results of this call are delivered by the on_gatt_descriptor_read
  callback. A client must wait for the callback to be called before calling
  read/write again. Otherwise the API call may fail and return an
  MLResult_UnspecifiedError.

  \apilevel 8

  \param[in] descriptor The descriptor to read from the remote device.
  \retval MLResult_Ok The read operation has been initiatied successfully.
  \retval MLResult_InvalidParam The parameter is invalid.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothGattReadDescriptor(
    const MLBluetoothGattDescriptor *descriptor);

/*!
  \brief Writes the value of a given descriptor to the connected device.

  The results of this call are delivered by the on_gatt_descriptor_write
  callback. A client must wait for the callback to be called before calling
  read/write again. Otherwise the API call may fail and return an
  MLResult_UnspecifiedError.

  \apilevel 8

  \param[in] descriptor The descriptor to write to the remote device.
  \retval MLResult_Ok The write operation has been initiatied successfully.
  \retval MLResult_InvalidParam The parameter is invalid.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothGattWriteDescriptor(
    const MLBluetoothGattDescriptor *descriptor);

/*!
  \brief Enables or disables notifications/indications for a given chracteristic.

  The values for the client characterisitc configuration descriptor to start or stop
  the notification or indication.
  Enable notification: {0x01, 0x00}.
  Enable indication: {0x02, 0x0}.
  Disable: {0x00, 0x00}.

  \apilevel 8

  \param[in] characteristic The characteristic for which to enable notification.
  \param[in] enable Set to true to enable notification or indications.
  \retval MLResult_Ok The requested notification has been set successfully.
  \retval MLResult_InvalidParam The parameter is invalid.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothGattSetCharacteristicNotification(
    const MLBluetoothGattCharacteristic *characteristic, bool enable);

/*!
  \brief Requests a connection parameter update.

  The results of this call are delivered by the on_gatt_connection_parameters_updated
  callback.

  \apilevel 8

  \param[in] priority A specific connection priority.
  \retval MLResult_Ok The connection priority has been requested successfully.
  \retval MLResult_InvalidParam The parameter is invalid.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothGattRequestConnectionPriority(
    MLBluetoothGattConnectionPriority priority);

/*!
  \brief Requests to change MTU size.

  The results of this call are delivered by the on_gatt_mtu_changed callback.

  \apilevel 8

  \param[in] mtu A specific MTU size.
  \retval MLResult_Ok The MTU update has been requested successfully.
  \retval MLResult_InvalidParam the parameter is invalid.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.
  \retval MLBluetoothGattResult Specific GATT service error.

  \priv BluetoothGattWrite
*/
ML_API MLResult ML_CALL MLBluetoothGattRequestMtu(int32_t mtu);

/*!
  \brief Registers GATT client callbacks.

  \apilevel 8

  \param[in] cb GATT client callbacks to receive the events from GATT client.
  \param[in] context The caller can pass in a context that will be returned in the callback.
  \retval MLResult_Ok The GATT client callbacks have been registered.
  \retval MLResult_InvalidParam the parameter is invalid.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified
  error.

  \priv None
*/
ML_API MLResult ML_CALL MLBluetoothGattSetClientCallbacks(
    MLBluetoothLeGattClientCallbacks *cb, void *context);

/*!
  \brief Returns an ASCII string for each result code.

  \apilevel 8

  \param[in] result_code MLResult to select the result code.
  \retval ASCII string containing readable version of result code.

  \priv None
*/
ML_API const char* ML_CALL MLBluetoothGattGetResultString(MLResult result_code);

/*! \} */

ML_EXTERN_C_END
