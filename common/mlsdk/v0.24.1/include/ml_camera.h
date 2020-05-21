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
  \defgroup Camera Camera
  \addtogroup Camera
  \sharedobject ml_camera
  \brief APIs for accessing Camera Device and to do Camera Capture.

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.
  \{
*/

/*! Camera errors. */
typedef enum MLCameraError {
  MLCameraError_None = 0,
  /*! Invalid state. */
  MLCameraError_Invalid,
  /*! Camera disabled. */
  MLCameraError_Disabled,
  /*! Camera device failed. */
  MLCameraError_DeviceFailed,
  /*! Camera service failed. */
  MLCameraError_ServiceFailed,
  /*! Capture failed. */
  MLCameraError_CaptureFailed,
  /*! Unknown capture error. */
  MLCameraError_Unknown,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraError_Ensure32Bits = 0x7FFFFFFF
} MLCameraError;

/*! Captured output format. */
typedef enum MLCameraOutputFormat {
  MLCameraOutputFormat_Unknown,
  /*! YUV planar format. */
  MLCameraOutputFormat_YUV_420_888,
  /*! Compressed output stream. */
  MLCameraOutputFormat_JPEG,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraOutputFormat_Ensure32Bits = 0x7FFFFFFF
} MLCameraOutputFormat;

/*! Per plane info for captured output. */
typedef struct MLCameraPlaneInfo {
  /*! Width of the output image in pixels. */
  uint32_t width;
  /*! Height of the output image in pixels. */
  uint32_t height;
  /*! Stride of the output image in bytes. */
  uint32_t stride;
  /*! Number of bytes used to represent a pixel. */
  uint32_t bytes_per_pixel;
  /*! Image data. */
  uint8_t  *data;
  /*! Number of bytes in the image output data. */
  uint32_t size;
} MLCameraPlaneInfo;

enum {
  /*! Number of planes representing the image color space. */
  MLCamera_MaxImagePlanes = 3
};

/*! Captured output. */
typedef struct MLCameraOutput {
  /*!
    \brief Number of output image planes:
    1 for compressed output such as JPEG stream,
    3 for separate color component output such as YUV/RGB.
  */
  uint8_t plane_count;
  /*! Output image plane info. The number of output planes is specified by plane_count. */
  MLCameraPlaneInfo planes[MLCamera_MaxImagePlanes];
  /*! Supported output format specified by MLCameraOutputFormat. */
  MLCameraOutputFormat format;
} MLCameraOutput;

/*!
  \brief Capture operation type.
  \apilevel 5
*/
typedef enum MLCameraCaptureType {
  /*! To capture an image and save the JPEG-compressed data to a file. */
  MLCameraCaptureType_Image = 0,
  /*! To capture an image and access the raw buffer of the image. */
  MLCameraCaptureType_ImageRaw,
  /*! To capture a video and save it to a file. */
  MLCameraCaptureType_Video,
  /*! To capture a video and and access the raw buffer of the frames. */
  MLCameraCaptureType_VideoRaw,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraCaptureType_Ensure32Bits = 0x7FFFFFFF
} MLCameraCaptureType;

/*! Structure to encapsulate various indices for a capture result. */
typedef struct MLCameraResultExtras {
  /*! An integer to index the request sequence that this result belongs to. */
  int32_t request_id;
  /*! An integer to index this result inside a request sequence, starting from 0. Reserved for future use. */
  int32_t burst_id;
  /*! A 64bit integer to index the frame number associated with this result. */
  int64_t frame_number;
  /*! The partial result count (index) for this capture result. */
  int32_t partial_result_count;
  /*! Frame timestamp in microseconds. */
  uint64_t vcam_timestamp_us;
} MLCameraResultExtras;

/*!
  \brief Structure to encapsulate camera frame specific metadata.
  \apilevel 5
*/
typedef struct MLCameraFrameMetadata {
  /*! Frame exposure time for the given frame in nanoseconds. */
  int64_t exposure_time_ns;
} MLCameraFrameMetadata;

/*! Device status callbacks to be implemented by client to receive device status if callback mechanism is used. */
typedef struct MLCameraDeviceStatusCallbacks {
  /*!
    \brief Callback is invoked when the camera becomes available.

    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetDeviceStatusCallbacks.
  */
  void (*on_device_available)(void *data);
  /*!
    \brief Callback is invoked when the camera becomes unavailable.

    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetDeviceStatusCallbacks.
  */
  void (*on_device_unavailable)(void *data);
  /*!
    \brief Callback is invoked when the camera is opened.

    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetDeviceStatusCallbacks.
  */
  void (*on_device_opened)(void *data);
  /*!
    \brief Callback is invoked when the camera is closed.

    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetDeviceStatusCallbacks.
  */
  void (*on_device_closed)(void *data);
  /*!
    \brief Callback is invoked when the camera is disconnected.

    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetDeviceStatusCallbacks.
  */
  void (*on_device_disconnected)(void *data);
  /*!
    \brief Callback is invoked when the camera encountered errors.

    Invalid: The camera device is not available.

    Disabled: The camera device can't be opened due to a device policy.

    Device error: The camera device encountered a fatal error such as the
    Lightwear device has lost connection or the hardware comm bus is busy and
    not be able to response and caused a timeout.

    Service error: The camera service has encountered a fatal error and
    could not provide service.

    Capture failed: The capure request has failed. on_capture_failed or
    on_capture_buffer_lost callbacks will be invoked when this error happens.

    \param[in] error Represents the error type when happened.
    \param[in] data Custom data to be returned when callback is triggered. This data
                    passed in #MLCameraSetDeviceStatusCallbacks.
  */
  void (*on_device_error)(MLCameraError error, void *data);
  /*!
    \brief Callback is invoked when a preview frame is available. During
    capturing to image or video files, preview frames will not be available,
    and this callback won't be invoked. After capturing is done, preview will
    resume and this callback will be invoked again.

    The preview image's format is YUV_420_888 with the camera sensor's pixel size.

    \param[in] output Handle of preview buffer allocated for preview.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetDeviceStatusCallbacks.
  */
  void (*on_preview_buffer_available)(MLHandle output, void *data);
} MLCameraDeviceStatusCallbacks;

/*!
  \deprecated Deprecated since 0.21.0. Scheduled for removal. Replaced by #MLCameraCaptureCallbacksEx.
  \brief Capture callbacks to be implemented by client to receive capture status if callback mechanism is used.
*/
typedef ML_DEPRECATED_MSG("Replaced by MLCameraCaptureCallbacksEx.")
struct MLCameraCaptureCallbacks {
  /*!
    \brief Callback is invoked when a capture has started.

    MLCameraResultExtras *extra carries capture result information of current captured frame.

    \param[in] extra Carries capture result information of current captured frame.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacks.
  */
  void (*on_capture_started)(const MLCameraResultExtras *extra, void *data);
  /*!
    \brief Callback is invoked when a capture has failed when the camera device
           failed to produce a capture result for the request.

    \param[in] extra Carries capture result information of current captured frame.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacks.
  */
  void (*on_capture_failed)(const MLCameraResultExtras *extra, void *data);
  /*!
    \brief Callback is invoked when a capture has failed when a single buffer for capture
           could not send to its desitintion surface due to lost or unavailable.

    \param[in] extra Carries capture result information of current captured frame.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacks.
  */
  void (*on_capture_buffer_lost)(const MLCameraResultExtras *extra, void *data);
  /*!
    \brief Callback is invoked when a partial capture result is available.

    \param[in] metadata_handle represents camera metadata which could be used as
                               input handle for camera metadata related functions.
    \param[in] extra Carries capture result information of current captured frame.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacks.
  */
  void (*on_capture_progressed)(MLHandle metadata_handle, const MLCameraResultExtras *extra, void *data);
  /*!
    \brief Callback is invoked when capturing single frame is completed and result is available.

    \param[in] metadata_handle represents camera metadata which could be used as
                               input handle for camera metadata related functions.
    \param[in] extra Carries capture result information of current captured frame.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacks.
  */
  void (*on_capture_completed)(MLHandle metadata_handle, const MLCameraResultExtras *extra, void *data);
  /*!
    \brief Callback is invoked when a captured image buffer is available with #MLCameraCaptureType_ImageRaw.

    \param[in] output Provides image properties and buffer pointer to image data.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacks.
  */
  void (*on_image_buffer_available)(const MLCameraOutput *output, void *data);
} MLCameraCaptureCallbacks;


/*!
  \brief Capture callbacks to be implemented by client to receive capture status if callback mechanism is used.

  This structure must be initialized by calling MLCameraCaptureCallbacksExInit() before use.

  \apilevel 5
*/
typedef struct MLCameraCaptureCallbacksEx {
  /*! version contains the version number for this structure. */
  uint32_t version;
  /*!
    \brief Callback is invoked when a capture has started.

    MLCameraResultExtras *extra carries capture result information of current captured frame.

    \param[in] extra Carries capture result information of current captured frame.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacksEx.
  */
  void (*on_capture_started)(const MLCameraResultExtras *extra, void *data);
  /*!
    \brief Callback is invoked when a capture has failed when the camera device
           failed to produce a capture result for the request.

    \param[in] extra Carries capture result information of current captured frame.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacksEx.
  */
  void (*on_capture_failed)(const MLCameraResultExtras *extra, void *data);
  /*!
    \brief Callback is invoked when a capture has failed when a single buffer for capture
           could not send to its desitintion surface due to lost or unavailable.

    \param[in] extra Carries capture result information of current captured frame.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacksEx.
  */
  void (*on_capture_buffer_lost)(const MLCameraResultExtras *extra, void *data);
  /*!
    \brief Callback is invoked when a partial capture result is available.

    \param[in] metadata_handle represents camera metadata which could be used as
                               input handle for camera metadata related functions.
    \param[in] extra Carries capture result information of current captured frame.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacksEx.
  */
  void (*on_capture_progressed)(MLHandle metadata_handle, const MLCameraResultExtras *extra, void *data);
  /*!
    \brief Callback is invoked when capturing single frame is completed and result is available.

    \param[in] metadata_handle represents camera metadata which could be used as
                               input handle for camera metadata related functions.
    \param[in] extra Carries capture result information of current captured frame.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacksEx.
  */
  void (*on_capture_completed)(MLHandle metadata_handle, const MLCameraResultExtras *extra, void *data);
  /*!
    \brief Callback is invoked when a captured image buffer is available with #MLCameraCaptureType_ImageRaw.

    \param[in] output Provides image properties and buffer pointer to image data.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacksEx.
  */
  void (*on_image_buffer_available)(const MLCameraOutput *output, void *data);
  /*!
    \brief Callback is invoked when a captured video frame buffer is available with #MLCameraCaptureType_VideoRaw.

    \param[in] output Provides video frame properties and buffer pointer to video frame data.
    \param[in] extra Carries capture result information of current captured frame.
    \param[in] frame_metadata Carries captured frame specific metadata.
    \param[in] data Custom data to be returned when callback is triggered. This data
               passed in #MLCameraSetCaptureCallbacksEx.
  */
  void (*on_video_buffer_available)(const MLCameraOutput *output, const MLCameraResultExtras *extra, const MLCameraFrameMetadata *frame_metadata, void *data);
} MLCameraCaptureCallbacksEx;

 /*!
  \brief Initialize the callback structure.

  Shall be called before calling MLCameraSetCaptureCallbacksEx().

  \apilevel 5

  \param[in, out] capture_callbacks Capture status callbacks structure to initialize.

  \retval MLResult_Ok Set capture callbacks successfully.
  \retval MLResult_UnspecifiedFailure An internal failure occurred.

  \priv None
*/
ML_STATIC_INLINE void MLCameraCaptureCallbacksExInit(MLCameraCaptureCallbacksEx *inout_capture_callbacks) {
  if (inout_capture_callbacks) {
    inout_capture_callbacks->version = 1;
    inout_capture_callbacks->on_capture_started = NULL;
    inout_capture_callbacks->on_capture_failed = NULL;
    inout_capture_callbacks->on_capture_buffer_lost = NULL;
    inout_capture_callbacks->on_capture_progressed = NULL;
    inout_capture_callbacks->on_capture_completed = NULL;
    inout_capture_callbacks->on_image_buffer_available = NULL;
    inout_capture_callbacks->on_video_buffer_available = NULL;
  }
}

/*! Client can implement polling mechanism to retrieve device status and use these masks to view device status. */
typedef enum MLCameraDeviceStatusFlag {
  /*! Available. */
  MLCameraDeviceStatusFlag_Available     = 1 << 0,
  /*! Opened. */
  MLCameraDeviceStatusFlag_Opened        = 1 << 1,
  /*! Disconnected. */
  MLCameraDeviceStatusFlag_Disconnected  = 1 << 2,
  /*! Error. */
  MLCameraDeviceStatusFlag_Error         = 1 << 3,
} MLCameraDeviceStatusFlag;

/*! Client can implement polling mechanism to retrieve capture status and use these masks to view capture status. */
typedef enum MLCameraCaptureStatusFlag {
  /*! Started. */
  MLCameraCaptureStatusFlag_Started      = 1 << 0,
  /*! Failed. */
  MLCameraCaptureStatusFlag_Failed       = 1 << 1,
  /*! Buffer lost. */
  MLCameraCaptureStatusFlag_BufferLost   = 1 << 2,
  /*! In progress. */
  MLCameraCaptureStatusFlag_InProgress   = 1 << 3,
  /*! Completed. */
  MLCameraCaptureStatusFlag_Completed    = 1 << 4,
} MLCameraCaptureStatusFlag;

/*!
  \brief Connect to camera device.

  After this function returns, a preview stream will be created. The preview
  might not have good quailty image at beginning due to the camera sensor
  requires to adjust and lock the exposure(AE) and white balance(AWB). This
  process takes several frames and it might take up to half second in low light
  condition environment.

  Caller could receive the buffer through use on_preview_buffer_available
  callback.

  \retval MLResult_AllocFailed Failed to allocate memory.
  \retval MLResult_Ok Connected to camera device successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_Timeout Failed to connect to camera device due to timeout.
  \retval MLResult_UnspecifiedFailure Failed to connect to camera device due to internal error.
  \retval MLMediaGenericResult_UnexpectedNull Failed to connect to camera device due to null pointer.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraConnect();

/*!
  \brief Disconnect from camera device.

  \retval MLResult_Ok Connected to camera device successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraDisconnect();

/*!
  \brief Prepare for capture.

  This API prepares for capture per specified #MLCameraCaptureType by creating
  a capture request, and a handle to which is returned to the user, who can choose
  to manipulate the request data(metadata) via APIs defined in ml_camera_metadata.h
  before performing the capture.

  Shall be called after MLCameraConnect().

  \param[in] type Capture operation type.
  \param[out] out_handle Handle to the capture metadata. Only valid if result is MLResult_Ok.

  \retval MLResult_AllocFailed Failed to allocate memory.
  \retval MLResult_InvalidParam Failed to prepare for capture due to invalid input parameter.
  \retval MLResult_Ok Prepared for capture successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to prepare for capture due to internal error.
  \retval MLMediaGenericResult_BadType Failed to prepare for capture due to invalid capture type.
  \retval MLMediaGenericResult_UnexpectedNull Failed to prepare for capture due to null pointer.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraPrepareCapture(MLCameraCaptureType type, MLHandle *out_handle);

/*!
  \brief Set the client-implemented callbacks to convey camera device status.

  Client needs to implement the callbacks defined by #MLCameraDeviceStatusCallbacks.
  The library passes the camera device status to the client via those callbacks.

  Shall be called before MLCameraConnect().

  \param[in] device_status_callbacks Camera device status callbacks.
  \param[in] data User metadata.

  \retval MLResult_Ok Set device status callbacks successfully.
  \retval MLResult_UnspecifiedFailure An internal failure occurred.

  \priv None
*/
ML_API MLResult ML_CALL MLCameraSetDeviceStatusCallbacks(MLCameraDeviceStatusCallbacks *device_status_callbacks, void *data);

 /*!
  \deprecated Deprecated since 0.21.0. Scheduled for removal. Replaced by #MLCameraSetCaptureCallbacksEx.

  \brief Set the client-implemented callbacks to convey capture status.

  Client needs to implement the callbacks defined by #MLCameraCaptureCallbacks.
  The library passes the capture status to the client via those callbacks.

  Shall be called before capture operation for preview/image/video.

  \param[in] capture_callbacks Capture status callbacks.
  \param[in] data User metadata.

  \retval MLResult_Ok Set capture callbacks successfully.
  \retval MLResult_UnspecifiedFailure An internal failure occurred.

  \priv None
*/
ML_DEPRECATED_MSG("Replaced by MLCameraSetCaptureCallbacksEx.")
ML_API MLResult ML_CALL MLCameraSetCaptureCallbacks(MLCameraCaptureCallbacks *capture_callbacks, void *data);

 /*!
  \brief Set the client-implemented callbacks to convey capture status.

  Client needs to implement the callbacks defined by #MLCameraCaptureCallbacksEx.
  The library passes the capture status to the client via those callbacks.

  Shall be called before capture operation for preview/image/video.

  \apilevel 5

  \param[in] capture_callbacks Capture status callbacks.
  \param[in] data User metadata.

  \retval MLResult_Ok Set capture callbacks successfully.
  \retval MLResult_UnspecifiedFailure An internal failure occurred.

  \priv None
*/
ML_API MLResult ML_CALL MLCameraSetCaptureCallbacksEx(MLCameraCaptureCallbacksEx *capture_callbacks, void *data);

/*!
  \brief Set the output format of captured image file or image raw buffer.

  After changing output format, MLCameraPrepareCapture must be invoked to take
  effect or capture will fail.

  Without calling this function, default output format will be JPEG.

  \param[in] format Output format.

  \retval MLResult_Ok Set output format successfully.
  \retval MLResult_InvalidParam Invalid input parameter.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraSetOutputFormat(MLCameraOutputFormat format);

/*!
  \brief Capture still image and save output to a file

  The output image will be stored in the format set by MLCameraSetOutputFormat and
  saved into the file specified by the file path. The library is responsible for
  opening and closing the file.

  If this function is invoked before the camera sensor has locked AE and AWB,
  it will be blocked till AE,AWB is locked and then starts to capture.

  \param[in] path File path to store the output image.

  \retval MLResult_InvalidParam Failed to capture image due to invalid input parameter.
  \retval MLResult_Ok Captured image successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_Timeout Failed to capture image due to timeout.
  \retval MLMediaGenericResult_InvalidOperation Failed to capture image due to on-going video recording.
  \retval MLMediaGenericResult_UnexpectedNull Failed to capture image due to null pointer.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraCaptureImage(const char *path);

/*!
  \brief Capture still image and get output data in buffer.

  The output is the raw image data with format set by MLCameraSetOutputFormat
  and passed to client via on_image_buffer_available. Client can also choose to
  implement polling mechanism and obtain the stream by MLCameraPollImageStream.

  If this function is invoked before the camera sensor has locked AE and AWB,
  it will be blocked till AE,AWB is locked and then starts to capture.

  \retval MLResult_Ok Captured raw image successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_Timeout Failed to capture raw image due to timeout.
  \retval MLMediaGenericResult_InvalidOperation Failed to capture raw image due to on-going video recording.
  \retval MLMediaGenericResult_UnexpectedNull Failed to capture raw image due to null pointer.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraCaptureImageRaw();

/*!
  \brief Start video capture and save output to a file.

  The captured video and audio streams will be encoded with AVC and AAC codecs
  and packed in mp4 container format and stored into the file specified by the
  file path. The library is responsible for opening and closing the file. The
  current supported video resolution is 1080p.

  If this function is invoked before the camera sensor has locked AE and AWB,
  it will be blocked till AE,AWB is locked and then starts to capture.

  MLCameraCaptureVideoStop() needs to be called to stop the capture.

  \param[in] path File path to store the output video.

  \retval MLResult_AllocFailed Failed to allocate memory.
  \retval MLResult_InvalidParam Failed to start video recording due to invalid input parameter.
  \retval MLResult_Ok Started video recording successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_Timeout Failed to start video recording due to timeout.
  \retval MLResult_UnspecifiedFailure Failed to start video recording due to internal error.
  \retval MLMediaGenericResult_UnexpectedNull Failed to start video recording due to null pointer.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraCaptureVideoStart(const char *path);

/*!
  \brief Start video capture and provide raw frames through callback

  The captured video YUV frames will be returned to the application via on_video_buffer_available.
  The current supported video resolution is 1080p.

  If this function is invoked before the camera sensor has locked AE and AWB,
  it will be blocked till AE,AWB is locked and then starts to capture.

  MLCameraCaptureVideoStop() needs to be called to stop the capture.

  \apilevel 5

  \retval MLResult_AllocFailed Failed to allocate memory.
  \retval MLResult_InvalidParam Failed to start video recording due to invalid input parameter.
  \retval MLResult_Ok Started video recording successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_Timeout Failed to start video recording due to timeout.
  \retval MLResult_UnspecifiedFailure Failed to start video recording due to internal error.
  \retval MLMediaGenericResult_UnexpectedNull Failed to start video recording due to null pointer.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraCaptureRawVideoStart();

/*!
  \brief Stop video capture.

  User should allow some time, i.e., >500ms, after MLCameraCaptureVideoStart and before
  calling this API, as captured frames are being encoded. Otherwise, MLResult_UnspecifiedFailure
  will be returned.

  \retval MLResult_Ok Stopped video recording successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to stop video recording due to internal error.
  \retval MLMediaGenericResult_UnexpectedNull Failed to stop video recording due to null pointer.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraCaptureVideoStop();

/*!
  \brief Poll camera device status.

  Use #MLCameraDeviceStatusFlag to view specific status bit.
  Call MLCameraGetErrorCode() to obtain the error code if
  #MLCameraDeviceStatusFlag_Error bit is set.

  Note: This API can still be used even if MLCameraSetDeviceStatusCallbacks() has been called.

  \param[out] out_device_status Device status.

  \retval MLResult_InvalidParam Failed to obtain device status due to invalid input parameter.
  \retval MLResult_Ok Obtained device status successfully.

  \priv None
*/
ML_API MLResult ML_CALL MLCameraGetDeviceStatus(uint32_t *out_device_status);

/*!
  \brief Poll capture status.

  Use #MLCameraCaptureStatusFlag to view specific status bit.
  Call MLCameraGetErrorCode() to obtain the error code if
  #MLCameraCaptureStatusFlag_Error bit is set.

  Note: This API can still be used even if MLCameraSetCaptureStatusCallbacks() has been called.

  \param[out] out_capture_status Capture status.

  \retval MLResult_InvalidParam Failed to obtain capture status due to invalid input parameter.
  \retval MLResult_Ok Obtained capture status successfully.

  \priv None
*/
ML_API MLResult ML_CALL MLCameraGetCaptureStatus(uint32_t *out_capture_status);

/*!
  \brief Obtain device error code.

  \param[out] out_error_code Device error code.

  \retval MLResult_InvalidParam Failed to obtain device error code due to invalid input parameter.
  \retval MLResult_Ok Obtained device error code successfully.

  \priv None
*/
ML_API MLResult ML_CALL MLCameraGetErrorCode(MLCameraError *out_error_code);

/*!
  \brief Poll preview stream.

  The preview stream and image stream are separate streams. The preview data is
  available upon MLCameraConnect(), and will always be available; whereas the image stream
  is produced when the user captures images.
  The library allocates the buffer and destroys it when disconnecting from the camera.

  Note: This API can still be used even if MLCameraSetDeviceStatusCallbacks() has been called.

  \param[out] out_preview Preview stream.

  \retval MLResult_InvalidParam Failed to obtain preview stream due to invalid input parameter.
  \retval MLResult_Ok Obtained preview stream successfully.

  \priv None
*/
ML_API MLResult ML_CALL MLCameraGetPreviewStream(MLHandle *out_preview);

/*!
  \brief Poll raw image stream.

  The library allocates the buffer and destroys it when disconnecting from the camera.

  Note: This API can still be used even if MLCameraSetCaptureStatusCallbacks() has been called.

  \param[out] out_image Raw image stream.

  \retval MLResult_InvalidParam Failed to obtain raw image stream due to invalid input parameter.
  \retval MLResult_Ok Obtained raw image stream successfully.

  \priv None
*/
ML_API MLResult ML_CALL MLCameraGetImageStream(MLCameraOutput **out_image);

/*!
  \brief Poll capture result extras data.

  The library allocates the buffer and destroys it when disconnecting from the camera.

  Note: This API can still be used even if MLCameraSetCaptureStatusCallbacks() has been called.

  \param[out] out_result_extras Capture result extras data.

  \retval MLResult_InvalidParam Failed to obtain result extras due to invalid input parameter.
  \retval MLResult_Ok Obtained result extras data successfully.

  \priv None
*/
ML_API MLResult ML_CALL MLCameraGetCaptureResultExtras(MLCameraResultExtras **out_result_extras);

/*!
  \brief Obtain handle for retrieving camera characteristics.

  This API provides the handle for retrieving camera characteristics via APIs
  defined in ml_camera_metadata.h.

  \param[out] out_handle Handle to access camera characteristic metadata. Only valid if result is MLResult_Ok.

  \retval MLResult_AllocFailed Failed to allocate memory.
  \retval MLResult_InvalidParam Failed to obtain camera characteristic handle due to invalid input parameter.
  \retval MLResult_Ok Obtained camera characteristic handle successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLMediaGenericResult_UnexpectedNull obtain camera characteristic handle due to null pointer.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraGetCameraCharacteristics(MLHandle *out_handle);

/*!
  \brief Obtains handle for retrieving capture result metadata

  This API provides the handle for retrieving capture result metadata via APIs
  defined in ml_camera_metadata.h.

  Note: that this handle is also available via callbacks if capture callbacks has been set.

  \param[out] out_handle Handle to access capture result metadata. Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Failed to obtain capture result handle due to invalid input parameter.
  \retval MLResult_Ok Obtained capture result handle successfully.
  \retval MLMediaGenericResult_UnexpectedNull obtain capture result handle due to null pointer.

  \priv None
*/
ML_API MLResult ML_CALL MLCameraGetResultMetadata(MLHandle *out_handle);

/*! \} */

ML_EXTERN_C_END
