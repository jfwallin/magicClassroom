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

ML_EXTERN_C_BEGIN

/*!
  \defgroup Networking Networking
  \addtogroup Networking
  \sharedobject ml_networking
  \brief Networking APIs provide network status.
  There are two APIs.
  MLNetworkingIsInternetConnected indicates the internet is up and accessible,
  link layer is up, DNS is good, and internet is accessible.
  MLNetworkingGetWiFiData returns WiFi parameters if WiFi is connected.
  \{
*/

enum {
  /*! Defines the prefix for MLNetResult codes. */
  MLResultAPIPrefix_Networking = MLRESULT_PREFIX(0x4c62)
};

/*!
  \brief Return values for Networking Service API calls.

  \apilevel 5
*/
typedef enum MLNetworkingResult {
  /*! The corresponding service is not available. */
  MLNetworkingResult_ServiceNotAvailable = MLResultAPIPrefix_Networking,
  /*! The corresponding service returned with error. */
  MLNetworkingResult_ServiceError,
  /*! The version number in MLNetworkingWiFiData is not recognized. */
  MLNetworkingResult_WiFiDataStructureVersionError,
  /*! WiFi service is not in the right state. */
  MLNetworkingResult_WiFiServiceInvalidState,
  /*! Ensure enum is represented as 32 bits. */
  MLNetworkingResult_Ensure32Bits = 0x7FFFFFFF
} MLNetworkingResult;

/*!
  \brief WiFi related data.

  \apilevel 5
*/
typedef struct MLNetworkingWiFiData {
  /*! Version of this structure. */
  uint32_t version;
  /*! WiFi RSSI in dbM. */
  int32_t rssi;
  /*! WiFi link speed in Mb/s. */
  int32_t linkspeed;
  /*! WiFi frequency in MHz. */
  float frequency;
} MLNetworkingWiFiData;

/*!
  \brief Intializes the default values for #MLNetworkingWiFiData.

  \apilevel 5

  \param[in, out] in_out_data The object to initialize with default values.
*/
ML_STATIC_INLINE void MLNetworkingWiFiDataInit(MLNetworkingWiFiData *in_out_data) {
  if(NULL != in_out_data) {
    in_out_data->version = 1u;
    in_out_data->rssi = 0;
    in_out_data->linkspeed = 0;
    in_out_data->frequency = 0.0f;
  }
}

/*!
  \brief Returns an ASCII string for each result code.

  This call returns strings for all of the MLResult and MLNetworkingResult codes.

  \apilevel 5

  \param[in] result_code MLResult to select the result code.

  \retval ASCII string containing readable version of result code.
*/
ML_API const char* ML_CALL MLNetworkingGetResultString(MLResult result_code);

/*!
  \brief Returns whether the internet is connected.
  It means link layer is up, internet is accessible, DNS is good.
  The out_is_connected value is valid only if the function returns MLResult_Ok.

  \apilevel 5

  \param[out] out_is_connected Indicates whether the internet is up and accessible.

  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok If the call was successful.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Internal error.
  \retval MLNetworkingResult Network Manager Service specific errors.

  \priv MLPrivilegeID_Internet
*/
ML_API MLResult ML_CALL MLNetworkingIsInternetConnected(bool *out_is_connected);

/*!
  \brief Gets device WiFi related data.
  The out_wifidata is valid only if the API returns MLResult_Ok.

  \apilevel 5

  \param[out] out_wifidata. It has WiFi related data.

  \retval MLResult_InvalidParam Failed due to invalid input parameter.
  \retval MLResult_Ok Obtained WiFi data successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed due to internal error.
  \retval MLNetworkingWiFiResult Specific WiFi service error.

  \priv MLPrivilegeID_WifiStatusRead
*/
ML_API MLResult ML_CALL MLNetworkingGetWiFiData(MLNetworkingWiFiData *out_wifidata);

/*! \} */
ML_EXTERN_C_END

