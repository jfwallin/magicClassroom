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
  \defgroup MediaPlayer Media Player
  \addtogroup MediaPlayer
  \sharedobject ml_mediacodec
  \brief APIs for creaitng media surface texture that works with graphics.

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.
  \{
*/

/*! Graphic backend to use.
  \apilevel 6
*/
typedef enum MLMediaSurfaceTextureBackend {
  /*! OpenGL texture. */
  MLMediaSurfaceTextureBackend_OpenGL = 0,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaSurfaceTextureBackend_Ensure32Bits = 0x7FFFFFFF
} MLMediaSurfaceTextureBackend;

typedef void (*MLMediaSurfaceTextureOnFrameAvailableCallback) (MLHandle handle, void *user_data);

/*!
  \brief Create a new MediaSurfaceTexture.

  \apilevel 6

  \param[in] backend The graphic backend to use.
  \param[out] out_handle Upon successful return will point to handle to the created MediaSurfaceTexture.
              Or else, it will point to #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok MediaSurfaceTexture object was created successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaSurfaceTextureCreate(MLMediaSurfaceTextureBackend backend,
    MLHandle *out_handle);

/*!
  \brief Destroy a MediaSurfaceTexture.

  \apilevel 6

  \param[in,out] inout_handle #MLHandle to the MediaSurfaceTexture.

  \retval MLResult_InvalidParam MediaSurfaceTexture object is invalid.
  \retval MLResult_Ok MediaSurfaceTexture object was successfully destroyed.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaSurfaceTextureDestroy(MLHandle *inout_handle);

/*!
  \brief Update the texture image to the most recent frame from the image stream.

  May only be called on the thread with the graphic context that contains the texture object

  \apilevel 6

  \param[in] handle #MLHandle to the MediaSurfaceTexture.

  \retval MLResult_InvalidParam MediaSurfaceTexture object is invalid.
  \retval MLResult_Ok the texture was updated successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaSurfaceTextureUpdateTexImage(MLHandle handle);

/*!
  \brief Attach the MediaSurfaceTexture to the graphic context that is current on the calling thread.

  \apilevel 6

  \param[in] handle #MLHandle to the MediaSurfaceTexture.
  \param[in] texture_id The graphic texture object id.

  \retval MLResult_InvalidParam MediaSurfaceTexture object is invalid.
  \retval MLResult_Ok the texture was detached successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaSurfaceTextureAttachToGLContext(MLHandle handle, uint32_t texture_id);

/*!
  \brief Detach the MediaSurfaceTexture from the graphic context that owns the graphic texture object.

  \apilevel 6

  \param[in] handle #MLHandle to the MediaSurfaceTexture.

  \retval MLResult_InvalidParam MediaSurfaceTexture object is invalid.
  \retval MLResult_Ok the texture was detached successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaSurfaceTextureDetachFromGLContext(MLHandle handle);

/*!
  \brief Retrieve the 4x4 texture coordinate transform matrix associated with the texture image set by the most recent
         call to MLMediaSurfaceTextureUpdateTexImage.

  \apilevel 6

  \param[in] handle #MLHandle to the MediaSurfaceTexture.
  \param[out] out_matrix A 4x4 matrix stored in column-major order as 16 consecutive values.

  \retval MLResult_InvalidParam MediaSurfaceTexture object is invalid.
  \retval MLResult_Ok Operation was successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaSurfaceTextureGetTransformationMatrix(MLHandle handle, float out_matrix[16]);

/*!
  \brief Retrieve the timestamp associated with the texture image set by the most recent call to
         MLMediaSurfaceTextureUpdateTexImage.

  \apilevel 6

  \param[in] handle #MLHandle to the MediaSurfaceTexture.
  \param[out] out_result The timestamp in nanoseconds.

  \retval MLResult_InvalidParam MediaSurfaceTexture object is invalid.
  \retval MLResult_Ok Operation was successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaSurfaceTextureGetTimestamp(MLHandle handle, int64_t *out_result);

/*!
  \brief Set callback to notify client when a frame is available.

  \apilevel 6

  \param[in] handle #MLHandle to the MediaSurfaceTexture.
  \param[in] callback A MLMediaSurfaceTextureOnFrameAvailableCallback function.
  \param[in] user_data Pointer to user payload data.

  \retval MLResult_InvalidParam MediaSurfaceTexture object is invalid.
  \retval MLResult_Ok Callback was set successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaSurfaceTextureSetOnFrameAvailableCallback(MLHandle handle,
    MLMediaSurfaceTextureOnFrameAvailableCallback callback, void *user_data);

/*! \} */

ML_EXTERN_C_END
