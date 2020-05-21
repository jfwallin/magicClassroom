// %BANNER_BEGIN%
// ---------------------------------------------------------------------
// %COPYRIGHT_BEGIN%
//
// Copyright (c) 2017 Magic Leap, Inc. (COMPANY) All Rights Reserved.
// Magic Leap, Inc. Confidential and Proprietary
//
// NOTICE:  All information contained herein is, and remains the property
// of COMPANY. The intellectual and technical concepts contained herein
// are proprietary to COMPANY and may be covered by U.S. and Foreign
// Patents, patents in process, and are protected by trade secret or
// copyright law.  Dissemination of this information or reproduction of
// this material is strictly forbidden unless prior written permission is
// obtained from COMPANY.  Access to the source code contained herein is
// hereby forbidden to anyone except current COMPANY employees, managers
// or contractors who have executed Confidentiality and Non-disclosure
// agreements explicitly covering such access.
//
// The copyright notice above does not evidence any actual or intended
// publication or disclosure  of  this source code, which includes
// information that is confidential and/or proprietary, and is a trade
// secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION,
// PUBLIC  PERFORMANCE, OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS
// SOURCE CODE  WITHOUT THE EXPRESS WRITTEN CONSENT OF COMPANY IS
// STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE LAWS AND
// INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE
// CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS
// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE,
// USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.
//
// %COPYRIGHT_END%
// --------------------------------------------------------------------*/
// %BANNER_END%

#pragma once

#include "ml_api.h"

ML_EXTERN_C_BEGIN

/*!
  \defgroup MediaError Media Error
  \addtogroup MediaError
  \sharedobject ml_mediaerror
  \{
*/

enum {
  /*! Defines the prefix for MLMediaGenericResult codes. */
  MLResultAPIPrefix_MediaGeneric = MLRESULT_PREFIX(0xbf3b)
};

/*! Generic return values for Multimedia API calls. */
typedef enum MLMediaGenericResult {
  MLMediaGenericResult_InvalidOperation = MLResultAPIPrefix_MediaGeneric,
  MLMediaGenericResult_BadType,
  MLMediaGenericResult_NameNotFound,
  MLMediaGenericResult_HandleNotFound,
  MLMediaGenericResult_NoInit,
  MLMediaGenericResult_AlreadyExists,
  MLMediaGenericResult_DeadObject,
  MLMediaGenericResult_FailedTransaction,
  MLMediaGenericResult_BadIndex,
  MLMediaGenericResult_NotEnoughData,
  MLMediaGenericResult_WouldBlock,
  MLMediaGenericResult_UnknownTransaction,
  MLMediaGenericResult_FDSNotAllowed,
  MLMediaGenericResult_UnexpectedNull,
  MLMediaGenericResult_NotAvailable,
} MLMediaGenericResult;

enum {
  /*! Defines the prefix for MLMediaResult codes. */
  MLResultAPIPrefix_Media = MLRESULT_PREFIX(0x4184)
};

/*! Detailed return values for Multimedia API calls. */
typedef enum MLMediaResult {
  MLMediaResult_AlreadyConnected = MLResultAPIPrefix_Media,
  MLMediaResult_NotConnected,
  MLMediaResult_UnknownHost,
  MLMediaResult_CannotConnect,
  MLMediaResult_IO,
  MLMediaResult_ConnectionLost,
  MLMediaResult_Legacy1,
  MLMediaResult_Malformed,
  MLMediaResult_OutOfRange,
  MLMediaResult_BufferTooSmall,
  MLMediaResult_Unsupported,
  MLMediaResult_EndOfStream,
  MLMediaResult_FormatChanged,
  MLMediaResult_Discontinuity,
  MLMediaResult_OutputBuffersChanged,
  MLMediaResult_PermissionRevoked,
  MLMediaResult_UnsupportedAudioFormat,
  MLMediaResult_HeartbeatTerminateRequested
} MLMediaResult;

enum {
  /*! Defines the prefix for MLMediaDRMResult codes. */
  MLResultAPIPrefix_MediaDRM = MLRESULT_PREFIX(0x62ce)
};

/*! DRM related return values for Multimedia API calls. */
typedef enum MLMediaDRMResult {
  /*! Error code for undefined type. */
  MLMediaDRMResult_Unknown = MLResultAPIPrefix_MediaDRM,
  /*! Error code for no DRM license. */
  MLMediaDRMResult_NoLicense,
  /*! Error code for DRM license expired. */
  MLMediaDRMResult_LicenseExpired,
  /*! Error code for DRM session not expired. */
  MLMediaDRMResult_SessionNotOpened,
  /*! Error code for DRM when decrypt unit is not initialized. */
  MLMediaDRMResult_DecryptUnitNotInitialized,
  /*! Error code for DRM when failed to decrypt data. */
  MLMediaDRMResult_Decrypt,
  /*! Error code for DRM can not handle the operation. */
  MLMediaDRMResult_CannotHandle,
  /*! Error code for DRM when data is tampered/ */
  MLMediaDRMResult_TamperDetect,
  /*!
    \brief Error Code when an operation on a \ref MLMediaDRM handle is attempted
    and the device does not have a certificate.
    The app should obtain and install a certificate using the \ref MLMediaDRM
    provisioning methods then retry the operation.
  */
  MLMediaDRMResult_NotProvisioned,
  /*! Error code for Device License Revoked. */
  MLMediaDRMResult_DeviceRevoked,
  /*! Error code if the MLMediaDRM operation fails when the required resources are in use. */
  MLMediaDRMResult_ResourceBusy,
  /*! Error code for insufficient output protection. */
  MLMediaDRMResult_InsufficientOutputProtection,
  MLMediaDRMResult_LastUsedErrorCode = MLMediaDRMResult_InsufficientOutputProtection,

  /*! Range for vendor specific DRM errors. */
  MLMediaDRMResult_VendorMin = MLResultAPIPrefix_MediaDRM + 500,
  MLMediaDRMResult_VendorMax = MLResultAPIPrefix_MediaDRM + 999,

} MLMediaDRMResult;

enum {
  /*! Defines the prefix for OMX specific error codes. */
  MLResultAPIPrefix_MediaOMX = MLRESULT_PREFIX(0x5f76),
  /*! Defines the prefix for OMX extensions specific error codes. */
  MLResultAPIPrefix_MediaOMXExtensions = MLRESULT_PREFIX(0x11d9),
  /*! Defines the prefix for OMX vendor specific error codes. */
  MLResultAPIPrefix_MediaOMXVendors = MLRESULT_PREFIX(0xe4d0),
};

enum {
  /*! Defines the prefix for MLMediaPlayerResult codes. */
  MLResultAPIPrefix_MediaPlayer = MLRESULT_PREFIX(0xc435),
};

/*! Media player related return values for Multimedia API calls. */
typedef enum MLMediaPlayerResult {
  /*! Media errors (eg. Codec not supported). */
  MLMediaPlayerResult_ServerDied = MLResultAPIPrefix_MediaPlayer,
  /*! Runtime errors. */
  MLMediaPlayerResult_NotValidForProgressivePlayback,
} MLMediaPlayerResult;

/*! \brief Translate a media error code into a human-readable string

    \param[in] code Error code to translate to a string.

    \return C-style ASCII string containing readable version of result code.
            Note that user should not keep a reference to the returned char pointer for future
            use as subsequent calls to MLMediaResultGetString will overwrite its content.
            Consequently, user *must not* explicitly free the returned pointer.

    \priv None
 */
ML_API const char * ML_CALL MLMediaResultGetString(MLResult code);

/*! \} */

ML_EXTERN_C_END
