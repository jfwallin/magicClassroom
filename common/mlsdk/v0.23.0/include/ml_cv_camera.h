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

ML_EXTERN_C_BEGIN

/*!
  \defgroup CVCamera CV Camera
  \addtogroup CVCamera
  \sharedobject ml_perception_client
  \brief APIs for camera pose and camera intrinsics.
  \attention EXPERIMENTAL

  \{
*/

/*!
  \brief Camera distortion vector size.
  \apilevel 5
*/
enum {
  /*! Default distortion vector size. */
  MLCVCameraIntrinsics_MaxDistortionCoefficients = 5
};

/*!
  \brief Camera id enum.
  \apilevel 5
*/
typedef enum MLCVCameraID {
  /*! Default camera id. */
  MLCVCameraID_ColorCamera = 0,
  /*! Camera id. */
  MLCVCameraID_Ensure32Bits = 0x7FFFFFFF
} MLCVCameraID;

/*!
  \brief Camera intrinsic parameter.
  \apilevel 5
*/
typedef struct MLCVCameraIntrinsicCalibrationParameters {
  /*! Structure version. */
  uint32_t version;
  /*! Camera width. */
  uint32_t width;
  /*! Camera height. */
  uint32_t height;
  /*! Camera focal length. */
  MLVec2f focal_length;
  /*! Camera principal point. */
  MLVec2f principal_point;
  /*! Field of view. */
  float fov;
  /*! \brief Distortion vector.
      The distortion co-efficients are in the following order:
      [k1, k2, p1, p2, k3]
   */
  double distortion[MLCVCameraIntrinsics_MaxDistortionCoefficients];
} MLCVCameraIntrinsicCalibrationParameters;

/*!
  \brief Intializes the default values for #MLCVCameraIntrinsicCalibrationParameters.
  \apilevel 5

  \param[in, out] in_out_data The object to initialize with default values.
*/
ML_STATIC_INLINE void MLCVCameraIntrinsicCalibrationParametersInit(MLCVCameraIntrinsicCalibrationParameters *in_out_data) {
  if (NULL != in_out_data) {
    in_out_data->version = 1u;
    in_out_data->width = 0u;
    in_out_data->height = 0u;
    in_out_data->focal_length.x = 0.0f;
    in_out_data->focal_length.y = 0.0f;
    in_out_data->principal_point.x = 0.0f;
    in_out_data->principal_point.y = 0.0f;
    for (int i = 0; i < MLCVCameraIntrinsics_MaxDistortionCoefficients; i++) {
      in_out_data->distortion[i] = 0;
    }
  }
}

/*!
  \brief Create Camera Tracker.

  \apilevel  5

  \param[out] out_handle Tracker handle.

  \retval MLResult_Ok On success.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Unable to create tracker.

  \priv ComputerVision
*/
ML_API MLResult ML_CALL MLCVCameraTrackingCreate(MLHandle *out_handle);

/*!
  \brief Get camera intrinsic parameters.

  \apilevel 5

  \param[in]  camera_handle MLHandle previously created with MLCVCameraTrackingCreate.
  \param[in]  id Camera id.
  \param[out] out_intrinsics Camera intrinsic parameters.

  \retval MLResult_Ok On success.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Unable to retrieve intrinsic parameter.

  \priv ComputerVision
*/
ML_API MLResult ML_CALL MLCVCameraGetIntrinsicCalibrationParameters(MLHandle camera_handle, MLCVCameraID id, MLCVCameraIntrinsicCalibrationParameters *out_intrinsics);

/*!
  \brief Get the camera pose in the world coordiante system.

  Use the timestamp provided from the #on_video_buffer_available callback from
  ml_camera.h. The camera tracker only caches a limited set of poses. Retrieve
  the camera pose as soon as the timestamp is available.

  \apilevel  5

  \param[in]  camera_handle MLHandle previously created with MLCVCameraTrackingCreate.
  \param[in]  head_handle MLHandle previously created with MLHeadTrackingCreate.
  \param[in]  id Camera id.
  \param[in]  camera_timestamp_ns Time in nanoseconds to request the pose.
  \param[out] out_transform Transfom from camera to world origin.

  \retval MLResult_Ok Obtained transform successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_InvalidParam id parameter was not valid or out_transform parameter
                                was not valid (null).
  \retval MLResult_UnspecifiedFailure Failed to obtain transform due to internal error.

  \priv ComputerVision
*/
ML_API MLResult ML_CALL MLCVCameraGetFramePose(MLHandle camera_handle, MLHandle head_handle, MLCVCameraID id, uint64_t camera_timestamp_ns, MLTransform *out_transform);

/*!
  \brief Destroy Tracker after usage.

  \apilevel 5

  \param[in] camera_handle MLHandle previously created with MLCVCameraTrackingCreate.

  \retval MLResult_Ok On success.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Unable to create tracker.

  \priv ComputerVision
*/
ML_API MLResult ML_CALL MLCVCameraTrackingDestroy(MLHandle camera_handle);

/*! \} */

ML_EXTERN_C_END
