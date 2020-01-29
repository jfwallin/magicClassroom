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
#include "ml_types.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Perception
  \sharedobject ml_perception_client
  \{
*/

enum {
  /*! Defines the prefix for MLSnapshotResult codes. */
  MLResultAPIPrefix_Snapshot = MLRESULT_PREFIX(0x87b8)
};

/*! Return values for Snapshot API calls. */
typedef enum MLSnapshotResult {
  /*! Pose not found. */
  MLSnapshotResult_PoseNotFound = MLResultAPIPrefix_Snapshot
} MLSnapshotResult;

/*! Opaque snapshot of system state. */
typedef struct MLSnapshot MLSnapshot;

/*!
  \brief Get transform between world origin and the coordinate frame `id'.

  \param[in] snapshot A snapshot of tracker state. Can be obtained with MLPerceptionGetSnapshot().
  \param[in] id Look up the transform between the current origin and this coordinate frame id.
  \param[out] out_transform Valid pointer to a #MLTransform. To be filled out with requested transform data.

  \retval MLResult_InvalidParam Failed to obtain transform due to invalid parameter.
  \retval MLResult_Ok Obtained transform successfully.
  \retval MLResult_UnspecifiedFailure Failed to obtain transform due to internal error.
  \retval MLSnapshotResult_PoseNotFound Coordinate Frame is valid, but not found in the current pose snapshot.

  \priv None
*/
ML_API MLResult ML_CALL MLSnapshotGetTransform(const MLSnapshot *snapshot, const MLCoordinateFrameUID *id, MLTransform *out_transform);

/*!
  \brief Returns an ASCII string representation for each result code.

  This call returns strings for all of the global MLResult and
  MLSnapshotResult codes.

  \param[in] result_code MLResult type to be converted to string.

  \return ASCII string containing readable version of the result code.

  \priv None
*/
ML_API const char* ML_CALL MLSnapshotGetResultString(MLResult result_code);

/*! \} */

ML_EXTERN_C_END
