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
// ---------------------------------------------------------------------
// %BANNER_END%

#pragma once

#include "ml_api.h"

ML_EXTERN_C_BEGIN

/*!
  \defgroup LightingTracking Lighting Tracking
  \addtogroup LightingTracking
  \sharedobject ml_perception_client
  \brief APIs for the Lighting Tracking system.

  - Capturing images or video will stop the lighting information update,
    causing the retrieved data to be stale (old timestamps).
  \{
*/

/*! Which camera the information is related to. */
typedef enum MLLightingTrackingCamera {
  /*! Left. */
  MLLightingTrackingCamera_Left,
  /*! Right. */
  MLLightingTrackingCamera_Right,
  /*! Far left. */
  MLLightingTrackingCamera_FarLeft,
  /*! Far right. */
  MLLightingTrackingCamera_FarRight,
  /*! Number of cameras. */
  MLLightingTrackingCamera_Count,
  /*! Ensure enum is represented as 32 bits. */
  MLLightingTrackingCamera_Ensure32Bits = 0x7FFFFFFF
} MLLightingTrackingCamera;

enum {
  MLLightingTracking_GridWidth = 8,
  MLLightingTracking_GridHeight = 6,
};

/*! Information about the ambient light sensor global state. */
typedef struct MLLightingTrackingAmbientGlobalState {
  /*!
    \brief Array stores values for each world camera, ordered left, right, far left, far right.
    Luminance estimate is in nits (cd/m^2).
  */
  uint16_t als_global[MLLightingTrackingCamera_Count];
  /*! Time when captured in nanoseconds since the Epoch. */
  uint64_t timestamp_ns;
} MLLightingTrackingAmbientGlobalState;

/*! Information about the ambient light sensor grid state. */
typedef struct MLLightingTrackingAmbientGridState {
  /*!
    \brief Array stores grid for each world camera, ordered left, right, far left, far right.
    Luminance estimate is in nits (cd/m^2).
  */
  uint16_t als_grid[MLLightingTrackingCamera_Count][MLLightingTracking_GridHeight][MLLightingTracking_GridWidth];
  /*! Time when captured in nanoseconds since the Epoch. */
  uint64_t timestamp_ns;
} MLLightingTrackingAmbientGridState;

/*! Information about the color temperature state. */
typedef struct MLLightingTrackingColorTemperatureState {
  /*! Color temperature is in Kelvin */
  uint16_t color_temp;
  uint16_t R_raw_pixel_avg;
  uint16_t G_raw_pixel_avg;
  uint16_t B_raw_pixel_avg;
  /*!
    \brief The Chromaticity coordinates (x, y) are based on standard CIE tristimulus
    values (XYZ) and may be computed as follows:
    x = X / (X + Y + Z)
    y = Y / (X + Y + Z)

    The Correlated Color Temperature (CCT) may be computed as follows:
    CCT = 449 * n^3 + 3525 * n^2 + 6823.3 * n + 5520.33 (McCamy's Formula)
    where n = (x − 0.3320) / (0.1858 − y)

    NOTE: Y is a measure of the illuminance in nits.
  */
  float cie_x;
  /*!
    \see #cie_x
  */
  float cie_y;
  /*!
    \see #cie_x
  */
  float cie_z;
  /*! Time when captured in nanoseconds since the Epoch. */
  uint64_t timestamp_ns;
} MLLightingTrackingColorTemperatureState;

/*!
  \brief Create a Lighting Tracker.

  \param[out] out_handle A pointer to an #MLHandle which will contain the handle to the lighting tracker.
              If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam The parameter out_handle was not valid (null).
  \retval MLResult_Ok Lighting tracker has been created successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLLightingTrackingCreate(MLHandle *out_handle);

/*!
  \brief  Destroy a Lighting Tracker.

  \param[in] lighting_tracker A handle to a lighting Tracker created by MLLightingTrackingCreate().

  \retval MLResult_Ok The lighting tracker was successfully destroyed.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLLightingTrackingDestroy(MLHandle lighting_tracker);

/*!
  \brief  Gets information about the ambient light sensor global state.

  \param[in] lighting_tracker A handle to a Lighting Tracker created by MLLightingTrackingCreate().
  \param[out] out_state Information about the global lighting state.

  \retval MLResult_InvalidParam The parameter out_state was not valid (null).
  \retval MLResult_Ok Received valid ambient blobal state data.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLLightingTrackingGetAmbientGlobalState(MLHandle lighting_tracker, MLLightingTrackingAmbientGlobalState *out_state);

/*!
  \brief Gets information about the ambient light sensor grid state.

  \param[in] lighting_tracker A handle to a Lighting Tracker created by MLLightingTrackingCreate().
  \param[out] out_state Information about the grid lighting state.

  \retval MLResult_InvalidParam The parameter out_state was not valid (null).
  \retval MLResult_Ok Received valid ambient grid state data.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLLightingTrackingGetAmbientGridState(MLHandle lighting_tracker, MLLightingTrackingAmbientGridState *out_state);

/*!
  \brief Gets information about the color temperature state.

  \param[in] lighting_tracker A handle to a Lighting Tracker created by MLLightingTrackingCreate().
  \param[out] out_state Information about the color temperature state.

  \retval MLResult_InvalidParam The parameter out_state was not valid (null).
  \retval MLResult_Ok Received color temperature state successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLLightingTrackingGetColorTemperatureState(MLHandle lighting_tracker, MLLightingTrackingColorTemperatureState *out_state);

/*! \} */

ML_EXTERN_C_END
