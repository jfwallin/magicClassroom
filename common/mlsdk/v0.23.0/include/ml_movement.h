// %BANNER_BEGIN%
// ---------------------------------------------------------------------
// %COPYRIGHT_BEGIN%
//
// Copyright (c) 2017 Magic Leap, Inc. (COMPANY) All Rights Reserved.
// Magic Leap, Inc. Confidential and Proprietary
//
// NOTICE:  All information contained herein is, and remains the property
// of COMPANY. The intellectual and technical concepts contained herein
// are proprietary to COMPANY and may be covered by U.S. and Foreign
// Patents, patents in process, and are protected by trade secret or
// copyright law.  Dissemination of this information or reproduction of
// this material is strictly forbidden unless prior written permission is
// obtained from COMPANY.  Access to the source code contained herein is
// hereby forbidden to anyone except current COMPANY employees, managers
// or contractors who have executed Confidentiality and Non-disclosure
// agreements explicitly covering such access.
//
// The copyright notice above does not evidence any actual or intended
// publication or disclosure  of  this source code, which includes
// information that is confidential and/or proprietary, and is a trade
// secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION,
// PUBLIC  PERFORMANCE, OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS
// SOURCE CODE  WITHOUT THE EXPRESS WRITTEN CONSENT OF COMPANY IS
// STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE LAWS AND
// INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE
// CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS
// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE,
// USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.
//
// %COPYRIGHT_END%
// --------------------------------------------------------------------*/
// %BANNER_END%
#pragma once

#include "ml_api.h"
#include "ml_types.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Movement
  \sharedobject ml_movement
  Lumin OS designed spatial movement for objects to feel like they have mass and inertia by tilting and swaying during
  movement, but still respecting the users desired placement.  The MLMovement API provides an interface for an application
  to achieve the same feel of movement while allowing it to be configured to the applications preference.  The library also
  offers two options for handling collision during movement (hard and soft).  This library depends on an external system to
  detect collisions, but if provided with a contact normal, the library can create an inpenetrable collision which disallows
  movement against the collision normal.  Alternatively, the library can support allowing a degree of interpenetration
  before hitting an impenetrable core as defined by the parameters of the collision.
  \{
*/

enum {
  MLResultAPIPrefix_Movement = MLRESULT_PREFIX(0xdffe)
};

/*!
  \brief Return values for Movement Service API calls.

  \apilevel 7
*/
typedef enum MLMovementResult {
  MLMovementResult_InvalidMovementHandle = MLResultAPIPrefix_Movement,  //!< Not a valid MLHandle for movement session.
  MLMovementResult_InvalidCollisionHandle,    //!< Not a valid MLHandle for a collision session.
  MLMovementResult_Ensure32Bits = 0x7FFFFFFF  //!< Ensure enum is 32-bits.
} MLMovementResult;

/*!
  \brief Settings for a movement session.

  \apilevel 7
*/
typedef struct MLMovementSettings {
  /*! Struct API version. */
  uint32_t version;

  /*!
    \brief Number of frames of sway history to track. Increase to improve smoothing. Minimum value of 3.
      This is defaulted to 30.
  */
  uint32_t sway_history_size;

  /*!
    \brief Maximum angle, in radians, between the oldest and newest headpose to object vector.
      Increasing this will increase the maximum speed of movement. Must be greater than zero.
      This is defaulted to 0.5.
  */
  float max_delta_angle;

  /*!
    \brief A unitless number that governs the smoothing of Control input.
      Larger values will make the movement more twitchy, smaller values will make it smoother by
      increasing latency between Control input and object movement response by averaging multiple
      frames of input values. Must be greater than zero. Typical values would be between 0.5 and 10.
      This is defaulted to 3.5.
  */
  float control_dampening_factor;

  /*!
    \brief The maximum angle, in radians, that the object will be tilted left/right and front/back.
      Cannot be a negative value, but may be zero. This is defaulted to M_PI / 6.0.
  */
  float max_sway_angle;

  /*!
    \brief The speed of rotation that will stop implicit depth translation from happening.
      The speed of rotation about the headpose Y-axis, in radians per second, that if exceeded,
      stops implicit depth translation from happening. Must be greater than zero.
      This is defaulted to M_PI * 5.0 / 3.0.
  */
  float maximum_headpose_rotation_speed;

  /*!
    \brief The maximum speed that headpose can move, in meters per second, that will stop implicit depth translation.
      If the headpose is moving faster than this speed (meters per second) implicit depth translation doesn't happen.
      Must be greater than zero. This is defaulted to 0.75.
  */
  float maximum_headpose_movement_speed;

  /*!
    \brief Distance in meters the object must move in depth since the last frame to cause maximum
      push/pull sway. Must be greater than zero. This is defaulted to 0.1.
  */
  float maximum_depth_delta_for_sway;

  /*!
    \brief The minimum distance in meters the object can be moved in depth relative to the headpose.
      This must be greater than zero and less than maximum_distance. This is defaulted to 0.5.
  */
  float minimum_distance;

  /*!
    \brief The maximum distance in meters the object can be moved in depth relative to the headpose.
      This must be greater than zero and minimum_distance. This is defaulted to 15.0.
  */
  float maximum_distance;

  /*!
    \brief Maximum length of time, in seconds, that lateral sway should take to decay.
      Maximum length of time (in seconds) lateral sway should take to decay back to an upright
      orientation once lateral movement stops. Defaults to 0.4, must be greater than zero.
  */
  float maximum_sway_time_s;

  /*!
    \brief Maximum length of time, in seconds, to allow MLMovementEnd to resolve before forcefully aborting.
      This serves as a fail-safe for instances where the object is in a bad position and can't resolve to a safe
      position. This is defaulted to 10.0 and must be greater than or equal to zero.
  */
  float end_resolve_timeout_s;
} MLMovementSettings;

/*!
  \brief Initializes the MLMovementSettings structure.

  \apilevel 7
*/
ML_STATIC_INLINE void MLMovementSettingsInit(MLMovementSettings *in_out_settings) {
  if (in_out_settings != NULL) {
    in_out_settings->version = 1;
    in_out_settings->sway_history_size = 0;
    in_out_settings->max_delta_angle = 0.0f;
    in_out_settings->control_dampening_factor = 0.0f;
    in_out_settings->max_sway_angle = 0.0f;
    in_out_settings->maximum_headpose_rotation_speed = 0.0f;
    in_out_settings->maximum_headpose_movement_speed = 0.0f;
    in_out_settings->maximum_depth_delta_for_sway = 0.0f;
    in_out_settings->minimum_distance = 0.0f;
    in_out_settings->maximum_distance = 0.0f;
    in_out_settings->maximum_sway_time_s = 0.0f;
    in_out_settings->end_resolve_timeout_s = 0.0f;
  }
}

/*!
  \brief Parameters that define the input controls for 3DoF movement.

  Under 3DoF movement mode, only the orientation of the user's pointing device is taken into account
  for moving the target object and the pointing device's position is not used. Headpose
  position is also required by the algorithm, and is used as the center point around which
  the target object moves.

  When the movement session starts, the initial orientation of the control forms a baseline, from
  which further changes in the orientation will cause the object to move around the user.

  \apilevel 7
*/
typedef struct MLMovement3DofControls {
  /*! Struct API version. */
  uint32_t version;

  /*! The worldspace position of the device's headpose. */
  MLVec3f headpose_position;

  /*! The worldspace orientation of the user's pointing device. */
  MLQuaternionf control_rotation;
} MLMovement3DofControls;

/*!
  \brief Initializes the MLMovement3DofControls structure.

  \apilevel 7
*/
ML_STATIC_INLINE void MLMovement3DofControlsInit(MLMovement3DofControls *in_out_3Dof_controls) {
  if (in_out_3Dof_controls != NULL) {
    in_out_3Dof_controls->version = 1;

    in_out_3Dof_controls->headpose_position.x = 0.0f;
    in_out_3Dof_controls->headpose_position.y = 0.0f;
    in_out_3Dof_controls->headpose_position.z = 0.0f;

    in_out_3Dof_controls->control_rotation.x = 0.0f;
    in_out_3Dof_controls->control_rotation.y = 0.0f;
    in_out_3Dof_controls->control_rotation.z = 0.0f;
    in_out_3Dof_controls->control_rotation.w = 1.0f;
  }
}

/*!
  \brief Parameters that define the input controls for 6DoF movement.

  Under 6DoF movement mode, both the position and orientation of the user's pointing device is
  taken into account when moving the target object. Headpose position and orientation is also
  required by the algorithm.

  When the movement session starts, the initial orientation and position of the pointing device
  form a baseline, from which further changes in orientation or position will cause the object to
  move relative to the pointing device.

  \apilevel 7
*/
typedef struct MLMovement6DofControls {
  /*! Struct API version. */
  uint32_t version;

  /*! The worldspace position of the device's headpose. */
  MLVec3f headpose_position;

  /*! The worldspace orientation of the device's headpose. */
  MLQuaternionf headpose_rotation;

  /*! The worldspace position of the user's pointing device. */
  MLVec3f control_position;

  /*! The worldspace orientation of the user's pointing device. */
  MLQuaternionf control_rotation;
} MLMovement6DofControls;

/*!
  \brief Initializes the MLMovement6DofControls structure.

  \apilevel 7
*/
ML_STATIC_INLINE void MLMovement6DofControlsInit(MLMovement6DofControls *in_out_6Dof_controls) {
  if (in_out_6Dof_controls != NULL) {
    in_out_6Dof_controls->version = 1;

    in_out_6Dof_controls->headpose_position.x = 0.0f;
    in_out_6Dof_controls->headpose_position.y = 0.0f;
    in_out_6Dof_controls->headpose_position.z = 0.0f;

    in_out_6Dof_controls->headpose_rotation.x = 0.0f;
    in_out_6Dof_controls->headpose_rotation.y = 0.0f;
    in_out_6Dof_controls->headpose_rotation.z = 0.0f;
    in_out_6Dof_controls->headpose_rotation.w = 1.0f;

    in_out_6Dof_controls->control_position.x = 0.0f;
    in_out_6Dof_controls->control_position.y = 0.0f;
    in_out_6Dof_controls->control_position.z = 0.0f;

    in_out_6Dof_controls->control_rotation.x = 0.0f;
    in_out_6Dof_controls->control_rotation.y = 0.0f;
    in_out_6Dof_controls->control_rotation.z = 0.0f;
    in_out_6Dof_controls->control_rotation.w = 1.0f;
  }
}

/*!
  \brief The relevant state of the object being moved.

  \apilevel 7
*/
typedef struct MLMovementObject {
  /*! Struct API version. */
  uint32_t version;

  /*! The worldspace translation of the object to move. */
  MLVec3f object_position;

  /*! The worldspace orientation of the object to move. */
  MLQuaternionf object_rotation;
} MLMovementObject;

/*!
  \brief Initializes the MLMovementObject structure.

  \apilevel 7
*/
ML_STATIC_INLINE void MLMovementObjectInit(MLMovementObject *in_out_movement_object) {
  if (in_out_movement_object != NULL) {
    in_out_movement_object->version = 1;

    in_out_movement_object->object_position.x = 0.0f;
    in_out_movement_object->object_position.y = 0.0f;
    in_out_movement_object->object_position.z = 0.0f;

    in_out_movement_object->object_rotation.x = 0.0f;
    in_out_movement_object->object_rotation.y = 0.0f;
    in_out_movement_object->object_rotation.z = 0.0f;
    in_out_movement_object->object_rotation.w = 1.0f;
  }
}

/*!
  \brief 3DoF specific movement settings that must be provided when starting a 3DoF movement session.

  A 3DoF movement session relies on the orientation of the user's pointing device, but ignores the
  position, when moving the target object.

  \apilevel 7
*/
typedef struct MLMovement3DofSettings {
  /*! Struct API version. */
  uint32_t version;

  /*! If the object should automatically center on the control direction when beginning movement. */
  bool auto_center;
} MLMovement3DofSettings;

/*!
  \brief Initializes the MLMovement3DofSettings structure.

  \apilevel 7
*/
ML_STATIC_INLINE void MLMovement3DofSettingsInit(MLMovement3DofSettings *in_out_3Dof_settings) {
  if (in_out_3Dof_settings != NULL) {
    in_out_3Dof_settings->version = 1;
    in_out_3Dof_settings->auto_center = false;
  }
}

/*!
  \brief 6DoF specific movement settings that must be provided when starting a 6DoF movement session.

  A 6DoF movement session relies on both the position and orientation of the user's pointing device
  when moving the target object.

  \apilevel 7
*/
typedef struct MLMovement6DofSettings {
  /*! Struct API version. */
  uint32_t version;

  /*! If the object should automatically center on the control direction when beginning movement. */
  bool auto_center;
} MLMovement6DofSettings;

/*!
  \brief Initializes the MLMovement6DofSettings structure.

  \apilevel 7
*/
ML_STATIC_INLINE void MLMovement6DofSettingsInit(MLMovement6DofSettings *in_out_6Dof_settings) {
  if (in_out_6Dof_settings != NULL) {
    in_out_6Dof_settings->version = 1;
    in_out_6Dof_settings->auto_center = false;
  }
}

/*!
  \brief Get the current movement settings.

  This populates the provided MLMovementSettings struct with all of the current
  movement settings. Call this to initialize a MLMovementSettings struct before making
  changes to particular values before passing into MLMovementStart3Dof or MLMovementStart6Dof.

  \apilevel 7

  \param[out]  settings MLMovementSettings populated with default movement settings.
  \retval MLResult_Ok Settings were successfully provided.
  \retval MLResult_InvalidParam nullptr was provided.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementGetDefaultSettings(MLMovementSettings* out_settings);

/*!
  \brief Start a 3Dof movement session.

  A 3DoF movement session relies on the orientation of the user's pointing device, but ignores the
  position, when moving the target object.

  \apilevel 7

  \param[in]  session_settings The MLMovementSettings for this session.
  \param[in]  dof_settings The 3DoF specific settings for this movement session.
  \param[in]  controls The starting values for the input controls for this movement session.
  \param[in]  object The starting values for the movement object for this movement session.
  \param[out]  out_handle Handle of the movement session created.  Only valid if result is MLResult_Ok.
  \retval MLResult_Ok Movement session was successfully started.
  \retval MLMovementResult_InvalidMovementHandle out_handle passed is nullptr.
  \retval MLResult_InvalidParam One or more of the input arguments are nullptr.
  \retval MLResult_UnspecifiedFailure The movement session could not be created.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementStart3Dof(const MLMovementSettings* session_settings, const MLMovement3DofSettings* dof_settings, const MLMovement3DofControls* controls, const MLMovementObject* object, MLHandle* out_handle);

/*!
  \brief Start a 6Dof movement session.

  A 6DoF movement session relies on both the position and orientation of the user's pointing device
  when moving the target object.

  \apilevel 7

  \param[in]  session_settings The MLMovementSettings for this session.
  \param[in]  dof_settings The 6DoF specific settings for this movement session.
  \param[in]  controls The starting values for the input controls for this movement session.
  \param[in]  object The starting values for the movement object for this movement session.
  \param[out]  out_handle Handle of the movement session created.  Only valid if result is MLResult_Ok.
  \retval MLResult_Ok Movement session was successfully started.
  \retval MLMovementResult_InvalidMovementHandle out_handle passed is nullptr.
  \retval MLResult_InvalidParam One or more of the input arguments are nullptr.
  \retval MLResult_UnspecifiedFailure The movement session could not be created.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementStart6Dof(const MLMovementSettings* session_settings, const MLMovement6DofSettings* dof_settings, const MLMovement6DofControls* controls, const MLMovementObject* object, MLHandle* out_handle);

/*!
  \brief Change the depth offset of the object from the user's headpose (when using a 3DoF movement
  session) or from the pointing device (when using a 6DoF movement session).

  \apilevel 7

  \param[in]  session_handle Handle to the movement session.
  \param[in]  delta_depth The change in depth.
  \retval MLResult_Ok Depth change was successful.
  \retval MLMovementResult_InvalidMovementHandle session_handle is invalid.
  \retval MLResult_UnspecifiedFailure Depth could not be changed.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementChangeDepth(MLHandle session_handle, float delta_depth);

/*!
  \brief Change the rotation about the up-axis of the object being moved.

  This rotation applies to the object in its local space, and not to the rotation relative to
  the user's headpose (when using a 3DoF movement session) or relative to the pointing device
  (when using a 6DoF movement session).

  \apilevel 7

  \param[in]  session_handle Handle to the movement session.
  \param[in]  delta_radians angle, in radians, to rotate.
  \retval MLResult_Ok Rotation change was successful.
  \retval MLMovementResult_InvalidMovementHandle session_handle is invalid.
  \retval MLResult_UnspecifiedFailure Rotation could not be changed.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementChangeRotation(MLHandle session_handle, float delta_radians);

/*!
  \brief Update movement for a 3Dof movement session.

  This function is intended to be called once per frame per active 3Dof movement session.

  \apilevel 7

  \param[in]  session_handle Handle to the movement session.
  \param[in]  controls The current 3DoF input control values.
  \param[in]  delta_time The change in time since the last update.
  \param[in, out] in_out_object The current values of the movement object which will be updated by movement.
  \retval MLResult_Ok The movement session provided was updated.
  \retval MLMovementResult_InvalidMovementHandle session_handle is invalid.
  \retval MLResult_InvalidParam One or more of the input arguments are invalid.
  \retval MLResult_UnspecifiedFailure The movement session could not be updated.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementUpdate3Dof(MLHandle session_handle, const MLMovement3DofControls* controls, float delta_tme, MLMovementObject* in_out_object);

/*!
  \brief Update movement for a 6Dof movement session.

  This function is intended to be called once per frame per active 6Dof movement session.

  \apilevel 7

  \param[in]  session_handle Handle to the movement session.
  \param[in]  controls The current 6DoF input control values.
  \param[in]  delta_time The change in time since the last update.
  \param[in, out] in_out_object The current values of the movement object which will be updated by movement.
  \retval MLResult_Ok The movement session provided was updated.
  \retval MLMovementResult_InvalidMovementHandle session_handle is invalid.
  \retval MLResult_InvalidParam One or more of the input arguments are invalid.
  \retval MLResult_UnspecifiedFailure The movement session could not be updated.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementUpdate6Dof(MLHandle session_handle, const MLMovement6DofControls* controls, float delta_time, MLMovementObject* in_out_object);

/*!
  \brief End a movement session (either 3Dof or 6Dof).

  This function is intended to be called once per frame per active movement session while it returns MLResult_Pending.
  This will allow the session to resolve any remaining soft collisions.

  \apilevel 7

  \param[in]  session_handle Handle to the movement session.
  \param[in]  delta_time The change in time since the previous frame.
  \param[out] out_object The final values of the movement object after the movement session has ended.
  \retval MLResult_Ok The movement session successfully ended.
  \retval MLResult_Pending The movement session is still resolving.
  \retval MLResult_Timeout The movement session was unable to resolve and was forcefully ended.
  \retval MLMovementResult_InvalidMovementHandle session_handle is invalid.
  \retval MLResult_InvalidParam One or more of the input arguments are invalid.
  \retval MLResult_UnspecifiedFailure The movement session could not be ended.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementEnd(MLHandle session_handle, float delta_time, MLMovementObject* out_object);

/*!
  \brief Start tracking a hard collision session for a movement session.

  Call this to inform the movement library that the moving object has collided with something impenetrable.
  The movement library will not allow the moving object to penetrate the other object in the direction
  opposite the contact normal. You can update the contact normal for the returned collision instance handle
  each frame by calling MLMovementUpdateHardCollision. This is useful if the moving object is colliding with a
  curved surface so the contact normal is changing as the moving object slides against the collision surface.

  \apilevel 7

  \param[in]  session_handle Handle to the movement session.
  \param[in]  contact_normal normalized Vector3 surface normal of object that movement is colliding with.
  \param[out]  out_collision_handle Handle for the collision session.  Only valid if result is MLResult_Ok.
  \retval MLResult_Ok The collision session was successfully created and added to the movement session provided.
  \retval MLMovementResult_InvalidCollisionHandle out_collision_handle is nullptr.
  \retval MLMovementResult_InvalidMovementHandle session_handle is invalid.
  \retval MLResult_InvalidParam contact_normal is nullptr.
  \retval MLResult_UnspecifiedFailure The collision could not be created and added to the movment session.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementStartHardCollision(MLHandle session_handle, const MLVec3f* contact_normal, MLHandle* out_collision_handle);

/*!
  \brief Start tracking a soft collision session for a movement session.

  Soft collisions will allow a degree of interpenetration during movement.  Collisions will resolve over time
  giving the colliding objects a softness to them.

  \apilevel 7

  \param[in]  session_handle Handle to the movement session.
  \param[in]  other_position position of the center of the thing we are soft-colliding with, the source of the repulsion force.
  \param[in]  closest_distance the position of the the object being moved will never be closer to other_position than this distance.
  \param[in]  max_distance the moving object will not be affected by this soft collision once its position is this distance from other_position.
  \param[out]  out_collision_handle Handle for the collision session.  Only valid if result is MLResult_Ok.
  \retval MLResult_Ok The collision session was successfully created and added to the movement session provided.
  \retval MLMovementResult_InvalidCollisionHandle out_collision_handle is nullptr.
  \retval MLMovementResult_InvalidMovementHandle session_handle is invalid.
  \retval MLResult_InvalidParam other_position is nullptr.
  \retval MLResult_UnspecifiedFailure The collision could not be created and added to the movment session.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementStartSoftCollision(MLHandle session_handle, const MLVec3f* other_position, float closest_distance, float max_distance, MLHandle* out_collision_handle);

/*!
  \brief Update the collision normal for an existing hard collision session in an existing movement session.

  This function is intended to be called once per hard collision per movement session per frame.

  \apilevel 7

  \param[in]  session_handle Handle to the movement session.
  \param[in]  collision_handle Handle to the collision session for the movement session.
  \param[in]  contact_normal the updated normal for this collision.
  \retval MLResult_Ok The collision was successfully updated.
  \retval MLMovementResult_InvalidMovementHandle session_handle is invalid.
  \retval MLResult_InvalidParam contact_normal is invalid.
  \retval MLMovementResult_InvalidCollisionHandle The collision isn't hard or collision_handle is invalid.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementUpdateHardCollision(MLHandle session_handle, MLHandle collision_handle, const MLVec3f* contact_normal);

/*!
  \brief End the tracking of a soft or hard collision session for a movement session.

  \apilevel 7

  \param[in]  session_handle Handle to the movement session.
  \param[in]  collision_handle Handle to the collision session for the movement session.
  \retval MLResult_Ok The collision was successfully ended.
  \retval MLMovementResult_InvalidMovementHandle session_handle is invalid.
  \retval MLMovementResult_InvalidCollisionHandle collision_handle is invalid.

  \priv None
*/
ML_API MLResult ML_CALL MLMovementEndCollision(MLHandle session_handle, MLHandle collision_handle);

/*!
  \brief Returns an ASCII string for MLMovementResult codes.

  \apilevel 7

  \param[in] result_code The input MLResult enum from MLMovement functions.
  \return ASCII string containing readable version of result code.

  \priv None
*/
ML_API const char* ML_CALL MLMovementGetResultString(MLResult result_code);

/*! \}*/

ML_EXTERN_C_END
