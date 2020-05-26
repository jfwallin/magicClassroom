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
  \addtogroup CameraMetadata
  \brief Enumeration definitions for the various metadata entries.

  \{
*/

/*! Color correction mode. */
typedef enum MLCameraMetadataColorCorrectionMode {
  /*! Transform Matrix. */
  MLCameraMetadataColorCorrectionMode_TransformMatrix = 0,
  /*! Fast. */
  MLCameraMetadataColorCorrectionMode_Fast,
  /*! High Quality. */
  MLCameraMetadataColorCorrectionMode_HighQuality,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraMetadataColorCorrectionMode_Ensure32Bits  = 0x7FFFFFFF
} MLCameraMetadataColorCorrectionMode;

/*! Color correction aberration mode. */
typedef enum MLCameraMetadataColorCorrectionAberrationMode {
  /*! Off. */
  MLCameraMetadataColorCorrectionAberrationMode_Off = 0,
  /*! Fast. */
  MLCameraMetadataColorCorrectionAberrationMode_Fast,
  /*! High Quality. */
  MLCameraMetadataColorCorrectionAberrationMode_HighQuality,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraMetadataColorCorrectionAberrationMode_Ensure32Bits  = 0x7FFFFFFF
} MLCameraMetadataColorCorrectionAberrationMode;

/*! Control AE anti-banding mode. */
typedef enum MLCameraMetadataControlAEAntibandingMode {
  /*! Off. */
  MLCameraMetadataControlAEAntibandingMode_Off = 0,
  /*! 50Hz. */
  MLCameraMetadataControlAEAntibandingMode_50Hz,
  /*! 60Hz. */
  MLCameraMetadataControlAEAntibandingMode_60Hz,
  /*! Auto. */
  MLCameraMetadataControlAEAntibandingMode_Auto,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraMetadataControlAEAntibandingMode_Ensure32Bits  = 0x7FFFFFFF
} MLCameraMetadataControlAEAntibandingMode;

/*! Control AE lock. */
typedef enum MLCameraMetadataControlAELock {
  /*! Off. */
  MLCameraMetadataControlAELock_Off = 0,
  /*! On. */
  MLCameraMetadataControlAELock_On,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraMetadataControlAELock_Ensure32Bits  = 0x7FFFFFFF
} MLCameraMetadataControlAELock;

/*!  Control AE mode. */
typedef enum MLCameraMetadataControlAEMode {
  /*! Off. */
  MLCameraMetadataControlAEMode_Off = 0,
  /*! On. */
  MLCameraMetadataControlAEMode_On,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraMetadataControlAEMode_Ensure32Bits  = 0x7FFFFFFF
} MLCameraMetadataControlAEMode;

/*! Control AWB lock. */
typedef enum MLCameraMetadataControlAWBLock {
  /*! Off. */
  MLCameraMetadataControlAWBLock_Off = 0,
  /*! On. */
  MLCameraMetadataControlAWBLock_On,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraMetadataControlAWBLock_Ensure32Bits  = 0x7FFFFFFF
} MLCameraMetadataControlAWBLock;

/*! Control AWB mode. */
typedef enum MLCameraMetadataControlAWBMode {
  /*! Off. */
  MLCameraMetadataControlAWBMode_Off = 0,
  /*! Auto. */
  MLCameraMetadataControlAWBMode_Auto,
  /*! Incandescent. */
  MLCameraMetadataControlAWBMode_Incandescent,
  /*! Fluorescent. */
  MLCameraMetadataControlAWBMode_Fluorescent,
  /*! Warm fluorescent. */
  MLCameraMetadataControlAWBMode_WarmFluorescent,
  /*! Daylight. */
  MLCameraMetadataControlAWBMode_Daylight,
  /*! Cloudy daylight. */
  MLCameraMetadataControlAWBMode_CloudyDaylight,
  /*! Twilight. */
  MLCameraMetadataControlAWBMode_Twilight,
  /*! Shade. */
  MLCameraMetadataControlAWBMode_Shade,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraMetadataControlAWBMode_Ensure32Bits  = 0x7FFFFFFF
} MLCameraMetadataControlAWBMode;

/*! Control AE state. */
typedef enum MLCameraMetadataControlAEState {
  /*! Inactive. */
  MLCameraMetadataControlAEState_Inactive = 0,
  /*! Searching. */
  MLCameraMetadataControlAEState_Searching,
  /*! Converged. */
  MLCameraMetadataControlAEState_Converged,
  /*! Locked. */
  MLCameraMetadataControlAEState_Locked,
  /*! Flash required. */
  MLCameraMetadataControlAEState_FlashRequired,
  /*! Pre-capture. */
  MLCameraMetadataControlAEState_PreCapture,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraMetadataControlAEState_Ensure32Bits  = 0x7FFFFFFF
} MLCameraMetadataControlAEState;

/*! Control AWB state. */
typedef enum MLCameraMetadataControlAWBState {
  /*! Inactive. */
  MLCameraMetadataControlAWBState_Inactive = 0,
  /*! Searching. */
  MLCameraMetadataControlAWBState_Searching,
  /*! Converged. */
  MLCameraMetadataControlAWBState_Converged,
  /*! Locked. */
  MLCameraMetadataControlAWBState_Locked,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraMetadataControlAWBState_Ensure32Bits  = 0x7FFFFFFF
} MLCameraMetadataControlAWBState;

/*! Scaler available formats. */
typedef enum MLCameraMetadataScalerAvailableFormats {
  /*! Raw16. */
  MLCameraMetadataScalerAvailableFormats_RAW16                  = 0x20,
  /*! Raw opaque. */
  MLCameraMetadataScalerAvailableFormats_RAW_OPAQUE             = 0x24,
  /*! TV12. */
  MLCameraMetadataScalerAvailableFormats_YV12                   = 0x32315659,
  /*! YCrCb 420 SP. */
  MLCameraMetadataScalerAvailableFormats_YCrCb_420_SP           = 0x11,
  /*! Implementation defined. */
  MLCameraMetadataScalerAvailableFormats_IMPLEMENTATION_DEFINED = 0x22,
  /*! YCrCb 420 888. */
  MLCameraMetadataScalerAvailableFormats_YCbCr_420_888          = 0x23,
  /*! BLOB. */
  MLCameraMetadataScalerAvailableFormats_BLOB                   = 0x21,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraMetadataScalerAvailableFormats_Ensure32Bits           = 0x7FFFFFFF
} MLCameraMetadataScalerAvailableFormats;

/*! Scaler available stream configuration. */
typedef enum MLCameraMetadataScalerAvailableStreamConfigurations {
  /*! Output. */
  MLCameraMetadataScalerAvailableStreamConfigurations_OUTPUT = 0,
  /*! Input. */
  MLCameraMetadataScalerAvailableStreamConfigurations_INPUT,
  /*! Ensure enum is represented as 32 bits. */
  MLCameraMetadataScalerAvailableStreamConfigurations_Ensure32Bits = 0x7FFFFFFF
} MLCameraMetadataScalerAvailableStreamConfigurations;

/*! \} */

ML_EXTERN_C_END
