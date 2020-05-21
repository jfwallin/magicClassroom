// %BANNER_BEGIN%
// ---------------------------------------------------------------------
// %COPYRIGHT_BEGIN%
//
// Copyright (c) 2019 Magic Leap, Inc. (COMPANY) All Rights Reserved.
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
  \defgroup Location Location
  \addtogroup Location
  \sharedobject ml_location
  \brief APIs for device location.

  \{
*/

enum {
  MLResultAPIPrefix_Location = MLRESULT_PREFIX(0xda19)
};

/*!
  \brief A set of possible location result codes.
  \apilevel 4
*/
typedef enum MLLocationResult {
  /*! Unknown location error. */
  MLLocationResult_Unknown = MLResultAPIPrefix_Location,
  /*! No connection to server. */
  MLLocationResult_NetworkConnection,
  /*! No location data received. */
  MLLocationResult_NoLocation,
  /*! Location provider is not found. */
  MLLocationResult_ProviderNotFound,
  /*! Ensure enum is represented as 32 bits. */
  MLLocationResult_Ensure32Bits = 0x7FFFFFFF
} MLLocationResult;

/*!
  \brief Returns an ASCII string for each result code.

  This call returns strings for all of the MLResult and MLLocationResult codes.

  \apilevel 4

  \param[in] result_code MLResult to select the result code.

  \retval ASCII string containing readable version of result code.
*/
ML_API const char* ML_CALL MLLocationGetResultString(MLResult result_code);

/*!
  \brief Mask value to determine the validity of #MLLocationData.
  \apilevel 7
*/
typedef enum MLLocationDataMask {
  /*! Mask for determinig the validity of the postal code. */
  MLLocationDataMask_HasPostalCode   = 1 << 0,
  /*! Mask for determinig the validity of the accuracy. */
  MLLocationDataMask_HasAccuracy     = 1 << 1
} MLLocationDataMask;

enum {
  /*! Maximum buffer size for a postal code. */
  MLLocation_MaxPostalCodeSize = 64
};

/*!
  \brief Location request result.
  \apilevel 7
*/
typedef struct MLLocationData {
  /* Version of this structure. */
  uint32_t version;
  /*! Location latitude. */
  float latitude;
  /*! Location longitude. */
  float longitude;
  /*! Location mask value, see #MLLocationDataMask. */
  uint32_t location_mask;
  /*! Approximate postal code. */
  char postal_code[MLLocation_MaxPostalCodeSize];
  /*! Epoch timestamp in milliseconds of the location data. */
  uint64_t timestamp_ms;
  /*! The accuracy radius in meters of the estimated location. */
  float accuracy;
} MLLocationData;

/*!
  \brief Intializes the default values for #MLLocationData.
  \apilevel 7

  \param[in, out] inout_data The object to initialize with default values.
*/
ML_STATIC_INLINE void MLLocationDataInit(MLLocationData *inout_data) {
  inout_data->version = 2u;
  inout_data->latitude = 0.0f;
  inout_data->longitude = 0.0f;
  inout_data->location_mask = 0u;
  inout_data->postal_code[0] = '\0';
  inout_data->timestamp_ms = 0ul;
  inout_data->accuracy = 0.0f;
}

/*!
  \brief Gets device last known coarse location.

  Coarse location provides latitude and longitude estimate from block to city accuracy.
  Accuracy field of the MLLocationData is not available for this API.

  \apilevel 4

  \param[out] out_location Device's last known location. This is only valid when the API returns MLResult_Ok.

  \retval MLResult_InvalidParam Failed due to invalid input parameter.
  \retval MLResult_Ok Obtained location successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed due to internal error.
  \retval MLLocationResult Specific location error.

  \priv CoarseLocation
*/
ML_API MLResult ML_CALL MLLocationGetLastCoarseLocation(MLLocationData *out_location);

/*!
  \brief Gets device last known fine location.

  Accuracy field of the MLLocationData provides the estimate accuracy radius in meters.

  \apilevel 7

  \param[out] out_location Device's last known location. This is only valid when the API returns MLResult_Ok.

  \retval MLResult_InvalidParam Failed due to invalid input parameter.
  \retval MLResult_Ok Obtained location successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed due to internal error.
  \retval MLLocationResult Specific location error.

  \priv FineLocation
*/
ML_API MLResult ML_CALL MLLocationGetLastFineLocation(MLLocationData *out_location);

/*! \} */

ML_EXTERN_C_END
