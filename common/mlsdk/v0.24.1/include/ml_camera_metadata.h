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
#include "ml_camera_metadata_tags.h"

ML_EXTERN_C_BEGIN

/*!
  \defgroup CameraMetadata Camera Metadata
  \addtogroup CameraMetadata
  \sharedobject ml_camera_metadata
  \brief APIs for getting/setting camera characteristic/request/result metadata.

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.
  \{
*/

/*! Rational data type. */
typedef struct MLCameraMetadataRational {
  /*! Numerator. */
  int32_t numerator;
  /*! Denominator. */
  int32_t denominator;
} MLCameraMetadataRational;

/*!
  \brief Get color correction aberration modes.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data Array of aberration modes. The Library allocates and maintains the lifetime of the output data.
  \param[out] out_count Number of output data elements.

  \retval MLResult_InvalidParam Failed to obtain color correction aberration modes due to invalid input parameter.
  \retval MLResult_Ok Obtained color correction aberration modes successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain color correction aberration modes due to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetColorCorrectionAvailableAberrationModes(MLHandle handle, MLCameraMetadataColorCorrectionAberrationMode **out_data, size_t *out_count);

/*!
  \brief Get AE modes.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data Array of AE modes. The Library allocates and maintains the lifetime of the output data.
  \param[out] out_count Number of output data elements.

  \retval MLResult_InvalidParam Failed to obtain AE modes due to invalid input parameter.
  \retval MLResult_Ok Obtained AE modes successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE modes due to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAEAvailableModes(MLHandle handle, MLCameraMetadataControlAEMode **out_data, size_t *out_count);

/*!
  \brief Get AE compensation range.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data Array of min (1st) and max (2nd) of AE compensation.

  \retval MLResult_InvalidParam Failed to obtain AE compensation range due to invalid input parameter.
  \retval MLResult_Ok Obtained AE compensation range successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE compensation range to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAECompensationRange(MLHandle handle, int32_t out_data[2]);

/*!
  \brief Get AE compensation step.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data AE compensation step.

  \retval MLResult_InvalidParam Failed to obtain AE compensation step due to invalid input parameter.
  \retval MLResult_Ok Obtained AE compensation step successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE compensation step to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAECompensationStep(MLHandle handle, MLCameraMetadataRational *out_data);

/*!
  \brief Get AE lock.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data AE lock.

  \retval MLResult_InvalidParam Failed to obtain AE lock due to invalid input parameter.
  \retval MLResult_Ok Obtained AE lock successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE lock to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAELockAvailable(MLHandle handle, MLCameraMetadataControlAELock *out_data);

/*!
  \brief Get AWB modes.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data Array of AWB modes. The Library allocates and maintains the lifetime of the output data.
  \param[out] out_count Number of output data elements.

  \retval MLResult_InvalidParam Failed to obtain AWB modes due to invalid input parameter.
  \retval MLResult_Ok Obtained AWB modes successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AWB modes to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAWBAvailableModes(MLHandle handle, MLCameraMetadataControlAWBMode **out_data, size_t *out_count);

/*!
  \brief Get AWB lock.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data AWB lock.

  \retval MLResult_InvalidParam Failed to obtain AWB lock due to invalid input parameter.
  \retval MLResult_Ok Obtained AWB lock successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AWB lock to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAWBLockAvailable(MLHandle handle, MLCameraMetadataControlAWBLock *out_data);

/*!
  \brief Get scaler processed sizes list

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data List of [width, height] pairs.
              The Library allocates and maintains the lifetime of the output data.
  \param[out] out_count Number of output data elements(total of 2 x pairs).

  \retval MLResult_InvalidParam Failed to obtain scaler processed sizes list due to invalid input parameter.
  \retval MLResult_Ok Obtained scaler processed sizes list successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain scaler processed sizes list to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetScalerProcessedSizes(MLHandle handle, int32_t **out_data, size_t *out_count);

/*!
  \brief Get scaler available max digital zoom.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data Max digital zoom.

  \retval MLResult_InvalidParam Failed to obtain scaler available max digital zoom due to invalid input parameter.
  \retval MLResult_Ok Obtained scaler available max digital zoom successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain scaler available max digital zoom to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetScalerAvailableMaxDigitalZoom(MLHandle handle, float *out_data);

/*!
  \brief Get scaler available stream configrations.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data List of stream configuration settings.
              Each setting is consisted of [#MLCameraMetadataScalerAvailableFormats, width, height,
              #MLCameraMetadataScalerAvailableStreamConfigurations].
              The Library allocates and maintains the lifetime of the output data.
  \param[out] out_count Number of output data elements(total of 4 x settings).

  \retval MLResult_InvalidParam Failed to obtain scaler available stream configrations due to invalid input parameter.
  \retval MLResult_Ok Obtained scaler available stream configrations successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain scaler available stream configrations to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetScalerAvailableStreamConfigurations(MLHandle handle, int32_t **out_data, size_t *out_count);

/*!
  \brief Get sensor info active array sizes.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data Active array size [left, top, right, bottom].

  \retval MLResult_InvalidParam Failed to obtain sensor info active array sizes due to invalid input parameter.
  \retval MLResult_Ok Obtained sensor info active array sizes successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain sensor info active array sizes to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetSensorInfoActiveArraySize(MLHandle handle, int32_t out_data[4]);

/*!
  \brief Get sensor info sensitivity range.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data Sensor info sensitivity range[min, max].

  \retval MLResult_InvalidParam Failed to obtain sensor info sensitivity range due to invalid input parameter.
  \retval MLResult_Ok Obtained sensor info sensitivity range successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain sensor info sensitivity range to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetSensorInfoSensitivityRange(MLHandle handle, int32_t out_data[2]);

/*!
  \brief Get sensor info exposure time range.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data Sensor info exposure time range[min, max] in microseconds.

  \retval MLResult_InvalidParam Failed to obtain sensor info exposure time range due to invalid input parameter.
  \retval MLResult_Ok Obtained sensor info exposure time range successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain sensor info exposure time range to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetSensorInfoExposureTimeRange(MLHandle handle, int64_t out_data[2]);

/*!
  \brief Get sensor orientation degree.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] out_data Sensor orientation degree.

  \retval MLResult_InvalidParam Failed to obtain sensor orientation degree due to invalid input parameter.
  \retval MLResult_Ok Obtained sensor orientation degree successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain sensor orientation degree to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetSensorOrientation(MLHandle handle, int32_t *out_data);

/*!
  \brief Set sensor info exposure time range. Only max time will be set currently.

  \param[in] handle Camera characteristic metadata handle acquired from #MLCameraGetCameraCharacteristics.
  \param[out] data Sensor info exposure time range[min, max] in microseconds.

  \retval MLResult_InvalidParam Failed to set sensor info exposure time range due to invalid input parameter.
  \retval MLResult_Ok Set sensor info exposure time range successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set sensor info exposure time range to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetSensorInfoExposureTimeRange(MLHandle handle, const int64_t data[2]);

/*!
  \brief Get color correction mode.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data Color correction mode.

  \retval MLResult_InvalidParam Failed to obtain color correction mode due to invalid input parameter.
  \retval MLResult_Ok Obtained color correction mode successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain color correction mode to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetColorCorrectionModeRequestMetadata(MLHandle handle, MLCameraMetadataColorCorrectionMode *out_data);

/*!
  \brief Get color correction transform.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data 3x3 color correction transform.

  \retval MLResult_InvalidParam Failed to obtain color correction transform due to invalid input parameter.
  \retval MLResult_Ok Obtained color correction transform successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain color correction transform to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetColorCorrectionTransformRequestMetadata(MLHandle handle, MLCameraMetadataRational out_data[3][3]);

/*!
  \brief Get color correction gains.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data Color correction gains.

  \retval MLResult_InvalidParam Failed to obtain color correction gains due to invalid input parameter.
  \retval MLResult_Ok Obtained color correction gains successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain color correction gains to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetColorCorrectionGainsRequestMetadata(MLHandle handle, float out_data[4]);

/*!
  \brief Get color correction aberration.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data Color correction aberration.

  \retval MLResult_InvalidParam Failed to obtain color correction aberration due to invalid input parameter.
  \retval MLResult_Ok Obtained color correction aberration successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain color correction aberration to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetColorCorrectionAberrationModeRequestMetadata(MLHandle handle, MLCameraMetadataColorCorrectionAberrationMode *out_data);

/*!
  \brief Get AE antibanding mode.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data AE antibanding mode.

  \retval MLResult_InvalidParam Failed to obtain AE antibanding mode due to invalid input parameter.
  \retval MLResult_Ok Obtained AE antibanding mode successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE antibanding mode to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAEAntibandingModeRequestMetadata(MLHandle handle, MLCameraMetadataControlAEAntibandingMode *out_data);

/*!
  \brief Get AE exposure compensation.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data Exposure compensation value.

  \retval MLResult_InvalidParam Failed to obtain AE exposure compensation due to invalid input parameter.
  \retval MLResult_Ok Obtained AE exposure compensation successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE exposure compensation to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAEExposureCompensationRequestMetadata(MLHandle handle, int32_t *out_data);

/*!
  \brief Get AE lock.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data AE Lock.

  \retval MLResult_InvalidParam Failed to obtain AE lock due to invalid input parameter.
  \retval MLResult_Ok Obtained AE lock successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE lock to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAELockRequestMetadata(MLHandle handle, MLCameraMetadataControlAELock *out_data);

/*!
  \brief Get AE mode.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data AE Lock.

  \retval MLResult_InvalidParam Failed to obtain AE mode due to invalid input parameter.
  \retval MLResult_Ok Obtained AE mode successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE mode to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAEModeRequestMetadata(MLHandle handle, MLCameraMetadataControlAEMode *out_data);

/*!
  \brief Get AE target FPS range.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data AE target FPS range.

  \retval MLResult_InvalidParam Failed to obtain AE target FPS range due to invalid input parameter.
  \retval MLResult_Ok Obtained AE target FPS range successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE target FPS range to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAETargetFPSRangeRequestMetadata(MLHandle handle, int32_t out_data[2]);

/*!
  \brief Get AWB lock.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data AWB Lock.

  \retval MLResult_InvalidParam Failed to obtain AWB lock due to invalid input parameter.
  \retval MLResult_Ok Obtained AWB lock successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AWB lock to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAWBLockRequestMetadata(MLHandle handle, MLCameraMetadataControlAWBLock *out_data);

/*!
  \brief Get AWB mode.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data AWB mode.

  \retval MLResult_InvalidParam Failed to obtain AWB mode due to invalid input parameter.
  \retval MLResult_Ok Obtained AWB mode successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AWB mode to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAWBModeRequestMetadata(MLHandle handle, MLCameraMetadataControlAWBMode *out_data);

/*!
  \brief Get sensor exposure time.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data Sensor exposure time.

  \retval MLResult_InvalidParam Failed to obtain sensor exposure time due to invalid input parameter.
  \retval MLResult_Ok Obtained sensor exposure time successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain sensor exposure time to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetSensorExposureTimeRequestMetadata(MLHandle handle, int64_t *out_data);

/*!
  \brief Get sensor sensitivity.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data Sensor sensitivity.

  \retval MLResult_InvalidParam Failed to obtain sensor exposure time due to invalid input parameter.
  \retval MLResult_Ok Obtained sensor exposure time successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain sensor exposure time to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetSensorSensitivityRequestMetadata(MLHandle handle, int32_t *out_data);

/*!
  \brief Get scaler crop region.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] out_data Cropped region [left, top, right, bottom].

  \retval MLResult_InvalidParam Failed to obtain sensor exposure time due to invalid input parameter.
  \retval MLResult_Ok Obtained sensor exposure time successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain sensor exposure time to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetScalerCropRegionRequestMetadata(MLHandle handle, int32_t out_data[4]);

/*!
  \brief Set color correction mode.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data Color correction mode. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set color correction mode due to invalid input parameter.
  \retval MLResult_Ok Set color correction mode successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set color correction mode to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetColorCorrectionMode(MLHandle handle, const MLCameraMetadataColorCorrectionMode *data);

/*!
  \brief Set color correction transform.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data 3x3 color correction transform. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set color correction transform due to invalid input parameter.
  \retval MLResult_Ok Set color correction transform successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set color correction transform to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetColorCorrectionTransform(MLHandle handle, const MLCameraMetadataRational data[3][3]);

/*!
  \brief Set color correction gains.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data Color correction gains. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set color correction gains due to invalid input parameter.
  \retval MLResult_Ok Set color correction gains successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set color correction gains to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetColorCorrectionGains(MLHandle handle, const float data[4]);

/*!
  \brief Set color correction aberration.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data Color correction aberration. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set color correction aberration due to invalid input parameter.
  \retval MLResult_Ok Set color correction aberration successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set color correction aberration to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetColorCorrectionAberrationMode(MLHandle handle, const MLCameraMetadataColorCorrectionAberrationMode *data);

/*!
  \brief Set AE antiband mode.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data AE antibanding mode. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set AE antiband mode due to invalid input parameter.
  \retval MLResult_Ok Set AE antiband mode successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set AE antiband mode to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetControlAEAntibandingMode(MLHandle handle, const MLCameraMetadataControlAEAntibandingMode *data);

/*!
  \brief Set AE exposure compensation.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data Exposure compensation value. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set AE exposure compensation due to invalid input parameter.
  \retval MLResult_Ok Set AE exposure compensation successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set AE exposure compensation to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetControlAEExposureCompensation(MLHandle handle, const int32_t *data);

/*!
  \brief Set AE lock.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data AE Lock. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set AE lock due to invalid input parameter.
  \retval MLResult_Ok Set AE lock successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set AE lock to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetControlAELock(MLHandle handle, const MLCameraMetadataControlAELock *data);

/*!
  \brief Set AE mode.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data AE Mode. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set AE mode due to invalid input parameter.
  \retval MLResult_Ok Set AE mode successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set AE mode to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetControlAEMode(MLHandle handle, const MLCameraMetadataControlAEMode *data);

/*!
  \brief Set AE target FPS range.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data AE target FPS range.

  \retval MLResult_InvalidParam Failed to set AE target FPS range due to invalid input parameter.
  \retval MLResult_Ok Set AE target FPS range successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set AE target FPS range to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetControlAETargetFPSRange(MLHandle handle, const int32_t data[2]);

/*!
  \brief Set AWB lock.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data AWB Lock. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set AE target FPS range due to invalid input parameter.
  \retval MLResult_Ok Set AE target FPS range successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set AE target FPS range to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetControlAWBLock(MLHandle handle, const MLCameraMetadataControlAWBLock *data);

/*!
  \brief Set AWB mode.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data AWB mode. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set AWB mode due to invalid input parameter.
  \retval MLResult_Ok Set AWB mode successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set AWB mode to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetControlAWBMode(MLHandle handle, const MLCameraMetadataControlAWBMode *data);

/*!
  \brief Set sensor exposure time.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data Sensor exposure time. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set sensor exposure time due to invalid input parameter.
  \retval MLResult_Ok Set sensor exposure time successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set sensor exposure time to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetSensorExposureTime(MLHandle handle, const int64_t *data);

/*!
  \brief Set sensor sensitivity.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data Sensor sensitivity. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set sensor sensitivity due to invalid input parameter.
  \retval MLResult_Ok Set sensor sensitivity successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set sensor sensitivity to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetSensorSensitivity(MLHandle handle, const int32_t *data);

/*!
  \brief Set scaler crop region.

  \param[in] handle Camera request metadata handle acquired from #MLCameraPrepareCapture.
  \param[out] data Cropped region [left, top, right, bottom]. Null pointer will clear the data.

  \retval MLResult_InvalidParam Failed to set scaler crop region due to invalid input parameter.
  \retval MLResult_Ok Set scaler crop region successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to set scaler crop region to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataSetScalerCropRegion(MLHandle handle, const int32_t data[4]);

/*!
  \brief Get color correction.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data Color correction mode.

  \retval MLResult_InvalidParam Failed to obtain color correction due to invalid input parameter.
  \retval MLResult_Ok Obtained color correction successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain color correction to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetColorCorrectionModeResultMetadata(MLHandle handle, MLCameraMetadataColorCorrectionMode *out_data);

/*!
  \brief Get color correction transform.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data 3x3 color correction transform matrix.

  \retval MLResult_InvalidParam Failed to obtain color correction transform due to invalid input parameter.
  \retval MLResult_Ok Obtained color correction transform successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain color correction transform to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetColorCorrectionTransformResultMetadata(MLHandle handle, MLCameraMetadataRational out_data[3][3]);

/*!
  \brief Get color correction aberration.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data Color correction aberration mode.

  \retval MLResult_InvalidParam Failed to obtain color correction aberration due to invalid input parameter.
  \retval MLResult_Ok Obtained color correction aberration successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain color correction aberration to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetColorCorrectionAberrationModeResultMetadata(MLHandle handle, MLCameraMetadataColorCorrectionAberrationMode *out_data);

/*!
  \brief Get color correction gains.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data Color correction gains.

  \retval MLResult_InvalidParam Failed to obtain color correction gains due to invalid input parameter.
  \retval MLResult_Ok Obtained color correction gains successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain color correction gains to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetColorCorrectionGainsResultMetadata(MLHandle handle, float out_data[4]);

/*!
  \brief Get AE antibanding mode.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data AE antibanding mode.

  \retval MLResult_InvalidParam Failed to obtain AE antibanding mode due to invalid input parameter.
  \retval MLResult_Ok Obtained AE antibanding mode successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE antibanding mode to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAEAntibandingModeResultMetadata(MLHandle handle, MLCameraMetadataControlAEAntibandingMode *out_data);

/*!
  \brief Get AE exposure compensation.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data AE exposure compensation.

  \retval MLResult_InvalidParam Failed to obtain AE exposure compensation due to invalid input parameter.
  \retval MLResult_Ok Obtained AE exposure compensation successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE exposure compensation to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAEExposureCompensationResultMetadata(MLHandle handle, int32_t *out_data);

/*!
  \brief Get AE lock.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data AE lock.

  \retval MLResult_InvalidParam Failed to obtain AE lock due to invalid input parameter.
  \retval MLResult_Ok Obtained AE lock successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE lock to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAELockResultMetadata(MLHandle handle, MLCameraMetadataControlAELock *out_data);

/*!
  \brief Get AE mode.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data AE control mode.

  \retval MLResult_InvalidParam Failed to obtain AE mode due to invalid input parameter.
  \retval MLResult_Ok Obtained AE mode successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE mode to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAEModeResultMetadata(MLHandle handle, MLCameraMetadataControlAEMode *out_data);

/*!
  \brief Get AE target FPS range.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data AE target FPS range.

  \retval MLResult_InvalidParam Failed to obtain AE target FPS range due to invalid input parameter.
  \retval MLResult_Ok Obtained AE target FPS range successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE target FPS range to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAETargetFPSRangeResultMetadata(MLHandle handle, int32_t out_data[2]);

/*!
  \brief Get AE state.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data AE state.

  \retval MLResult_InvalidParam Failed to obtain AE state due to invalid input parameter.
  \retval MLResult_Ok Obtained AE state successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AE state to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAEStateResultMetadata(MLHandle handle, MLCameraMetadataControlAEState *out_data);

/*!
  \brief Get AWB lock.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data AWB Lock.

  \retval MLResult_InvalidParam Failed to obtain AWB lock due to invalid input parameter.
  \retval MLResult_Ok Obtained AWB lock successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AWB lock to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAWBLockResultMetadata(MLHandle handle, MLCameraMetadataControlAWBLock *out_data);

/*!
  \brief Get AWB state.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data AWB state.

  \retval MLResult_InvalidParam Failed to obtain AWB state due to invalid input parameter.
  \retval MLResult_Ok Obtained AWB state successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain AWB state to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetControlAWBStateResultMetadata(MLHandle handle, MLCameraMetadataControlAWBState *out_data);

/*!
  \brief Get sensor exposure time.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data Sensor exposure time.

  \retval MLResult_InvalidParam Failed to obtain sensor exposure time due to invalid input parameter.
  \retval MLResult_Ok Obtained sensor exposure time successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain sensor exposure time to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetSensorExposureTimeResultMetadata(MLHandle handle, int64_t *out_data);

/*!
  \brief Get sensor sensitivity.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data Sensor sensitivity.

  \retval MLResult_InvalidParam Failed to obtain sensor sensitivity due to invalid input parameter.
  \retval MLResult_Ok Obtained sensor sensitivity successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain sensor sensitivity to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetSensorSensitivityResultMetadata(MLHandle handle, int32_t *out_data);

/*!
  \brief Get frame captured timestamp.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data Timestamp in nanoseconds when captured.

  \retval MLResult_InvalidParam Failed to obtain frame captured timestamp due to invalid input parameter.
  \retval MLResult_Ok Obtained frame captured timestamp successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain frame captured timestamp to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetSensorTimestampResultMetadata(MLHandle handle, int64_t *out_data);

/*!
  \brief Get scaler crop region.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data Cropped region [left, top, right, bottom].

  \retval MLResult_InvalidParam Failed to obtain scaler crop region due to invalid input parameter.
  \retval MLResult_Ok Obtained scaler crop region successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain scaler crop region to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetScalerCropRegionResultMetadata(MLHandle handle, int32_t out_data[4]);

/*!
  \brief Get sensor frame duration.

  \param[in] handle Camera result metadata handle acquired from #MLCameraGetResultMetadata.
  \param[out] out_data Sensor frame duration.

  \retval MLResult_InvalidParam Failed to obtain sensor frame duration due to invalid input parameter.
  \retval MLResult_Ok Obtained sensor frame duration successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Failed to obtain sensor frame duration to internal error.

  \priv CameraCapture
*/
ML_API MLResult ML_CALL MLCameraMetadataGetSensorFrameDurationResultMetadata(MLHandle handle, int64_t *out_data);

/*! \} */

ML_EXTERN_C_END
