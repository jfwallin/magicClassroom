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
// --------------------------------------------------------------------*/
// %BANNER_END%

#pragma once

#include "ml_api.h"
#include "ml_types.h"

#include <string.h>

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Graphics Graphics
  \sharedobject ml_graphics
  \brief The APIs for the graphics system.

  The enclosed APIs allow for interaction with the graphics system. In general,
  to integrate with the graphics system you will do so with a graphics client
  handle. There are two types of clients currently supported, a GL/GLES client
  supporting all GL/GLES APIs from 3.0 to 4.5, and a VK client which supports
  the latest VK definition on the platform. Once created, the client handle
  can be used to start/end frames, set the requested timing characteristics for
  the client, and query useful information about the client.

  All timing, alignment, etc. of the CPU domain of the client with the asynchronous
  composition is also handled automatically by the API through the blocking
  behavior in #MLGraphicsBeginFrame.

  Note : In the current definition of the API, perception must have already been
  started prior to interacting with the graphics system and the perception lifetime
  is managed separate of the graphics system.
  \{
*/

/*!
  \brief The graphics flags used as part of client creation.

  The flags are used to define the #graphics_flags member of the #MLGraphicsOptions as
  part of the definition to create the client.
*/
typedef enum MLGraphicsFlags {
  /*!
    \brief The client will not provide a depth buffer to the graphics system
    as part of rendering. Therefore the handle returned for all depth related
    resources from the graphics APIs will be invalid.
  */
  MLGraphicsFlags_NoDepth = 1 << 0,
  /*!
    \deprecated Deprecated since 0.18.0. The client must ensure correct order in its final output buffer.
    \brief The client is requesting a rendering convention such that the upper
    left hand corner of the screen is the default origin (rather than lower left).
  */
  MLGraphicsFlags_OriginUpperLeft = 1 << 1,
  /*!
    \brief Create a debug client with a debug configuration that provides extended logcat
    error logging. Useful to debug failures in API calls to the graphics system.
  */
  MLGraphicsFlags_DebugMode = 1 << 2,
  /*!
    \brief When #graphics_flags is set to default, the client configuration returned
    is the default configuration (color/depth, origin lower left, non-debug).
  */
  MLGraphicsFlags_Default = 0,
  /*! Ensure enum is represented as 32 bits. */
  MLGraphicsFlags_Ensure32Bits = 0x7FFFFFFF
} MLGraphicsFlags;

/*!
  \brief The definition of the supported color/depth surface formats.

  Formats that can be specified for the color_format and depth_format in
  #MLGraphicsOptions. For the best quality the recommended color format is
  #MLSurfaceFormat_RGBA8UNormSRGB, and for depth (when defined) either
  #MLSurfaceFormat_D32Float or #MLSurfaceFormat_D32FloatS8.
*/
typedef enum MLSurfaceFormat {
  /*! Invalid format for color/depth. */
  MLSurfaceFormat_Unknown = 0,
  /*! 32bit linear color format (RGBA8). */
  MLSurfaceFormat_RGBA8UNorm,
  /*!
    \brief Gamma 2.2 32bit sRGB color format (RGBA8). Clients requesting this format
    are required to encode the final color into this surface as defined by the
    IEC 61966-2-1 specification.
  */
  MLSurfaceFormat_RGBA8UNormSRGB,
  /*! 32bit linear color format (R10G10B10A2). */
  MLSurfaceFormat_RGB10A2UNorm,
  /*! 64bit linear color format (R16G16B16A16). */
  MLSurfaceFormat_RGBA16Float,
  /*! 32bit floating point depth buffer (D32F). */
  MLSurfaceFormat_D32Float,
  /*! 24bit linear depth format with 8bit stencil (D24S8). */
  MLSurfaceFormat_D24NormS8,
  /*! 32bit floating point depth format with 8bit stencil (D32FS8). */
  MLSurfaceFormat_D32FloatS8,
  /*! Ensure enum is represented as 32 bits. */
  MLSurfaceFormat_Ensure32Bits = 0x7FFFFFFF
} MLSurfaceFormat;

/*!
  \brief The graphics options provided for client creation.

  #MLGraphicsOptions defines the requested client configuration for
  creating GL/GLES or VK clients.
*/
typedef struct MLGraphicsOptions {
  /*! The combined graphics flags, as defined by #MLGraphicsFlags. */
  uint32_t graphics_flags;
  /*! The color surface format, defined in #MLSurfaceFormat. */
  MLSurfaceFormat color_format;
  /*! The depth surface format, defined in #MLSurfaceFormat. */
  MLSurfaceFormat depth_format;
} MLGraphicsOptions;

/*!
  \brief Frame timing hint specified at runtime to control the client cadence.

  Timing hint defines the requested client rate when interacting with
  the graphics system and is used as part of the blocking behavior specified by
  #MLGraphicsBeginFrame. The timing hit can be updated at runtime for a valid client
  at any time and will take effect on the next frame period for the client.

  The recommended setting for the timing hint is #MLGraphicsFrameTimingHint_60Hz.
*/
typedef enum MLGraphicsFrameTimingHint {
  /*! Default rate is unspecified, adjusted based on system conditions. */
  MLGraphicsFrameTimingHint_Unspecified = 0,
  /*! Run at the maximum rate allowed by the system. */
  MLGraphicsFrameTimingHint_Maximum,
  /*! Run at a specified rate of 60Hz (i.e. one frame every ~16.67 ms). */
  MLGraphicsFrameTimingHint_60Hz,
  /*! Run at a specified rate of 120Hz (i.e. one frame every ~8.33 ms). */
  MLGraphicsFrameTimingHint_120Hz,
  /*! Ensure enum is represented as 32 bits. */
  MLGraphicsFrameTimingHint_Ensure32Bits = 0x7FFFFFFF
} MLGraphicsFrameTimingHint;

/*!
  \brief The metadata name associated with each virtual camera.

  APIs within the graphics system will return information specific to
  a single virtual camera or both. The metadata name informs the client
  how to correlate the outputs of the API to a specific camera or both
  cameras.
*/
typedef enum MLGraphicsVirtualCameraName {
  /*! Name of the combined camera, the union of all virtual cameras. */
  MLGraphicsVirtualCameraName_Combined = -1,
  /*! Name of virtual camera for left display. */
  MLGraphicsVirtualCameraName_Left = 0,
  /*! Name of virtual camera for right display. */
  MLGraphicsVirtualCameraName_Right,
  /*! The maximum number of render virtual cameras. */
  MLGraphicsVirtualCameraName_Count,
  /*! Ensure enum is represented as 32 bits. */
  MLGraphicsVirtualCameraName_Ensure32Bits = 0x7FFFFFFF
} MLGraphicsVirtualCameraName;

/*! Definitions for Graphics system maximums. */
enum {
  /*! The number of buffered entries for each virtual camera. */
  MLGraphics_BufferCount = 3
};

/*!
  \brief Defines the requested projection matrix model for rendering.

  Provided as part of the #MLGraphicsFrameParams, defines the requested
  projection model for the intrinsic definition returned for each virtual
  camera from #MLGraphicsBeginFrame.

  The signed Z projection model is optimized for OpenGL use and transforms
  points into a left-hand, right-up-forward, Z range -1 (near) to 1 (far)
  projected coordinate space.

  The reverse infinite Z projection model maps the Z range from 1 (near) to
  0 (infinity).

  Vulkan clients should use the unsigned Z projection model to ensure that
  NDC points end up in Z range 0 (near) to 1 (far).
*/
typedef enum MLGraphicsProjectionType {
  /*! Projection model with signed Z. */
  MLGraphicsProjectionType_SignedZ = 0,
  /*! Projection model reversed infinite Z. */
  MLGraphicsProjectionType_ReversedInfiniteZ = 1,
  /*! Projection model with unsigned Z. */
  MLGraphicsProjectionType_UnsignedZ = 2,
  /*!
    \deprecated Deprecated since 0.18.0. Replaced by #MLGraphicsProjectionType_SignedZ.
    \brief Deprecated legacy projection model names.
  */
  MLGraphicsProjectionType_Default = MLGraphicsProjectionType_SignedZ,
  /*! Ensure enum is represented as 32 bits. */
  MLGraphicsProjectionType_Ensure32Bits = 0x7FFFFFFF
} MLGraphicsProjectionType;

/*!
  \deprecated Deprecated since 0.18.0. Scheduled for removal. Replaced by #MLGraphicsFrameParamsEx.
  \brief The frame parameters requested for the next rendered frame.

  The parameters defined by #MLGraphicsFrameParams are supplied to
  #MLGraphicsBeginFrame from the client to define the requested
  characteristics of the next rendered frame.

  Note : If a near clip is provided that is closer than the min_clip returned from
  #MLGraphicsRenderTargetsInfo the provided near clip will be clamped up to the
  min clip.
*/
typedef ML_DEPRECATED_MSG("Replaced by MLGraphicsFrameParamsEx.") struct MLGraphicsFrameParams {
  /*! Requested near clip plane distance in meters. */
  float near_clip;
  /*! Far clipping plane distance in meters. */
  float far_clip;
  /*!
    \brief Distance, in meters, to defined focus point for the client content. The focus
    distance should be calculated as the positive distance to the client determined
    object of interest (relative to the forward vector of the Lightwear).
  */
  float focus_distance;
  /*!
    \brief The scale factor for the render surfaces, valid range is 0.0->1.0. When
    supplying a scale factor < 1.0 the viewport returned from #MLGraphicsBeginFrame
    will be adjusted to the closest width/height match to the request that can
    be supported by the graphics system.
  */
  float surface_scale;
  /*!
    \brief Content for this frame is protected and should not be recorded or captured
    outside the graphics system.
  */
  bool protected_surface;
  /*! Requested projection model for rendering. */
  MLGraphicsProjectionType projection_type;
} MLGraphicsFrameParams;

/*!
  \brief The frame parameters requested for the next rendered frame.

  The parameters defined by #MLGraphicsFrameParamsEx are supplied to
  #MLGraphicsBeginFrameEx from the client to define the requested
  characteristics of the next rendered frame.

  This structure must be initialized by calling MLGraphicsFrameParamsExInit() before use.

  \apilevel 3
*/
typedef struct MLGraphicsFrameParamsEx {
  /*! Struct API version. */
  uint32_t version;
  /*! Requested near clip plane distance in meters. */
  float near_clip;
  /*! Far clipping plane distance in meters. */
  float far_clip;
  /*!
    \brief Distance, in meters, to defined focus point for the client content. The focus
    distance should be calculated as the positive distance to the client determined
    object of interest (relative to the forward vector of the Lightwear).
  */
  float focus_distance;
  /*!
    \brief Depth, in meters, for which to optimize content stabilization for time warp.
    Stabilization depth should be calculated as the positive depth of content to be
    most stable under time warp (relative to the forward vector of the Lightwear).

    This value should be set between focus_distance and far_clip. It usually provides a better
    experience not having content farther than this distance, so a value close to the far clip
    is recommended. The best value will depend on the actual depth distribution of content.
  */
  float stabilization_depth;
  /*!
    \brief The scale factor for the render surfaces, valid range is 0.0->1.0. When
    supplying a scale factor < 1.0 the viewport returned from #MLGraphicsBeginFrame
    will be adjusted to the closest width/height match to the request that can
    be supported by the graphics system.
  */
  float surface_scale;
  /*! Enable the platform level vignette for this frame. */
  bool vignette;
  /*!
    \brief Content for this frame is protected and should not be recorded or captured
    outside the graphics system.
  */
  bool protected_surface;
  /*! Requested projection model for rendering. */
  MLGraphicsProjectionType projection_type;
} MLGraphicsFrameParamsEx;

/*!
  \brief Initializes a MLGraphicsFrameParamsEx with the default parameters.
  \apilevel 3

  \param[out] frame_params The object to initialize with default settings.

  \priv None
*/
ML_STATIC_INLINE void MLGraphicsFrameParamsExInit(MLGraphicsFrameParamsEx *frame_params) {
  frame_params->version             = 2;
  frame_params->near_clip           = 0.5f;
  frame_params->far_clip            = 1000.0f;
  frame_params->focus_distance      = 1.0f;
  frame_params->stabilization_depth = 1000.0f;
  frame_params->surface_scale       = 1.0f;
  frame_params->projection_type     = MLGraphicsProjectionType_Default;
  frame_params->vignette            = false;
  frame_params->protected_surface   = false;
}

/*!
  \brief The static clip extents defined for all possible eye positions.

  Defines the conservative clip extents for the camera name that can be used for early
  update frustum culling in the client update path (prior to calling #MLGraphicsBeginFrame).

  Note: The client will need to obtain a headpose to combine with the provided matrices
  (transform and projection) to define the frustum. The general definition for camera from
  world (for the specified camera name) is MATprojection * inv(MATtransform) * inv(MATheadpose)
  for RHS multiplication.
*/
typedef struct MLGraphicsClipExtentsInfo {
  /*! The virtual camera meta data name. */
  MLGraphicsVirtualCameraName virtual_camera_name;
  /*!
    \brief The projection matrix, defined with infinite far clip plane distance. If the client
    is using a different projection model the matrix will need to be adjusted to match
    the client model prior to frustum culling.
  */
  MLMat4f projection;
  /*! The camera to world transform (i.e. local space relative to the head). */
  MLTransform transform;
} MLGraphicsClipExtentsInfo;

/*!
  \deprecated Deprecated since 0.20.0. Scheduled for removal. Replaced by MLGraphicsClipExtentsInfoArrayEx.

  \brief The clip extents array returned from #MLGraphicsGetClipExtents.

  The clip extents array provides the conservative clip extents per metadata
  camera as defined by #MLGraphicsClipExtentsInfo.
*/
typedef ML_DEPRECATED_MSG("Replaced by MLGraphicsClipExtentsInfoArrayEx.") struct MLGraphicsClipExtentsInfoArray {
  /*! The number of virtual cameras. */
  uint32_t num_virtual_cameras;
  /*!
    \brief The combined extents for all the virtual cameras, can be used for conservative frustum
    culling of all visible content defined by the union of what is visible from all of
    the defined virtual cameras.
  */
  MLGraphicsClipExtentsInfo full_extents;
  /*! The per virtual camera clip extents. */
  MLGraphicsClipExtentsInfo virtual_camera_extents[MLGraphicsVirtualCameraName_Count];
} MLGraphicsClipExtentsInfoArray;

/*!
  \brief The clip extents parameters

  The parameters defined by #MLGraphicsClipExtentsParams are supplied to
  MLGraphicsGetClipExtentsEx() from the client to define the requested
  characteristics of the clip extents.

  \apilevel 4
*/
typedef struct MLGraphicsClipExtentsParams {
  /*! Struct API version. */
  uint32_t version;
} MLGraphicsClipExtentsParams;

/*!
  \brief Initializes default values for MLGraphicsClipExtentsParams.
  \apilevel 4

  \param[in,out] inout_params The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLGraphicsClipExtentsParamsInit(MLGraphicsClipExtentsParams *inout_params) {
  inout_params->version = 1;
}

/*!
  \brief The clip extents array returned from MLGraphicsGetClipExtentsEx().

  The clip extents array provides the conservative clip extents per metadata
  camera as defined by #MLGraphicsClipExtentsInfo.

  This structure must be initialized by calling MLGraphicsClipExtentsInfoArrayExInit() before use.

  \apilevel 4
*/
typedef struct MLGraphicsClipExtentsInfoArrayEx {
  /*! Struct API version. */
  uint32_t version;
  /*! The number of virtual cameras. */
  uint32_t num_virtual_cameras;
  /*!
    \brief The combined extents for all the virtual cameras, can be used for conservative frustum
    culling of all visible content defined by the union of what is visible from all of
    the defined virtual cameras.
  */
  MLGraphicsClipExtentsInfo full_extents;
  /*! The per virtual camera clip extents. */
  MLGraphicsClipExtentsInfo virtual_camera_extents[MLGraphicsVirtualCameraName_Count];
} MLGraphicsClipExtentsInfoArrayEx;

/*!
  \brief Initializes default values for MLGraphicsClipExtentsInfoArrayEx.
  \apilevel 4

  \param[in,out] inout_params The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLGraphicsClipExtentsInfoArrayExInit(MLGraphicsClipExtentsInfoArrayEx *array) {
  array->version             = 1;
  array->num_virtual_cameras = 0;
}

/*!
  \brief Definition of a single render target defined in #MLGraphicsRenderBufferInfo.

  Each render target defines one of the buffered entries color/depth surfaces (with
  maximum count defined by #MLGraphics_BufferCount). Clients can assume that the handle
  provided here will be in the list of handles returned from #MLGraphicsBeginFrame.
*/
typedef struct MLGraphicsRenderTarget {
  /*! The width of the target. */
  uint32_t width;
  /*! The height of the target. */
  uint32_t height;
  /*! The handle (ID) of the 2D texture array, each layer represents a virtual camera. */
  MLHandle id;
  /*! The surface format of the target. */
  MLSurfaceFormat format;
} MLGraphicsRenderTarget;

/*!
  \brief Definition of a single buffer entry for all virtual cameras as defined
         in #MLGraphicsRenderTargetsInfo.

  Each buffer entry can contain a color / depth target and the target is valid
  if the handle != 0.

  Note : When the client specifies the #MLGraphicsFlags_NoDepth flag as part of
  client creation the depth targets will always be 0.
*/
typedef struct MLGraphicsRenderBufferInfo {
  /*! The color target for this entry. */
  MLGraphicsRenderTarget color;
  /*! The depth target for this entry. */
  MLGraphicsRenderTarget depth;
} MLGraphicsRenderBufferInfo;

/*!
  \brief The complete definition of all possible buffered entries along with associated metadata.

  Statically defined after successfully creating the client, the #MLGraphicsRenderTargetsInfo
  provides the definition of all of the buffered entries along with the global metadata. The
  client can assume that once queried the definition will not change and therefore does not need
  to be queried more than once at runtime.
 */
typedef struct MLGraphicsRenderTargetsInfo {
  /*! The enforced minimum clip distance for rendering content, in meters. */
  float min_clip;
  /*! The un-enforced, but recommended maximum clip distance for rendering content, in meters. */
  float max_clip;
  /*!
    \brief The total number of virtual camera for this platform,
    i.e (num_virtual_cameras <= #MLGraphicsVirtualCameraName_Count).
  */
  uint32_t num_virtual_cameras;
  /*! The buffer entries for all virtual cameras. */
  MLGraphicsRenderBufferInfo buffers[MLGraphics_BufferCount];
} MLGraphicsRenderTargetsInfo;

/*!
  \brief The per virtual camera information as defined in #MLGraphicsVirtualCameraInfoArray.

  For each virtual camera the client is provided the exact definition of the camera properties
  along with the matrices needed to construct the full transform chain. In general the
  transform chain for each virtual camera is defined as MATprojection * inv(MATtransform) for
  RHS multiplication.

  Note : The projection model is asymmetric and clients should not assume any of the
  symmetric properties of projection. Additionally both the transform and projection will
  change per frame and cannot be assumed to be static.
*/
typedef struct MLGraphicsVirtualCameraInfo {
  /*! The FOV left half angle, in radians. */
  float left_half_angle;
  /*! The FOV right half angle, in radians. */
  float right_half_angle;
  /*! The FOV top half angle, in radians. */
  float top_half_angle;
  /*! The FOV bottom half angle, in radians. */
  float bottom_half_angle;
  /*!
    \brief The handle of the sync object to signal after submitting all GPU work for the
    virtual camera.
  */
  MLHandle sync_object;
  /*! The projection matrix for the virtual camera. */
  MLMat4f projection;
  /*!
    \brief The local space (relative to world) camera transform needed to render content,
    inverse is view matrix.
  */
  MLTransform transform;
  /*! The name of the virtual camera. */
  MLGraphicsVirtualCameraName virtual_camera_name;
} MLGraphicsVirtualCameraInfo;

/*!
  \deprecated Deprecated since 0.22.0. Scheduled for removal. Replaced by MLGraphicsFrameInfo.
  \brief The virtual camera info array populated by MLGraphicsBeginFrame().

  The info array defines the single color and depth (if #MLGraphicsFlags_NoDepth is
  not specified) handles along with all the required metadata.
*/
typedef ML_DEPRECATED_MSG("Replaced by MLGraphicsFrameInfo.") struct MLGraphicsVirtualCameraInfoArray {
  /*! The number of virtual cameras for the frame, each virtual camera is a layer. */
  uint32_t num_virtual_cameras;
  /*! The handle (ID) of the color surface for the virtual cameras. */
  MLHandle color_id;
  /*! The handle (ID) of the depth surface for the virtual cameras. */
  MLHandle depth_id;
  /*! The common viewport definition for all virtual cameras. */
  MLRectf viewport;
  /*! The array of virtual cameras to render to for the frame. */
  MLGraphicsVirtualCameraInfo virtual_cameras[MLGraphicsVirtualCameraName_Count];
} MLGraphicsVirtualCameraInfoArray;

/*!
  \brief Information about the current render frame populated by MLGraphicsBeginFrameEx().
  \This struct must be populated by calling MLGraphicsFrameInfoInit() before being used in
  \a call to MLGraphicsBeginFrameEx().
  \apilevel 7
*/
typedef struct MLGraphicsFrameInfo {
  /*! Struct API version. */
  uint32_t version;
  /*! A reference handle for this frame to be provided at end of frame. */
  MLHandle handle;
  /*!
    \brief The handle (ID) of the texture array color surface for the virtual cameras.
    There is one texture layer per virtual camera.
  */
  MLHandle color_id;
  /*!
    \brief The handle (ID) of the texture array depth surface for the virtual cameras.
    There is one texture layer per virtual camera.
  */
  MLHandle depth_id;
  /*! The common viewport definition for all virtual cameras. */
  MLRectf viewport;
  /*!
    \brief A handle to a synchronization object that must be cast to VkSemaphore
    and used during queue submission of a Vulkan app. GL apps can ignore this member
  */
  MLHandle wait_sync_object;
  /*! The number of virtual cameras for the frame, each virtual camera is a layer. */
  uint32_t num_virtual_cameras;
  /*! The array of virtual cameras to render to for the frame. */
  MLGraphicsVirtualCameraInfo virtual_cameras[MLGraphicsVirtualCameraName_Count];
} MLGraphicsFrameInfo;

/*!
  \brief Initializes default values for MLGraphicsFrameInfo.
  \apilevel 7

  \param[in,out] inout_params The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLGraphicsFrameInfoInit(MLGraphicsFrameInfo *frame_info) {
  if (NULL != frame_info) {
    memset(frame_info, 0, sizeof(MLGraphicsFrameInfo));
    frame_info->version = 2;
    frame_info->handle  = (MLHandle)ML_INVALID_HANDLE;
  }
}

/*!
  \brief Performance timing information for the graphics client.

  Populated by #MLGraphicsGetClientPerformanceInfo, the performance information
  contains all metrics tracked by the graphics system for the specified client.
  Metrics are calculated using a sliding window and can be used to adjust the
  client workload characteristics (i.e. scale, fidelity, etc).

  Note : The internal metrics are also provided, informing the client of both the
  CPU and GPU cost of the internal operations of the graphics system (in the client
  process).
*/
typedef struct MLGraphicsClientPerformanceInfo {
  /*! The average frame start CPU to composition acquisition of the frame, in nanoseconds. */
  uint64_t frame_start_cpu_comp_acquire_cpu_ns;
  /*! The average frame start CPU to frame end GPU, in nanoseconds. */
  uint64_t frame_start_cpu_frame_end_gpu_ns;
  /*! The average frame cadence, CPU start to CPU start, in nanoseconds. */
  uint64_t frame_start_cpu_frame_start_cpu_ns;
  /*! The average frame CPU duration, in nanoseconds. */
  uint64_t frame_duration_cpu_ns;
  /*! The average frame GPU duration, in nanoseconds. */
  uint64_t frame_duration_gpu_ns;
  /*! The average CPU duration for internal operations per frame, in nanoseconds. */
  uint64_t frame_internal_duration_cpu_ns;
  /*! The average GPU duration for internal operations per frame, in nanoseconds. */
  uint64_t frame_internal_duration_gpu_ns;
} MLGraphicsClientPerformanceInfo;

/*! Default values suitable for use with MLGraphicsEnableBlobCacheGL(). */
enum {
  MLGraphicsBlobCache_DefaultMaxEntryBytes = 512 * 1024,
  MLGraphicsBlobCache_DefaultMaxTotalBytes = 8 * 1024 * 1024,
};

/*!
  \brief Enable shader blob cache for OpenGL.

  OpenGL can cache & reuse shaders and other driver artifacts using a blob cache,
  speeding up loading & processing of shaders significantly on subseqent occurrences.
  This behavior persists across runs of the app as well, speeding up load times.
  This function is used to enable the blob caching using the specified file. The
  file path must be writeable by the application. The file will be created if it
  doesn't already exist.

  \param[in] filename Absolute path to a file to use for caching. Location must be writeable by application.
  \param[in] max_entry_bytes Reserved for future use. Maximum size for a single blob entry. Currently defaults to 512KB.
  \param[in] max_total_bytes Reserved for future use. Maximum total file size for the blob. Currently defaults to 8MB.

  \retval MLResult_InvalidParam Filename parameter null, malformed, or inaccessible to the application.
  \retval MLResult_Ok Blob cache enabled successfully.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsEnableBlobCacheGL(const char *filename, uint32_t max_entry_bytes, uint32_t max_total_bytes);

/*!
  \brief Create the GL/GLES graphics client.

  Create a graphics client for use with GL/GLES APIs using the provided options and
  specified opengl context. It is required that the opengl context specified here is
  only bound on the thread used to render the client frame and is the same context
  used to render the frame using the provided information from #MLGraphicsBeginFrame.

  \param[in] options The graphics options for resource creation.
  \param[in] opengl_context The OpenGL context to use for resource creation.
  \param[out] out_graphics_client The handle to the created graphics client. Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Graphics Client was not created due to an invalid parameter.
  \retval MLResult_Ok Graphics Client was created successfully.
  \retval MLResult_UnspecifiedFailure Graphics Client was not created due to an unknown error.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsCreateClientGL(const MLGraphicsOptions *options, MLHandle opengl_context, MLHandle *out_graphics_client);

/*!
  \brief Convert MLSurfaceFormat to OpenGL internal format.

  \param[in] format The MLSurfaceFormat to convert.
  \return The returned internal format.

  \priv None
*/
ML_API uint32_t ML_CALL MLGraphicsGLFormatFromMLSurfaceFormat(MLSurfaceFormat format);

/*!
  \brief Convert OpenGL internal format to MLSurfaceFormat.

  \param[in] format The internal format to convert.
  \return The returned MLSurfaceFormat.

  \priv None
*/
ML_API MLSurfaceFormat ML_CALL MLGraphicsMLSurfaceFormatFromGLFormat(uint32_t format);

#ifdef VK_VERSION_1_0
/*!
  \brief Create the VK graphics client.

  Create a graphics client for use with VK API using the provided options and specified
  physical and logical device. The color and depth surfaces are created internally, then exported
  to the application context. The initial layout of these surfaces is VK_IMAGE_LAYOUT_UNDEFINED.
  It is the application's responsibility to perform a layout transition of the surfaces before usage.

  \param[in] options The graphics options for resource creation.
  \param[in] vulkan_instance The Vulkan instance used to create the physical and logical device.
  \param[in] vulkan_physical_device The Vulkan physical device used to create the specified logical device.
  \param[in] vulkan_logical_device The Vulkan logical device to use for resource creation.
  \param[out] out_graphics_client The handle to the created graphics client. Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Graphics Client was not created due to an invalid parameter.
  \retval MLResult_Ok Graphics Client was created successfully.
  \retval MLResult_UnspecifiedFailure Graphics Client was not created due to an unknown error.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsCreateClientVk(const MLGraphicsOptions *options, const VkInstance vulkan_instance, const VkPhysicalDevice vulkan_physical_device, const VkDevice vulkan_logical_device, MLHandle *out_graphics_client);

/*!
  \brief Convert MLSurfaceFormat to VkFormat.

  \param[in] format The MLSurfaceFormat to convert.
  \return The returned VkFormat.

  \priv None
*/
ML_API VkFormat ML_CALL MLGraphicsVkFormatFromMLSurfaceFormat(const MLSurfaceFormat format);

/*!
  \brief Convert VkFormat to MLSurfaceFormat.

  \param[in] format The VkFormat to convert.
  \return The returned MLSurfaceFormat.

  \priv None
*/
ML_API MLSurfaceFormat ML_CALL MLGraphicsMLSurfaceFormatFromVkFormat(const VkFormat format);
#endif

/*!
  \brief Destroy a GL/GLES or VK client.

  Destroying the client will invalidate all resources associated with the client handle
  and invalidate the provided handle.

  \param[in,out] graphics_client The graphics client to destroy.

  \retval MLResult_InvalidParam Graphics Client was not destroyed due to an invalid parameter.
  \retval MLResult_Ok Graphics Client was destroyed successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsDestroyClient(MLHandle *graphics_client);

/*!
  \brief Set the frame timing hint, can be set at runtime and is updated on the
         next call to #MLGraphicsBeginFrame.

  \param[in] graphics_client The graphics client to modify.
  \param[in] hint The graphics frame timing hint.

  \retval MLResult_InvalidParam Timing Hint was not changed due to an invalid parameter.
  \retval MLResult_Ok Timing Hint was changed successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsSetFrameTimingHint(MLHandle graphics_client, MLGraphicsFrameTimingHint hint);

/*!
  \deprecated Deprecated since 0.18.0. Scheduled for removal.

  \brief Initializes a MLGraphicsFrameParams with the default parameters.

  \param[in] data The object to initialize with default settings.

  \retval MLResult_InvalidParam Frame Parameters were not initialized due to an invalid parameter.
  \retval MLResult_Ok Frame Parameters were initialized successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLGraphicsInitFrameParams(MLGraphicsFrameParams *data);

/*!
  \deprecated Deprecated since 0.18.0. Scheduled for removal. Replaced by MLGraphicsBeginFrameEx().
  \brief Request to begin rendering a new frame in the graphics system.

  Called to request the start of a new render frame and should be called as close as possible to
  the start of rendering to minimize the motion to photon latency. Only one frame can be rendered
  at a time and must be submitted by providing the returned handle to #MLGraphicsEndFrame prior to
  another call to #MLGraphicsBeginFrame. The call to begin/end frame should be made on the render
  thread alone of the client.

  Note : Calls to #MLGraphicsBeginFrame can and will block the calling process. There are several reasons
  that lead to a block, including, the client is executing faster than the defined timing requested (and the
  block re-aligns the client timing), the client is not yet visible in the system, and the compositor has
  not yet moved to the next buffered entry (i.e. wraparound cases from fast client rendering). The call
  to #MLGraphicsBeginFrame will timeout the block and return when the conditions to unblock have not
  been satisfied for the internal timeout period. The most common occurrence of the timeout is when the
  application is not currently visible.

  \param[in] graphics_client The graphics client to query.
  \param[in] params The frame parameters for the frame.
  \param[out] out_handle A reference handle for this frame to be provided at end of frame. Only valid if result is MLResult_Ok.
  \param[out] out_array The virtual camera info array with all information required for rendering to each virtual camera.
              Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Frame was not started due to an invalid parameter.
  \retval MLResult_Ok Frame was started successfully.
  \retval MLResult_Timeout Blocking timeout limit reached.
  \retval MLResult_UnspecifiedFailure Frame was not started due to an unknown error.

  \priv None
*/
ML_DEPRECATED_MSG("Replaced by MLGraphicsBeginFrameEx.")
ML_API MLResult ML_CALL MLGraphicsBeginFrame(MLHandle graphics_client, const MLGraphicsFrameParams *params, MLHandle *out_handle, MLGraphicsVirtualCameraInfoArray *out_array);

/*!
  \brief Request to begin rendering a new frame in the graphics system.

  Called to request the start of a new render frame and should be called as close as possible to
  the start of rendering to minimize the motion to photon latency. Only one frame can be rendered
  at a time and must be submitted by providing the returned handle to #MLGraphicsEndFrame prior to
  another call to #MLGraphicsBeginFrameEx. The call to begin/end frame should be made on the render
  thread alone of the client. It is required that a successful call to #MLGraphicsFrameInfoInit
  be made before calling #MLGraphicsBeginFrameEx. Faiure to call #MLGraphicsFrameInfoInit before
  calling #MLGraphicsBeginFrameEx will return a result of MLResult_NotImplemented.

  Note : Calls to #MLGraphicsBeginFrameEx can and will block the calling process. There are several reasons
  that lead to a block, including, the client is executing faster than the defined timing requested (and the
  block re-aligns the client timing), the client is not yet visible in the system, and the compositor has
  not yet moved to the next buffered entry (i.e. wraparound cases from fast client rendering). The call
  to #MLGraphicsBeginFrameEx will timeout the block and return when the conditions to unblock have not
  been satisfied for the internal timeout period. The most common occurrence of the timeout is when the
  application is not currently visible.

  \apilevel 2

  \param[in] graphics_client The graphics client to query.
  \param[in] params The frame parameters for the frame.
  \param[out] out_frame_info Information returned for use when rendering the frame. Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Frame was not started due to an invalid parameter.
  \retval MLResult_Ok Frame was started successfully.
  \retval MLResult_Timeout Blocking timeout limit reached.
  \retval MLResult_UnspecifiedFailure Frame was not started due to an unknown error.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsBeginFrameEx(MLHandle graphics_client, const MLGraphicsFrameParamsEx *params, MLGraphicsFrameInfo *out_frame_info);

/*!
  \brief Signal the sync object provided for each virtual camera for GL/GLES clients.

  Required for each frame, after rendering each virtual camera (or multiple if stereo rendering)
  to signal the sync object in order to inform the graphics system that the rendering for the
  camera(s) has completed. All sync objects can be signaled after rendering all cameras, but
  results in lower fidelity in the graphics system.

  Note: Vulkan clients are required to cast the sync object to VkSemaphore and add it to
  the signal semaphore list when submitting the last command buffer for the virtual camera(s).

  \param[in] graphics_client The graphics client owning the sync object.
  \param[in] sync_object The sync object to be signaled.

  \retval MLResult_InvalidParam Sync Object was not signaled due to an invalid parameter.
  \retval MLResult_Ok Sync Object was signaled successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsSignalSyncObjectGL(MLHandle graphics_client, MLHandle sync_object);

/*!
  \deprecated Deprecated since 0.20.0. Scheduled for removal. Replaced by MLGraphicsGetClipExtentsEx().

  \brief Get the conservative clip extents information for each virtual camera.

  Clients can assume the definition is unchanging and therefore only need to call once
  after client initialization.

  \param[in] graphics_client The graphics client to query.
  \param[out] out_array The clip extents info array with all information required for update of each virtual camera.
              Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Clip Extents not obtained due to an invalid parameter.
  \retval MLResult_Ok Clip Extents obtained successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_DEPRECATED_MSG("Replaced by MLGraphicsGetClipExtentsEx.")
ML_API MLResult ML_CALL MLGraphicsGetClipExtents(MLHandle graphics_client, MLGraphicsClipExtentsInfoArray *out_array);

/*!
  \attention EXPERIMENTAL

  \brief Get the conservative clip extents information for each virtual camera.

  Clients should call this function once a frame to get the most accurate clip extents.

  \apilevel 4

  \param[in] graphics_client The graphics client to query.
  \param[in] params The clip extents parameters.
  \param[out] out_array The clip extents info array with all information required for update of each virtual camera.
              Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Clip Extents not obtained due to an invalid parameter.
  \retval MLResult_Ok Clip Extents obtained successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsGetClipExtentsEx(MLHandle graphics_client, const MLGraphicsClipExtentsParams *params, MLGraphicsClipExtentsInfoArrayEx *out_array);

/*!
  \brief Get a list of all buffered entries.

  Called after client initialization to obtain the definition of all buffered entries
  along with the associated metadata. Like #MLGraphicsGetClipExtents can be assumed that
  the output is unchanging for the lifetime of the client.

  \param[in] graphics_client The graphics client to query.
  \param[out] out_targets The render target info array. Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Render Targets not obtained due to an invalid parameter.
  \retval MLResult_Ok Render Targets obtained successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsGetRenderTargets(MLHandle graphics_client, MLGraphicsRenderTargetsInfo *out_targets);

/*!
  \brief Submit a completed frame to the graphics system.

  Submit a completed frame to the graphics system using the handle provided from
  #MLGraphicsBeginFrame. Prior to submiting the frame the client must signal the sync objects
  provided by #MLGraphicsBeginFrame, for GL/GLES clients with #MLGraphicsSignalSyncObjectGL or
  for VK clients including that sync object as part of the signal semaphore list. For VK clients,
  the color and depth surfaces can be in any image layout when calling #MLGraphicsEndFrame.

  \param[in] graphics_client The graphics client to update.
  \param[in] handle The handle provided from the call to MLGraphicsBeginFrame.

  \retval MLResult_InvalidParam Frame not ended due to an invalid parameter.
  \retval MLResult_Ok Frame ended successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsEndFrame(MLHandle graphics_client, MLHandle handle);

/*!
  \brief Obtain latest performance metrics for the client.

  Can be called any time after the client has begun rendering frames to obtain the internally tracked
  performance metrics for the client and graphics system. Updated internally once per completed frame.

  \param[in] graphics_client The graphics client in which to query the performance.
  \param[out] out_info Current performance information for the client. Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Client Performance Info not obtained due to an invalid parameter.
  \retval MLResult_Ok Render Client Performance Info obtained successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsGetClientPerformanceInfo(MLHandle graphics_client, MLGraphicsClientPerformanceInfo *out_info);

/*! \} */

ML_EXTERN_C_END
