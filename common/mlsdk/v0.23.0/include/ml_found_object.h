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
  \defgroup FoundObject Found Object
  \addtogroup FoundObject
  \sharedobject ml_perception_client
  \brief APIs for the Found Object Tracker system.

  - Found Object Tracker allows for spatial querying of real objects and
    attaching content to them.

  \attention EXPERIMENTAL
  \attention Not expected to function at runtime. API for early review only.

 \{
*/

enum {
  /*! Maximum char size for a label. */
  MLFoundObject_MaxLabelSize = 64,
  /*! Maximum char size for a property key. */
  MLFoundObject_MaxPropertyKeySize = 64,
  /*! Maximum char size for a property value. */
  MLFoundObject_MaxPropertyValueSize = 64,
  /*! Maximum char size for the file type used for property data.*/
  MLFoundObject_MaxFileTypeSize = 2048
};

/*! Properties are key value pairs used for either filters or characteristics of an object. */
typedef struct MLFoundObjectProperty {
  /*!
    \brief Key for an objects property. Type is string. Max size is defined by
           MLFoundObject_MaxPropertyKeySize. Example of a key would be 'texture'.
  */
  char key[MLFoundObject_MaxPropertyKeySize];
  /*!
    \brief Value for an objects property. Type is string. Max size is defined by
           MLFoundObject_MaxPropertyValueSize. Example of a value would be 'smooth'.
  */
  char value[MLFoundObject_MaxPropertyValueSize];
  /*! Size of the data for this found object.This a binary blob. Use file_type to deserialize correctly. */
  uint32_t data_size;
  /*!
    \brief File type for the data belonging to this property. This does not need to be
           filled out when making a query. It serves as a way to know how to deserialize
           the data belonging to the property as the data is in blob format. It could
           also be used as a way of filtering for found objects that have a explicit
           property containing data in a specific format.
  */
  char file_type[MLFoundObject_MaxFileTypeSize];
  /*! Last time this object was updated in UTC time. Not filled out when creating a query. */
  uint64_t last_update_epoch_time;
} MLFoundObjectProperty;

/*! Struct used to identify the supported types of found objects. */
typedef enum MLFoundObjectType {
  /*! None. */
  MLFoundObjectType_None = -1,
  /*! Object refers to a physical object. */
  MLFoundObjectType_Object,
  /*! Ensure enum is represented as 32 bits. */
  MLFoundObjectType_Ensure32Bits = 0x7FFFFFFF
} MLFoundObjectType;

/*! Struct used to compose a query. */
typedef struct MLFoundObjectQueryFilter {
  /*! Valid ID for a found object. This can be obtained from sources such as a prior session. */
  MLUUID id;
  /*! Which type of found object this query is looking for. */
  const MLFoundObjectType *types;
  /*! How many found object types are you looking for. */
  uint32_t types_count;
  /*! Properties to be used as filters for the query. */
  const MLFoundObjectProperty *properties;
  /*! Number of attributes. */
  uint32_t properties_count;
  /*! Vector3 float of where you want the spatial query to originate. */
  MLVec3f center;
  /*! Vector3 float of the max distance you want the spatial query to span relative to the center of the query. */
  MLVec3f max_distance;
  /*! Maximum number of results. Used to allocate memory. */
  uint32_t max_results;
} MLFoundObjectQueryFilter;

/*! Struct to represent a Found Object. */
typedef struct MLFoundObject {
  /*! Identifier of the Found Object. */
  MLUUID id;
  /*! Type of the found object. */
  MLFoundObjectType type;
  /*! Number of properties. */
  uint32_t property_count;
  /*! Center position of found object. */
  MLVec3f position;
  /*! Rotation of found object. */
  MLQuaternionf rotation;
  /*! Vector3 float position at the center of the object. */
  MLVec3f size;
  /*! Reference the found object is relative to. */
  MLUUID reference_id;
  /*! Rate at which the found objects position can be updated at in milliseconds. */
  uint32_t refresh_rate_ms;
} MLFoundObject;

/*!
  \brief Bindings are used to anchor virtual content to the real world.
         Bindings are persistent across sessions and are explicitly removed.
*/
typedef struct MLFoundObjectBinding {
  /*! ID of the virtual object. Virtual objects are game objects created by the app. */
  MLUUID virtual_object_id;
  /*! ID of the found object the virtual object is in reference to. */
  MLUUID found_object_id;
  /*!
    \brief ID of the previous found object ID this object was in reference to
           This is used when there was an object merge and the object that was
           previously referenced has changed its ID.
  */
  MLUUID previous_found_object_id;
  /*! Position of the virtual object. */
  MLVec3f position;
  /*! Rotation of the virtual object. */
  MLQuaternionf rotation;
} MLFoundObjectBinding;

/*! Enum for the various types of state change events. */
typedef enum MLFoundObjectStateChangeType {
  MLFoundObjectStateChange_None = -1,
  /*! A property belonging to the found object has changed. */
  MLFoundObjectStateChange_Property,
  /*! Position has changed for the found object. */
  MLFoundObjectStateChange_Position,
  /*! Orientation (rotation) has changed for the found object. */
  MLFoundObjectStateChange_Orientation,
  /*! Bindings have changed for the found object. */
  MLFoundObjectStateChange_Binding,
  /*! An event has occurred which requires getting new transforms for found objects. */
  MLFoundObjectStateChange_MapEvent,
  /*! Ensure enum is represented as 32 bits. */
  MLFoundObjectStateChange_Ensure32Bits = 0x7FFFFFFF
} MLFoundObjectStateChangeType;

/*!
  \brief State changes belonging to a found object. Only subscribed for found objects
         will have state change events broad casted. Found Objects are auto-subscribed
         for if they have a binding associated with them. All others must be manually
         subscribed for.
*/
typedef struct MLFoundObjectStateChange {
  /*! Type of state change. */
  MLFoundObjectStateChangeType type;
  /*! ID of the found object the state change applies to. */
  MLUUID id;
  /*! Seconds since the last update. Timezone is UTC. Format is epoch. */
  uint32_t last_update_time;
} MLFoundObjectStateChange;

/*!
  \brief Configure the tracker to set defaults. Values that go out of bounds will be clamped
         to system ceiling/roof. Allows for tuning the frequency and quantity
         of object updates as well as the maximum result per query.
*/
typedef struct MLFoundObjectTrackerSettings {
    /*!
      \brief Max refresh rate that the tracker will update in hertz base 10. This impacts
             System resources that are used for tracking updates.
    */
    uint32_t max_refresh_rate_hz;
    /*!
      \brief Max amount of subscriptions the tracker will manage. This is used for the quantity of
             objects actively looking for updates. Subscriptions are explained later in this document.
    */
    uint32_t max_subscription_count;
    /*! Max result returned by a query. */
    uint32_t max_query_result;
    /*! Maximum number of found objects to be stored. */
     uint32_t max_object_cache;
} MLFoundObjectTrackerSettings;

/*!
  \brief Create a Found Object Query tracker.

  \param[out] out_handle A pointer to an #MLHandle which will contain the handle of the found object tracker.
              If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam The parameter out_handle was not valid (null).
  \retval MLResult_Ok Found object tracker was created successfully.
  \retval MLResult_UnspecifiedFailure Failed to create the found object tracker.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectTrackerCreate(MLHandle *out_handle);

/*!
  \brief Set tracker to the default settings.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met. Check app manifest.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectTrackerInitSettings(MLHandle tracker_handle);

/*!
  \brief Update the tracker settings.

  \param[in] tracker_handle Handleto a Found Object Query Tracker.
  \param[in] settings Pointer to MLFoundObjectTrackerSettings.

  \retval MLResult_Ok Settings successfully set to initial settings.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectTrackerUpdateSettings(MLHandle tracker_handle, const MLFoundObjectTrackerSettings *settings);

/*!
  \brief Create a new Found Object Query.

         Creates a new query for requesting found objects. Query criteria is
         specified by filling out the MLFoundObjectQueryFilter.

  \param[in] tracker_handle Handle to a Found Object Tracker.
  \param[in] query Query filter to apply during the query.
  \param[out] out_query_handle Pointer to an MLHandle to hold the handle to the query object.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met. Check app manifest.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjectRead
*/
ML_API MLResult ML_CALL MLFoundObjectQuery(MLHandle tracker_handle, const MLFoundObjectQueryFilter *query, MLHandle *out_query_handle);

/*!
  \brief Gets the result count of a query.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] query_handle MLUUID of an active query.
  \param[out] out_num_results The number of max results from the query.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_PrivilegeDenied Privileges not met. Check app manifest.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectGetResultCount(MLHandle tracker_handle, MLHandle query_handle, uint32_t *out_num_results);

/*!
  \brief Get the result of a submitted query.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] query_handle MLUUID of an active query.
  \param[in] index Index of a found object result.
  \param[out] out_found_object MLFoundObjectStruct object to be written to.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectGetResult(MLHandle tracker_handle, MLHandle query_handle, uint32_t index, MLFoundObject *out_found_object);

/*!
  \brief Gets the property information for a found object ID by index.

         This is not the data for a property. This is a MLFoundObjectProperty. If the
         property has a data size greater than zero and you would like to get the data you
         will have to call MLFoundObjectRequestPropertyData and then MLFoundObjectGetPropertyData.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] id MLUUID of a found object.
  \param[in] index Index of a found object property.
  \param[out] out_property MLFoundObjectProperty for a found object.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectGetProperty(MLHandle tracker_handle, MLUUID id, uint32_t index, MLFoundObjectProperty *out_property);

/*!
  \brief Request property data for a found object.

         This does not return the data for the found object. It requests the data to be
         stored inside the tracker. You can then call MLFoundObjectGetPropertyData to
         access the data. It will return false if no data is available for the found object.

         To prevent bloating memory property data is retrieved on demand instead of all
         properties being greedily cached.

         Property data is a binary blob. Use the file_type for deserialization. An example of
         property data would be color, name, etc.

  \param[in] tracker_handle Handle to a Found Object Query.
  \param[in] id MLUUID to a found object.
  \param[in] property_index Index of the property that the data will be requested for.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectRequestPropertyData(MLHandle tracker_handle, MLUUID id, uint32_t property_index);

/*!
  \brief Returns the found data for an object.

         This is the data for a property in the form of a binary blob. It does not
         contain the metadata found in the MLFoundObjectProperty.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] id MLUUID of a found object.
  \param[in] property_index Index of the property that the data is for.
  \param[in] buffer_size Should be at least the size of the found objects data size + 1.
  \param[out] out_data_container Data container of a found object.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectGetPropertyData(MLHandle tracker_handle, MLUUID id, uint32_t property_index, uint32_t buffer_size, uint8_t *out_data_container);

/*!
 \brief Sets a binding for a virtual object to a found object.

        Bindings create a relative pose between virtual content and found objects. If
        a Found Object ID is present in the MLFoundObjectBinding then the virtual object
        will be bound relative to that found object. If no found object ID is present
        then the content will be bound to the nearest Found Object relative to the
        user's current location at time of the call. Bindings persist between sessions.
        Bindings can be removed manually or will be removed once the maximum number
        has been reached in a first in first out order.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in,out] in_out_binding Binding to be created.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectSetBinding(MLHandle tracker_handle, MLFoundObjectBinding *in_out_binding);

/*!
  \brief Removes a binding for a virtual object to a found object.

         Setting a binding will also add the found object the binding the the binding is
         attached to automatically be added to the subscriptions.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] virtual_object_id MLUUID to be removed from the found object.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectRemoveBinding(MLHandle tracker_handle, MLUUID virtual_object_id);

/*!
  \brief Returns all the bindings for a user for an app ID.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[out] out_binding_count Number of bindings for this app.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectGetBindingCount(MLHandle tracker_handle, uint32_t *out_binding_count);

/*!
  \brief Returns the MLUUID and count of a Found Object that has bindings.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] index Index of the Found Object with bindings being fetched.
  \param[out] out_found_object_id MLUUID of the found object that is being fetched.
  \param[out] out_binding_count Number of bindings belonging to the Found Object that was fetched.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectGetFoundObjectBindingCount(MLHandle tracker_handle, uint32_t index, MLUUID *out_found_object_id, uint32_t *out_binding_count);

/*!
  \brief     Get the binding for an app by index.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] Index of the binding being fetched.
  \param[out] out_binding MLFoundObjectBinding at the index submitted for a particular app_id.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectGetBinding(MLHandle tracker_handle, MLUUID found_object_id, uint32_t index, MLFoundObjectBinding *out_binding);

/*!
  \brief Gets the binding by the virtual object ID.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] virtual_object_id MLUUID generated by the application.
  \param[out] out_binding Binding that will return with the virtual content.
              The position and rotation will be in the world coordinate frame.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectGetBindingByVirtualId(MLHandle tracker_handle, MLUUID virtual_object_id, MLFoundObjectBinding *out_binding);

/*!
  \brief Returns the count for all the unique labels available in the environment you are currently in.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[out] out_unique_label_count Count of all the unique labels in the area.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectGetAvailableLabelsCount(MLHandle tracker_handle, uint32_t *out_unique_label_count);

/*!
  \brief Returns the unique label by index in the area you are currently in.

         Each found object has an array of labels. To facilitate better understanding of the
         environment, you can get all the unique labels in the area. This is used for
         discovering what is available in the users area. Unique labels have the
         potential to change and expand as the area is explored.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] unique_label_index Index of the unique label you are fetching.
  \param[in] buffer_size Size of the buffer the out_label will be written to.
  \param[out] out_label Pointer that will be used to write the label to.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectGetUniqueLabel(MLHandle tracker_handle, uint32_t unique_label_index, uint32_t buffer_size, char *out_label);

/*!
  \brief Subscribes to updates for a Found Object.

         Subscribing to a found object has two core mechanics. The first one is that it
         will cause the tracker to poll at a specific frequency, defined by the tracker setting.
         The other mechanic is that it will cause there to be state changes that can be fetched.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] id MLUUID of the found object to subscribe to.

  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectSubscribe(MLHandle tracker_handle, MLUUID id);

/*!
  \brief Unsubscribes to updates for a Found Object.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] id MLUUID of the found object to unsubscribe to.

  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectUnsubscribe(MLHandle tracker_handle, MLUUID id);

/*!
  \brief Returns the number of Found Objects that have been subscribed to.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[out] out_subscription_list_count Number of Found Objects that have been subscribed for.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectGetSubscriptionList(MLHandle tracker_handle, uint32_t *out_subscription_list_count);

/*!
  \brief Returns the ID at the index of the subscriptions list.

  \param[in] tracker_handle Handle to a Found Object Query Tracker.
  \param[in] index Index of the Found Object that is being fetched.
  \param[out] out_id MLUUID of the Found Object at index location.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectGetSubscriptionIdAtIndex(MLHandle tracker_handle, uint32_t index, MLUUID *out_id);

/*!
  \brief Get the count for how many Found Objects have state changes.

         This keeps the last change per the enum type of change. For example a found
         object has two changes position and properties change. Then a second
         position change comes in. Only 1 position type state change will be present
         for that particular found object. There will be a total of two state
         changes though. One for the position and one for the property change.

  \param[in] tracker_handle Handle to the Found Object Query Tracker.
  \param[out] out_state_change_count Pointer used to write the count for how many found objects have changes.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv PwFoundObjRead
*/
ML_API MLResult ML_CALL MLFoundObjectGetStateChangesCount(MLHandle tracker_handle, uint64_t *out_state_change_count);

/*!
  \brief Get the count of state changes for a particular index.

  \param[in] tracker_handle Handle to the Found Object Query Tracker.
  \param[in] index Index into the state changes list.
  \param[out] out_id UUID of the found object.
  \param[out] out_count Pointer to a uint32_t for the count to be written to.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectGetStateChangesCountForIndex(MLHandle tracker_handle, uint32_t index, MLUUID *out_id, uint32_t *out_count);

/*!
  \brief Get the state change for a found object by index.

  \param[in] tracker_handle Handle to the Found Object Query Tracker.
  \param[in] id MLUUID of the found object.
  \param[out] out_state_change Pointer for where the state change will be written to.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectGetNextStateChange(MLHandle tracker_handle, MLUUID id, MLFoundObjectStateChange *out_state_change);

/*!
  \brief Releases the resources assigned to the tracker.

  \param[in] tracker_handle Handle to the found Object Query Tracker.

  \retval MLResult_InvalidParam Invalid parameter.
  \retval MLResult_Ok Returned a valid MLHandle.
  \retval MLResult_UnspecifiedFailure Unspecified failure.

  \priv None
*/
ML_API MLResult ML_CALL MLFoundObjectTrackerDestroy(MLHandle tracker_handle);

/*! \} */

ML_EXTERN_C_END
