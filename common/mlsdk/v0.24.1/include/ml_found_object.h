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

#include <string.h>

ML_EXTERN_C_BEGIN

/*!
  \defgroup FoundObject Found Object
  \addtogroup FoundObject
  \sharedobject ml_perception_client
  \brief APIs for the Found Object Tracker system.

  - Found Object Tracker allows for spatial querying of real objects

  \attention EXPERIMENTAL

 \{
*/

enum {
  /*! Maximum char size for a label. */
  MLFoundObject_MaxLabelSize = 64,
  /*! Maximum char size for a property key. */
  MLFoundObject_MaxPropertyKeySize = 64,
  /*! Maximum char size for a property value. */
  MLFoundObject_MaxPropertyValueSize = 64,
};

/*!
  \brief Properties are key value pairs used for either filters or characteristics of an object.

  Examples: {"label": "chair"}, {"score", "0.9"}, {"class": "furniture"}, {"seen_count", "3"}, etc
  The following properties are currently supported: ["label", "score"]
  "label" can be one of the following: ["sofa", "chair", "table", "poster", "screen"]
  "score" is value between [0,1] with 1 indicating high confidence.

  \apilevel 8
*/
typedef struct MLFoundObjectProperty {
  /*!
    \brief Key for an objects property.

    Type is string. Max size is defined by MLFoundObject_MaxPropertyKeySize.
    Example of a key would be 'label'.
  */
  char key[MLFoundObject_MaxPropertyKeySize];

  /*!
    \brief Value for an objects property.

    Type is string. Max size is defined by MLFoundObject_MaxPropertyValueSize.
    Example of a value would be 'chair'.
  */
  char value[MLFoundObject_MaxPropertyValueSize];

  /*!
    \brief Last time this object was updated in UTC time.

    Note: This field should not be filled when used for creating a query.
  */
  uint64_t last_update_epoch_time_ns;
} MLFoundObjectProperty;

/*!
  \brief Struct used to compose a query.

  \apilevel 8
*/
typedef struct MLFoundObjectQueryFilter {
  /*!
    \brief Version of this structure.
  */
  uint32_t version;

  /*!
    \brief Valid ID for a found object.

    This can be obtained from sources such as a prior session. If the id is set
    to 0 then the query will not be filtered using id.
  */
  MLUUID id;

  /*!
    \brief Properties to be used as filters for the query.

    If the properties are not provided then the query will not be filtered using
    properties. Filtering is done via direct string matching.
  */
  const MLFoundObjectProperty *properties;

  /*!
    \brief Number of attributes.
  */
  uint32_t properties_count;

  /*!
    \brief Vector3 float of where you want the spatial query to originate.
  */
  MLVec3f center;

  /*!
    \brief Vector3 float of the max distance you want the spatial query to span relative
    to the center of the query.

    If the max distance is set to 0 then the query will not be filtered using max_distance.
  */
  MLVec3f max_distance;

  /*!
    \brief Maximum number of results. Used to allocate memory.
  */
  uint32_t max_results;
} MLFoundObjectQueryFilter;

/*
  \brief Initializes a MLFoundObjectQueryFilter with the default values.

  \apilevel 8

  \param[in, out] inout_filter A pointer to the object to initialize.

  \priv None
*/
ML_STATIC_INLINE void MLFoundObjectQueryFilterInit(MLFoundObjectQueryFilter *inout_filter){
  if(inout_filter != NULL){
    inout_filter->version = 1;
    memset(&(inout_filter->id), 0, sizeof(MLUUID));
    inout_filter->properties = NULL;
    inout_filter->properties_count = 0;
    inout_filter->center.x = 0.0f;
    inout_filter->center.y = 0.0f;
    inout_filter->center.z = 0.0f;
    inout_filter->max_distance.x = 0.0f;
    inout_filter->max_distance.y = 0.0f;
    inout_filter->max_distance.z = 0.0f;
    inout_filter->max_results = 5;
  }
}

/*!
  \brief Struct to represent a Found Object.

  \apilevel 8
*/
typedef struct MLFoundObject {
  /*!
    \brief Identifier of the Found Object.

    The system will attempt to persist IDs when it can be reasonably sure an
    object has already been assigned an ID in a previous session.
  */
  MLUUID id;

  /*! Number of properties. */
  uint32_t property_count;

  /*! Center position of found object. */
  MLVec3f position;

  /*! Rotation of found object. */
  MLQuaternionf rotation;

  /*! Vector3 extents of the object where each dimension is defined as max-min. */
  MLVec3f size;
} MLFoundObject;

/*!
  \brief Configure the tracker to set defaults.

  Values that go out of bounds will be clamped to system ceiling/roof. Allows
  for tuning the frequency and quantity of object updates as well as the
  maximum result per query.

  \apilevel 8
*/
typedef struct MLFoundObjectTrackerSettings {
  /*!
    \brief Version of this structure.
  */
  uint32_t version;

  /*!
    \brief Max result returned by a query.
  */
  uint32_t max_query_result;

  /*!
    \brief Maximum number of found objects to be stored.
  */
  uint32_t max_object_cache;
} MLFoundObjectTrackerSettings;


/*!
  \brief Initializes a MLFoundObjectTrackerSettings with the default values.

  \apilevel 8

  \param[in, out] inout_settings A pointer to the object to initialize.

  \priv None
*/
ML_STATIC_INLINE void MLFoundObjectTrackerSettingsInit(MLFoundObjectTrackerSettings *inout_settings){
  if(inout_settings != NULL){
    inout_settings->version = 1;
    inout_settings->max_query_result = 256;
    inout_settings->max_object_cache = 1024;
  }
}

/*!
  \brief Create a Found Object Query tracker.

  \apilevel 8

  \param[out] out_handle A pointer to an MLHandle which will contain the handle of the found object tracker.
              If this operation fails, out_handle will be ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam The parameter out_handle was not valid (null).
  \retval MLResult_Ok Found object tracker was created successfully.
  \retval MLResult_PrivilegeDenied Privileges not met.
  \retval MLResult_UnspecifiedFailure Failed to create the found object tracker.

  \priv ObjectData
*/
ML_API MLResult ML_CALL MLFoundObjectTrackerCreate(MLHandle *out_handle);

/*!
  \brief Update the tracker settings.

  \apilevel 8

  \param[in] tracker_handle Handle to a found object tracker.
  \param[in] settings Pointer to MLFoundObjectTrackerSettings.

  \retval MLResult_Ok Settings successfully set to initial settings.
  \retval MLResult_PrivilegeDenied Privileges not met.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv ObjectData
*/
ML_API MLResult ML_CALL MLFoundObjectTrackerUpdateSettings(MLHandle tracker_handle, const MLFoundObjectTrackerSettings *settings);

/*!
  \brief Create a new Found Object Query.

   Creates a new query for requesting found objects. Query criteria is
   specified by filling out the MLFoundObjectQueryFilter. Multiple queries
   can be submitted.

  \apilevel 8

  \param[in] tracker_handle Handle to a found object tracker.
  \param[in] query Query filter to apply during the query.
  \param[out] out_query_handle Pointer to an MLHandle to hold the handle to the query object.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv ObjectData
*/
ML_API MLResult ML_CALL MLFoundObjectQuery(MLHandle tracker_handle, const MLFoundObjectQueryFilter *query, MLHandle *out_query_handle);

/*!
  \brief Gets the result count of a query.

  \apilevel 8

  \param[in] tracker_handle Handle to a found object tracker.
  \param[in] query_handle Handle of an active query.
  \param[out] out_num_results The number of max results from the query.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv ObjectData
*/
ML_API MLResult ML_CALL MLFoundObjectGetResultCount(MLHandle tracker_handle, MLHandle query_handle, uint32_t *out_num_results);

/*!
  \brief Get the result of a submitted query.

  \apilevel 8

  \param[in] tracker_handle Handle to a found object query tracker.
  \param[in] query_handle Handle of an active query.
  \param[in] index Index of a found object result.
  \param[out] out_found_object Pointer to a MLFoundObject array.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv ObjectData
*/
ML_API MLResult ML_CALL MLFoundObjectGetResult(MLHandle tracker_handle, MLHandle query_handle, uint32_t index, MLFoundObject *out_found_object);

/*!
  \brief Gets the property information for a found object ID by index.

  \apilevel 8

  \param[in] tracker_handle Handle to a found object tracker.
  \param[in] id MLUUID of a found object.
  \param[in] index Index of a found object property.
  \param[out] out_property MLFoundObjectProperty for a found object.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv ObjectData
*/
ML_API MLResult ML_CALL MLFoundObjectGetProperty(MLHandle tracker_handle, MLUUID id, uint32_t index, MLFoundObjectProperty *out_property);

/*!
  \brief Returns the count for all the unique labels available in the environment you are currently in.

  \apilevel 8

  \param[in] tracker_handle Handle to a found object tracker.
  \param[out] out_unique_label_count Count of all the unique labels in the area.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv ObjectData
*/
ML_API MLResult ML_CALL MLFoundObjectGetAvailableLabelsCount(MLHandle tracker_handle, uint32_t *out_unique_label_count);

/*!
  \brief Returns the unique label by index in the area you are currently in.

  Each found object has a label. To facilitate better understanding of the
  environment, you can get all the unique labels in the area. This is used for
  discovering what is available in the user's area. Unique labels have the
  potential to change and expand as the area is explored.

  \apilevel 8

  \param[in] tracker_handle Handle to a found object tracker.
  \param[in] unique_label_index Index of the unique label you are fetching.
  \param[in] buffer_size Size of the buffer the out_label will be written to.
  \param[out] out_label Pointer that will be used to write the label to.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv ObjectData
*/
ML_API MLResult ML_CALL MLFoundObjectGetUniqueLabel(MLHandle tracker_handle, uint32_t unique_label_index, uint32_t buffer_size, char *out_label);

/*!
  \brief Releases the resources assigned to the tracker.

  \apilevel 8

  \param[in] tracker_handle Handle to the found object tracker.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectTrackerDestroy(MLHandle tracker_handle);

/*! \} */

ML_EXTERN_C_END
