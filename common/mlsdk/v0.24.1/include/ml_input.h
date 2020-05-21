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
#include "ml_keycodes.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Input
  \sharedobject ml_input
  \brief APIs for the Input system.
  \{
*/

enum {
  /*! Defines the prefix for MLInputResult codes. */
  MLResultAPIPrefix_Input = MLRESULT_PREFIX(0x21DC)
};

/*!
  \brief Return values for Input API calls.
*/
typedef enum MLInputResult {
  MLInputResult_ServiceNotAvailable = MLResultAPIPrefix_Input,
  /*! \brief Operation failed because a required permission has not been granted.
   Example: This can happen when the app is not on the foreground.
  */
  MLInputResult_PermissionDenied,
  /*! Operation failed because a required device was not found. */
  MLInputResult_DeviceNotFound,
  /*! Operation failed because the service was in an illegal state.
   Example: Such a state could be when the service is still initializing.
  */
  MLInputResult_IllegalState,
  /*! Operation failed because of an internal error. */
  MLInputResult_InternalError,
  /*! Ensure enum is represented as 32 bits. */
  MLInputResult_Ensure32Bits = 0x7FFFFFFF
} MLInputResult;

enum {
  /*! Maximum recognized number of input controllers. */
  MLInput_MaxControllers = 2,

  /*! Maximum recognized number of controller touchpad touches. */
  MLInput_MaxControllerTouchpadTouches = 2
};

/*! Touchpad gesture state. */
typedef enum MLInputControllerTouchpadGestureState {
  /*! End(Default). */
  MLInputControllerTouchpadGestureState_End,
  /*! Continue. */
  MLInputControllerTouchpadGestureState_Continue,
  /*! Start. */
  MLInputControllerTouchpadGestureState_Start,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerTouchpadGestureState_Ensure32Bits = 0x7FFFFFFF
} MLInputControllerTouchpadGestureState;

/*! Recognized touchpad gesture types. */
typedef enum MLInputControllerTouchpadGestureType {
  /*! None. */
  MLInputControllerTouchpadGestureType_None,
  /*! Tap. This is a discrete gesture */
  MLInputControllerTouchpadGestureType_Tap,
  /*! Force tap down. This is a discrete gesture */
  MLInputControllerTouchpadGestureType_ForceTapDown,
  /*! Force tap up. This is a discrete gesture */
  MLInputControllerTouchpadGestureType_ForceTapUp,
  /*! Force dwell. This is a discrete gesture */
  MLInputControllerTouchpadGestureType_ForceDwell,
  /*! Second force down. This is a discrete gesture */
  MLInputControllerTouchpadGestureType_SecondForceDown,
  /*! Long hold. This is a continuous gesture */
  MLInputControllerTouchpadGestureType_LongHold,
  /*! Radial scroll. This is a continuous gesture */
  MLInputControllerTouchpadGestureType_RadialScroll,
  /*! Swipe. This is a continuous gesture */
  MLInputControllerTouchpadGestureType_Swipe,
  /*! Scroll. This is a continuous gesture */
  MLInputControllerTouchpadGestureType_Scroll,
  /*! Pinch. This is a continuous gesture */
  MLInputControllerTouchpadGestureType_Pinch,
  /*! Number of gesture types. */
  MLInputControllerTouchpadGestureType_Count,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerTouchpadGestureType_Ensure32Bits = 0x7FFFFFFF
} MLInputControllerTouchpadGestureType;

/*! Direction of touchpad gesture. */
typedef enum MLInputControllerTouchpadGestureDirection {
  /*! None. */
  MLInputControllerTouchpadGestureDirection_None,
  /*! Up. */
  MLInputControllerTouchpadGestureDirection_Up,
  /*! Down. */
  MLInputControllerTouchpadGestureDirection_Down,
  /*! Left. */
  MLInputControllerTouchpadGestureDirection_Left,
  /*! Right. */
  MLInputControllerTouchpadGestureDirection_Right,
  /*! In. */
  MLInputControllerTouchpadGestureDirection_In,
  /*! Out. */
  MLInputControllerTouchpadGestureDirection_Out,
  /*! Clockwise. */
  MLInputControllerTouchpadGestureDirection_Clockwise,
  /*! Counter clockwise. */
  MLInputControllerTouchpadGestureDirection_CounterClockwise,
  /*! Number of directions. */
  MLInputControllerTouchpadGestureDirection_Count,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerTouchpadGestureDirection_Ensure32Bits = 0x7FFFFFFF
} MLInputControllerTouchpadGestureDirection;

/*! Information about a recognized touchpad gesture. */
typedef struct MLInputControllerTouchpadGesture {
  /*!
    \brief Gesture position (x,y) and force (z).
    Position is in the [-1.0,1.0] range and force is in the [0.0,1.0] range.
  */
  MLVec3f pos_and_force;
  /*! Type of gesture. */
  MLInputControllerTouchpadGestureType type;
  /*! Direction of gesture. */
  MLInputControllerTouchpadGestureDirection direction;
  /*!
    \brief Speed of gesture. Note that this takes on different meanings depending
    on the gesture type being performed:
      - For radial gestures, this will be the angular speed around the axis.
      - For pinch gestures, this will be the speed at which the distance
        between fingers is changing. The touchpad is defined as having extents
        of [-1.0,1.0] so touchpad distance has a range of [0.0,2.0]; this value
        will be in touchpad distance per second.
  */
  float speed;
  /*!
    \brief For radial gestures, this is the absolute value of the angle. For scroll
    and pinch gestures, this is the absolute distance traveled in touchpad
    distance. The touchpad is defined as having extents of [-1.0,1.0] so
    this distance has a range of [0.0,2.0].
  */
  float distance;
  /*!
    \brief Distance between the two fingers performing the gestures in touchpad
    distance. The touchpad is defined as having extents of [-1.0,1.0] so
    this distance has a range of [0.0,2.0].
  */
  float finger_gap;
  /*!
    \brief For radial gestures, this is the radius of the gesture. The touchpad
    is defined as having extents of [-1.0,1.0] so this radius has a range
    of [0.0,2.0].
  */
  float radius;
  /*! Angle from the center of the touchpad to the finger in radians. */
  float angle;
} MLInputControllerTouchpadGesture;

/*! Types of input controllers recognized by Magic Leap platform. */
typedef enum MLInputControllerType {
  /*! None. */
  MLInputControllerType_None,
  /*! Device. */
  MLInputControllerType_Device,
  /*! Mobile app. */
  MLInputControllerType_MobileApp,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerType_Ensure32Bits = 0x7FFFFFFF
} MLInputControllerType;

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal. Instead use ml_controller.h.

  \brief Degrees-of-freedom mode of input controller.
*/
typedef enum MLInputControllerDof {
  /*! None. */
  MLInputControllerDof_None,
  /*! 3DoF. */
  MLInputControllerDof_3,
  /*! 6DoF. */
  MLInputControllerDof_6,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerDof_Ensure32Bits = 0x7FFFFFFF
} MLInputControllerDof;

/*! Buttons on input controllers (Magic Leap device and Mobile Companion App). */
typedef enum MLInputControllerButton {
  MLInputControllerButton_None,
  /*!
    \deprecated Move button is obsolete.
  */
  MLInputControllerButton_Move,
  /*!
    \deprecated App button is obsolete.
  */
  MLInputControllerButton_App,
  /*! Bumper. */
  MLInputControllerButton_Bumper,
  /*!
    \brief Home tap
    The controller button cannot be polled, its is only available
    via callbacks.
  */
  MLInputControllerButton_HomeTap,
  /*! Number of buttons */
  MLInputControllerButton_Count,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerButton_Ensure32Bits = 0x7FFFFFFF
} MLInputControllerButton;

/*!
  \deprecated Deprecated since 0.24.0. Scheduled for removal. Replaced by MLInputControllerCallbacksEx.

  \brief A structure containing callbacks for input controller events. The final
  parameter to all the callbacks is a void *, which will point to whatever
  payload data the user provides in MLInputSetControllerCallbacks.
  Individual callbacks which are not required by the client can be NULL.
*/
typedef ML_DEPRECATED_MSG("Replaced by MLInputControllerCallbacksEx.") struct MLInputControllerCallbacks {
  /*! This callback will be invoked whenever a detected touchpad gesture begins. */
  void (*on_touchpad_gesture_start)(uint8_t controller_id, const MLInputControllerTouchpadGesture *touchpad_gesture, void *data);
  /*! \brief This callback will be invoked as a detected touchpad gesture continues.
      This callback is only valid for continious gestures.
   */
  void (*on_touchpad_gesture_continue)(uint8_t controller_id, const MLInputControllerTouchpadGesture *touchpad_gesture, void *data);
  /*! \bireif This callback will be invoked whenever a detected touchpad gesture ends.
      This callback is only valid for continious gestures.
   */
  void (*on_touchpad_gesture_end)(uint8_t controller_id, const MLInputControllerTouchpadGesture *touchpad_gesture, void *data);
  /*! This callback will be invoked whenever a button press is detected. */
  void (*on_button_down)(uint8_t controller_id, MLInputControllerButton button, void *data);
  /*! This callback will be invoked whenever a button release is detected. */
  void (*on_button_up)(uint8_t controller_id, MLInputControllerButton button, void *data);
  /*! This callback will be invoked when a controller is connected. */
  void (*on_connect)(uint8_t controller_id, void *data);
  /*! This callback will be invoked when a controller is disconnected. */
  void (*on_disconnect)(uint8_t controller_id, void *data);
} MLInputControllerCallbacks;

/*!
  \brief A structure containing callbacks for input controller events.

  The final parameter to all the callbacks is a void *, which will point to
  whatever payload data the user provides in MLInputSetControllerCallbacksEx.
  Individual callbacks which are not required by the client can be NULL.

  This structure must be initialized by calling MLInputControllerCallbacksExInit()
  before use.

  \apilevel 8
*/
typedef struct MLInputControllerCallbacksEx {
  /*! Version of this structure. */
  uint32_t version;
  /*!
    \brief This callback will be invoked whenever a touch gesture is detected.
    This callback will be called for discrete and continuous gestures.
    \param[in] controller_id ID of the controller.
    \param[in] touchpad_gesture Pointer to the struct containing state of the controller.
    \param[out] user_data User data as passed to MLInputSetControllerCallbacksEx().
  */
  void (*on_touchpad_gesture)(uint8_t controller_id, const MLInputControllerTouchpadGesture *touchpad_gesture, void *data);
  /*!
    \brief This callback will be invoked whenever a continuation of a touch gesture
    is detected.
    This callback will be called only for continuous gestures.
    \param[in] controller_id ID of the controller.
    \param[in] touchpad_gesture Pointer to the struct containing state of the controller.
    \param[out] user_data User data as passed to MLInputSetControllerCallbacksEx().
  */
  void (*on_touchpad_gesture_continue)(uint8_t controller_id, const MLInputControllerTouchpadGesture *touchpad_gesture, void *data);
  /*!
    \brief This callback will be invoked whenever a continuous touch gesture ends.
    This callback will be called only for continuous gestures.
    \param[in] controller_id ID of the controller.
    \param[in] touchpad_gesture Pointer to the struct containing state of the controller.
    \param[out] user_data User data as passed to MLInputSetControllerCallbacksEx().
  */
  void (*on_touchpad_gesture_end)(uint8_t controller_id, const MLInputControllerTouchpadGesture *touchpad_gesture, void *data);
  /*!
    \brief This callback will be invoked whenever a controller button press is detected.
    \param[in] controller_id ID of the controller.
    \param[in] button Value of the controller button.
    \param[out] user_data User data as passed to MLInputSetControllerCallbacksEx().
  */
  void (*on_button_down)(uint8_t controller_id, MLInputControllerButton button, void *data);
  /*!
    \brief This callback will be invoked whenever a controller button release is detected.
    \param[in] controller_id ID of the controller.
    \param[in] button Value of the controller button.
    \param[out] user_data User data as passed to MLInputSetControllerCallbacksEx().
  */
  void (*on_button_up)(uint8_t controller_id, MLInputControllerButton button, void *data);
  /*!
    \brief This callback will be invoked whenever a controller is connected.
    \param[in] controller_id ID of the controller.
    \param[out] user_data User data as passed to MLInputSetControllerCallbacksEx().
  */
  void (*on_connect)(uint8_t controller_id, void *data);
  /*!
    \brief This callback will be invoked whenever a controller is disconnected.
    \param[in] controller_id ID of the controller.
    \param[out] user_data User data as passed to MLInputSetControllerCallbacksEx().
  */
  void (*on_disconnect)(uint8_t controller_id, void *data);
} MLInputControllerCallbacksEx;

/*!
  \brief Initializes default values for MLInputControllerCallbacksEx.

  \param[in,out] inout_callbacks The object to initialize with default settings.

  \apilevel 8
*/
ML_STATIC_INLINE void MLInputControllerCallbacksExInit(MLInputControllerCallbacksEx *inout_callbacks) {
  if (inout_callbacks) {
    inout_callbacks->version = 1;
    inout_callbacks->on_touchpad_gesture = NULL;
    inout_callbacks->on_touchpad_gesture_continue = NULL;
    inout_callbacks->on_touchpad_gesture_end = NULL;
    inout_callbacks->on_button_down = NULL;
    inout_callbacks->on_button_up = NULL;
    inout_callbacks->on_connect = NULL;
    inout_callbacks->on_disconnect = NULL;
  }
}

/*! A structure containing information about the current state of an input controller. */
typedef struct MLInputControllerState {
  /*!
    \deprecated Deprecated since 0.23.0. Scheduled for removal. Instead use ml_controller.h.
    \brief Orientaton of the controller.
  */
  MLQuaternionf orientation;
  /*!
    \deprecated Deprecated since 0.23.0. Scheduled for removal. Instead use ml_controller.h.
    \brief Postion of the controller.
  */
  MLVec3f position;
  /*!
    \brief Current touch position (x,y) and force (z).
    Position is in the [-1.0,1.0] range and force is in the [0.0,1.0] range.
  */
  MLVec3f touch_pos_and_force[MLInput_MaxControllerTouchpadTouches];
  /*! Normalized trigger value [0.0,1.0]. */
  float trigger_normalized;
  /*! Button states. */
  bool button_state[MLInputControllerButton_Count];
  /*! Is touch active. */
  bool is_touch_active[MLInput_MaxControllerTouchpadTouches];
  /*! If this controller is connected. */
  bool is_connected;
  /*! Current touchpad gesture. */
  MLInputControllerTouchpadGesture touchpad_gesture;
  /*! Current touchpad gesture state. */
  MLInputControllerTouchpadGestureState touchpad_gesture_state;
  /*! Type of this controller. */
  MLInputControllerType type;
  /*!
    \deprecated Deprecated since 0.23.0. Scheduled for removal. Instead use ml_controller.h.
    \brief Current degrees of freedom mode of the controller.
  */
  MLInputControllerDof dof;
  /*!
    \brief Hardware index of this controller. If it is a physical controller this
    will be either 0 or 1. If it is MLMA it will be 0xFF.
  */
  uint8_t hardware_index;
} MLInputControllerState;

/*! Feedback patterns for LED target. */
typedef enum MLInputControllerFeedbackPatternLED
{
  MLInputControllerFeedbackPatternLED_None,
  MLInputControllerFeedbackPatternLED_Clock1,
  MLInputControllerFeedbackPatternLED_Clock2,
  MLInputControllerFeedbackPatternLED_Clock3,
  MLInputControllerFeedbackPatternLED_Clock4,
  MLInputControllerFeedbackPatternLED_Clock5,
  MLInputControllerFeedbackPatternLED_Clock6,
  MLInputControllerFeedbackPatternLED_Clock7,
  MLInputControllerFeedbackPatternLED_Clock8,
  MLInputControllerFeedbackPatternLED_Clock9,
  MLInputControllerFeedbackPatternLED_Clock10,
  MLInputControllerFeedbackPatternLED_Clock11,
  MLInputControllerFeedbackPatternLED_Clock12,
  MLInputControllerFeedbackPatternLED_Clock1And7,
  MLInputControllerFeedbackPatternLED_Clock2And8,
  MLInputControllerFeedbackPatternLED_Clock3And9,
  MLInputControllerFeedbackPatternLED_Clock4And10,
  MLInputControllerFeedbackPatternLED_Clock5And11,
  MLInputControllerFeedbackPatternLED_Clock6And12,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerFeedbackPatternLED_Ensure32Bits = 0x7FFFFFFF,
} MLInputControllerFeedbackPatternLED;

/*! Feedback effects for LED target. */
typedef enum MLInputControllerFeedbackEffectLED
{
  MLInputControllerFeedbackEffectLED_RotateCW,
  MLInputControllerFeedbackEffectLED_RotateCCW,
  MLInputControllerFeedbackEffectLED_Pulse,
  MLInputControllerFeedbackEffectLED_PaintCW,
  MLInputControllerFeedbackEffectLED_PaintCCW,
  MLInputControllerFeedbackEffectLED_Blink,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerFeedbackEffectLED_Ensure32Bits = 0x7FFFFFFF
} MLInputControllerFeedbackEffectLED;

/*! Feedback colors for LED target. */
typedef enum MLInputControllerFeedbackColorLED
{
  MLInputControllerFeedbackColorLED_BrightMissionRed,
  MLInputControllerFeedbackColorLED_PastelMissionRed,
  MLInputControllerFeedbackColorLED_BrightFloridaOrange,
  MLInputControllerFeedbackColorLED_PastelFloridaOrange,
  MLInputControllerFeedbackColorLED_BrightLunaYellow,
  MLInputControllerFeedbackColorLED_PastelLunaYellow,
  MLInputControllerFeedbackColorLED_BrightNebulaPink,
  MLInputControllerFeedbackColorLED_PastelNebulaPink,
  MLInputControllerFeedbackColorLED_BrightCosmicPurple,
  MLInputControllerFeedbackColorLED_PastelCosmicPurple,
  MLInputControllerFeedbackColorLED_BrightMysticBlue,
  MLInputControllerFeedbackColorLED_PastelMysticBlue,
  MLInputControllerFeedbackColorLED_BrightCelestialBlue,
  MLInputControllerFeedbackColorLED_PastelCelestialBlue,
  MLInputControllerFeedbackColorLED_BrightShaggleGreen,
  MLInputControllerFeedbackColorLED_PastelShaggleGreen,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerFeedbackColorLED_Ensure32Bits = 0x7FFFFFFF
} MLInputControllerFeedbackColorLED;

/*! Feedback effect speed for LED target. */
typedef enum MLInputControllerFeedbackEffectSpeedLED
{
  MLInputControllerFeedbackEffectSpeedLED_Slow,
  MLInputControllerFeedbackEffectSpeedLED_Medium,
  MLInputControllerFeedbackEffectSpeedLED_Fast,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerFeedbackEffectSpeedLED_Ensure32Bits = 0x7FFFFFFF
} MLInputControllerFeedbackEffectSpeedLED;

/*! Feedback patterns for vibration, which can be applied to touchpad and body. */
typedef enum MLInputControllerFeedbackPatternVibe
{
  MLInputControllerFeedbackPatternVibe_None,
  MLInputControllerFeedbackPatternVibe_Click,
  MLInputControllerFeedbackPatternVibe_Bump,
  MLInputControllerFeedbackPatternVibe_DoubleClick,
  MLInputControllerFeedbackPatternVibe_Buzz,
  MLInputControllerFeedbackPatternVibe_Tick,
  MLInputControllerFeedbackPatternVibe_ForceDown,
  MLInputControllerFeedbackPatternVibe_ForceUp,
  MLInputControllerFeedbackPatternVibe_ForceDwell,
  MLInputControllerFeedbackPatternVibe_SecondForceDown,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerFeedbackPatternVibe_Ensure32Bits = 0x7FFFFFFF,
} MLInputControllerFeedbackPatternVibe;

/*! Discrete intensity of feedback. */
typedef enum MLInputControllerFeedbackIntensity
{
  MLInputControllerFeedbackIntensity_Low,
  MLInputControllerFeedbackIntensity_Medium,
  MLInputControllerFeedbackIntensity_High,
  /*! Ensure enum is represented as 32 bits. */
  MLInputControllerFeedbackIntensity_Ensure32Bits = 0x7FFFFFFF,
} MLInputControllerFeedbackIntensity;

/*!
  \brief A structure containing callbacks for input keyboard events. The final
  parameter to all the callbacks is a void *, which will point to whatever
  payload data the user provides in MLInputSetKeyboardCallbacks. Individual
  callbacks which are not required by the client can be NULL.
*/
typedef struct MLInputKeyboardCallbacks {
  /*! This callback will be invoked whenever a printable char is typed on the keyboard. */
  void (*on_char)(uint32_t char_utf32, void *data);
  /*! This callback will be invoked whenever any key is pressed. */
  void (*on_key_down)(MLKeyCode key_code, uint32_t modifier_mask, void *data);
  /*! This callback will be invoked whenever any key is released. */
  void (*on_key_up)(MLKeyCode key_code, uint32_t modifier_mask, void *data);
} MLInputKeyboardCallbacks;

/*!
  \brief A structure containing information about the current keyboard state. Note that
  all connected keyboards will be muxed together into one state.
*/
typedef struct MLInputKeyboardState {
  /*! Current state of keys: true if currently pressed, false otherwise. */
  bool key_state[MLKEYCODE_COUNT];
  /*! Currently active modifier keys. */
  uint32_t modifier_mask;
} MLInputKeyboardState;

/*!
  \brief Types of input tablet devices recognized.
  \apilevel 5
*/
typedef enum MLInputTabletDeviceType {
  MLInputTabletDeviceType_Unknown  = 0,
  MLInputTabletDeviceType_Wacom = 1,
  MLInputTabletDeviceType_Ensure32Bits = 0x7FFFFFFF
} MLInputTabletDeviceType;

/*!
  \brief Types of tools used with the tablet device.
  \apilevel 5
*/
typedef enum MLInputTabletDeviceToolType {
  MLInputTabletDeviceToolType_Unknown = 0,
  MLInputTabletDeviceToolType_Pen = 1,
  MLInputTabletDeviceToolType_Eraser = 2,
  MLInputTabletDeviceTootlType_Ensure32Bits = 0x7FFFFFFF
} MLInputTabletDeviceToolType;

/*!
  \brief Buttons on input tablet device.
  \apilevel 5
*/
typedef enum MLInputTabletDeviceButton {
  MLInputTabletDeviceButton_Unknown,
  MLInputTabletDeviceButton_Button1,
  MLInputTabletDeviceButton_Button2,
  MLInputTabletDeviceButton_Button3,
  MLInputTabletDeviceButton_Button4,
  MLInputTabletDeviceButton_Button5,
  MLInputTabletDeviceButton_Button6,
  MLInputTabletDeviceButton_Button7,
  MLInputTabletDeviceButton_Button8,
  MLInputTabletDeviceButton_Button9,
  MLInputTabletDeviceButton_Button10,
  MLInputTabletDeviceButton_Button11,
  MLInputTabletDeviceButton_Button12,
  MLInputTabletDeviceButton_Count,
  MLInputTabletDeviceButton_Ensure32Bits = 0x7FFFFFFF
} MLInputTabletDeviceButton;

/*!
  \brief Mask value to determine the validity of #MLInputTabletDeviceStateMask.
  \apilevel 5
*/
typedef enum MLInputTabletDeviceStateMask {
  /*! Mask for determinig the validity of the additional pen data. */
  MLInputTabletDeviceStateMask_HasType = 1 << 0,
  MLInputTabletDeviceStateMask_HasToolType = 1 << 1,
  MLInputTabletDeviceStateMask_HasPenTouchPosAndForce = 1 << 2,
  MLInputTabletDeviceStateMask_HasAdditionalPenTouchData = 1 << 3,
  MLInputTabletDeviceStateMask_HasPenTouchActive = 1 << 4,
  MLInputTabletDeviceStateMask_HasConnectionState = 1 << 5,
  MLInputTabletDeviceStateMask_HasPenDistance = 1 << 6,
  MLInputTabletDeviceStateMask_HasTimestamp = 1 << 7,
} MLInputTabletDeviceStateMask ;

/*!
  \brief A structure containing information about the state of the tablet device.
  \apilevel 5
*/
typedef struct MLInputTabletDeviceState {
  /*! Version of this structure. */
  uint32_t version;
  /*! Type of this tablet device. */
  MLInputTabletDeviceType type;
  /*! Type of tool used with the tablet. */
  MLInputTabletDeviceToolType tool_type;
  /*!
    \brief Current touch position (x,y) and force (z).
    Position is in the [-1.0,1.0] range and force is in the [0.0,1.0] range.
  */
  MLVec3f pen_touch_pos_and_force;
  /*!
    \brief Additional coordinate values (x, y, z).
    It could contain data specific to the device type.
    For example, it could hold tilt values while using pen.
  */
  int32_t additional_pen_touch_data[3];
  /*! Is touch active. */
  bool is_pen_touch_active;
  /*! If this tablet is connected. */
  bool is_connected;
  /*! Distance between pen and tablet. */
  float pen_distance;
  /*! Time stamp of the event. */
  uint64_t timestamp;
  /*!
    \brief Flags to denote which of the above fields are valid.
    #MLInputTabletDeviceStateMask defines the bitmap.
    */
  uint32_t valid_fields_flag;
} MLInputTabletDeviceState;

/*!
  \brief Initializes default values for MLInputTabletDeviceState.
  \apilevel 5
  \param[in,out] in_out_attr_list The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLInputTabletDeviceStateInit(MLInputTabletDeviceState *in_out_attr_list) {
  if (in_out_attr_list) {
    in_out_attr_list->version = 1u;
    in_out_attr_list->type = MLInputTabletDeviceType_Unknown;
    in_out_attr_list->tool_type = MLInputTabletDeviceToolType_Unknown;
    in_out_attr_list->pen_touch_pos_and_force.x = 0.0f;
    in_out_attr_list->pen_touch_pos_and_force.y = 0.0f;
    in_out_attr_list->pen_touch_pos_and_force.z = 0.0f;
    size_t i = 0;
    for (i = 0; i < 3; i++) {
      in_out_attr_list->additional_pen_touch_data[i] = 0;
    }
    in_out_attr_list->is_pen_touch_active = false;
    in_out_attr_list->is_connected = false;
    in_out_attr_list->pen_distance = 0.0f;
    in_out_attr_list->timestamp = 0u;
    in_out_attr_list->valid_fields_flag = 0u;
  }
}

/*!
  \brief Stores a list of #MLInputTabletDeviceState.
  \apilevel 5
*/
typedef struct MLInputTabletDeviceStatesList {
  /*! Version of this structure. */
  uint32_t version;
  /*! Number of tablet device states in this list. */
  uint32_t count;
  /*! Pointer referring to the array of #MLInputTabletDeviceState. */
  const MLInputTabletDeviceState *data;
} MLInputTabletDeviceStatesList;

/*!
  \brief Initializes default values for #MLInputTabletDeviceStatesList.
  \apilevel 5
  \param[in,out] in_out_attr_list The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLInputTabletDeviceStatesListInit(MLInputTabletDeviceStatesList *in_out_attr_list) {
  if (in_out_attr_list) {
    in_out_attr_list->version = 1u;
    in_out_attr_list->count = 0u;
    in_out_attr_list->data = NULL;
  }
}

/*!
  \brief A structure to contain information about the connected devices.
  \apilevel 5
*/
typedef struct MLInputConnectedDevicesList {
  /*! Version of this structure. */
  uint32_t version;
  /*! Number of connected controllers. */
  uint32_t connected_controller_count;
  /*! Pointer to the array of connected controller IDs. */
  const uint8_t *connected_controller_ids;
  /*! Number of connected tablet devices. */
  uint32_t connected_tablet_device_count;
  /*! Pointer to the array of connected tablet device IDs. */
  const uint8_t *connected_tablet_device_ids;
} MLInputConnectedDevicesList;

/*!
  \brief Initializes default values for #MLInputConnectedDevicesList.
  \apilevel 5
  \param[in,out] in_out_devices_list The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLInputConnectedDevicesListInit(MLInputConnectedDevicesList *in_out_devices_list) {
  if (in_out_devices_list) {
    in_out_devices_list->version = 1u;
    in_out_devices_list->connected_controller_count = 0u;
    in_out_devices_list->connected_controller_ids = NULL;
    in_out_devices_list->connected_tablet_device_count = 0u;
    in_out_devices_list->connected_tablet_device_ids = NULL;
  }
}

/*!
  \brief A structure containing callbacks for input tablet device events.

  The final parameter to all the callbacks is a void *, which will point to whatever
  payload data the user provides in MLInputSetTabletDeviceCallbacks. Individual
  callbacks which are not required by the client can be NULL.

  \apilevel 5
*/
typedef struct MLInputTabletDeviceCallbacks {
  /*! Version of this callback. */
  uint32_t version;
  /*!
    \brief This callback will be invoked whenever a pen touch event is detected.
    \param[in] table_device_id ID of the tablet device.
    \param[in] tablet_device_state Pointer to the struct containing state of the tablet device.
    \param[out] user_data User data as passed to MLInputSetTabletDeviceCallbacks().
  */
  void (*on_pen_touch_event)(uint8_t tablet_device_id, const MLInputTabletDeviceState *tablet_device_state, void *data);
  /*!
    \brief This callback will be invoked whenever a touch ring event is detected.
    \param[in] tablet_device_id ID of the tablet device.
    \param[in] touch_ring_value Touch ring value.
    \param[in] timestamp Time at which this event occured.
    \param[out] user_data User data as passed to MLInputSetTabletDeviceCallbacks().
  */
  void (*on_touch_ring_event)(uint8_t tablet_device_id, int32_t touch_ring_value, uint64_t timestamp, void *data);
  /*!
    \brief This callback will be invoked whenever a tablet device button press is detected.
    \param[in] table_device_id ID of the tablet device.
    \param[in] tablet_device_button Value of the tablet device button.
    \param[in] timestamp Time at which this event occured.
    \param[out] user_data User data as passed to MLInputSetTabletDeviceCallbacks().
  */
  void (*on_button_down)(uint8_t tablet_device_id, MLInputTabletDeviceButton tablet_device_button, uint64_t timestamp, void *data);
  /*!
    \brief This callback will be invoked whenever a tablet device button release is detected.
    \param[in] table_device_id ID of the tablet device.
    \param[in] tablet_device_button Value of the tablet device button.
    \param[in] timestamp Time at which this event occured.
    \param[out] user_data User data as passed to MLInputSetTabletDeviceCallbacks().
  */
  void (*on_button_up)(uint8_t tablet_device_id, MLInputTabletDeviceButton tablet_device_button, uint64_t timestamp, void *data);
  /*!
    \brief This callback will be invoked whenever a tablet device is connected.
    \param[in] table_device_id ID of the tablet device.
    \param[out] user_data User data as passed to MLInputSetTabletDeviceCallbacks().
  */
  void (*on_connect)(uint8_t tablet_device_id, void *data);
  /*!
    \brief This callback will be invoked whenever a tablet device is disconnected.
    \param[in] table_device_id ID of the tablet device.
    \param[out] user_data User data as passed to MLInputSetTabletDeviceCallbacks().
  */
  void (*on_disconnect)(uint8_t tablet_device_id, void *data);
} MLInputTabletDeviceCallbacks;

/*!
  \brief Initializes default values for MLInputTabletDeviceCallbacks.
  \apilevel 5
  \param[in,out] in_out_callbacks The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLInputTabletDeviceCallbacksInit(MLInputTabletDeviceCallbacks *in_out_callbacks) {
  if (in_out_callbacks) {
    in_out_callbacks->version = 1u;
    in_out_callbacks->on_pen_touch_event = NULL;
    in_out_callbacks->on_touch_ring_event = NULL;
    in_out_callbacks->on_button_down = NULL;
    in_out_callbacks->on_button_up = NULL;
    in_out_callbacks->on_connect = NULL;
    in_out_callbacks->on_disconnect = NULL;
  }
}

/*! Desired configuration of the input system. */
typedef struct MLInputConfiguration {
  /*!
    \deprecated Deprecated since 0.23.0. Scheduled for removal. Instead use ml_controller.h.
    \brief Desired degrees-of-freedom mode of each controller.
  */
  MLInputControllerDof dof[MLInput_MaxControllers];
} MLInputConfiguration;

/*!
 \brief Returns an ASCII string for MLInputsResult and MLResult codes.

 \apilevel 8

 \param[in] result_code The input MLResult enum from MLInput functions.

 \return ASCII string containing readable version of result code.

 \priv None
*/
ML_API const char *ML_CALL MLInputGetResultString(MLResult result);

/*!
  \brief Create an input tracker.

  \param[in] config #MLInputConfiguration specifying desired configuration. Pass NULL for default configuration.
  \param[out] out_handle A handle to the created input tracker. Only valid if the return value is MLResult_Ok.

  \retval MLResult_InvalidParam Input tracker was not created due to invalid out_handle.
  \retval MLResult_Ok Input tracker was created successfully.
  \retval MLResult_UnspecifiedFailure Input tracker was not created due to an unknown error.
  \retval MLInputResult_IllegalState Input tracker was not created because the service was in an unexpected state.

  \priv None
*/
ML_API MLResult ML_CALL MLInputCreate(const MLInputConfiguration *config, MLHandle *out_handle);

/*!
  \deprecated Deprecated since 0.24.0. Scheduled for removal. Replaced by MLInputSetControllerCallbacksEx.

  \brief Sets the callbacks for controller input events.

  \param[in] handle Handle to the input tracker.
  \param[in] controller_callbacks Pointer to #MLInputControllerCallbacks structure (can be NULL).
  \param[in] user_data Pointer to user payload data; this will be the first argument
             passed to all the callback functions (can be NULL).

  \retval MLResult_InvalidParam Failed to set the callback for controller input events due to an invalid handle.
  \retval MLResult_Ok Successfully set the callback for controller input events.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_DEPRECATED_MSG("Replaced by MLInputSetControllerCallbacksEx.")
ML_API MLResult ML_CALL MLInputSetControllerCallbacks(MLHandle handle, const MLInputControllerCallbacks *controller_callbacks, void *user_data);

/*!
  \brief Sets the callbacks for controller input events.

  \param[in] handle Handle to the input tracker.
  \param[in] controller_callbacks Pointer to #MLInputControllerCallbacksEx structure (can be NULL).
  \param[in] user_data Pointer to user payload data; this will be the first argument
             passed to all the callback functions (can be NULL).

  \retval MLResult_InvalidParam Failed to set the callback for controller input events due to an invalid handle.
  \retval MLResult_Ok Successfully set the callback for controller input events.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None

  \apilevel 8
*/
ML_API MLResult ML_CALL MLInputSetControllerCallbacksEx(MLHandle handle, const MLInputControllerCallbacksEx *controller_callbacks, void *user_data);

/*!
  \brief Return current state of all possible input controllers.

  \param[in] handle Handle to the input tracker.
  \param[out] out_state Array, of at least size #MLInput_MaxControllers, of
              #MLInputControllerState structures that will be populated.

  \retval MLResult_InvalidParam Fetching the controller state failed due to an invalid handle.
  \retval MLResult_Ok Successfully fetched the controller state.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLInputGetControllerState(MLHandle handle, MLInputControllerState out_state[MLInput_MaxControllers]);

/*!
  \brief Starts a vibe feedback pattern on the specified controller.

  \param[in] handle Handle to the input tracker.
  \param[in] controller_id ID of the input controller.
  \param[in] pattern Element of #MLInputControllerFeedbackPatternVibe type specifying pattern to start.
  \param[in] intensity Element of #MLInputControllerFeedbackIntensity type specifying intensity.

  \retval MLResult_InvalidParam Failed to start a vibe feedback pattern due to an invalid handle.
  \retval MLResult_Ok Successfully started a vibe feedback pattern on the specified controller.
  \retval MLResult_UnspecifiedFailure Failed to start a vibe feedback pattern due to an unknown error.
  \retval MLInputResult_DeviceNotFound Failed to start a vibe feedback pattern because the device was not found.
  \retval MLInputResult_IllegalState Failed to start a vibe feedback pattern because the service was in an unexpected state.
  \retval MLInputResult_PermissionDenied Failed to start a vibe feedback pattern because of permission denial.

  \priv None
*/
ML_API MLResult ML_CALL MLInputStartControllerFeedbackPatternVibe(MLHandle handle, uint8_t controller_id, MLInputControllerFeedbackPatternVibe pattern, MLInputControllerFeedbackIntensity intensity);

/*!
  \brief Starts a LED feedback pattern on the specified controller.

  \param[in] handle Handle to the input tracker.
  \param[in] controller_id ID of the input controller.
  \param[in] pattern Element of #MLInputControllerFeedbackPatternLED type specifying pattern to start.
  \param[in] color Element of #MLInputControllerFeedbackColorLED type specifying color.
  \param[in] duration Duration in milliseconds.

  \retval MLResult_InvalidParam Failed to start a LED feedback pattern due to an invalid handle.
  \retval MLResult_Ok Successfully started a LED feedback pattern on the specified controller.
  \retval MLResult_UnspecifiedFailure Failed to start a LED feedback pattern due to an unknown error.
  \retval MLInputResult_DeviceNotFound Failed to start a LED feedback pattern because the device was not found.
  \retval MLInputResult_IllegalState Failed to start a LED feedback pattern because the service was in an illegal state.
  \retval MLInputResult_PermissionDenied Failed to start a LED feedback pattern because of permission denial.

  \priv None
*/
ML_API MLResult ML_CALL MLInputStartControllerFeedbackPatternLED(MLHandle handle, uint8_t controller_id, MLInputControllerFeedbackPatternLED pattern, MLInputControllerFeedbackColorLED color, uint32_t duration);

/*!
  \brief Starts a LED feedback effect using the specified pattern on the specified controller.

  \param[in] handle Handle to the input tracker.
  \param[in] controller_id ID of the input controller.
  \param[in] effect Element of #MLInputControllerFeedbackEffectLED type specifying effect to start.
  \param[in] speed Element of #MLInputControllerFeedbackEffectSpeedLED type specifying speed.
  \param[in] pattern Element of #MLInputControllerFeedbackPatternLED type specifying pattern to use.
  \param[in] color Element of #MLInputControllerFeedbackColorLED type specifying color.
  \param[in] duration Duration in milliseconds.

  \retval MLResult_InvalidParam Failed to start a LED feedback effect due to an invalid handle.
  \retval MLResult_Ok Successfully started a LED feedback effect using the specified pattern on the specified controller.
  \retval MLResult_UnspecifiedFailure Failed to start a LED feedback effect due to an unknown error.
  \retval MLInputResult_DeviceNotFound Failed to start a LED feedback effect because the device was not found.
  \retval MLInputResult_IllegalState Failed to start a LED feedback effect because the service was in an illegal state.
  \retval MLInputResult_PermissionDenied Failed to start a LED feedback effect because of permission denial.

  \priv None
*/
ML_API MLResult ML_CALL MLInputStartControllerFeedbackPatternEffectLED(MLHandle handle, uint8_t controller_id, MLInputControllerFeedbackEffectLED effect, MLInputControllerFeedbackEffectSpeedLED speed, MLInputControllerFeedbackPatternLED pattern, MLInputControllerFeedbackColorLED color, uint32_t duration);

/*!
  \brief Sets the callbacks for keyboard input events.

  \param[in] handle Handle to the input tracker.
  \param[in] keyboard_callbacks Pointer to #MLInputKeyboardCallbacks structure (can be NULL).
  \param[in] user_data Pointer to user payload data; this will be the first argument
             passed to all the callback functions (can be NULL).

  \retval MLResult_InvalidParam Failed to set the callback for keyboard input events due to an invalid handle.
  \retval MLResult_Ok Successfully set the callback for keyboard input events.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLInputSetKeyboardCallbacks(MLHandle handle, const MLInputKeyboardCallbacks *keyboard_callbacks, void *user_data);

/*!
  \brief Return current state of keyboard keys (down=true).

  \param[in] handle Handle to the input tracker.
  \param[out] out_state Pointer to #MLInputKeyboardState structure that will receive data.

  \retval MLResult_InvalidParam Failed to fetch the keyboard state due to an invalid handle.
  \retval MLResult_Ok Successfully fetched the keyboard state.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLInputGetKeyboardState(MLHandle handle, MLInputKeyboardState *out_state);

/*!
  \brief    Gets the device IDs of all connected devices.

  \apilevel 5

  \param[in]      handle Handle to the input tracker.
  \param[in,out]  out_devices Pointer to #MLInputConnectedDevicesList structure that will be populated.
                  The app should call #MLInputReleaseConnectedDevicesList to release the contents after use.

  \retval         MLResult_InvalidParam Failed to get the IDs of all connected devices.
  \retval         MLResult_Ok Successfully fetched the IDs of the connected devices.
  \retval         MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv           None
*/
ML_API MLResult ML_CALL MLInputGetConnectedDevices(MLHandle handle, MLInputConnectedDevicesList *out_devices);

/*!
  \brief Release the contents of #MLInputConnectedDevicesList populated by #MLInputGetConnectedDevices.

  \apilevel 5

  \param[in]      handle Handle to the input tracker.
  \param[in]      devices Pointer to #MLInputConnectedDevicesList struct. Its contents will be released.

  \retval         MLResult_Ok If operation was successful.
  \retval         MLResult_InvalidParam Releasing the contents failed due to an invalid param.
  \retval         MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv           None
*/
ML_API MLResult ML_CALL MLInputReleaseConnectedDevicesList(MLHandle handle, MLInputConnectedDevicesList *devices);

/*!
  \brief    Sets the callbacks for tablet device input events.

  \apilevel 5

  \param[in]      handle Handle to the input tracker.
  \param[in]      touch_device_callbacks Pointer to #MLInputTabletDeviceCallbacks structure (can be NULL).
  \param[in]      user_data Pointer to user payload data (can be NULL).

  \retval         MLResult_InvalidParam Failed to set the callback for tablet device input events due to an invalid param.
  \retval         MLResult_Ok Successfully set the callbacks for tablet device input events.
  \retval         MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv           None
*/
ML_API MLResult ML_CALL MLInputSetTabletDeviceCallbacks(MLHandle handle, const MLInputTabletDeviceCallbacks *touch_device_callbacks, void *user_data);

/*!
  \brief Return a list of states of the tablet device.

  This API will return all the states of tablet device since the last query upto a maximum of 20 states.
  The memory used to store the list is maintained by the library. User should call
  MLInputReleaseTabletDeviceStates() to release it.

  \apilevel 5

  \param[in]      handle Handle to the input tracker.
  \param[in]      tablet_device_id ID of the tablet device to fetch the states from.
  \param[in,out]  out_state Pointer to #MLInputTabletDeviceStatesList structure that will be populated.
                  The app should call #MLInputReleaseTabletDeviceStates to release the contents after use.

  \retval         MLResult_Ok Successfully fetched the tablet device state.
  \retval         MLResult_InvalidParam Fetching tablet device states failed due to an invalid param.
  \retval         MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv           None
*/
ML_API MLResult ML_CALL MLInputGetTabletDeviceStates(MLHandle handle, uint8_t tablet_device_id, MLInputTabletDeviceStatesList *out_state);

/*!
  \brief Release the contents of #MLInputTabletDeviceStatesList populated by #MLInputGetTabletDeviceStates.

  \apilevel 5

  \param[in]      handle Handle to the input tracker.
  \param[in]      tablet_device_states Pointer to #MLInputTabletDeviceStatesList structure. Its contents will be released.

  \retval         MLResult_Ok If operation was successful.
  \retval         MLResult_InvalidParam Releasing the contents failed due to an invalid param.
  \retval         MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv           None
*/
ML_API MLResult ML_CALL MLInputReleaseTabletDeviceStates(MLHandle handle, MLInputTabletDeviceStatesList *tablet_device_states);

/*!
  \brief Destroy an input tracker.

  \param[in] handle #MLHandle to the input tracker to destroy.

  \retval MLResult_InvalidParam Failed to destroy the input tracker due to an invalid handle.
  \retval MLResult_Ok Successfully destroyed the input tracker.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLInputResult_IllegalState Input tracker could not be destroyed because the service was in an unexpected state.

  \priv None
*/
ML_API MLResult ML_CALL MLInputDestroy(MLHandle handle);

/*! \} */

ML_EXTERN_C_END
