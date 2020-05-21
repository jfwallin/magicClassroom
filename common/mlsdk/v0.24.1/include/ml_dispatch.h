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
#include "ml_fileinfo.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Dispatch
  \sharedobject ml_dispatch
  \brief This interface lets an application query the platform to handle things
  that the app itself cannot or wants someone else to handle.
  For example, if an application comes across a schema tag that it doesn't know
  what to do with, it can query the platform to see if some other application might.
  This can be useful for handling http://, https:// or other custom Schema://<arg1>/<arg2>
  Apart from handling schema tags in URIs, this interface can also be used
  to query the platform to handle a type of file based on file-extension or mime-type.
  \{
*/

enum {
  /*! Defines the prefix for MLDispatchResult codes. */
  MLResultAPIPrefix_Dispatch = MLRESULT_PREFIX(0xBBE0)
};

/*!
  \brief Return values for Dispatch API calls.
  \apilevel 6
*/
typedef enum MLDispatchResult {
  /*! Cannot start app. */
  MLDispatchResult_CannotStartApp = MLResultAPIPrefix_Dispatch,
  /*! Invalid packet. */
  MLDispatchResult_InvalidPacket,
  /*! No app found. */
  MLDispatchResult_NoAppFound,
  /*! App packet dialog failure. */
  MLDispatchResult_AppPickerDialogFailure,
  MLDispatchResult_InvalidSchema,
  /*! Invalid url */
  MLDispatchResult_InvalidUrl,
  /*! Schema already has been registered */
  MLDispatchResult_SchemaAlreadyRegistered,
  /*! Ensure enum is represented as 32 bits. */
  MLDispatchResult_Ensure32Bits = 0x7FFFFFFF
} MLDispatchResult;

/*!
  \brief #MLDispatchPacket type can be used with this interface.
  It can be used to pass a URI string or #MLFileInfo objects
  to the platform.
*/
typedef struct MLDispatchPacket MLDispatchPacket;

/*!
  \deprecated Deprecated since 0.24.0. Scheduled for removal. Replaced by \ref MLDispatchOAuthCallbacks.
  \brief #ml_dispatch_oauth_schema_handler is invoked when the reply from oauth
  authorization step has been completed.

  \apilevel 6

  \param[in] response The response url from authorization service.
*/
typedef ML_DEPRECATED_MSG("Replaced by MLDispatchOAuthCallbacks")
void (*ml_dispatch_oauth_schema_handler)(const char* response);

/*!
  \brief MLDispatch OAuth response.
  This response is provided via the #MLDispatchOAuthCallbacks when the reply
  from the OAuth authorization step has been completed.

  \apilevel 8
*/
typedef struct MLDispatchOAuthResponse {
  /*! Pointer to the context passed during call to MLDispatchOAuthRegisterSchemaEx*/
  void* context;
  /*! The response url from authorization service.*/
  const char* response;
 } MLDispatchOAuthResponse;

/*!
  \brief Dispatch OAuth callback functions.

  This structure must be initialized by calling MLLDispatchOAuthCallbacksInit() before use.

  \apilevel 8
*/
typedef struct MLDispatchOAuthCallbacks {
  /*! Version of this structure. */
  uint32_t version;
  /*!
    \brief #oauth_schema_handler is invoked when the reply from oauth
    authorization step has been completed.

    \param[in] context Pointer to the MLDispatch OAuth response.
  */
  void (*oauth_schema_handler)(MLDispatchOAuthResponse* response);
} MLDispatchOAuthCallbacks;

/*!
  \brief Initializes MLDispatchOAuthCallbacks with default values.

  \apilevel 8

  \param[in,out] inout_callbacks OAuth callbacks structure defined by
                 #MLDispatchOAuthCallbacks that needs to be initialized.
*/
ML_STATIC_INLINE void MLDispatchOAuthCallbacksInit(MLDispatchOAuthCallbacks *inout_callbacks) {
  if (inout_callbacks) {
    inout_callbacks->version = 1;
    inout_callbacks->oauth_schema_handler = NULL;
  }
}

/*!
  \brief The oauth window open request parameters

  The parameters defined by #MLDispatchOAuthOpenWindowRequest are supplied to
  #MLDispatchOAuthOpenWindowEx from the client to open an oauth window.

  This structure must be initialized by calling MLDispatchOAuthOpenWindowRequestInit() before use.

  \apilevel 8
*/
typedef struct MLDispatchOAuthOpenWindowRequest {
  /*! Version of this structure. */
  uint32_t version;
  /*! The oauth url to navigate to. */
  const char* url;
  /*! The cancel uri called by the browser when users cancels the window. */
  const char* cancel_uri;
  /*!
    \brief Flag to set if http requests are not allowed. Default: false.

    If this is set to true and url points to http, the request will fail
    and the cancel uri callback will return the failure response.
  */
  bool disallow_http;
} MLDispatchOAuthOpenWindowRequest;

/*!
  \brief Initializes MLDispatchOAuthOpenWindowRequest with default values.

  \apilevel 8

  \param[in,out] inout_request OAuth open window request structure defined by
                 #MLDispatchOAuthOpenWindowRequest that needs to be initialized.
*/
ML_STATIC_INLINE void MLDispatchOAuthOpenWindowRequestInit(MLDispatchOAuthOpenWindowRequest *inout_request) {
  if (inout_request) {
    inout_request->version = 1;
    inout_request->url = NULL;
    inout_request->cancel_uri = NULL;
    inout_request->disallow_http = false;
  }
}

/*!
  \brief Create empty dispatch packet.

  The MLFileInfo array in the #MLDispatchPacket returned by this function
  will be NULL as will the uri. In order to allocate memory for these
  members, caller will have to explicitly call #MLDispatchAllocateFileInfoList
  and #MLDispatchAllocateUri respectively.

  The caller will need to free this structure by calling #MLDispatchReleasePacket.

  \param[out] out_packet A pointer to #MLDispatchPacket structure on success and NULL on failure.

  \retval MLResult_AllocFailed If failed to allocate dispatch packet.
  \retval MLResult_Ok If dispatch packet was allocated successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLDispatchAllocateEmptyPacket(MLDispatchPacket **out_packet);

/*!
  \brief Release the #MLDispatchPacket that is allocated by #MLDispatchAllocateEmptyPacket
  and all its resources. The pointer to the #MLDispatchPacket struct will point to NULL after this call.

  \param[in] packet Pointer to #MLDispatchPacket struct pointer.
  \param[in] release_members If \c true, function will attempt to release/free
             #MLFileInfo array and uri members from the MLDispatchPacket.
  \param[in] close_fds If \c true, function will attempt to close the fds in #MLFileInfo,
             If \c false, caller will have to close fds.

  \retval MLResult_InvalidParam If a function parameter is not valid.
  \retval MLResult_Ok If memory allocated to dispatch packet is released successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLDispatchReleasePacket(MLDispatchPacket **packet, bool release_members, bool close_fds);

/*!
  \brief Allocate an empty #MLFileInfo array in the #MLDispatchPacket for given length.

  The caller can release/free by calling MLDispatchReleaseFileInfo() or by calling
  MLDispatchReleasePacket()

  \param[in,out] packet Pointer to #MLDispatchPacket whose #MLFileInfo* member will be
                 allocated file_info_list_length entries.
  \param[in] file_info_list_length Maximum length of the file info array to be allocated.

  \retval MLResult_AllocFailed If allocation of #MLFileInfo array failed.
  \retval MLResult_InvalidParam If a function parameter is not valid.
  \retval MLResult_Ok If #MLFileInfo array is successfully allocated file_info_list_length entries.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLDispatchAllocateFileInfoList(MLDispatchPacket *packet, uint64_t file_info_list_length);

/*!
  \brief This API retrieves length of the #MLFileInfo array in the given #MLDispatchPacket.

  This function can return length of 0 which implies there is no file info
  available.

  \param[in] packet Pointer to #MLDispatchPacket whose #MLFileInfo* member's list length
             is calculated and stored in out_file_info_list_length.
  \param[out] out_file_info_list_length length of #MLFileInfo array.

  \retval MLResult_InvalidParam If a function parameter is not valid.
  \retval MLResult_Ok If operation is successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLDispatchGetFileInfoListLength(const MLDispatchPacket *packet, uint64_t *out_file_info_list_length);

/*!
  \brief Get the #MLFileInfo at the given index.

  The #MLFileInfo array should have been allocated by calling MLDispatchAllocateFileInfoList().

  \param[in] packet Pointer to #MLDispatchPacket whose #MLFileInfo array for given index will be returned.
  \param[in] index Index of the #MLFileInfo array.
  \param[out] out_file_info Pointer to #MLFileInfo* in packet at index.

  \retval MLResult_InvalidParam If a function parameter is not valid.
  \retval MLResult_Ok if the #MLFileInfo object at the parameter index can be retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLDispatchGetFileInfoByIndex(const MLDispatchPacket *packet, int64_t index, MLFileInfo **out_file_info);

/*!
  \brief Populate #MLFileInfo array in the #MLDispatchPacket for current index.

  The caller will have to call MLDispatchAllocateFileInfoList() before calling this
  function. After obtaining the length of the list through MLDispatchGetFileInfoListLength(),
  the caller should get each #MLFileInfo structures in the array through
  MLDispatchGetFileInfoByIndex(). After setting the fields of #MLFileInfo using mutator
  functions, this function should be called to add the #MLFileInfo just set to the
  dispatch packet.

  \param[in,out] packet Pointer to #MLDispatchPacket whose #MLFileInfo* member will be populated.
  \param[in] finfo #MLFileInfo structure that will be added to the #MLDispatchPacket.

  \retval MLResult_AllocFailed If failed to allocate memory for one of the fields in #MLFileInfo
          or there are no available slots in the #MLFileInfo array in the packet.
  \retval MLResult_InvalidParam If a function parameter is not valid.
  \retval MLResult_Ok If parameter finfo was successfully added to #MLFileInfo array in the packet.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLDispatchAddFileInfo(MLDispatchPacket *packet, const MLFileInfo *finfo);

/*!
  \brief Release the pointer to #MLFileInfo array that is allocated by MLDispatchAllocateFileInfoList().

  The caller will have to call MLDispatchAllocateFileInfoList() before calling this
  function. The #MLFileInfo pointer in #MLDispatchPacket will point to NULL after
  this call.

  \param[in] packet Pointer to MLDispatchPacket.
  \param[in] close_fds If \c true, function will attempt to close the fds in #MLFileInfo,
             if \c false, caller will have to close fds.

  \retval MLResult_InvalidParam If a function parameter is not valid.
  \retval MLResult_Ok If memory allocated to #MLFileInfo array was released successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLDispatchReleaseFileInfoList(MLDispatchPacket *packet, bool close_fds);

/*!
  \brief Allocate and assign URI in the #MLDispatchPacket.

  The caller can release/free by calling MLDispatchReleaseUri() or by calling
  MLDispatchReleasePacket().

  \param[in,out] packet Pointer to #MLDispatchPacket whose uri member will be allocated and populated.
  \param[in] uri Value assigned to #MLDispatchPacket's uri member.

  \retval MLResult_AllocFailed If failed to allocate uri.
  \retval MLResult_InvalidParam If a function parameter is not valid.
  \retval MLResult_Ok If operation is succesfful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLDispatchSetUri(MLDispatchPacket *packet, const char *uri);

/*!
  \brief Release uri that is allocated by MLDispatchAllocateUri().

  The caller will have to call MLDispatchSetUri() before calling this
  function. The char pointer uri in #MLDispatchPacket will point to NULL after
  this call.

  \param[in] packet Pointer to #MLDispatchPacket struct pointer.

  \retval MLResult_InvalidParam If a function parameter is not valid.
  \retval MLResult_Ok If operation is successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLDispatchReleaseUri(MLDispatchPacket *packet);

/*!
  \brief Try to open the application that supports a given mime type or schema type.

  If the caller does not specify a mime-type or schema type in the dispatch packet,
  dispatch service will try to open an application which supports the file extension
  specified in the file name.

  \param[in] packet Pointer to #MLDispatchPacket structure.

  \retval MLResult_Ok If an application was found that can open a given mime or schema type,.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLDispatchResult Dispatch specific error occurred.

  \priv None
*/
ML_API MLResult ML_CALL MLDispatchTryOpenApplication(const MLDispatchPacket *packet);

/*!
  \deprecated Deprecated since 0.24.0. Scheduled for removal. Replaced by \ref MLDispatchOAuthRegisterSchemaEx.
  \brief Register a unique schema for OAuth redirect handler.

  The caller need to ensure that the schema is unique. If the schema
  is already registered the function will return an error. The handler
  will be called once the authorization procedure has been completed.
  The caller should register two schema callbacks. The first will be for
  authorization redirect and the second schema will be in case the user cancels
  or an error happens in the authentication procedure.

  \apilevel 6

  \param[in] schema A unique string that will match the redirect uri schema
  \param[in] handler The callback handler when this schema redirect uri has been invoked.

  \retval MLResult_Ok If the new schema has been registered correctly.
  \retval MLResult_PrivilegeDenied If SecureBrowserWindow privilege is denied.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLDispatchResult_SchemaAlreadyRegistered Schema already is registered.
  \retval MLDispatchResult Dispatch specific error occurred.

  \priv SecureBrowserWindow
*/
ML_DEPRECATED_MSG("Replaced by MLDispatchOAuthRegisterSchemaEx.")
ML_API MLResult ML_CALL MLDispatchOAuthRegisterSchema(const char* schema, ml_dispatch_oauth_schema_handler handler);


/*!
  \brief Register a unique schema for OAuth redirect handler.

  The caller need to ensure that the schema is unique. If the schema
  is already registered the function will return an error. The handler
  will be called once the authorization procedure has been completed.
  The caller should register two schema callbacks. The first will be for
  authorization redirect and the second schema will be in case the user cancels
  or an error happens in the authentication procedure.

  \apilevel 8

  \param[in] schema A unique string that will match the redirect uri schema
  \param[in] callbacks Pointer to an #MLDispatchOAuthCallbacks structure.
  \param[in] context Pointer to the context that the application wants a
             reference to during handler callback.

  \retval MLResult_Ok If the new schema has been registered correctly.
  \retval MLResult_PrivilegeDenied If SecureBrowserWindow privilege is denied.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLDispatchResult_SchemaAlreadyRegistered Schema already is registered.
  \retval MLDispatchResult Dispatch specific error occurred.

  \priv SecureBrowserWindow
*/
ML_API MLResult ML_CALL MLDispatchOAuthRegisterSchemaEx(const char* schema, const MLDispatchOAuthCallbacks*  callbacks, void* context);


/*!
  \brief Unregister a unique schema for OAuth redirect handler.

  \apilevel 6

  \param[in] schema A unique string that will match the redirect uri schema.

  \retval MLResult_Ok If the new schema has been registered correctly.
  \retval MLResult_PrivilegeDenied If SecureBrowserWindow privilege is denied.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLDispatchResult Dispatch specific error occurred.

  \priv SecureBrowserWindow
*/
ML_API MLResult ML_CALL MLDispatchOAuthUnregisterSchema(const char* schema);


/*!
  \deprecated Deprecated since 0.24.0. Scheduled for removal. Replaced by \ref MLDispatchOAuthOpenWindowEx.
  \brief Open a secure browser window to perfrom an oauth authentication.

  Will open a special browser window that will be lazy head locked to
  the user's head movement. The browser window will close once the
  authentication procedure has been completed.

  \apilevel 6

  \param[in] url The oauth url to navigate to.
  \param[in] cancel_uri The cancel uri called by the browser when users cancels the window.

  \retval MLResult_Ok If the browser oauth window opened correctly.
  \retval MLResult_PrivilegeDenied If SecureBrowserWindow privilege is denied.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLDispatchResult Dispatch specific error occurred.

  \priv SecureBrowserWindow
*/
ML_DEPRECATED_MSG("Replaced by MLDispatchOAuthOpenWindowEx.")
ML_API MLResult ML_CALL MLDispatchOAuthOpenWindow(const char* url, const char* cancel_uri);


/*!
  \brief Open a secure browser window to perfrom an oauth authentication.

  Will open a special browser window that will be lazy head locked to
  the user's head movement. The browser window will close once the
  authentication procedure has been completed.

  \apilevel 8

  \param[in] request Pointer to an #MLDispatchOAuthOpenWindowRequest structure.

  \retval MLResult_Ok If the browser oauth window opened correctly.
  \retval MLResult_PrivilegeDenied If SecureBrowserWindow privilege is denied.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLDispatchResult Dispatch specific error occurred.

  \priv SecureBrowserWindow
*/
ML_API MLResult ML_CALL MLDispatchOAuthOpenWindowEx(const MLDispatchOAuthOpenWindowRequest* request);

/*!
  \brief Returns an ASCII string for MLDispatchResult and MLResultGlobal codes.

  \param[in] result_code The input MLResult enum from MLDispatch functions.

  \return ASCII string containing readable version of result code.

  \priv None
*/
ML_API const char* ML_CALL MLDispatchGetResultString(MLResult result_code);

/*! \} */

ML_EXTERN_C_END
