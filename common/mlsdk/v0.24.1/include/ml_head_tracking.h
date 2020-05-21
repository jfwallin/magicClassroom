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
#include "ml_pose.h"

ML_EXTERN_C_BEGIN

/*!
  \defgroup HeadTracking Head Tracking
  \addtogroup HeadTracking
  \sharedobject ml_perception_client
  \brief APIs for the Head Tracking system.

  \{
*/

/*! Static information about a Head Tracker. Populate this structure with MLHeadTrackingGetStaticData(). */
typedef struct MLHeadTrackingStaticData {
  /*! Coordinate frame ID. */
  MLCoordinateFrameUID coord_frame_head;
} MLHeadTrackingStaticData;

/*! A set of possible error conditions that can cause Head Tracking to be less than ideal. */
typedef enum MLHeadTrackingError {
  /*! No error, tracking is nominal. */
  MLHeadTrackingError_None,
  /*! There are not enough features in the environment. */
  MLHeadTrackingError_NotEnoughFeatures,
  /*! Lighting in the environment is not sufficient to track accurately. */
  MLHeadTrackingError_LowLight,
  /*! Head tracking failed for an unkown reason. */
  MLHeadTrackingError_Unknown,
  /*! Ensure enum is represented as 32 bits. */
  MLHeadTrackingError_Ensure32Bits = 0x7FFFFFFF
} MLHeadTrackingError;

/*! A set of possible tracking modes the Head Tracking system can be in. */
typedef enum MLHeadTrackingMode {
  /*! Full 6 degrees of freedom tracking (position and orientation). */
  MLHeadTrackingMode_6DOF,
  /*!
    \deprecated Use #MLHeadTrackingMode_Unavailable instead.
    \brief Limited 3 degrees of freedom tracking (orientation only).
  */
  MLHeadTrackingMode_3DOF,
  /*! Head tracking is unavailable. */
  MLHeadTrackingMode_Unavailable = MLHeadTrackingMode_3DOF,
  /*! Ensure enum is represented as 32 bits. */
  MLHeadTrackingMode_Ensure32Bits = 0x7FFFFFFF
} MLHeadTrackingMode;

/*! A structure containing information on the current state of the Head Tracking system. */
typedef struct MLHeadTrackingState {
  /*! What tracking mode the Head Tracking system is currently in. */
  MLHeadTrackingMode mode;
  /*! A confidence value (from [0, 1]) representing the confidence in the current pose estimation. */
  float confidence;
  /*! Represents what tracking error (if any) is present. */
  MLHeadTrackingError error;
} MLHeadTrackingState;

/*!
  \brief Different types of map events that can occur that a developer may have to handle.
  \apilevel 2
*/
typedef enum MLHeadTrackingMapEvent {
  /*! Map was lost. It could possibly recover. */
  MLHeadTrackingMapEvent_Lost = (1 << 0),
  /*! Previous map was recovered. */
  MLHeadTrackingMapEvent_Recovered = (1 << 1),
  /*! Failed to recover previous map. */
  MLHeadTrackingMapEvent_RecoveryFailed = (1 << 2),
  /*! New map session created. */
  MLHeadTrackingMapEvent_NewSession = (1 << 3)
} MLHeadTrackingMapEvent;

/*!
  \brief Create a Head Tracker.

  \param[out] out_handle A pointer to an MLHandle which will contain the handle of the head tracker.
              If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_Ok The head tracker was created successfully.
  \retval MLResult_UnspecifiedFailure The head tracker failed to create successfully.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLHeadTrackingCreate(MLHandle *out_handle);

/*!
  \brief Destroy a Head Tracker.

  \param[in] head_tracker A handle to a Head Tracker created by MLHeadTrackingCreate().

  \retval MLResult_Ok If the tracker was successfully destroyed.
  \retval MLResult_UnspecifiedFailure The head tracker was not successfully destroyed.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLHeadTrackingDestroy(MLHandle head_tracker);

/*!
  \brief Return static information about the Head Tracker.

  \param[in] head_tracker A handle to the tracker.
  \param[out] out_data Target to populate the data about that Head Tracker.

  \retval MLResult_InvalidParam The out_data pointer was not valid (null).
  \retval MLResult_Ok The static data was successfully received.
  \retval MLResult_UnspecifiedFailure The static data could not be received.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLHeadTrackingGetStaticData(MLHandle head_tracker, MLHeadTrackingStaticData *out_data);

/*!
  \brief Return the most recent Head Tracking state.

  \param[in] head_tracker A handle to the tracker.
  \param[out] out_state Pointer to valid #MLHeadTrackingState object to be filled with current state information.

  \retval MLResult_InvalidParam The out_state parameter was not valid (null).
  \retval MLResult_Ok The state was successfully received.
  \retval MLResult_UnspecifiedFailure Failed to receive the head tracking state.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLHeadTrackingGetState(MLHandle head_tracker, MLHeadTrackingState *out_state);

/*!
  \brief Get map events.

  A developer must be aware of certain events that can occur under degenerative conditions
  in order to cleanly handle it. The most important event to be aware of is when a map changes.

  In the case that a new map session begins, or recovery fails, all formerly cached transform
  and world reconstruction data (raycast, planes, mesh) is invalidated and must be updated.

  \apilevel 2

  \param[in] head_tracker A handle to the tracker.
  \param[out] out_map_events Pointer to a uint64_t representing a bitmask of
              MLHeadTrackerMapEvents, allocated by the caller.

  \retval MLResult_InvalidParam out_map_events was not valid (null).
  \retval MLResult_Ok Operation succeeded.
  \retval MLResult_UnspecifiedFailure Operation failed.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLHeadTrackingGetMapEvents(MLHandle head_tracker, uint64_t *out_map_events);

/*! \} */

ML_EXTERN_C_END
