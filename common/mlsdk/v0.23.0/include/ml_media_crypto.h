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

#include "ml_media_drm.h"

ML_EXTERN_C_BEGIN

/*!
  \defgroup MediaPlayer Media Player
  \addtogroup MediaPlayer
  \sharedobject ml_mediacrypto
  \brief APIs for creaitng crypto session for setting up encrypted media playback.

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.
  \{
*/

/*!
  \brief Create a \ref MLMediaCrypto instance from a UUID.

  \param[in] uuid The universal unique ID of the crypto scheme. uuid must be 16 bytes.
  \param[in] data Opaque initialization data specific to the crypto scheme, This value must never be NULL.
  \param[out] out_handle Upon successful return will point to handle to the created \ref MLMediaCrypto.
              Or else, it will point to #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok \ref MLMediaCrypto object was created successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCryptoCreate(MLUUID uuid, const MLMediaDRMByteArray *data, MLHandle *out_handle);

/*!
  \brief Release the \ref MLMediaCrypto instance.

  \param[in] media_crypto #MLHandle to the \ref MLMediaCrypto.

  \retval MLResult_InvalidParam \ref MLMediaCrypto object is invalid.
  \retval MLResult_Ok \ref MLMediaCrypto object was successfully destroyed.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCryptoRelease(MLHandle media_crypto);

/*!
  \brief Query if the given scheme identified by its UUID is supported on this device.

  \param[in] uuid Identifies the universal unique ID of the crypto scheme. uuid must be 16 bytes.
  \param[out] out_supported On successful return contains \c true or \c false based on
              whether the given crypto scheme is supported or not.

  \retval MLResult_InvalidParam Parameter is invalid.
  \retval MLResult_Ok The query has run successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCryptoIsCryptoSchemeSupported(MLUUID uuid, bool *out_supported);

/*!
  \brief Query if the crypto scheme requires the use of a secure decoder to decode data of
  the given mime type.

  \param[in] mime_type The MIME type of the media container, e.g. "video/mp4".
  \param[out] out_required On successful return contains \c true or \c false based on
              whether the given mime type requires a secure decoder.

  \retval MLResult_InvalidParam Parameter is invalid.
  \retval MLResult_Ok The query has run successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCryptoRequiresSecureDecoderComponent(const char *mime_type, bool *out_required);

/*!
  \brief Associate a #MediaDRM session with this \ref MLMediaCrypto instance.

  \param[in] media_crypto #MLHandle to the \ref MLMediaCrypto.
  \param[in] session_id Session ID for the DRM session.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Media DRM session was set successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCryptoSetMediaDRMSession(MLHandle media_crypto, const MLMediaDRMByteArray *session_id);

/*! \} */

ML_EXTERN_C_END
