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
  \defgroup MediaPlayer Media Player
  \addtogroup MediaPlayer
  \sharedobject ml_mediacodec
  \brief APIs for encoding and decoding audio and video buffers.

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.
  \{
*/

/*! Media codec buffer info. */
typedef struct MLMediaCodecBufferInfo {
  /*! Offset. */
  size_t   offset;
  /*! Size. */
  size_t   size;
  /*! Flags. */
  size_t   flags;
  /*! Presentation time in us. */
  int64_t  presentation_time_us;
} MLMediaCodecBufferInfo;

/*! Media codec creation method. */
typedef enum MLMediaCodecCreation {
  /*! Create by name. */
  MLMediaCodecCreation_ByName,
  /*! Create by type. */
  MLMediaCodecCreation_ByType,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaCodecCreation_Ensure32Bits = 0x7FFFFFFF
} MLMediaCodecCreation;

/*! Media codec type. */
typedef enum MLMediaCodecType {
  /*! Encoder. */
  MLMediaCodecType_Encoder,
  /*! Decoder. */
  MLMediaCodecType_Decoder,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaCodecType_Ensure32Bits = 0x7FFFFFFF
} MLMediaCodecType;

/*! Media codec surface hint. */
typedef enum MLMediaCodecSurfaceHint {
  /*! Hardware. */
  MLMediaCodecSurfaceHint_Hardware,
  /*! Software. */
  MLMediaCodecSurfaceHint_Software,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaCodecSurfaceHint_Ensure32Bits = 0x7FFFFFFF
} MLMediaCodecSurfaceHint;

/*!
  \brief Indicate the type of data when queuing the input buffer via
  MLMediaCodecQueueInputBuffer() or MLMediaCodecQueueSecureInputBuffer().
*/
typedef enum MLMediaCodecBufferFlag {
  MLMediaCodecBufferFlag_KeyFrame     = 1,
  MLMediaCodecBufferFlag_CodecConfig  = 2,
  MLMediaCodecBufferFlag_EOS          = 4,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaCodecBufferFlag_Ensure32Bits = 0x7FFFFFFF
} MLMediaCodecBufferFlag;

/*!
  \brief Other than the normal buffer index, the output of MLMediaCodecDequeueOutputBuffer()
  can take on one of these statuses.
*/
typedef enum MLMediaCodecOutputBufferStatus {
  /*! Output buffer changed. */
  MLMediaCodec_OutputBuffersChanged           = -1014,
  /*! Format changed. */
  MLMediaCodec_FormatChanged                  = -1012,
  /*! Try again later. */
  MLMediaCodec_TryAgainLater                  = -11,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaCodecOutputBufferStatus_Ensure32Bits = 0x7FFFFFFF
} MLMediaCodecOutputBufferStatus;

#define MAX_CODEC_NAME_SIZE 64

/*!
  \brief Callbacks for notifying client about codec events. If not using the SYNC API
  calls hereunder, client is expected to implement these callbacks and take
  proper actions where appropriate.
*/
typedef struct MLMediaCodecCallbacks {
  /*!
    \brief This callback function is invoked when an input buffer becomes available.

    This should not be set to NULL as codec will not function properly if client has
    no means to specify incoming input buffers.

    \note Client should *not* call MLMediaCodec APIs from within this callback but
    rather set internal flags or use synchronization mechanisms to wake up another
    thread that will queue encoded data into decoder.

    \param[in] media_codec MLMediaCodec instance for which callback was called.
    \param[in] input_buffer_index The index of the available input buffer.
    \param[in] data Custom data to be returned when callback is fired.
  */
  void (*on_input_buffer_available)(MLHandle media_codec, int64_t input_buffer_index, void* data);
  /*!
    \brief This callback function is invoked when an output buffer.

    This should not be set to NULL as codec will not function properly.
    Typical implementation on on_output_buffer_available is simply to
    1) render the buffer to audio subsystem [if codec is audio]
    2) release-and-render-to-surface with MLMediaCodecReleaseOutputBuffer(.., true) [if codec is video].

    \note Client should *not* call MLMediaCodec APIs from within this callback but
    rather set internal flags or use synchronization mechanisms to wake up another
    thread that will dequeue and release the buffer.

    \param[in] media_codec MLMediaCodec instance for which callback was called.
    \param[in] output_buffer_index The index of the available output buffer.
    \param[in] buffer_info The buffer info about the available output buffer
    \param[in] data Custom data to be returned when callback is fired.
  */
  void (*on_output_buffer_available)(MLHandle media_codec, int64_t output_buffer_index, MLMediaCodecBufferInfo *buffer_info, void* data);
  /*!
    \brief This callback function is invoked when the output format has changed.

    This can be set to NULL if client is not interested in receiving this callback.

    \note Client should *not* call MLMediaCodec APIs from within this callback but
    rather set internal flags or use synchronization mechanisms to wake up another
    thread that will handle the change.

    \param[in] media_codec MLMediaCodec instance for which callback was called.
    \param[in] new_output_format Handle to the new output format.
    \param[in] data Custom data to be returned when callback is fired.
  */
  void (*on_output_format_changed)(MLHandle media_codec, MLHandle new_output_format, void* data);
  /*!
    \brief This callback function is invoked when \ref MLMediaCodec encountered an error.

    This can be set to NULL if client is not interested in receiving this callback.

    \note Client should *not* call MLMediaCodec APIs from within this callback.

    \param[in] media_codec MLMediaCodec instance for which callback was called.
    \param[in] error_code Error code.
    \param[in] data Custom data to be returned when callback is fired.
  */
  void (*on_error)(MLHandle media_codec, int error_code, void* data);
  /*!
    \brief This callback function is invoked when an output frame has
    rendered on the output surface.

    This is usually the same as specified in
    MLMediaCodecQueueInputBuffer(); however, some codecs may alter the
    media time by applying some time-based transformation, such as frame
    rate conversion. In that case, presentation time corresponds
    to the actual output frame rendered.

    This callback is for informational purposes only: to get precise
    render timing samples, and can be significantly delayed and batched.
    Some frames may have been rendered even if there was no callback
    generated. This can be set to NULL if client is not interested in receiving
    this callback.

    \note Client should *not* call MLMediaCodec APIs from within this callback.

    \param[in] media_codec MLMediaCodec instance for which callback was called.
    \param[in] presentation_time_us The presentation time (media time) in microseconds of the frame rendered.
    \param[in] system_time_nano System time when the frame was rendered.
    \param[in] data Custom data to be returned when callback is fired.
  */
  void (*on_frame_rendered)(MLHandle media_codec, int64_t presentation_time_us, int64_t system_time_nano, void* data);
  /*!
    \brief Callback called when decoded frame is available for consumption on client side.

    This is the callback clients should implement when they want to use decoded frame as
    a texture mapped to 3D geometry, for example.

    This callback is typically fired when MLMediaCodecReleaseOutputBuffer(.., true)'s
    rendering operation has not only finished (on_frame_rendered),
    but has reached client-side and is ready to be mapped and rendered from.

    \note Client should *not* call MLMediaCodec APIs from within this callback but
    rather set internal flags or use synchronization mechanisms to wake up another
    thread that will acquire the frame.

    \param[in] media_codec MLMediaCodec instance for which callback was called.
    \param[in] data Custom data to be returned when callback is fired.
  */
  void (*on_frame_available)(MLHandle media_codec, void *data);
} MLMediaCodecCallbacks;

/*!
  \brief Create a new \ref MLMediaCodec.

  \param[in] method One of the creation methods defined by #MLMediaCodecCreation.
  \param[in] type One of the codec types defined by #MLMediaCodecType.
  \param[in] name_str If creating a codec by name, this is the name of the codec.
             If creating a codec by type, this is the mime type of the codec.
             Refer to APIs in \ref MLMediaCodec.ist for retrieving the list of names
             and mime types of supported codecs.
  \param[out] out_handle Upon successful return will point to handle to the created \ref MLMediaCodec.
              Or else, it will point to #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok \ref MLMediaCodec object was created successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecCreateCodec(MLMediaCodecCreation method, MLMediaCodecType type, const char *name_str, MLHandle *out_handle);

/*!
  \brief Destroy a \ref MLMediaCodec.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.

  \retval MLResult_InvalidParam \ref MLMediaCodec object is invalid.
  \retval MLResult_Ok \ref MLMediaCodec object was successfully destroyed.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecDestroy(MLHandle handle);

/*!
  \brief Set callbacks to notify client codec events. Client needs to implement the callback functions.

   If the client intends to use the codec component in asynchronous mode, a valid callback should be
   provided before MLMediaCodecConfigure is called.
   When asynchronous callback is enabled, the client should not call:
     - MLMediaCodecGetInputBufferPointer
     - MLMediaCodecGetOutputBufferPointer
     - MLMediaCodecDequeueInputBuffer
     - MLMediaCodecDequeueOutputBuffer
   Calling those functions will return MLMediaGenericResult_InvalidOperation.

   Also, MLMediaCodecFlush() behaves differently in asynchronous mode.
   After calling MLMediaCodecFlush, you must call MLMediaCodecStart() to "resume" receiving input buffers, even if an input surface was created.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[in] callbacks Set of codec callbacks. Can be set to NULL to unset all the callback altogether.
  \param[in] user_data Pointer to user payload data.

  \retval MLResult_InvalidParam \ref MLMediaCodec object is invalid.
  \retval MLResult_Ok callback was set successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecSetCallbacks(MLHandle handle, MLMediaCodecCallbacks *callbacks, void *user_data);

/*!
  \brief Obtain the name of the created codec.

  User shall allocate at least #MAX_CODEC_NAME_SIZE bytes of memory for
  storing the output name string.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[out] out_name Pointer to the user-maintained buffer for storing
              the output C-style name string.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok \ref MLMediaCodec name was retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecGetName(MLHandle handle, char *out_name);

/*!
  \brief Set surface hint on decoder.

  Note that MLMediaCodecSetSurfaceHint *has* to be called before MLMediaCodecConfigure
  or else, behavior will default to MLMediaCodecSurfaceHint_Hardware.

  If surface_hint is set to 'MLMediaCodecSurfaceHint_Hardware', codec will try to create
  an internal native hardware surface onto which frames will be directly decoded (zero-copy
  fast path). In that case, calls to MLMediaCodecGetOutputBufferPointer() will return NULL for
  out_buffer_ptr and 0 for out_buffer_size, since decoded buffers will be residing in GPU memory.
  (note: Only true if codec was correctly configured with an available Hardware decoder on
  the system, otherwise, decoder will be software and MLMediaCodecGetOutputBufferPointer() will
  return CPU-allocated buffer pointer and size). When using MLMediaCodecSurfaceHint_Hardware with
  any Hardware Decoder, dequeued output buffers need to be "rendered" onto the internal native surface
  by setting 'render' = true when calling MLMediaCodecReleaseOutputBuffer() and then, rendered frame
  can be retrieved in the form of a native buffer handle by calling MLMediaCodecAcquireNextAvailableFrame().
  This handle points to an android native buffer and can for example be bound as an external texture
  with OpenGLES.

  If surface_hint is set to MLMediaCodecSurfaceHint_Software, Codec will allocate an internal
  buffer on CPU memory onto which decoded buffers will be copied (slow path). In that case,
  note that there is no need to "render" the output buffer when releasing it. To access
  decoded pixels, call MLMediaCodecGetOutputBufferPointer() on the buffer handle and
  out_buffer_ptr and out_buffer_size will return the correct pointer to data and frame size.
  Note this mode of operation is very slow and is generally intended for callers with no access
  to a compliant graphics library or with very specific needs.

  Note: This API is only relevant if current codec is VIDEO. For AUDIO codec, this call will
  just be ignored.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[in] surface_hint Hint about intended usage of the decoded buffers. \c See notes in description
             above for important details about this parameter.

  \retval MLResult_Ok Surface hint was correctly set. Note: If called after MLMediaCodecConfigure,
          the call will be a NOOP and behavior will default to MLMediaCodecSurfaceHint_Hardware at
          configuration time.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecSetSurfaceHint(MLHandle handle, MLMediaCodecSurfaceHint surface_hint);

/*!
  \brief Configure the \ref MLMediaCodec.

  Note that caller should consider calling MLMediaCodecSetSurfaceHint() prior to configuring the
  codec, as this hint may have important incidence on buffer usage once decoded. See documentation
  for MLMediaCodecSetSurfaceHint above.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[in] format_handle #MLHandle to the #MediaFormat object.
  \param[in] crypto_handle #MLHandle to the #MediaCrypto object.
             Pass 0 for clear content.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok \ref MLMediaCodec was configured successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Internal native surface could not be allocated by graphics
          hardware (only for surface_hint = MLMediaCodecSurfaceHint_Hardware. With
          MLMediaCodecSurfaceHint_Hardware surface_hint, this return value can't occur).
          NOTE: In the surface_hint=MLMediaCodecSurfaceHint_Hardware case, always make sure caller
          has created a valid graphics context *before* calling MLMediaCodecConfigure !
          The MLMediaGenericResult_NoInit error is not recoverable and caller has to clean codec
          object and start anew with different configuration.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecConfigure(MLHandle handle, MLHandle format_handle, MLHandle crypto_handle);

/*!
  \brief Configure the MediaCodec with a MediaSurfaceTexture.

  Same as MLMediaCodecConfigure(), but allow passing a custom surface texture created by
  MLMediaSurfaceTextureCreate(). The value set by MLMediaCodecSetSurfaceHint() is ignored.

  \apilevel 6

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[in] format_handle #MLHandle to the #MediaFormat object.
  \param[in] surface_handle #MLHandle to the #MediaSurfaceTexture object.
  \param[in] crypto_handle #MLHandle to the #MediaCrypto object.
             Pass 0 for clear content.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok \ref MLMediaCodec was configured successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecConfigureWithSurface(MLHandle handle, MLHandle format_handle, MLHandle surface_handle, MLHandle crypto_handle);

/*!
  \brief Start the codec.

  Shall be called upon successful configuration.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.

  \retval MLResult_InvalidParam \ref MLMediaCodec object is invalid.
  \retval MLResult_Ok \ref MLMediaCodec was started successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecStart(MLHandle handle);

/*!
  \brief Stop the codec.

  Codec is stopped and not destroyed, therefore can be started again.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.

  \retval MLResult_InvalidParam \ref MLMediaCodec object is invalid.
  \retval MLResult_Ok \ref MLMediaCodec was stopped successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecStop(MLHandle handle);

/*!
  \brief Flush the both the input and output ports of the codec.

  Upon return, all indices previously returned in calls to
  MLMediaCodecDequeueInputBuffer() and MLMediaCodecDequeueOutputBuffer()
  become invalid, and all buffers are owned by the codec.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.

  \retval MLResult_InvalidParam \ref MLMediaCodec object is invalid.
  \retval MLResult_Ok \ref MLMediaCodec was flushed successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecFlush(MLHandle handle);

/*!
  \brief Obtain a writable buffer pointer for a dequeued
  input buffer index to contain the input data.

  After calling this method any buffer pointer previously
  returned for the same input index MUST no longer be used.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[in] buffer_handle The index of a client-owned input buffer
             previously returned from a call to
             MLMediaCodecDequeueInputBuffer().
  \param[out] out_buffer_ptr Pointer to returned input buffer.
  \param[out] out_buffer_size Size of returned input buffer.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Input buffer pointer is retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecGetInputBufferPointer(MLHandle handle, MLHandle buffer_handle, uint8_t **out_buffer_ptr, size_t *out_buffer_size);

/*!
  \brief Obtain a read-only buffer pointer for a dequeued output
  buffer index.

  The position and limit of the returned buffer are set to the valid output data.
  After calling this method any buffer pointer previously returned for the same output
  index MUST no longer be used.

  Note: For VIDEO codecs, this API will fail if codec was configured with a hardware surface
  (see return value documentation below).

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[in] buffer_handle The index of a client-owned output buffer
             previously returned from a call to
             MLMediaCodecDequeueOutputBuffer().
  \param[out] out_buffer_ptr Pointer to returned output buffer.
  \param[out] out_buffer_size Size of returned output buffer.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Output buffer pointer is retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Buffer pointer could not be retrieved. Note that this API
          returns MLMediaGenericResult_InvalidOperation especially if codec was VIDEO and configured
          with a hardware surface (see MLMediaCodecSetSurfaceHint documentation), and thus, out_buffer_ptr
          will be set to NULL and out_buffer_size set to 0 since decoded buffer lives in GPU memory and is
          not accessible via a pointer.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecGetOutputBufferPointer(MLHandle handle, MLHandle buffer_handle, const uint8_t **out_buffer_ptr, size_t *out_buffer_size);

/*!
  \brief After filling a range of the input buffer at the specified index
  submit it to the component.

  Once an input buffer is queued to the codec, it MUST NOT be used until it is later retrieved by
  MLMediaCodecGetInputBufferPointer() in response to a MLMediaCodecDequeueInputBuffer() return value.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[in] buffer_handle Index of a client-owned input buffer previously returned
             in a call to MLMediaCodecDequeueInputBuffer().
  \param[in] offset Byte offset into the input buffer at which the data starts.
  \param[in] size Number of bytes of valid input data.
  \param[in] time_us Presentation timestamp in microseconds for this buffer.
             This is normally the media time at which this buffer should
             be presented (rendered).
  \param[in] flags A bitmask of flags defined by #MLMediaCodecBufferFlag.
             While not prohibited, most codecs do not use the
             #MLMediaCodecBufferFlag_KeyFrame flag for input buffers.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Intput buffer is queued successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecQueueInputBuffer(MLHandle handle, MLHandle buffer_handle, int64_t offset, size_t size, uint64_t time_us, int flags);

/*!
  \brief Similar to queueInputBuffer but submits a buffer that is potentially encrypted.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[in] buffer_handle Index of a client-owned input buffer previously returned
             in a call to MLMediaCodecDequeueInputBuffer().
  \param[in] offset Byte offset into the input buffer at which the data starts.
  \param[in] crypto_info_handle Metadata required to facilitate decryption,
             the object can be reused immediately after this call returns.
  \param[in] time_us Presentation timestamp in microseconds for this
             buffer. This is normally the media time at which this
             buffer should be presented (rendered).
  \param[in] flags A bitmask of flags defined by #MLMediaCodecBufferFlag.
             While not prohibited, most codecs do not use the
             #MLMediaCodecBufferFlag_KeyFrame flag for input buffers.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Secure Intput buffer is queued successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecQueueSecureInputBuffer(MLHandle handle, MLHandle buffer_handle, int64_t offset, MLHandle crypto_info_handle, uint64_t time_us, int flags);

/*!
  \brief Dequeue an input buffer from buffer queue.

  This method will return immediately if timeoutUs == 0, wait indefinitely
  for the availability of an input buffer if timeoutUs < 0 or wait up
  to "timeoutUs" microseconds if timeoutUs > 0.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[in] timeout_us Timeout in microseconds.
             Negative timeout indicates "infinite".
  \param[out] out_buffer_handle Index of an Input buffer from buffer queue,
              or #MLMediaCodec_TryAgainLater status.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Intput buffer is dequeued successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecDequeueInputBuffer(MLHandle handle, int64_t timeout_us, int64_t *out_buffer_handle);

/*!
  \brief Dequeue an output buffer from buffer queue.

  This method will return immediately if timeoutUs == 0, wait indefinitely
  for the availability of an input buffer if timeoutUs < 0 or wait up
  to "timeoutUs" microseconds if timeoutUs > 0.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[out] out_buffer_info Will be filled with buffer meta data.
  \param[in] timeout_us Timeout in microseconds.
             Negative timeout indicates "infinite".
  \param[out] out_buffer_handle Index of an output buffer from buffer queue,
              or one of the statuses defined by #MLMediaCodecOutputBufferStatus.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Output buffer is dequeued successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecDequeueOutputBuffer(MLHandle handle, MLMediaCodecBufferInfo *out_buffer_info,int64_t timeout_us, int64_t *out_buffer_handle);

/*!
  \brief Obtain input format supported by the codec.

  Call this function, after successful codec configuration, to determine
  what optional configuration parameters were supported by the codec.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[out] out_format_handle Input format supported by the codec.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Input Media format is retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecGetInputFormat(MLHandle handle, MLHandle *out_format_handle);

/*!
  \brief Return output format supported by the codec, or the format to which the configuration has changed.

  \param[in] handle #MLHandle to the \ref MLMediaCodec.
  \param[out] out_format_handle Output format supported by the codec,
              or the format to which the configuration has changed.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Output Media format is retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecGetOutputFormat(MLHandle handle, MLHandle *out_format_handle);

/*!
  \brief Release buffer to codec or to render it on output surface.

  If codec is configured with an output surface, setting render
  to \c true will send the buffer to that output surface. The surface
  will release the buffer back to the codec once it is no longer
  used/displayed.

  Once an output buffer is released to codec, it MUST NOT
  be used until it is later retrieved by MLMediaCodecGetOutputBufferPointer()
  in response to a MLMediaCodecDequeueOutputBuffer() return value.

  \param[in] handle #MLHandle to the \ref MLMediaCodec object.
  \param[in] buffer_handle Index of a client-owned output buffer previously returned
             from a call to MLMediaCodecDequeueOutputBuffer().
  \param[in] render If a valid surface was specified when configuring the
             codec, passing \c true renders this output buffer to the surface.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Output buffer was released successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecReleaseOutputBuffer(MLHandle handle, MLHandle buffer_handle, bool render);

/*!
  \brief Update surface timestamp and returns it to the codec to render
  it on the output surface. If codec is not configured with output
  surface, this call simply returns the buffer to codec.

  The timestamp may have special meaning depending on the destination
  surface.

  \param[in] handle #MLHandle to the \ref MLMediaCodec object.
  \param[in] buffer_handle Index of a client-owned output buffer previously returned
             from a call to MLMediaCodecDequeueOutputBuffer().
  \param[in] timestamp_ns The timestamp in nanoseconds to associate with this buffer when
             it is sent to the Surface.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Output buffer was released successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecReleaseOutputBufferAtTime(MLHandle handle, MLHandle buffer_handle, int64_t timestamp_ns);

/*!
  \brief Consume next available buffer.

  Note: This function is non-blocking and should only be called from the rendering
  thread *after* #MLMediaCodecOnFrameAvailableCallback callback has been fired.

  Note: The returned buffer's color format is multi-planar YUV420. Since our
  underlying hardware interops do not support multiplanar formats, advanced
  texture features like mipmaping, anisotropic filtering and MSAA will *not*
  work on those returned buffers. If such advanced texturing features are
  required, one has to acquire the YUV buffers as usual and *render* to an
  RGB texture and only then, will one be able to enable hardware optimization
  for such advanced texture features.

  \param[in] handle #MLHandle to the \ref MLMediaCodec instance.
  \param[out] out_native_buffer A pointer to the acquired native buffer of the frame
              When buffer is not required anymore, caller must call
              MLMediaCodecReleaseFrame() with this nativeBuffer handle to free up
              queuing space for incoming buffers.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Next available frame is acquired successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecAcquireNextAvailableFrame(MLHandle handle, MLHandle *out_native_buffer);

/*!
  \brief Release a native buffer previously acquired.

  \param[in] handle #MLHandle to the \ref MLMediaCodec instance.
  \param[in] native_buffer The native buffer for the frame to release.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Frame was released successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecReleaseFrame(MLHandle handle, MLHandle native_buffer);

/*!
  \brief Retrieves the 4x4 texture coordinate transform matrix associated with
  the last acquired Frame.

  This transform matrix maps 2D homogeneous texture coordinates of the form
  (s, t, 0, 1) with s and t in the inclusive range [0, 1] to the texture
  coordinate that should be used to sample that location from the texture.
  Sampling the texture outside of the range of this transform is undefined.

  This transform is necessary to compensate for transforms that the stream
  content producer may implicitly apply to the content. By forcing users of
  a GLConsumer to apply this transform we avoid performing an extra
  copy of the data that would be needed to hide the transform from the
  user.

  The matrix is stored in column-major order so that it may be passed
  directly to OpenGL ES via the glLoadMatrixf or glUniformMatrix4fv
  functions.

  \param[in] handle #MLHandle to the \ref MLMediaCodec instance.
  \param[out] out_mtx A pointer to an array of 16 floats that will receive retrieved
              transformation matrix.

  \return MLResult_InvalidParam One of the parameters is invalid.
  \return MLResult_Ok Transformation matrix was returned.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecGetFrameTransformationMatrix(MLHandle handle, float out_mtx[16]);

/*!
  \brief Retrieves the timestamp associated with the last acquired Frame
  The timestamp is in nanoseconds, and is monotonically increasing. Its
  other semantics (zero point, etc) are codec-dependent and should be
  documented by the codec vendor.

  \param[in] handle #MLHandle to the \ref MLMediaCodec instance.
  \param[out] out_timestamp A pointer to user variable for storing retrieved timestamp.

  \return MLResult_InvalidParam One of the parameters is invalid.
  \return MLResult_Ok Timestamp was returned.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecGetFrameTimestamp(MLHandle handle, int64_t *out_timestamp);

/*!
  \brief Retrieves the QueueBuffer timestamp associated with the last acquired Frame
  This timestamp is for profiling purposes only and corresponds to the time when this
  frame was queued by internal Codec's producer code. The timestamp is in nanoseconds,
  and is monotonically increasing. Its other semantics (zero point, etc) are codec-
  dependent and should by documented by the codec vendor.

  \param[in] handle #MLHandle to the \ref MLMediaCodec instance.
  \param[out] out_timestamp A pointer to user variable for storing retrieved timestamp.

  \return MLResult_InvalidParam One of the parameters is invalid.
  \return MLResult_Ok Timestamp was returned.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecGetFrameQueueBufferTimestamp(MLHandle handle, int64_t *out_timestamp);

/*!
  \brief Retrieves the last acquired frame number.
  The frame number is an incrementing counter set to 0 at Codec configuration time.

  \param[in] handle #MLHandle to the \ref MLMediaCodec instance.
  \param[out] out_number A pointer to user variable for storing retrieved frame number.

  \return MLResult_InvalidParam One of the parameters is invalid.
  \return MLResult_Ok Frame number was returned.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCodecGetFrameNumber(MLHandle handle, uint64_t *out_number);

/*! \} */

ML_EXTERN_C_END
