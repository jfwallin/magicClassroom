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
  \defgroup ImageTracker Image Tracker
  \addtogroup ImageTracker
  \sharedobject ml_perception_client
  \brief APIs for the Image Tracker system.

  - The Image Tracker enables experiences that recognize 2D planar images
    (i.e. image targets) in the physical world and render 3D virtual content that
    is registered to the images in the real world.
  \{
*/

/*!
  \brief Represent the list of Image Tracker settings.

  This will be used to initialize the Image Tracker system.
  MLImageTrackerInitSettings() can be used to get the default values that will
  be used by the Image Tracker system.
*/
typedef struct MLImageTrackerSettings {
  /*!
    \brief Maximum number of targets that need to be actively tracked at one time.

    Image Tracker looks for new images to track in the environment only if it
    is not already tracking the maximum number of simultaneous targets.
    This helps reduce load on the CPU.

    For example, if you are only interested in recognizing and tracking two
    targets from your image set at any one time, set this parameter to 2.
    Higher values will work, but Image Tracker would waste CPU resources
    needlessly looking for and tracking images that are not going to be used.

    The valid range for this parameter is 1 to 25.
  */
  uint32_t max_simultaneous_targets;

  /*!
    \brief If \c true, image tracker will detect and track targets.

    Image Tracker should be disabled when app is paused and enabled when app
    resumes.

    When enabled Image Tracker will gain access to the camera and start
    tracking images. Enabling image tracker is expensive, takes about 1500ms
    on the average.

    When disabled Image Tracker will release the camera and stop tracking
    images. Internal state of the tracker will be maintained (i.e. list of
    active/inactive targets and their target_handles).
  */
  bool enable_image_tracking;
} MLImageTrackerSettings;

/*!
  \brief Identifies the status of an Image Target.

  Each #MLImageTrackerTargetResult will include a #MLImageTrackerTargetStatus
  giving the current status of the target.
*/
typedef enum MLImageTrackerTargetStatus {
  /*!
    \brief Image Target is tracked.

    The Image Tracker system will provide a 6 DOF pose when queried using
    MLSnapshotGetTransform() function.
  */
  MLImageTrackerTargetStatus_Tracked,
  /*!
    \brief Image Target is tracked with low confidence.

    The Image Tracker system will still provide a 6 DOF pose. But this
    pose might be inaccurate and might have jitter. When the tracking is
    unreliable one of the following two events will happen quickly : Either the
    tracking will recover to #MLImageTrackerTargetStatus_Tracked or tracking
    will be lost and the status will change to #MLImageTrackerTargetStatus_NotTracked.
  */
  MLImageTrackerTargetStatus_Unreliable,
  /*!
    \brief Image Target is lost.

    The Image Tracker system will not report any pose for this target. Querying
    for the pose using MLSnapshotGetTransform() will return \c false.
  */
  MLImageTrackerTargetStatus_NotTracked,
  /*! Ensure enum is represented as 32 bits. */
  MLImageTrackerTargetStatus_Ensure32Bits = 0x7FFFFFFF
} MLImageTrackerTargetStatus;

/*!
  \brief Identifies the Image format.

  When adding image targets to the Image Tracker using
  MLImageTrackerAddTargetFromArray() specify the image format using this enum.
  For a pixel at (x, y) the channel c data of that pixel is at the position
  given by [(width * y + x)*channels + c].
*/
typedef enum MLImageTrackerImageFormat {
  /*! Grayscale, range: [0, 255]. */
  MLImageTrackerImageFormat_Grayscale,
  /*! RGB, range: [0, 255]. */
  MLImageTrackerImageFormat_RGB,
  /*! RGBA, range: [0, 255]. */
  MLImageTrackerImageFormat_RGBA,
  /*! Ensure enum is represented as 32 bits. */
  MLImageTrackerImageFormat_Ensure32Bits = 0x7FFFFFFF
} MLImageTrackerImageFormat;

/*!
  \brief Represents the settings of a Image Target.

  All fields are required for an Image Target to be tracked.
*/
typedef struct MLImageTrackerTargetSettings {
  /*!
    \brief Name of the target.

    This name has to be unique across all targets added to the Image Tracker.
    Caller should allocate memory for this.
    Encoding should be in UTF8.
    This will be copied internally.
  */
  const char *name;
  /*!
    \brief longer_dimension Size of longer side of the Image Target in scene units.

    The scene units here refers to the units used in the game engine.
    This value does not affect the performance of the detection or tracking.
  */
  float longer_dimension;
  /*!
    \brief Set this to true to improve detection for stationary targets.

    An Image Target is a stationary target if its position in the physical
    world does not change.

    This is best suited for cases where the target is stationary and when the
    local geometry (environment surrounding the target) is planar.

    When in doubt set this to false.
  */
  bool is_stationary;

  /*!
    \brief Set this to \c true to track the image target.

    Disabling the target when not needed marginally improves the tracker CPU
    performance.

    This is best suited for cases where the target is temporarily not needed.
    If the target no longer needs to be tracked it is best to use
    MLImageTrackerRemoveTarget().
  */
  bool is_enabled;
} MLImageTrackerTargetSettings;

/*!
  \brief Represents the static information about an Image Target.

  Populate this structure with MLImageTrackerGetTargetStaticData().
*/
typedef struct MLImageTrackerTargetStaticData {
  /*!
    \brief  #MLCoordinateFrameUID of the target.

    This should be passed to the MLSnapshotGetTransform() function to get
    the 6 DOF pose of the target.

    First call MLImageTrackerGetTargetResult() to check to see if the target is
    being tracked. If it is not being tracked then the call to
    MLSnapshotGetTransform() will return \c false.
  */
  MLCoordinateFrameUID coord_frame_target;
} MLImageTrackerTargetStaticData;

/*!
  \brief Represents an Image Target result.

  The result for every Image Target added to the Image Tracker can be queried
  using MLImageTrackerGetTargetResult().
*/
typedef struct MLImageTrackerTargetResult {
  /*!
    \brief Status of the target.

    Every target will have an associated status indicating the current tracking status.
  */
  MLImageTrackerTargetStatus status;
} MLImageTrackerTargetResult;

/*!
  \brief Create the Image Tracker.

  This can be called only after starting the Perception system using
  MLPerceptionStartup() and HeadTracker using MLHeadTrackingCreate().

  Pass a nullptr for \p tracker_settings to use the default settings. Override
  the default settings by passing a #MLImageTrackerSettings struct as input.

  This function should be called only once. Do not create multiple Image Trackers.
  This function is time consuming and will block the calling thread. Current
  estimate for the execution time is 500ms - 6000ms.

  On an average cold start takes about 6000ms, otherwise it should be
  about 1500ms.

  \param[in] tracker_settings List of Image Tracker settings.
  \param[out] out_handle #MLHandle to the created Image Tracker. Only valid if the return
              value is MLResult_Ok.

  \retval MLResult_InvalidParam Image tracker was not created due to invalid out_handle.
  \retval MLResult_Ok Image tracker was created successfully.
  \retval MLResult_PrivilegeDenied Image tracker was not created due to lack of privilege(s).
  \retval MLResult_UnspecifiedFailure Image tracker was not created due to an internal error.

  \priv CameraCapture, LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLImageTrackerCreate(const MLImageTrackerSettings *tracker_settings, MLHandle *out_handle);

/*!
  \brief Update the Image Tracker with new settings.

  \param[in] tracker_handle MLHandle to the Image Tracker created by MLImageTrackerCreate().
  \param[in] tracker_settings List of Image Tracker settings.

  \retval MLResult_InvalidParam Failed to update the settings due to invalid tracker_settings.
  \retval MLResult_Ok Successfully updated the Image Tracker settings.
  \retval MLResult_PrivilegeDenied Failed to update the settings due to lack of privilege(s).
  \retval MLResult_UnspecifiedFailure Failed to update the settings due to an internal error.

  \priv CameraCapture, LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLImageTrackerUpdateSettings(MLHandle tracker_handle, const MLImageTrackerSettings *tracker_settings);

/*!
  \brief Destroy the Image Tracker.

  \param[in] tracker_handle #MLHandle to the Image Tracker created by MLImageTrackerCreate().

  \retval MLResult_Ok Successfully destroyed the Image Tracker.
  \retval MLResult_PrivilegeDenied  Failed to destroy the tracker due to lack of privilege(s).
  \retval MLResult_UnspecifiedFailure Failed to destroy the tracker due to an internal error.

  \priv CameraCapture, LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLImageTrackerDestroy(MLHandle tracker_handle);

/*!
  \brief Sets the values of an #MLImageTrackerSettings structure to defaults.

  Call this function to get the default #MLImageTrackerSettings used by
  the Image Tracker system.

  \param[out] out_settings Pointer to #MLImageTrackerSettings structure that will be initialized.

  \retval MLResult_InvalidParam Failed to initialise the settings due to invalid out_settings.
  \retval MLResult_Ok Successfully initialised out_settings with the default values.
  \retval MLResult_UnspecifiedFailure Failed to initialise the settings due to an internal error.

  \priv None
*/
ML_API MLResult ML_CALL MLImageTrackerInitSettings(MLImageTrackerSettings *out_settings);

/*!
  \brief Add an Image Target from an image file.

  "Create" and "Add" a new target to the Image Tracker from an image file.
  Supported image formats : jpg.
  Image should not be larger than 2MB.
  Image Tracker will start searching for this target once created.

  This function is time consuming and will block the calling thread. Time taken
  depends on the size and quality of the image.

  \param[in] tracker_handle #MLHandle to the Image Tracker created by MLImageTrackerCreate().
  \param[in] target_settings List of settings to be used for the new target.
  \param[in] path The Unix-style path of the file.
  \param[out] out_handle A target_handle to the created Image Target. Only valid if the
              return value is MLResult_Ok.

  \retval MLResult_Ok Successfully added the target to the Image Tracker.
  \retval MLResult_UnspecifiedFailure Failed to add the target due to invalid argument(s).
  \retval MLResult_UnspecifiedFailure Failed to add the target due to an internal error.

  \priv       None
*/
ML_API MLResult ML_CALL MLImageTrackerAddTargetFromImageFile(MLHandle tracker_handle, const MLImageTrackerTargetSettings *target_settings, const char *path, MLHandle *out_handle);

/*!
  \brief Add a target from an array.

  "Create" & "Add" a new target to the image tracker from an array.
  The data should be in the form of a pointer to unsigned char array.

  Supported formats: Grayscale, RGB, & RGBA, should be specified using the
  #MLImageTrackerImageFormat enum.

  For a pixel at (x,y) the channel c data of that pixel is at the position
  given by [(width * y + x)*channels + c].

  The calling function owns the pointer, image_data will be copied internally.
  Image Tracker will start searching for this target once created.

  This function is time consuming and will block the calling thread. Time taken
  depends on the size and quality of the image.

  \param[in] tracker_handle #MLHandle to the Image Tracker created by MLImageTrackerCreate().
  \param[in] target_settings List of settings to be used for the new target.
  \param[in] image_data Pointer to the array of pixel data.
  \param[in] width Width of the image.
  \param[in] height Height of the image.
  \param[in] format Specify the image format using #MLImageTrackerImageFormat.
  \param[out] out_handle A target_handle to the created Image Target. Only valid if the
              return value is MLResult_Ok.

  \retval MLResult_Ok Successfully added the target to the Image Tracker.
  \retval MLResult_UnspecifiedFailure Failed to add the target due to invalid argument(s).
  \retval MLResult_UnspecifiedFailure Failed to add the target due to an internal error.

  \priv None
*/
ML_API MLResult ML_CALL MLImageTrackerAddTargetFromArray(MLHandle tracker_handle, const MLImageTrackerTargetSettings *target_settings, const unsigned char *image_data, uint32_t width, uint32_t height, MLImageTrackerImageFormat format, MLHandle *out_handle);

/*!
  \brief Remove an Image Target from the Image Tracker.

  "Remove" and "destroy" a previously added Image Target from the Image Tracker.
  Image Tracker will stop searching for this Image Target once its destroyed.

  \param[in] tracker_handle #MLHandle to a Image Tracker created by MLImageTrackerCreate().
  \param[in] target_handle #MLHandle to a Image Target that needs to be removed.

  \retval MLResult_Ok Successfully removed the target from the Image Tracker.
  \retval MLResult_UnspecifiedFailure Failed to remove the target due to an internal error.

  \priv None
*/
ML_API MLResult ML_CALL MLImageTrackerRemoveTarget(MLHandle tracker_handle, MLHandle target_handle);

/*!
  \brief Update the settings of an Image Target.

  Updates the settings of an Image Target that is already added to the Image Tracker system.

  \param[in] tracker_handle #MLHandle to a Image Tracker created by MLImageTrackerCreate().
  \param[in] target_handle #MLHandle to a Image Target whose settings needs to be updated.
  \param[in] target_settings List of the updated settings to be used for the new target.

  \retval MLResult_InvalidParam Failed to update the target setting due to invalid target_settings.
  \retval MLResult_Ok Successfully updated the Target settings.
  \retval MLResult_UnspecifiedFailure Failed to update the target setting due to an internal error.

  \priv None
*/
ML_API MLResult ML_CALL MLImageTrackerUpdateTargetSettings(MLHandle tracker_handle, MLHandle target_handle, const MLImageTrackerTargetSettings *target_settings);

/*!
  \brief Get the static data for an Image Target in the Image Tracker.

  Use the #MLCoordinateFrameUID included in \p out_data to get the
  6DOF pose from the MLSnapshotGetTransform() function.

  First call MLImageTrackerGetTargetResult() to check to see if the target is
  being tracked. If it is not being tracked then the call to MLSnapshotGetTransform() will return \c false.

  \param[in] tracker_handle #MLHandle to an Image Tracker created by MLImageTrackerCreate().
  \param[in] target_handle #MLHandle to an Image Target whose static data needs to be retrieved.
  \param[out] out_data Pointer to #MLImageTrackerTargetStaticData which contains the static data.

  \retval MLResult_InvalidParam Failed to fetch the static data due to invalid out_data.
  \retval MLResult_Ok Successfully fetched the static data for specified target.
  \retval MLResult_UnspecifiedFailure Failed to fetch the static data due to an internal error.

  \priv None
*/
ML_API MLResult ML_CALL MLImageTrackerGetTargetStaticData(MLHandle tracker_handle, MLHandle target_handle, MLImageTrackerTargetStaticData *out_data);

/*!
  \brief Get the result for a Image Target from the Image Tracker.

  This function should always be called after MLPerceptionGetSnapshot().
  The result returned is from the result cached during the last call to MLPerceptionGetSnapshot().
  Query for the pose only when #MLImageTrackerTargetResult.status != #MLImageTrackerTargetStatus_NotTracked.

  \param[in] tracker_handle #MLHandle to a Image Tracker created by MLImageTrackerCreate().
  \param[in] target_handle #MLHandle to a Image Target whose result needs to be retrieved.
  \param[out] out_data Pointer to #MLImageTrackerTargetResult which contains the results.

  \retval MLResult_InvalidParam Failed to fetch the target result due to invalid out_data.
  \retval MLResult_Ok Successfully fetched the target result for specified target.
  \retval MLResult_UnspecifiedFailure Failed to fetch the target result due to an internal error.

  \priv None
*/
ML_API MLResult ML_CALL MLImageTrackerGetTargetResult(MLHandle tracker_handle, MLHandle target_handle, MLImageTrackerTargetResult *out_data);

/*! \} */

ML_EXTERN_C_END
