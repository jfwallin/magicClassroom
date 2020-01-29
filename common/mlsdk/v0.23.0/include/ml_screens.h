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
#include "ml_image.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Screens

  \deprecated Deprecated since 0.23.0. Scheduled for removal.

  \sharedobject ml_screens
  \brief APIs for the Screens system

  Screens API provides functions to:
    - add, remove, list and update the watch history.
    - list the screen information.

  \{
*/

enum {
  /*! Defines the prefix for MLScreensResult codes. */
  MLResultAPIPrefix_Screens = MLRESULT_PREFIX(0xFB4E)
};

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Return values for Screens API calls.
  \apilevel 6
*/
typedef ML_DEPRECATED enum MLScreensResult {
  MLScreensResult_ServiceNotAvailable = MLResultAPIPrefix_Screens,
  /*! Application does not have permission for the operation. */
  MLScreensResult_PermissionDenied,
  /*! Invalid screen id. */
  MLScreensResult_InvalidScreenId,
  /*! Ensure enum is represented as 32 bits. */
  MLScreensResult_Ensure32Bits = 0x7FFFFFFF
} MLScreensResult;

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief An item that may be displayed in the Screens Launcher application.
*/
typedef ML_DEPRECATED struct MLScreensWatchHistoryEntry {
  /*! Entry identification. */
  int64_t id;
  /*! Media title. */
  char *title;
  /*! Media subtitle. */
  char *subtitle;
  /*! Current media playback position in milliseconds. */
  uint32_t playback_position_ms;
  /*! Total duration of the media in milliseconds. */
  uint32_t playback_duration_ms;
  /*!
    \brief Custom data as C-String (NULL-terminated). This may represent an URL or any
    other generic custom information.
  */
  char *custom_data;
} MLScreensWatchHistoryEntry;

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief List of items returned by MLScreensGetWatchHistoryList().
*/
typedef ML_DEPRECATED struct MLScreensWatchHistoryList {
  /*! Number of elements in the list. */
  uint32_t count;
  /*! An array containing "count" entries. */
  MLScreensWatchHistoryEntry *entries;
} MLScreensWatchHistoryList;

/*!
  \deprecated Deprecated since 0.18.0. Scheduled for removal.
  \brief This contains the information necessary for positioning of a screen.
*/
typedef ML_DEPRECATED struct MLScreensScreenInfo {
  /*! Position and orientation of screen. */
  MLTransform transform;
  /*! Dimensions of the screen (in meters). */
  MLVec3f dimensions;
} MLScreensScreenInfo;

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief This contains the information necessary for positioning of a screen.

  This structure must be initialized by calling MLScreensScreenInfoExInit() before use.

  \apilevel 2
*/
typedef ML_DEPRECATED struct MLScreensScreenInfoEx {
  /*! Version of the struct. */
  uint32_t version;
  /*! Unique screen identification. */
  uint64_t screen_id;
  /*! Default screen dimensions. This is the approximate dimensions of the Screens Home prism. */
  MLVec3f dimensions;
  /*! Position, orientation and scale of the screen in a 4x4 matrix. */
  MLMat4f transform;
} MLScreensScreenInfoEx;

/*!
  \deprecated Deprecated since 0.18.0. Scheduled for removal.
  \brief List of items returned by MLScreensGetScreenInfoList().
*/
typedef ML_DEPRECATED struct MLScreensScreenInfoList {
  /*! Total number of elements in the list. */
  uint32_t count;
  /*! An array containing "count" entries. */
  struct MLScreensScreenInfo *entries;
} MLScreensScreenInfoList;

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief List of items returned by MLScreensGetScreenInfoListEx().

  This structure must be initialized by calling MLScreensScreenInfoListExInit() before use.

  \apilevel 2
*/
typedef ML_DEPRECATED struct MLScreensScreenInfoListEx {
  /*! Version of the struct. */
  uint32_t version;
  /*! Total number of elements in the list. */
  uint32_t count;
  /*! An array containing "count" entries. */
  MLScreensScreenInfoEx *entries;
} MLScreensScreenInfoListEx;

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Initializes a new instance of MLScreensScreenInfoEx.
  \apilevel 2

  \param[in,out] screenInfo Instance of MLScreensScreenInfoEx object.
*/
ML_DEPRECATED ML_STATIC_INLINE void MLScreensScreenInfoExInit(MLScreensScreenInfoEx *inout_screen_info) {
  if (inout_screen_info) {
    inout_screen_info->version = 1;
    inout_screen_info->screen_id = 0;
    inout_screen_info->dimensions.x = 1.0f;
    inout_screen_info->dimensions.y = 0.5625f;
    inout_screen_info->dimensions.z = 0.1f;
    for (size_t i = 0; i < (sizeof(MLMat4f) / sizeof(float)); i++) {
      inout_screen_info->transform.matrix_colmajor[i] = 0.0f;
    }
  }
}

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Initializes a new instance of MLScreensScreenInfoListEx.
  \apilevel 2

  \param[in,out] list Instance of MLScreensScreenInfoListEx object.
*/
ML_DEPRECATED ML_STATIC_INLINE void MLScreensScreenInfoListExInit(MLScreensScreenInfoListEx *inout_list) {
  if (inout_list) {
    inout_list->version = 1;
    inout_list->count = 0;
    inout_list->entries = NULL;
  }
}

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Release the watch history thumbnail.

  \param[in] thumbnail The thumbnail to be released.

  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
  \retval MLResult_Ok Operation completed successfully.
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensReleaseWatchHistoryThumbnail(MLImage *thumbnail);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Clean up the content of a list of type #MLScreensWatchHistoryList.

  The watch history entry list can be obtained using MLScreensGetWatchHistoryList().
  Use this function in order to clean up "the content" of the list.

  \param[in,out] list The watch history list.

  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensReleaseWatchHistoryList(MLScreensWatchHistoryList *list);

/*!
  \deprecated Deprecated since 0.18.0. Scheduled for removal.
  \brief Clean up the content of a list of type #MLScreensScreenInfoList.

  The screens info list can be obtained using MLScreensGetScreenInfoList().
  Use this function in order to clean up "the content" of the list.

  \param[in] list A list of #MLScreensScreenInfo elements.

  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensReleaseScreenInfoList(struct MLScreensScreenInfoList *list);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Clean up the content of a list of type #MLScreensScreenInfoListEx.

  The screens info list can be obtained using MLScreensGetScreenInfoListEx().
  Use this function in order to clean up "the content" of the list.

  \apilevel 2

  \param[in] list A list of #MLScreensScreenInfoEx elements.

  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensReleaseScreenInfoListEx(MLScreensScreenInfoListEx *list);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Insert a new watch history entry.

  This should receive a complete #MLScreensWatchHistoryEntry object with exception of the id.
  The id will be filled and returned by this function in the same object.

  \param[in,out] entry A new entry to the watch history.
  \param[in] thumbnail The thumbnail associated with the entry.

  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Operation failed due to internal error.
  \retval MLScreensResult_ServiceNotAvailable Screens service is not available.

  \priv ScreensProvider
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensInsertWatchHistoryEntry(MLScreensWatchHistoryEntry *entry, const MLImage *thumbnail);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Remove an existing entry from the watch history.

  Only entries associated with the calling application can be removed.

  \param[in] id The id of the entry to be removed from the watch history.

  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Operation failed due to internal error.
  \retval MLScreensResult_ServiceNotAvailable Screens service is not available.

  \priv ScreensProvider
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensRemoveWatchHistoryEntry(int64_t id);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Update a watch history entry.

  Only entries associated with the calling application can be updated.

  \param[in] entry The entry (with id from an existing entry) to be updated in the watch history.
  \param[in] thumbnail The thumbnail associated with the entry.

  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Operation failed due to internal error.
  \retval MLScreensResult_ServiceNotAvailable Screens service is not available.

  \priv ScreensProvider
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensUpdateWatchHistoryEntry(const MLScreensWatchHistoryEntry *entry, const MLImage *thumbnail);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Get a list with the watch history that belongs to the calling client.

  The #MLScreensWatchHistoryList argument itself must be allocated by the caller.
  The "content" of #MLScreensWatchHistoryList is allocated by this function and therefore
  must be cleaned up by #MLScreensReleaseWatchHistoryList() after no longer needed.

  Only entries associated with the calling application can be retrieved.

  \xrefitem req Example \"Example\"
  \code{.cpp}
  MLScreensWatchHistoryList my_list;           // reserve my_list on the stack
  MLScreensGetWatchHistoryList(&my_list);      // initialize my_list with the watch history
  //
  // ... do something with my_list...
  //
  MLScreensReleaseWatchHistoryList(&my_list);  // clean up the content of my_list
  \endcode

  \param[out] out_list The watch history list.

  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Operation failed due to internal error.
  \retval MLScreensResult_ServiceNotAvailable Screens service is not available.

  \priv ScreensProvider
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensGetWatchHistoryList(MLScreensWatchHistoryList *out_list);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Retrieve the watch history thumbnail.

  \param[in] id The id of the watch history entry.
  \param[out] out_thumbnail The thumbnail.
              It must be released by MLScreensReleaseWatchHistoryThumbnail() after used.

  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Operation failed due to internal error.
  \retval MLScreensResult_ServiceNotAvailable Screens service is not available.

  \priv ScreensProvider
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensGetWatchHistoryThumbnail(int64_t id, MLImage *out_thumbnail);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Retrieve information about the screen associated with the calling application.

  The id of the screen can be obtained from the Application's Lifecycle Init args (see #MLLifecycleInitArg).
  The key-value pair "screenId=N", where N is the screen id, will be contained in the argument URI string.

  \apilevel 2

  \param[in] screen_id The id of the screen.
  \param[out] out_screenInfo The information about the screen.

  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Operation failed due to internal error.
  \retval MLScreensResult_InvalidScreenId Invalid screen id as input argument.
  \retval MLScreensResult_PermissionDenied Application does not have permission to set information
          about the specific screen id.
  \retval MLScreensResult_ServiceNotAvailable Screens service is not available.

  \priv ScreensProvider
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensGetScreenInfo(uint64_t screen_id, MLScreensScreenInfoEx *out_screen_info);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Update information about the screen associated with the calling application.

  The id of the screen can be obtained from the Application's Lifecycle Init args (see #MLLifecycleInitArg).
  The key-value pair "screenId=N", where N is the screen id, will be contained in the argument URI string.

  \apilevel 2

  \param[in] screenInfo The information about the screen.

  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Operation failed due to internal error.
  \retval MLScreensResult_InvalidScreenId Invalid screen id as input argument.
  \retval MLScreensResult_PermissionDenied Application does not have permission to get information
          about the specific screen id.
  \retval MLScreensResult_ServiceNotAvailable Screens service is not available.

  \priv ScreensProvider
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensUpdateScreenInfo(const MLScreensScreenInfoEx *screen_info);

/*!
  \deprecated Deprecated since 0.18.0. Scheduled for removal.
  \brief Get a list with all screens information.

  \param[in,out] out_list A list of #MLScreensScreenInfo elements.

  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Operation failed due to internal error.
  \retval MLScreensResult_ServiceNotAvailable Screens service is not available.

  \priv ScreensProvider
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensGetScreenInfoList(struct MLScreensScreenInfoList *out_list);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Get a list with all screens information.
  \apilevel 2

  \param[in,out] out_list A list of #MLScreensScreenInfoEx elements.

  \retval MLResult_InvalidParam Operation failed due to invalid input parameter.
  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Operation failed due to internal error.
  \retval MLScreensResult_ServiceNotAvailable Screens service is not available.

  \priv ScreensProvider
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensGetScreenInfoListEx(MLScreensScreenInfoListEx *out_list);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Notify Screens Home that the channel instance associated with a particular screen is being closed.
  \apilevel 2

  \param[in] screen_id The id of the screen associated with the channel being closed.

  \retval MLResult_Ok Operation completed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure Operation failed due to internal error.
  \retval MLScreensResult_ServiceNotAvailable Screens service is not available.

  \priv ScreensProvider
*/
ML_DEPRECATED ML_API MLResult ML_CALL MLScreensCloseChannelAtScreen(uint64_t screen_id);

/*!
  \deprecated Deprecated since 0.23.0. Scheduled for removal.
  \brief Returns an ASCII string for MLScreensResult and MLResultGlobal codes.

  \param[in] result_code The input MLResult enum from MLScreens functions.

  \return ASCII string containing readable version of result code.

  \priv None
*/
ML_DEPRECATED ML_API const char* ML_CALL MLScreensGetResultString(MLResult result_code);

/*! \} */

ML_EXTERN_C_END
