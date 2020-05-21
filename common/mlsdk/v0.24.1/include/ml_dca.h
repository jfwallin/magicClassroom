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
  \defgroup DCA
  \addtogroup DCA
  \sharedobject ml_dca
  \brief APIs for Desktop Companion.

  This interface allows applications to register with DCA to subscribe to services provided by DCA
  and to receive callbacks from it. Applications on the host side (PC/Desktop) can send messages,
  data and files with the applications registered with DCA on the device side.

  \{
*/

/*! DCA file modification types in the application document folder. */
typedef enum MLDCAAppFileModificationFlag {
  /*!  New file is created by the DCA in the application's documents folder. */
  MLDCAAppFileModificationFlag_FileCreated = 0,
  /*!  A file is removed by the DCA in the application's documents folder. */
  MLDCAAppFileModificationFlag_FileRemoved = 1,
  /*!  A file is modified by the DCA in the application's documents folder. */
  MLDCAAppFileModificationFlag_FileModifed = 2,
  MLDCAAppFileModificationFlag_Ensure32Bits = 0x7FFFFFFF
} MLDCAAppFileModificationFlag;

/*!
  \brief Callback to inform that app is unexpectedly unregistered from the DCA.

  Application gets notified through this callback if there is an unspecified error in DCA or
  if DCA is getting restarted or killed. This does not get triggered if app itself wants to
  unregister from the DCA through #MLDCAUnregister.

  \param[in] context The caller can pass in a context that can be used in the callback.

*/
typedef void (*MLDCAServiceFailureCB)(void *context);

/*!
  \brief Callback to provide message to the registered application.

  Calling this indicates a message is being sent to the registered application by DCA.
  Any host side application can send messages to the registered application on the device through
  this callback from DCA.

  \param[in] msg A string consisting of the message.
  \param[in] context The caller can pass in a context that can be used in the callback.
  \param[in] max_ret_size The maximum size message that will be returned to the desktop.
  \param[out] msg_response  A null terminated string that will be returned to the desktop so long as the function is
  complete within 10 milliseconds.
*/
typedef void (*MLDCAEncryptedMsgCB)(const char *msg, void *context, const int max_ret_size, char *out_msg_response);

/*!
  \brief Callback to provide ip address and port of the host side application to create a pipe.

  Calling this indicates a host side application running on the system with ipaddress is bound to
  the port and listening for connections. Application is expected to create a tcp connection with
  the ip address and port. Applications can choose the mode of connection (for example websockets)
  and it doesn't have to be plain tcp. Once connected, host side application can send data directly
  to the application on the device side and vice versa.

  \param[in] fd The file descriptor that has been created that represents a new socket communicating with an app
  \param[in] context The caller can pass in a context that can be used in the callback.
*/
typedef void (*MLDCACreateSockCB)(uint32_t fd, void *context);

/*!
  \brief Callback to inform file modification in application's document folder.

  This indicates that DCA have modified a file in the application's document folder. DCA uses this
  callback only if application have provided the write access to the document folder to DCA through
  #MLDCASetAppsDocFolderAccessMode.

  \param[in] file_path path of the file modified by the DCA.
  \param[in] file_modification_flag Type of modification made by the DCA on the file.
  \param[in] context The caller can pass in a context that can be used in the callback
*/
typedef void (*MLDCAFileModified)(const char *file_path, MLDCAAppFileModificationFlag file_modification_flag,
                                  void *context);

/*!
  \brief Register the application with DCA.

  DCA will start advertising the application to the host side once this call is made to DCA. This
  will allow host side applications to send messages to the registered applications through DCA.
  This should be the first DCA function called, as all others will fail when not yet registered.
  You may call MLDCARegister repeatedly to change msg_cb without interupting other operations.

  \param[in] msg_cb Callback triggered by DCA to the application if a host/PC side application
  sends message to the registered application.
  \param[in] context The caller can pass in a context that can be used in the callback.
  \param[in] max_response_size The maximum message response size possible.

  \retval MLResult_Ok if application is registerd with DCA successfully.
  \retval MLResult_UnspecifiedFailure Unable to register with DCA.

  \priv DesktopCompanion
*/

ML_API MLResult ML_CALL MLDCARegister(MLDCAEncryptedMsgCB msg_cb, void *context, int max_response_size);

/*!
  \brief Unregister the application with DCA.

  This will also remove any access provided to the DCA by the applciation and DCA will stop
  advertising the application to the host side. if application have created a dedicated socket
  with a host side application it is application's responsibility to close the pipe.

  \retval MLResult_Ok if application is unregistered with DCA successfully.
  \retval MLResult_UnspecifiedFailure if a failure occured during registration.

  \priv DesktopCompanion
*/
ML_API MLResult ML_CALL MLDCAUnregister();

/*!
  \brief Check if the calling application is currently registered with the DCA.

  out_currently_registered will be set to true if you have previoulsy called MLDCARegister unless some error has occured.
  While false, all other calls except MLDCARegister will fail.

  \param[out] out_currently_registered True if the application is currently registered to DCA, otherwise false.

  \retval MLResult_Ok if MLDCAPollFileWritesWithCB is successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLResult_InvalidParam if out_currently_registered is nullptr.

  \priv DesktopCompanion
*/
ML_API MLResult ML_CALL MLDCAGetCurrentlyRegistered(bool *out_currently_registered);

/*!
  \brief Sets a callback that will be immidiately triggered if there is an issue with the application's DCA subscription.

  If there is a service issue that breaks the connection between the calling app and the DCA, this callback will be
  triggered.
  Ideally this should never occur, but if an error needs to be noticed immidately this can be helpful.

  \param[in] service_failure_cb Callback triggered when a service failure occurs.
  \param[in] context Context to pass into the callback.

  \retval MLResult_Ok if MLDCAStopPipeSubscription is successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv DesktopCompanion
*/
ML_API MLResult ML_CALL MLDCASetServiceFailureCB(MLDCAServiceFailureCB service_failure_cb, void *context);

/*!
  \brief Indicate your app would like to have direct socket communication with a desktop app.

  Once this call is successful DCA will start advertising the calling application to the desktop
  as one of the applications allowed to create a direct socket. Once a desktop app connects to
  the calling app, createCb will be triggered, and its arguments will include a file descriptor
  that can be used to communicate with standard POSIX functions. Management of the lifecycle of
  this socket, including closing it, are the responsibility of the calling application. All
  communication over this socket is encrypted for you by the DCA.  Note that after recieving a
  socket, the calling app  will no longer be considered available for further new socket connections
  unless MLDCARequestNewSock is called again.

  \param[in] create_sock_cb MLDCACreateSockCB.
  \param[in] context context to pass into the callback.

  \retval MLResult_Ok if MLDCARequestNewSock is successful.
  \retval MLResult_UnspecifiedFailure if a failure occured during registration.

  \priv DesktopCompanion
*/
ML_API MLResult ML_CALL MLDCARequestNewSock(MLDCACreateSockCB create_sock_cb, void *context);

/*!
  \brief Stop subscription to creating pipes from DCA.

  Once this call is successful DCA will stop advertising the calling application from the list of
  applications allowed to create a direct socket.

  \retval MLResult_Ok if MLDCAStopPipeSubscription is successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv DesktopCompanion
*/
ML_API MLResult ML_CALL MLDCACancelSockRequest();

/*!
  \brief Provide read access to your application's documents folder for the DCA.

  Any files located below the specifed access path may be copied to connected desktops, and any directories can be
  scanned. Specify an access path of "." or "" to allow full access. The connected desktop will not be aware of any
  access
  restriction, nor the lack therof. Note that while you may specify separate read and write access paths are possible,
  this can easilly cause confusion if not intended.

  \param[in] access_path all desktop read requests will be prefixed with this access path.

  \retval MLResult_Ok if MLDCADocAllowReadAccess is successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv DesktopCompanion
*/
ML_API MLResult ML_CALL MLDCADocAllowReadAccess(const char *access_path);

/*!
  \brief Provide write access to your application's documents folder for the DCA.

  Connected desktops may create directories and write files below the specified access path. Specify an access path of
  "." or "" to allow full access. The connected desktop will not be aware of any access restriction, nor the lack
  therof.
  When a file is written, the file_modified callback will be triggered.
  Note that while you may specify separate read and write access paths are possible,
  this can easilly cause confusion if not intended.

  \param[in] access_path all desktop read requests will be prefixed with this access path.
  \param[in] file_modified callback triggered when any file write is complete.
  \param[in] context Context to pass into the callback.

  \retval MLResult_Ok if MLDCADocAllowReadAccess is successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv DesktopCompanion
*/
ML_API MLResult ML_CALL MLDCADocAllowWriteAccess(const char *access_path, MLDCAFileModified file_modified,
                                                 void *context);

/*!
  \brief Provide read and write access to your application's documents folder for the DCA.

  Equivelant to calling MLDCADocAllowReadAccess and MLDCADocAllowWriteAccess with the same access_path

  \param[in] access_path all desktop read requests will be prefixed with this access path.
  \param[in] file_modified callback triggered when any file write is complete.
  \param[in] context Context to pass into the callback.

  \retval MLResult_Ok if MLDCADocAllowWriteAccess is successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv DesktopCompanion
*/
ML_API MLResult ML_CALL MLDCADocAllowReadWriteAccess(const char *access_path, MLDCAFileModified file_modified,
                                                     void *context);

/*!
  \brief Stop allowing read access to the DCA.

  \retval MLResult_Ok if MLDCADocAllowReadWriteAccess is successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv DesktopCompanion
*/
ML_API MLResult ML_CALL MLDCADocRevokeReadAccess();

/*!
  \brief Stop allowing write access to the DCA.

  \retval MLResult_Ok if MLDCADocRevokeReadAccess is successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv DesktopCompanion
*/
ML_API MLResult ML_CALL MLDCADocRevokeWriteAccess();

/*!
  \brief Stop allowing read and write access to the DCA.

  \retval MLResult_Ok if MLDCADocRevokeWriteAccess is successful.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv DesktopCompanion
*/
ML_API MLResult ML_CALL MLDCADocRevokeReadWriteAccess();

ML_EXTERN_C_END
