// %BANNER_BEGIN%
//---------------------------------------------------------------------
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
#include "ml_fileinfo.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Lifecycle Application Lifecycle
  \sharedobject ml_lifecycle
  \brief API to manage callbacks for lifecycle events and query for data
         about paths that the application is permitted to access.
  \{
*/

/*!
  \deprecated Deprecated since 0.21.0. Scheduled for removal. Replaced by \ref MLLifecycleCallbacksEx.
  \brief Lifecycle callback functions.
*/
typedef ML_DEPRECATED_MSG("Replaced by MLLifecycleCallbacksEx.") struct MLLifecycleCallbacks {
  /*!
    \brief This callback is called when the application is being stopped by the system.

    The system expects the application to gracefully terminate when this function is called.

    If the application does not terminate after a pre-defined timeout,
    it will be terminated by the system.

    If this function is not registered, the default behavior is to terminate
    with an exit status 1.

    NOTE: The termination of the app's process is not yet
    enforced. This note will be removed once it is in place.

    \param[in] context Pointer to the application context passed during initialization with a call to
               MLLifecycleInit(). This parameter is NULL if NULL was passed to MLLifecycleInit().
  */
  void (*on_stop)(void *context);
  /*!
    \brief This callback is called when the application is being
    paused/suspended by the system.

    The application process is suspended after a pre-defined timeout. Applications should
    ensure any temporary data is flushed to disk for resumption later.

    \param[in] context Pointer to the application context passed during initialization by a call to
               MLLifecycleInit(). This parameter is NULL if NULL was passed to MLLifecycleInit().
  */
  void (*on_pause)(void *context);
  /*!
    \brief This callback is called when the application is being
    resumed after being suspended.

    The application can restore its state in the function call to resume normal execution.

    \param[in] context Pointer to the application context passed during initialization to call to
               MLLifecycleInit(). This parameter is NULL if NULL was passed to MLLifecycleInit().
  */
  void (*on_resume)(void *context);
  /*!
    \brief This callback is called when the application is being
    asked to free up cached resources by the system.

    This might happen when the system is running low on resources
    and requires to retain resources for other use cases.

    \param[in] context Pointer to the application context passed during initialization to call to
               MLLifecycleInit(). This parameter is NULL if NULL was passed to MLLifecycleInit().
  */
  void (*on_unload_resources)(void *context);
  /*!
    \brief This callback is called when the application has received
    new init args.

    The application can obtain the init args by calling MLLifecycleGetInitArgList().

    \param[in] context Pointer to the application context passed during initialization to call to
               MLLifecycleInit(). This parameter is NULL if NULL was passed to MLLifecycleInit().
  */
  void (*on_new_initarg)(void *context);
} MLLifecycleCallbacks;

/*!
  \brief Values for why focus was lost.

  The system will communicate to an application the reason for losing focus.

  \apilevel 7
*/
typedef enum {
/*! Value returned when focus is lost due to an unknown event */
      MLLifecycleFocusLostReason_Invalid = -1,
/*! Value returned when focus is lost due to a system dialog. */
      MLLifecycleFocusLostReason_System = 0,
} MLLifecycleFocusLostReason;

/*!
  \brief Lifecycle callback functions.

  This structure must be initialized by calling MLLifecycleCallbacksExInit() before use.

  \apilevel 7
*/
typedef struct MLLifecycleCallbacksEx {
  uint32_t version;
  /*!
    \brief This callback is called when the application is being stopped by the system.

    The system expects the application to gracefully terminate when this function is called.

    If the application does not terminate after a pre-defined timeout,
    it will be terminated by the system.

    If this function is not registered, the default behavior is to terminate
    with an exit status 1.

    NOTE: The termination of the app's process is not yet
    enforced. This note will be removed once it is in place.

    \param[in] context Pointer to the application context passed during initialization with a call to
               MLLifecycleInitEx(). This parameter is NULL if NULL was passed to MLLifecycleInitEx().
  */
  void (*on_stop)(void *context);
  /*!
    \brief This callback is called when the application is being
    paused/suspended by the system.

    The application process is suspended after a pre-defined timeout. Applications should
    ensure any temporary data is flushed to disk for resumption later.

    \param[in] context Pointer to the application context passed during initialization by a call to
               MLLifecycleInitEx(). This parameter is NULL if NULL was passed to MLLifecycleInitEx().
  */
  void (*on_pause)(void *context);
  /*!
    \brief This callback is called when the application is being
    resumed after being suspended.

    The application can restore its state in the function call to resume normal execution.

    \param[in] context Pointer to the application context passed during initialization to call to
               MLLifecycleInitEx(). This parameter is NULL if NULL was passed to MLLifecycleInitEx().
  */
  void (*on_resume)(void *context);
  /*!
    \brief This callback is called when the application is being
    asked to free up cached resources by the system.

    This might happen when the system is running low on resources
    and requires to retain resources for other use cases.

    \param[in] context Pointer to the application context passed during initialization to call to
               MLLifecycleInitEx(). This parameter is NULL if NULL was passed to MLLifecycleInitEx().
  */
  void (*on_unload_resources)(void *context);
  /*!
    \brief This callback is called when the application has received
    new init args.

    The application can obtain the init args by calling MLLifecycleGetInitArgList().

    \param[in] context Pointer to the application context passed during initialization to call to
               MLLifecycleInitEx(). This parameter is NULL if NULL was passed to MLLifecycleInitEx().
  */
  void (*on_new_initarg)(void *context);
  /*!
    \brief This callback is called when the device has transitioned to the active mode
    from reality or standby.

    This is triggered when the device comes out of the reality mode
    or when the wearable is back on head and is no longer in standby mode.

    \param[in] context Pointer to the application context passed during initialization to call to
               MLLifecycleInitEx(). This parameter is NULL if NULL was passed to MLLifecycleInitEx().
  */
  void (*on_device_active)(void *context);
  /*!
    \brief This callback is called when the device has transitioned to the reality mode.

    \param[in] context Pointer to the application context passed during initialization to call to
               MLLifecycleInitEx(). This parameter is NULL if NULL was passed to MLLifecycleInitEx().
  */
  void (*on_device_reality)(void *context);
  /*!
    \brief This callback is called when the device has transitioned to the standby mode.

    This is triggered when the wearable is off head.

    \param[in] context Pointer to the application context passed during initialization to call to
               MLLifecycleInitEx(). This parameter is NULL if NULL was passed to MLLifecycleInitEx().
  */
  void (*on_device_standby)(void *context);

  /*!
    \brief This callback is called when the application loses its focus.

    When focus is lost, the application will not receive input events.

    \param[in] context Pointer to the application context passed during initialization to call to
               MLLifecycleInitEx(). This parameter is NULL if NULL was passed to MLLifecycleInitEx().
    \param[in] reason the reason that input was lost
  */
  void (*on_focus_lost)(void *context, MLLifecycleFocusLostReason reason);

  /*!
    \brief This callback is called when the application regains focus.

    This is triggered when an application regains focus.

    \param[in] context Pointer to the application context passed during initialization to call to
               MLLifecycleInitEx(). This parameter is NULL if NULL was passed to MLLifecycleInitEx().
  */
  void (*on_focus_gained)(void *context);

} MLLifecycleCallbacksEx;

/*! This structure should be explicitly freed by calling MLLifecycleFreeSelfInfo(). */
typedef struct MLLifecycleSelfInfo {
  /*!
    \brief Path to the writable dir of the application. This path is valid when the
    user is logged in and using the device, i.e the device is unlocked. This path
    is not available when device is locked.
   */
  const char *writable_dir_path;
  /*! Path to the application package dir. */
  const char *package_dir_path;
  /*! Package name of the application. */
  const char *package_name;
  /*! Component name of the application. */
  const char *component_name;
  /*! Path to the application tmp dir. */
  const char *tmp_dir_path;
  /*! \brief Visible name of the application */
  const char *visible_name;

  /*!
    \brief Path to the writable dir of the application available when device is
    locked. This path is valid when the user has logged in once and the device
    is locked. An application that needs to write data when running in the
    background e.g. a music app should use this path.  The same application can
    continue using this path when the device is unlocked afterwards. Therefore
    this path is always available to an application.
   */
  const char *writable_dir_path_locked_and_unlocked;
} MLLifecycleSelfInfo;

/*!
  \brief Opaque structure containing array of init args and other fields.

  An app can have multiple initial argument objects accumulated if it has been triggered multiple
  times and the app hasn't retrieved its InitArgs. These objects are stored in
  #MLLifecycleInitArgList which the app can retrieve.

  Note: The platform does not keep a copy of the InitArgs once they have been retrieved by the app.

  The accessible fields in this structure are:
  InitArg Array length - Can be obtained by calling MLLifecycleGetInitArgListLength().
  InitArg Array        - Can be obtained by calling MLLifecycleGetInitArgByIndex().
*/
typedef struct MLLifecycleInitArgList MLLifecycleInitArgList;

/*!
  \brief Opaque structure containing array of file infos and other fields.

  The accessible fields in this structure are:
  URI                    - Can be obtained by calling MLLifecycleGetInitArgUri().
  File Info Array length - Can be obtained by calling MLLifecycleGetFileInfoListLength().
  File Info Array        - Can be obtained by calling MLLifecycleGetFileInfoByIndex().
*/
typedef struct MLLifecycleInitArg MLLifecycleInitArg;

/*!
  \deprecated Deprecated since 0.21.0. Scheduled for removal. Replaced by \ref MLLifecycleInitEx.
  \brief Initialize the lifecycle service interface.

  \param[in] callbacks Pointer to an #MLLifecycleCallbacks structure
  \param[in] context Pointer to the application context that the application wants a
             reference to during callbacks. This parameter is optional and the user can pass a
             NULL if not using it.

  \retval MLResult_AllocFailed If memory allocation fails.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure If internal error occurs.

  \priv None
*/
ML_DEPRECATED_MSG("Replaced by MLLifecycleInitEx.")
ML_API MLResult ML_CALL MLLifecycleInit(const struct MLLifecycleCallbacks *callbacks, void *context);

/*!
  \brief Retrieve the process specific information for the application.

  \param[out] out_self_info Pointer to the #MLLifecycleSelfInfo structure pointer. The
              user needs to explicitly free this structure by calling
              MLLifecycleFreeSelfInfo().

  \retval MLResult_AllocFailed If memory allocation fails.
  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure If internal error occurs.

  \priv None
*/
ML_API MLResult ML_CALL MLLifecycleGetSelfInfo(MLLifecycleSelfInfo **out_self_info);

/*!
  \brief Free the #MLLifecycleSelfInfo struct that is allocated by MLLifecycleGetSelfInfo().

  The pointer to the #MLLifecycleSelfInfo struct will point to NULL after this call.

  \param[in] info Pointer to #MLLifecycleSelfInfo struct pointer.

  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLLifecycleFreeSelfInfo(MLLifecycleSelfInfo **info);

/*!
  \brief Retrieve the initial arguments or "init args" for the application.
         The platform does not store the initial argument objects once the
         app has retrieved them. Subsequent calls to this API can return an
         empty list if there are no new init args available.

  \param[out] out_arg_list Pointer to the #MLLifecycleInitArgList structure pointer. The
              user needs to explicitly free this structure by calling
              MLLifecycleFreeInitArgList() function.

  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLResult_UnspecifiedFailure If internal error occurs.

  \priv None
*/
ML_API MLResult ML_CALL MLLifecycleGetInitArgList(MLLifecycleInitArgList **out_arg_list);

/*!
  \brief Retrieve the length of the init arg array.

  This function can return length of 0 which implies there are no init
  args available.

  \param[in] init_arg_list Pointer to #MLLifecycleInitArgList struct.
  \param[out] out_init_arg_list_length Pointer to variable that will hold length of #MLLifecycleInitArg array.

  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLLifecycleGetInitArgListLength(const MLLifecycleInitArgList *init_arg_list, int64_t* out_init_arg_list_length);

/*!
  \brief Retrieve the #MLLifecycleInitArg structure from MLLifecycleInitArgList() for given index.

  Note: The caller should not free the pointer returned.
  The memory will be released in the call to MLLifecycleFreeInitArgList().

  \param[in] init_arg_list Pointer to #MLLifecycleInitArgList struct.
  \param[in] index of the #MLLifecycleInitArg array.
  \param[out] out_init_arg Pointer to #MLLifecycleInitArg structure pointer.

  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLLifecycleGetInitArgByIndex(const MLLifecycleInitArgList *init_arg_list, int64_t index, const MLLifecycleInitArg **out_init_arg);

/*!
  \brief Retrieve the URI of the given MLLifecycleInitArg().

  This returned URI can be of the pattern <schema://> where schema can be
  http, ftp etc.

  Note: This field is typically used to pass URIs that the app can handle.
  However, it can be any string that the app developer wants.

  \param[in] init_arg Pointer to #MLLifecycleInitArg struct which was obtained by
             calling MLLifecycleGetInitArg().
  \param[out] out_uri Address of pointer to URI of the given #MLLifecycleInitArg pointer.
              The caller should not free the pointer returned.
              The memory will be released in the call to MLLifecycleFreeInitArgList()

  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLLifecycleGetInitArgUri(const MLLifecycleInitArg *init_arg, const char **out_uri);

/*!
  \brief Retrieve length of the #MLFileInfo array in the given #MLLifecycleInitArg.

  This function can return length of 0 which implies there is no file info available.

  \param[in] init_arg Pointer to #MLLifecycleInitArg array.
  \param[out] out_file_info_length Pointer to variable that will hold the length of #MLFileInfo array.

  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLLifecycleGetFileInfoListLength(const MLLifecycleInitArg *init_arg, int64_t *out_file_info_length);

/*!
  \brief Retrieve the #MLFileInfo structure from #MLLifecycleInitArg for given index.

  \param[in] init_arg Pointer to #MLLifecycleInitArg struct.
  \param[in] index of the #MLFileInfo array.
  \param[out] out_file_info Pointer to #MLFileInfo structure pointer.The caller should not free the pointer
              returned. The memory will be released in the call to
              MLLifecycleFreeInitArgList().

  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLLifecycleGetFileInfoByIndex(const MLLifecycleInitArg *init_arg, int64_t index, const MLFileInfo **out_file_info);

/*!
  \brief Free the #MLLifecycleInitArgList, #MLLifecycleInitArg and #MLFileInfo structures
  that are allocated by MLLifecycleGetInitArgList().

  The pointer to the #MLLifecycleInitArgList struct will point to NULL after this call.

  \param[in] init_arg_list Pointer to #MLLifecycleInitArgList struct pointer.

  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLLifecycleFreeInitArgList(MLLifecycleInitArgList **init_arg_list);

/*!
  \brief This function should be called by applications to indicate that they are
  done with their initialization sequences and ready for the user.

  Initialization checklist:
  Create graphics client connection with MLGraphicsCreateClient().

  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLResult_UnspecifiedFailure If internal error occurs.

  \priv None
*/
ML_API MLResult ML_CALL MLLifecycleSetReadyIndication();

/*!
  \brief Initializes MLLifecycleCallbacksEx with default values.

  \apilevel 7

  \param[in,out] inout_callbacks ML Lifecycle callbacks structure defined by
                 #MLLifecycleCallbacksEx that needs to be initialized.
*/
ML_STATIC_INLINE void MLLifecycleCallbacksExInit(MLLifecycleCallbacksEx *inout_callbacks) {
  if (inout_callbacks) {
    inout_callbacks->version = 2;
    inout_callbacks->on_stop = NULL;
    inout_callbacks->on_pause = NULL;
    inout_callbacks->on_resume = NULL;
    inout_callbacks->on_unload_resources = NULL;
    inout_callbacks->on_new_initarg = NULL;
    inout_callbacks->on_device_active = NULL;
    inout_callbacks->on_device_reality = NULL;
    inout_callbacks->on_device_standby = NULL;
    inout_callbacks->on_focus_lost = NULL;
    inout_callbacks->on_focus_gained = NULL;
  }
}

/*!
  \brief Initialize the lifecycle service interface.

  Applications MUST do this BEFORE intiliazing any other sub-system or requesting permissions as
  the application WILL be terminated if it does not register within the time-out period.

  \apilevel 5

  \param[in] callbacks Pointer to an #MLLifecycleCallbacksEx structure.
  \param[in] context Pointer to the application context that the application wants a
              reference to during callbacks. This parameter is optional and the user can pass a
              NULL if not using it.

  \retval MLResult_AllocFailed If memory allocation fails.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure If internal error occurs.

  \priv None
*/
ML_API MLResult ML_CALL MLLifecycleInitEx(const MLLifecycleCallbacksEx *callbacks, void *context);

/*! \} */

ML_EXTERN_C_END
