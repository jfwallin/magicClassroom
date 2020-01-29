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
  \defgroup EyeTracking Eye Tracking
  \addtogroup EyeTracking
  \sharedobject ml_perception_client
  \brief APIs for the Eye Tracking system.
  \{
*/

/*! A set of possible error codes that the Eye Tracking system can report. */
typedef enum MLEyeTrackingError {
  /*! No error, tracking is nominal. */
  MLEyeTrackingError_None,
  /*! Eye Tracker failed. */
  MLEyeTrackingError_Generic,
  /*! Ensure enum is represented as 32 bits. */
  MLEyeTrackingError_Ensure32Bits = 0x7FFFFFFF
} MLEyeTrackingError;

/*! A set of possible calibration status codes that the Eye Tracking system can report. */
typedef enum MLEyeTrackingCalibrationStatus {
  /*! Calibration not completed. */
  MLEyeTrackingCalibrationStatus_None,
  /*! Calibration completed but outside desired thresholds. */
  MLEyeTrackingCalibrationStatus_Bad,
  /*! Calibration completed and within desired thresholds. */
  MLEyeTrackingCalibrationStatus_Good,
  /*! Ensure enum is represented as 32 bits. */
  MLEyeTrackingCalibrationStatus_Ensure32Bits = 0x7FFFFFFF
} MLEyeTrackingCalibrationStatus;

/*! Static information about the Eye Tracker. Populate with MLEyeTrackingGetStaticData(). */
typedef struct MLEyeTrackingStaticData {
  /*! Fixation. */
  MLCoordinateFrameUID fixation;

  /*!
    Left center.
    The rotation component is the left gaze w.r.t. -Z (RUB)
    Note: Prior to API Level 7 rotation will only return identity.
  */
  MLCoordinateFrameUID left_center;

  /*!
    Right center.
    The rotation component is the right gaze w.r.t. -Z (RUB)
    Note: Prior to API Level 7 rotation will only return identity.
  */
  MLCoordinateFrameUID right_center;
} MLEyeTrackingStaticData;

/*! Information about the state of the Eye Tracking system. */
typedef struct MLEyeTrackingState {
  /*! A quality metric confidence value 0.0 - 1.0 to indicate accuracy of fixation. */
  float fixation_confidence;
  /*! A quality metric confidence value 0.0 - 1.0 to indicate accuracy of left eye center. */
  float  left_center_confidence;
  /*! A quality metric confidence value 0.0 - 1.0 to indicate accuracy of right eye center. */
  float right_center_confidence;
  /*! \c true if eyes are inside a blink. When not wearing the device, values can be arbitrary. */
  bool left_blink;
  bool right_blink;
  /*! Represents what eye tracking error (if any) is present. */
  MLEyeTrackingError error;
  /*! Has eye calibration been run. */
  MLEyeTrackingCalibrationStatus calibration_status;

  /*!
    \deprecated User should not fixate at an uncomfortable close distance.
    \brief Is the user's fixation point too close for sustained use. This value is true if the user is
           focused on a point that is closer than the clipping plane.
  */
  bool fixation_depth_is_uncomfortable;

  /*!
    \deprecated User should not fixate at an uncomfortable close distance.
    \brief Number of seconds remaining that the user may be fixated at an uncomfortable depth. If this
           persists for too long, the system may take action to move the fixation point further away.
  */
  float remaining_time_at_uncomfortable_depth;

  /*!
    \deprecated User should not fixate at an uncomfortable close distance.
    \brief True if the user has fixated on a point that is closer than the clipping plane for longer than
           0 seconds within the last minute.
  */
  bool fixation_depth_violation_has_occurred;

} MLEyeTrackingState;

/*!
  \brief Create an Eye Tracker.

  \param[out] out_handle A pointer to an #MLHandle which will contain the handle of the head tracker.
              If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam The out_handle parameter was not valid (null).
  \retval MLResult_Ok Tracker was successfully created.
  \retval MLResult_UnspecifiedFaiure The eye tracker was not created successfully.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLEyeTrackingCreate(MLHandle *out_handle);

/*!
  \brief Destroy an Eye Tracker.

  \param[in] eye_tracker A handle to an Eye Tracker created by MLEyeTrackingCreate().

  \retval MLResult_Ok The eye tracker was successfully destroyed.
  \retval MLResult_UnspecifiedFailure The eye tracker was not successfully destroyed.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLEyeTrackingDestroy(MLHandle eye_tracker);

/*!
  \brief Get static information about the eye tracker.

  \param[in] eye_tracker A handle to an Eye Tracker created by MLEyeTrackingCreate().
  \param[out] out_data Target to populate the data about the eye tracker.

  \retval MLResult_InvalidParam The out_data parameter was not valid (null).
  \retval MLResult_Ok Eye tracking static data was successfully received.
  \retval MLResult_UnspecifiedFailure Failed to receive eye tracking static data.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLEyeTrackingGetStaticData(MLHandle eye_tracker, MLEyeTrackingStaticData *out_data);

/*!
  \brief Get information about the user's eyes.

  \param[in] eye_tracker A handle to an Eye Tracker created by MLEyeTrackingCreate().
  \param[out] out_state Information about the eyes.

  \retval MLResult_InvalidParam The out_state parameter was not valid (null).
  \retval MLResult_Ok Eye tracking state was successfully received.
  \retval MLResult_UnspecifiedFailure Failed to receive eye tracking state data.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLEyeTrackingGetState(MLHandle eye_tracker, MLEyeTrackingState *out_state);

/*! \} */

ML_EXTERN_C_END

