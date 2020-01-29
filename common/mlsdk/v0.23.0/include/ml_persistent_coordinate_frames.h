// %BANNER BEGIN%
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
#include "ml_coordinate_frame_uid.h"
#include "ml_passable_world.h"

ML_EXTERN_C_BEGIN

/*!
  \defgroup PersistentCoordinateFrames Persistent Coordinate Frames
  \addtogroup PersistentCoordinateFrames
  \sharedobject ml_perception_client
  \brief APIs for the Persistent Coordinate Frame Tracker system.

  - Make content persist at locations in the real world.

  \attention EXPERIMENTAL

  \{
*/

/*!
  \brief This represents the state of a Persistent Coordinate Frame.

  It is exposed as a spherical region. That’s because the translation / rotation quality of
  objects attached to a Persistent Coordinate Frame varies greatly to how far away that object
  is to the Persistent Coordinate Frame itself (as well as the distance from the user to that
  Persistent Coordinate Frame).

  \apilevel 7

*/
typedef struct MLPersistentCoordinateFramesFrameState {
  uint32_t version;
  /*!
    \brief A confidence value (from [0, 1]) representing the confidence in the Persistent Coordinate Frame
    error within the valid radius.
  */
  float confidence;
  /*! The radius (in meters) within which the confidence is valid. */
  float valid_radius_m;
  /*! The rotational error (in degrees). */
  float rotation_err_deg;
  /*! The translation error (in meters). */
  float translation_err_m;
} MLPersistentCoordinateFramesFrameState;

/*!
  \brief Initializes default values for \ref MLPersistentCoordinateFramesFrameState.
  \apilevel 7

  \param[in,out] inout_frame_state The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLPersistentCoordinateFramesFrameStateInit(MLPersistentCoordinateFramesFrameState *inout_frame_state) {
  if(NULL != inout_frame_state) {
    inout_frame_state->version = 1u;
    inout_frame_state->confidence = .0F;
    inout_frame_state->valid_radius_m = .0F;
    inout_frame_state->rotation_err_deg = .0F;
    inout_frame_state->translation_err_m = .0F;
  }
}

/*!
  \brief Creates a Persistent Coordinate Frame Tracker.

  \param[out] out_tracker_handle Pointer to a MLHandle.

  \retval MLResult_InvalidParam out param is null.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met. Check app manifest.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PcfRead
*/
ML_API MLResult ML_CALL MLPersistentCoordinateFrameTrackerCreate(MLHandle *out_tracker_handle);

/*!
  \brief Returns the count of currently available Persistent Coordinate Frames.
  \param[in] tracker_handle Valid MLHandle to a Persistent Coordinate Frame Tracker.
  \param[out] out_count Number of currently available Persistent Coordinate Frames.

  \retval MLResult_InvalidParam out param is null.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met. Check app manifest.
  \retval MLResult_UnspecifiedFailure Unspecified failure.
  \retval MLPassableWorldResult_LowMapQuality Map quality too low for content persistence. Continue building the map.
  \retval MLPassableWorldResult_UnableToLocalize Currently unable to localize into any map. Continue building the map.

  \priv PcfRead
*/
ML_API MLResult ML_CALL MLPersistentCoordinateFrameGetCount(MLHandle tracker_handle, uint32_t *out_count);

/*!
  \deprecated Deprecated since 0.20.0. Scheduled for removal. Replaced by MLPersistentCoordinateFrameGetAllEx().

  \brief Returns all the Persistent Coordinate Frame currently available.

  \param[in] tracker_handle Valid MLHandle to a Persistent Coordinate Frame Tracker.
  \param[in] buffer_size Size allocated for out_cfuids. Will only write buffer_size/sizeof(MLCoorindateFrameUID) cfuids.
  \param[out] out_cfuids Pointer to be used for writing the Persistent Coordinate Frame's MLCoordinateFrameUID.

  \retval MLResult_AllocFailed Size allocated is not sufficient. Use MLPersistentCoordinateFrameGetCount to get the count. Size
          should be sizeof(MLCoordinateFrameUID) * count.
  \retval MLResult_InvalidParam out param is null.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met. Check app manifest.
  \retval MLResult_UnspecifiedFailure Unspecified failure.
  \retval MLPassableWorldResult_LowMapQuality Map quality too low for content persistence. Continue building the map.
  \retval MLPassableWorldResult_UnableToLocalize Currently unable to localize into any map. Continue building the map.

  \priv PcfRead
*/
ML_DEPRECATED_MSG("Replaced by MLPersistentCoordinateFrameGetAllEx.")
ML_API MLResult ML_CALL MLPersistentCoordinateFrameGetAll(MLHandle tracker_handle, uint32_t buffer_size, MLCoordinateFrameUID **out_cfuids);

/*!
  \brief Returns all the Persistent Coordinate Frame currently available.
  \apilevel 4

  \param[in] tracker_handle Valid MLHandle to a Persistent Coordinate Frame Tracker.
  \param[in] count The size of the out_cfuids array.
  \param[out] out_cfuids An array used for writing the Persistent Coordinate Frame's MLCoordinateFrameUID.

  \retval MLResult_AllocFailed Size allocated is not sufficient. Use MLPersistentCoordinateFrameGetCount to get the count.
  \retval MLResult_InvalidParam out param is null.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met. Check app manifest.
  \retval MLResult_UnspecifiedFailure Unspecified failure.
  \retval MLPassableWorldResult_LowMapQuality Map quality too low for content persistence. Continue building the map.
  \retval MLPassableWorldResult_UnableToLocalize Currently unable to localize into any map. Continue building the map.

  \priv PcfRead
*/
ML_API MLResult ML_CALL MLPersistentCoordinateFrameGetAllEx(MLHandle tracker_handle, uint32_t count, MLCoordinateFrameUID *out_cfuids);

/*!
  \brief Returns the closest Persistent Coordinate Frame to the target point passed in.

  \param[in] tracker_handle Valid MLHandle to a Persistent Coordinate Frames Tracker.
  \param[in] target XYZ of the destination that the nearest Persistent Coordinate Frame is requested for.
  \param[out] out_cfuid Pointer to be used to write the MLCoordinateFrameUID for the nearest Persistent Coordinate Frame to the target destination.

  \retval MLResult_InvalidParam out param is null.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met. Check app manifest.
  \retval MLResult_UnspecifiedFailure Unspecified failure.
  \retval MLPassableWorldResult_LowMapQuality Map quality too low for content persistence. Continue building the map.
  \retval MLPassableWorldResult_UnableToLocalize Currently unable to localize into any map. Continue building the map.

  \priv PcfRead
*/
ML_API MLResult ML_CALL MLPersistentCoordinateFrameGetClosest(MLHandle tracker_handle, const MLVec3f *target, MLCoordinateFrameUID *out_cfuid);

/*!
  \brief Destroys a Persistent Coordinate Frame Tracker.

  \param[in] tracker_handle PersistentCoordinateFrameTracker handle to be destroyed.

  \retval MLResult_InvalidParam Tracker handle is not known.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLPersistentCoordinateFrameTrackerDestroy(MLHandle tracker_handle);

/*!
  \brief Returns an ASCII string representation for each result code.

  This call returns strings for all of the global MLResult and
  MLPassableWorldResult codes.

  \param[in] result_code MLResult type to be converted to string.

  \return ASCII string containing readable version of the result code.

  \priv None
*/
ML_API const char* ML_CALL MLPersistentCoordinateFrameGetResultString(MLResult result_code);

/*!
  \brief Return the state of the Persistent Coordinate Frame passed in as parameter.
  \apilevel 7

  \param[in] tracker_handle Valid MLHandle to a Persistent Coordinate Frame Tracker.
  \param[in] cfuid Valid MLCoordinateFrameUID of a Persistent Coordinate Frame.
  \param[out] out_state Pointer to be used for writing the Persistent Coordinate Frame's state.

  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_InvalidParam Cfuid and/or out_state parameters are null.
  \retval MLResult_PrivilegeDenied Privileges not met. Check app manifest.
  \retval MLResult_UnspecifiedFailure Unspecified failure.
  \retval MLPassableWorldResult_LowMapQuality Map quality too low for content persistence. Continue building the map.
  \retval MLPassableWorldResult_NotFound Passed cfuid is not available.
  \retval MLPassableWorldResult_UnableToLocalize Currently unable to localize into any map. Continue building the map.

  \priv PcfRead
*/
ML_API MLResult ML_CALL MLPersistentCoordinateFramesGetFrameState(MLHandle tracker_handle, const MLCoordinateFrameUID *cfuid, MLPersistentCoordinateFramesFrameState *out_state);

/*! \} */
ML_EXTERN_C_END
