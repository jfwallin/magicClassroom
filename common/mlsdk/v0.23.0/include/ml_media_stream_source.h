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
  \defgroup MediaPlayer Media Player
  \addtogroup MediaPlayer
  \sharedobject ml_mediastream_source
  \brief APIs for accessing streaming media from memory.

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.
  \{
*/

/*!
  \brief Called when buffers becomes available.

  Signal that at least one buffer is now available. Application should then
  call getBuffer() as much as possible until it returns false, or until no
  more data is currently available in the application.

  \param[in] media_stream_source #MLHandle as returned by MLMediaStreamSourceCreate().
  \param[in,out] context User data as passed to MLMediaStreamSourceCreate().
*/
typedef void (*MLMediaStreamSourceOnBufferAvailable) (MLHandle media_stream_source, void *context);

/*!
  \brief Create a new MediaStreamSource object.

  \param[in] on_buffer_available #MLMediaStreamSourceOnBufferAvailable callback.
  \param[in] context User data to be passed to callbacks.
  \param[out] out_handle The #MLHandle to the new source object created. Only valid if result is #MLResult_Ok.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok If operation was successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaStreamSourceCreate(MLMediaStreamSourceOnBufferAvailable on_buffer_available, void *context, MLHandle *out_handle);

/*!
  \brief Destroy a MediaStreamSource object.

  \param[in] media_stream_source #MLHandle to the MediaStreamSource object to destroy.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok If operation was successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaStreamSourceDestroy(MLHandle media_stream_source);

/*!
  \brief Get a buffer where data can be written.

  If a buffer is available, true is returned and \c out_id, \c out_ptr and \c out_size
  are set. Application can then write up to \c out_size bytes into \c out_ptr and
  push that data using MLMediaStreamSourcePushBuffer().

  If no buffer is available, false is returned and application should wait
  for MLMediaStreamSourceOnBufferAvailable() to be called before retrying.

  \param[in] media_stream_source #MLHandle as returned by MLMediaStreamSourceCreate().
  \param[out] out_id An opaque ID that should be passed to MLMediaStreamSourcePushBuffer().
  \param[out] out_ptr The location where to write data.
  \param[out] out_size Maximum bytes that can be written in \c out_ptr.

  \retval MLResult_Ok If operation was successful.
  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaStreamSourceGetBuffer(MLHandle media_stream_source, size_t *out_id, uint8_t **out_ptr, size_t *out_size);

/*!
  \brief Push a buffer.

  Queue a buffer acquired from MLMediaStreamSourceGetBuffer() once data has been written.

  \param[in] media_stream_source #MLHandle as returned by MLMediaStreamSourceCreate().
  \param[in] id The ID of the buffer as given by MLMediaStreamSourceGetBuffer().
  \param[in] size The number of bytes actually written in the buffer.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok If operation was successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaStreamSourcePushBuffer(MLHandle media_stream_source, size_t id, size_t size);

/*!
  \brief Push End-Of-Stream event.

  Signal that the end of stream is reached and no more data will be pushed.

  \param[in] media_stream_source #MLHandle as returned by MLMediaStreamSourceCreate().

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok If operation was successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaStreamSourcePushEOS(MLHandle media_stream_source);

/*! \} */

ML_EXTERN_C_END
