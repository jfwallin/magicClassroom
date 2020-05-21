// %BANNER_BEGIN%
// ---------------------------------------------------------------------
// %COPYRIGHT_BEGIN%
//
// Copyright (c) 2019 Magic Leap, Inc. (COMPANY) All Rights Reserved.
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
#include "ml_coordinate_frame_uid.h"


ML_EXTERN_C_BEGIN

/*!
  \defgroup MarkerTracking Fiducial Marker Tracking
  \addtogroup MarkerTracking
  \sharedobject ml_perception_client
  \brief APIs for the ArUco Tracker system.

  This tracker is used to track square fiducial markers (also known as Augmented Reality Markers).
  It allows for rendering virtual content in relation to the location of so called ArUco
  markers that can be attached to flat surfaces. Each ArUco marker has a unique ID,
  so different markers can be tracked at the same time and are distinguishable as long as they
  belong to the same dictionary and have the same size.

  \{
*/


/*!
  \brief Supported pre-defined dictionary names.

  ArUco Tracker supports pre-defined dictionary names.
  Some of these dictionaries can be looked up and markers can be generated for them here:
  \link http://chev.me/arucogen/ \endlink

  \apilevel 8
*/
typedef enum MLArucoDictionaryName {
 MLArucoDictionaryName_DICT_4X4_50 = 0,     /*! 4 by 4 pixel ArUco marker dictionary with 50 IDs */
 MLArucoDictionaryName_DICT_4X4_100,        /*! 4 by 4 pixel ArUco marker dictionary with 100 IDs */
 MLArucoDictionaryName_DICT_4X4_250,        /*! 4 by 4 pixel ArUco marker dictionary with 250 IDs */
 MLArucoDictionaryName_DICT_4X4_1000,       /*! 4 by 4 pixel ArUco marker dictionary with 1000 IDs */
 MLArucoDictionaryName_DICT_5X5_50,         /*! 5 by 5 pixel ArUco marker dictionary with 50 IDs */
 MLArucoDictionaryName_DICT_5X5_100,        /*! 5 by 5 pixel ArUco marker dictionary with 100 IDs */
 MLArucoDictionaryName_DICT_5X5_250,        /*! 5 by 5 pixel ArUco marker dictionary with 250 IDs */
 MLArucoDictionaryName_DICT_5X5_1000,       /*! 5 by 5 pixel ArUco marker dictionary with 1000 IDs */
 MLArucoDictionaryName_DICT_6X6_50,         /*! 6 by 6 pixel ArUco marker dictionary with 50 IDs */
 MLArucoDictionaryName_DICT_6X6_100,        /*! 6 by 6 pixel ArUco marker dictionary with 100 IDs */
 MLArucoDictionaryName_DICT_6X6_250,        /*! 6 by 6 pixel ArUco marker dictionary with 250 IDs */
 MLArucoDictionaryName_DICT_6X6_1000,       /*! 6 by 6 pixel ArUco marker dictionary with 1000 IDs */
 MLArucoDictionaryName_DICT_7X7_50,         /*! 7 by 7 pixel ArUco marker dictionary with 50 IDs */
 MLArucoDictionaryName_DICT_7X7_100,        /*! 7 by 7 pixel ArUco marker dictionary with 100 IDs */
 MLArucoDictionaryName_DICT_7X7_250,        /*! 7 by 7 pixel ArUco marker dictionary with 250 IDs */
 MLArucoDictionaryName_DICT_7X7_1000,       /*! 7 by 7 pixel ArUco marker dictionary with 1000 IDs */
 MLArucoDictionaryName_DICT_ARUCO_ORIGINAL, /*! 5 by 5 pixel ArUco marker dictionary with 1024 IDs */
} MLArucoDictionaryName;

/*!
  \brief Represents the result for a single marker.

  A list of these detections will be returned by the Aruco Tracker, after processing a video
  frame succesfully.

  \apilevel 8
*/
typedef struct MLArucoTrackerResult {
  /*!
    \brief Unique marker ID as defined in selected MLArucoDictionaryName.
  */
  int id;

  /*!
    \brief MLCoordinateFrameUID of the marker.

    This should be passed to the MLSnapshotGetTransform() function to get
    the 6 DOF pose of the marker.
  */
  MLCoordinateFrameUID coord_frame_marker;

  /*!
    \brief The reprojection error of this marker detection in degrees.

    A high reprojection error means that the estimated pose of the marker doesn't match well with
    the 2D detection on the processed video frame and thus the pose might be inaccurate. The error
    is given in degrees, signifying by how much either camera or marker would have to be moved or
    rotated to create a perfect reprojection.
  */
  float reprojection_err;
} MLArucoTrackerResult;

/*!
  \brief An array of all the marker results from the Aruco Tracker.

  Only markers that belong to the dictionary with which the tracker was configured with will be
  found in this array.

  \apilevel 8
*/
typedef struct MLArucoTrackerResultArray {
  /*! Pointer to an array of pointers for MLArucoTrackerResult. */
  MLArucoTrackerResult** detections;

  /*! Number of markers being tracked. */
  size_t count;
} MLArucoTrackerResultArray;

/*!
  \brief ArUco Tracker Settings

  When creating the ArUco Tracker, this list of settings needs to be passed to configure the tracker
  properly. The tracker will only output detections of markers that belong to the defined dictionary
  and the estimated poses will only be correct if the marker length has been set correctly.

  \apilevel 8
*/
typedef struct MLArucoTrackerSettings {
  /*! Version of this structure. */
  uint32_t version;

  /*! Dictionary from which markers shall be tracked. */
  MLArucoDictionaryName dictionary;

  /*!
    \brief The length of the markers that shall be tracked.

    The marker length is important to know, because once a marker is detected we can only determine
    its 3D position if we know how large it is in real life.
    The length of a marker is given in meters and represents the distance between the four dominant
    corners of the squared marker.
  */
  float marker_length;

  /*!
    \brief If \c true, ArUco tracker will detect and track markers.

    ArUco Tracker should be disabled when app is paused and enabled when app resumes.
    When enabled, ArUco Tracker will gain access to the camera and start tracking images.
    When disabled ArUco Tracker will release the camera and stop tracking markers.
    Internal state of the tracker will be maintained.
  */
  bool enable_marker_tracking;
} MLArucoTrackerSettings;

/*!
  \brief Initializes default values for MLArucoTrackerSettings.

  \apilevel 8

  \param[in, out] inout_settings The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLArucoTrackerSettingsInit(MLArucoTrackerSettings *inout_settings) {
  inout_settings->version = 1u;
  inout_settings->dictionary = MLArucoDictionaryName_DICT_5X5_250;
  inout_settings->marker_length = 0.1f;
  inout_settings->enable_marker_tracking = true;
}

/*!
  \brief Create an ArUco Tracker.

  \apilevel 8

  \param[in]	settings List of settings of type MLArucoTrackerSettings that configure the tracker.
  \param[out]	out_handle A pointer to an MLHandle to the newly created ArUco Tracker.
              If this operation fails, out_handle will be ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam Failed to create ArUco Tracker due to invalid out_handle.
  \retval MLResult_Ok Successfully created ArUco Tracker.
  \retval MLResult_PrivilegeDenied Failed to create tracker due to lack of privilege(s).
  \retval MLResult_UnspecifiedFailure Failed to create the ArUco Tracker due to an internal error.

  \priv CameraCapture, LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLArucoTrackerCreate(const MLArucoTrackerSettings *settings, MLHandle *out_handle);

/*!
  \brief Update the ArUco Tracker with new settings.

  \apilevel 8

  \param[in] tracker_handle MLHandle to the ArUco Tracker created by MLArucoTrackerCreate().
  \param[in] tracker_settings List of new ArUco Tracker settings.

  \retval MLResult_InvalidParam Failed to update the settings due to invalid tracker_settings.
  \retval MLResult_Ok Successfully updated the ArUco Tracker settings.
  \retval MLResult_PrivilegeDenied Failed to update the settings due to lack of privilege(s).
  \retval MLResult_UnspecifiedFailure Failed to update the settings due to an internal error.

  \priv CameraCapture, LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLArucoTrackerUpdateSettings(MLHandle tracker_handle,
                                                     const MLArucoTrackerSettings *tracker_settings);

/*!
  \brief Destroy an ArUco Tracker.

  \apilevel 8

  \param[in] tracker_handle MLHandle to the ArUco Tracker created by MLArucoTrackerCreate().

  \retval MLResult_Ok Successfully destroyed the ArUco Tracker.
  \retval MLResult_UnspecifiedFailure Failed to destroy the tracker due to an internal error.

  \priv CameraCapture, LowLatencyLightwear
*/
ML_API MLResult ML_CALL MLArucoTrackerDestroy(MLHandle tracker_handle);

/*!
  \brief Get the results for Aurco Tracking.

  \apilevel 8

  \param[in]	tracker_handle #MLHandle to the ArUco Tracker created by MLArucoTrackerCreate().
  \param[out]	out_data Pointer to an array of pointers to MLArucoTrackerResult. 
              The content will be freed by the MLArucoTrackerReleaseResult.

  \retval MLResult_InvalidParam Failed to return detection data due to invalid out_data.
  \retval MLResult_Ok Successfully fetched and returned all detections.
  \retval MLResult_UnspecifiedFailure Failed to return detections due to an internal error.

  \priv None
*/
ML_API MLResult ML_CALL MLArucoTrackerGetResult(MLHandle tracker_handle,
                                                MLArucoTrackerResultArray *out_data);

/*!
  \brief Release the resources for the results array.

  \apilevel 8

  \param[in] data The list of detections to be freed.

  \retval MLResult_InvaldParam Failed to free structure due to invalid data.
  \retval MLResult_Ok Successfully freed data structure.
  \retval MLResult_UnspecifiedFailure Failed to free data due to an internal error.

  \priv None
*/
ML_API MLResult ML_CALL MLArucoTrackerReleaseResult(MLArucoTrackerResultArray *data);

/*! \} */

ML_EXTERN_C_END
