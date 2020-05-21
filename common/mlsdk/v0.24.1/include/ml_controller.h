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
#include "ml_coordinate_frame_uid.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Controller
  \sharedobject ml_perception_client
  \brief APIs for the direct access to the Controller system.
  \{
*/

/*! A set of possible tracking modes for the Controller system. */
typedef enum MLControllerMode {
  /*! Imu 3 degree of freedom tracking (orientation only). */
  MLControllerMode_Imu3Dof = 0,
  /*! Em 6 degrees of freedom tracking (position and orientation). */
  MLControllerMode_Em6Dof = 1,
  /*! High quality Em fused with Imu 6 degrees of freedom tracking (position and orientation). */
  MLControllerMode_Fused6Dof = 2,
  /*! Count of number of modes. */
  MLControllerMode_Count,
  /*! Ensure enum is represented as 32 bits. */
  MLControllerMode_Ensure32Bits = 0x7FFFFFFF
} MLControllerMode;

/*! Struct to configure controller's tracking-modes. */
typedef struct MLControllerConfiguration {
  /*! Imu only 3 degree of freedom tracking (orientation only). */
  bool enable_imu3dof;
  /*! Em only 6 degrees of freedom tracking (position and orientation). */
  bool enable_em6dof;
  /*! High quality Em fused with Imu 6 degrees of freedom tracking (position and orientation). */
  bool enable_fused6dof;
} MLControllerConfiguration;

enum {
  /*! Maximum number of active Controllers. */
  MLController_MaxActive = 2
};

/*! Calibration Accuracy levels for Controller. */
typedef enum MLControllerCalibAccuracy {
  /*! CalAccuracy is Bad. */
  MLControllerCalibAccuracy_Bad = 0,
  /*! CalAccuracy is Low. */
  MLControllerCalibAccuracy_Low = 1,
  /*! CalAccuracy is Medium. */
  MLControllerCalibAccuracy_Medium = 2,
  /*! CalAccuracy is High. */
  MLControllerCalibAccuracy_High = 3,
} MLControllerCalibAccuracy;

/*! A structure containing information about the tracking-stream of this Controller. */
typedef struct MLControllerStream {
  /*! Coordinate frame identifier for this Controller. */
  MLCoordinateFrameUID coord_frame_controller;
  /*! Flag to indicate if tracking-stream is active. */
  bool is_active;
  /*! Mode of the given controller stream. */
  MLControllerMode mode;
} MLControllerStream;

/*! Information about the current state of this Controller. */
typedef struct MLControllerState {
  /*! ID of this controller (1 or 2). */
  uint8_t controller_id;
  /*! Current controller's tracking streams. */
  MLControllerStream stream[MLControllerMode_Count];
  /*! Calibration accuracy for the current Controller. */
  MLControllerCalibAccuracy accuracy;
} MLControllerState;

/*! Information about the current state of the Controller system. */
typedef struct MLControllerSystemState {
  /*! State objects for each connected and active Controller. */
  MLControllerState controller_state[MLController_MaxActive];
  /*! Calibration accuracy for the Lightwear. */
  MLControllerCalibAccuracy lightwear_accuracy;
} MLControllerSystemState;

/*!
  \deprecated Deprecated since 0.20.0. Scheduled for removal. Replaced by MLControllerCreateEx().

  \brief Create a Controller Tracker.

  \param[in] mode Tracking modes to use for this session.
  \param[out] out_handle A pointer to an #MLHandle which will the handle to the controller tracker.
              If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam The parameter out_handle was not valid (null).
  \retval MLResult_Ok Controller Tracker was created successfully.
  \retval MLResult_PrivilegeDenied The application lacks privilege.
  \retval MLResult_UnspecifiedFailure Controller Tracker was not created due to an internal error.

  \priv ControllerPose
*/
ML_DEPRECATED_MSG("Replaced by MLControllerCreateEx.")
ML_API MLResult ML_CALL MLControllerCreate(MLControllerConfiguration mode, MLHandle *out_handle);

/*!
  \brief Create a Controller Tracker.
  \apilevel 4

  \param[in]  mode A pointer to #MLControllerConfiguration struct which contains the tracking modes to use for this session.
  \param[out] out_handle A pointer to an #MLHandle which will the handle to the controller tracker.
              If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam One of the parameters was not valid (null).
  \retval MLResult_Ok Controller Tracker was created successfully.
  \retval MLResult_PrivilegeDenied The application lacks privilege.
  \retval MLResult_UnspecifiedFailure Controller Tracker was not created due to an internal error.

  \priv ControllerPose
*/
ML_API MLResult ML_CALL MLControllerCreateEx(const MLControllerConfiguration *mode, MLHandle *out_handle);

/*!
  \brief Destroy a Controller Tracker.

  \param[in] tracker MLHandle to the tracker created by MLControllerCreate().

  \retval MLResult_Ok Controller Tracker was destroyed successfully.
  \retval MLResult_UnspecifiedFailure Failed to destroy the Controller Tracker due to an internal error.

  \priv ControllerPose
*/
ML_API MLResult ML_CALL MLControllerDestroy(MLHandle tracker);

/*!
  \brief Return the most recent Controller state.

  \param[in]  handle Handle to the tracker created by MLControllerCreate().
  \param[out] out_state Pointer to valid MLControllerSystemState object to be filled with
              current state information.

  \retval MLResult_InvalidParam The parameter out_state was not valid (null).
  \retval MLResult_Ok \c out_state has been filled with current state.
  \retval MLResult_PrivilegeDenied The application lacks privilege.
  \retval MLResult_UnspecifiedFailure Failed to get the Controller state due to an internal error.

  \priv ControllerPose
*/
ML_API MLResult ML_CALL MLControllerGetState(MLHandle handle, MLControllerSystemState *out_state);

/*! \} */

ML_EXTERN_C_END
