// %BANNER_BEGIN%
// ---------------------------------------------------------------------
// %COPYRIGHT_BEGIN%
//
// Copyright (c) 2019 Magic Leap, Inc. (COMPANY) All Rights Reserved.
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
// --------------------------------------------------------------------
// %BANNER_END%

#pragma once

#include "ml_api.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Connections
  \sharedobject ml_connections
  \brief APIs for Magic Leap connections.

  MLConnections APIs provide support for multi-user experiences by providing
  access to connect with a user's Magic Leap social connections.

  Applications can request to send an invite to initate a multi-user
  experience to a user's Magic Leap connections (it is assumed that the invite
  receiver has already installed a copy of the application). Invites can carry
  a given payload message to the invite receiver application.
  No particular connectivity mechanism is assumed by this API.

  Applications can request to receive an invite to a multi-user experience by
  registering with the API.

  When sending an invite, end-users will be presented with a
  invite dialog, requesting them to select which of their
  connections should be invited. Invite handling is handled internally by the
  system; no Magic Leap connections profile details are exposed to either the
  initiating or receiving application.

  MLConnections API users can submit a request for an operation and receive a
  handle, which can be used to retrieve the result of the operation asynchronously
  at a later time. Handles can be used to poll for operation result using a
  TryGet function. These functions are non-blocking and will either return
  request not ready status or request results once ready.

  MLConnections API users are also responsible for calling
  MLConnectionsReleaseRequestResources to free any resources associated with a
  request handle once it is no longer needed.
  \{
*/

enum {
  MLResultAPIPrefix_Connections = MLRESULT_PREFIX(0xbfae)
};

/*!
  \brief Return result values for API calls that are relevant to connections.

  \apilevel 7
*/
typedef enum MLConnectionsResult {
  /*! This MLHandle is not recognized. */
  MLConnectionsResult_InvalidHandle = MLResultAPIPrefix_Connections,

  /*! The number of invitees is beyond the system defined threshold. */
  MLConnectionsResult_InvalidInviteeCount = MLResultAPIPrefix_Connections + 1,

  /*! The number of selectees is beyond the system defined threshold. */
  MLConnectionsResult_InvalidSelecteeCount = MLConnectionsResult_InvalidInviteeCount,

  /*! Indicates invite request has been found and the system is attempting to cancel the process. */
  MLConnectionsResult_CancellationPending = MLResultAPIPrefix_Connections + 2,

  /*!
      Request failed due to system being in an illegal state,
      e.g. user has not successfully logged-in.
  */
  MLConnectionsResult_IllegalState = MLResultAPIPrefix_Connections + 3,

  /*!
      MLConnectionsRegisterForInvite() failed because the system had an error
      with network connectivity, or the servers could not be reached.
   */
  MLConnectionsResult_NetworkFailure = MLResultAPIPrefix_Connections + 4,

  /*!
      MLConnectionsRegisterForInvite() failed because the application is
      already registered to handle invite requests.
   */
  MLConnectionsResult_AlreadyRegistered = MLResultAPIPrefix_Connections + 5,

  /*!
      Indicates a general system failure.
   */
  MLConnectionsResult_SystemFailure = MLResultAPIPrefix_Connections + 6,

  MLConnectionsResult_Ensure32Bits = 0x7FFFFFFF
} MLConnectionsResult;

/*!
  \brief Defines default filter for Magic Leap connections when displaying invite dialog to the user MLConnectionsRequestSelection().

  \apilevel 8
*/
typedef enum MLConnectionsSelectionFilter {
  /*! Show Magic Leap connections who are online and followed by current user. */
  MLConnectionsSelectionFilter_Following = 0,

  /*! Show Magic Leap connections who are online and follow current user. */
  MLConnectionsSelectionFilter_Followers = 1,

  /*! Show Magic Leap connections who are online and are mutual followers for current user. */
  MLConnectionsSelectionFilter_Mutual = 2,

  MLConnectionsSelectionFilter_Ensure32Bits = 0x7FFFFFFF
} MLConnectionsSelectionFilter;

/*!
  \brief Defines possible status values for selecting connections using MLConnectionsRequestSelection().

  \apilevel 8
*/
typedef enum MLConnectionsSelectionStatus {
  /*! Indicates the request to start the sending process has been submitted to the system. */
  MLConnectionsSelectionStatus_Pending = 0,

  /*! Indicates that the selection is ready. */
  MLConnectionsSelectionStatus_Confirmed = 1,

  /*! Indicates that the selection was cancelled by user action. */
  MLConnectionsSelectionStatus_Cancelled = 2,

  /*! Indicates that the selection failed unexpectedly. */
  MLConnectionsSelectionStatus_Failed = 3,

  MLConnectionsSelectionStatus_Ensure32Bits = 0x7FFFFFFF
} MLConnectionsSelectionStatus;

/*!
  \brief Defines possible status values for sending invites MLConnectionsRequestInvite().

  \apilevel 7
*/
typedef enum MLConnectionsInviteStatus {
  /*! Indicates the request to start the sending process is being submitted to the system. */
  MLConnectionsInviteStatus_SubmittingRequest = 0,

  /*! Indicates sending process has successfully initiated and invite dialog is being displayed to the user. */
  MLConnectionsInviteStatus_Pending = 1,

  /*!
      Indicates invite dialog has been completed by the user the invite was successfully sent.
      Invite request resources ready to be freed MLConnectionsReleaseRequestResources().
  */
  MLConnectionsInviteStatus_Dispatched = 2,

  /*! Indicates that the user has completed the invite dialog but the system was unable to send the invite. */
  MLConnectionsInviteStatus_DispatchFailed = 3,

  /*! Indicates sending process was cancelled either by user, system or MLConnectionsCancelInvite(). */
  MLConnectionsInviteStatus_Cancelled = 4,

  /*! Unable to determine invite status, because provided handle is invalid. */
  MLConnectionsInviteStatus_InvalidHandle = 5,

  MLConnectionsInviteStatus_Ensure32Bits = 0x7FFFFFFF
} MLConnectionsInviteStatus;

/*!
  \brief Defines default filter for Magic Leap connections when displaying invite dialog to the user MLConnectionsRequestInvite().

  \apilevel 8
*/
typedef enum MLConnectionsInviteeFilter {
  /*! Show Magic Leap connections who are online and follow current user. */
  MLConnectionsInviteeFilter_Followers = 0,

  /*! Show Magic Leap connections who are online and are mutual followers for current user. */
  MLConnectionsInviteeFilter_Mutual = 1,

  /*! Defaults the invitation dialog to the join by code page. */
  MLConnectionsInviteeFilter_JoinByCode = 2,

  MLConnectionsInviteeFilter_Ensure32Bits = 0x7FFFFFFF
} MLConnectionsInviteeFilter;

/*!
  \brief Defines invitation status types in \see MLConnectionsInviteeStatus.

  \apilevel 8
*/
typedef enum MLConnectionsInvitationDeliveryStatus {
  /*! The invitation request is being processed. */
   MLConnectionsInvitationDeliveryStatus_Processing = 0,

  /*! The invitee was found and the invitation was dispatched. */
   MLConnectionsInvitationDeliveryStatus_Dispatched = 1,

  /*! The invitee is online and the invitation was delivered. */
   MLConnectionsInvitationDeliveryStatus_Delivered = 2,

  /*! A new participant joined. */
   MLConnectionsInvitationDeliveryStatus_Joined = 3,

  /*! A system level failure prevented the delivery of the invitation to this invitee. */
   MLConnectionsInvitationDeliveryStatus_Failed = 4
} MLConnectionsInvitationDeliveryStatus;

/*!
  \brief Stores a connection identifier.

  \apilevel 8
*/
typedef const char *MLConnectionsID;

/*!
  \struct MLConnectionsConnection

  \brief Representation of available information for a single connection in address book.

  \apilevel 8
*/
typedef struct MLConnectionsConnection {
  /*! Version of this structure. */
  uint32_t version;

  /*! Locally-unique connection identifier. Generated by the system. May change across reboots. */
  MLConnectionsID id;

  /*! Connections's username. */
  const char *username;

  /*! Connections's avatar personalization. */
  const char *avatar_personalization;

} MLConnectionsConnection;

/*!
  \brief Initializes default values for MLConnectionsConnection.

  \apilevel 8

  \param[in, out] inout_connection The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLConnectionsConnectionInit(MLConnectionsConnection *inout_connection) {
  inout_connection->version = 1u;
  inout_connection->id = NULL;
  inout_connection->username= NULL;
  inout_connection->avatar_personalization= NULL;
}

/*!
  \struct MLConnectionsConnectionList

  \brief Stores a list of MLConnectionsConnection.

  \apilevel 8
*/
typedef struct MLConnectionsConnectionList {
  /*! Version of this structure. */
  uint32_t version;

  /*! Number of connections. */
  size_t count;

  /*! Pointer referring to array of connections. */
  MLConnectionsConnection *const *connections;
} MLConnectionsConnectionList;

/*!
  \brief Initializes default values for MLConnectionsConnectionList.

  \apilevel 8

  \param[in, out] inout_connection_list The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLConnectionsConnectionListInit(MLConnectionsConnectionList *inout_connection_list) {
  inout_connection_list->version = 1u;
  inout_connection_list->count = 0;
  inout_connection_list->connections = NULL;
}

/*!
  \struct MLConnectionsInviteeStatus

  \brief Defines invitee status to be delivered in the invitee status callbacks.

  \apilevel 8
*/
typedef struct MLConnectionsInviteeStatus {
  /*! Version of this structure. */
  uint32_t version;

  /*! Locally-unique connection identifier. Generated by the system. May change across reboots. */
  const char *id;

  /*! Connections's username. */
  const char *username;

  /*! Connections's avatar personalization. */
  const char *avatar_personalization;

  /*! The invitation delivery status for the invitee. */
  MLConnectionsInvitationDeliveryStatus invitation_delivery_status;
} MLConnectionsInviteeStatus;

/*!
  \brief Initializes default values for MLConnectionsInviteeStatus.

  \apilevel 8

  \param[in, out] inout_invitee_status The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLConnectionsInviteeStatusInit(MLConnectionsInviteeStatus *inout_invitee_status) {
  inout_invitee_status->version = 1u;
  inout_invitee_status->id = NULL;
  inout_invitee_status->username = NULL;
  inout_invitee_status->avatar_personalization = NULL;
  inout_invitee_status->invitation_delivery_status = MLConnectionsInvitationDeliveryStatus_Processing;
}

/*!
  \struct MLConnectionsInvitationResult

  \brief Defines invitation result delivered by the callback registered with \see MLConnectionsRequestInvite.

  \apilevel 8
*/
typedef struct MLConnectionsInvitationResult {
  /*! Version of this structure. */
  uint32_t version;

  /*! General status of invitation request. */
  MLConnectionsInviteStatus invitation_status;

  /*! The join code to display in app. */
  const char *join_code;
} MLConnectionsInvitationResult;

/*!
  \brief Initializes default values for MLConnectionsInvitationResult.

  \apilevel 8

  \param[in, out] inout_invitation_result The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLConnectionsInvitationResultInit(MLConnectionsInvitationResult *inout_invitation_result) {
  inout_invitation_result->version = 1u;
  inout_invitation_result->invitation_status = MLConnectionsInviteStatus_SubmittingRequest;
  inout_invitation_result->join_code = NULL;
}

/*!
  \struct MLConnectionsInviteArgs

  \brief Stores arguments for the sending invite process.

  \apilevel 8
*/
typedef struct MLConnectionsInviteArgs {
  /*! Version of this structure. */
  uint32_t version;

  /*! Max number of connections to be invited. Min limit is 1. */
  uint32_t invitee_count;

  /*!
      Text prompt to be displayed to the user with invitee selection dialog.
      Caller should allocate memory for this.
      Encoding should be in UTF8.
      This will be copied internally.
      The max length for the prompt is 40 characters.
  */
  const char *invite_user_prompt;

  /*!
      Payload message to be delivered to remote copy of the application with invite in serialized text form.
      Caller should allocate memory for this.
      Encoding should be in UTF8.
      This will be copied internally.
      The max length for the payload is 1200 bytes.
  */
  const char *invite_payload;

  /*! Type of filter applied by default to ML connections list in invitee selection dialog. */
  MLConnectionsInviteeFilter default_invitee_filter;

  /*! The context for callbacks to be registered for this call. */
  void *context;

  /*!
    \brief Invitation Result Callback.

    This is the callback that will be called when invitation has been confirmed by the user.
    If this callback is supplied the handle is automatically cleaned up when invite is canceled,
    failed or dispatched.

    \param[in] result The invitation results.
    \param[in] context This is the context field that was passed to MLConnectionsRequestInvite().
  */
  void (*on_invitation_result)(const MLConnectionsInvitationResult *result, void *context);

  /*!
    \brief Invitee Status Callback.

    This is the callback that will be called when invitee statuses become available.

    \note
    Refer to MLConnectionsRequestInvite, when sending invitation using join code
    this callback is not expected to be triggred, this may change in the future.

    \param[in] status The invitee status and details.
    \param[in] context This is the context field that was passed to MLConnectionsRequestInvite().
  */
  void (*on_invitee_status)(const MLConnectionsInviteeStatus *status, void *context);
} MLConnectionsInviteArgs;

/*!
  \brief Initializes default values for #MLConnectionsInviteArgs.

  \apilevel 8

  \param[in,out] inout_args.
*/
ML_STATIC_INLINE void MLConnectionsInviteArgsInit(MLConnectionsInviteArgs *inout_args) {
  inout_args->version = 2u;
  inout_args->invitee_count = 5u;
  inout_args->invite_user_prompt = NULL;
  inout_args->invite_payload = NULL;
  inout_args->default_invitee_filter = MLConnectionsInviteeFilter_Followers;
  inout_args->context = NULL;
  inout_args->on_invitation_result = NULL;
  inout_args->on_invitee_status = NULL;
}

/*!
  \struct MLConnectionsInviteCallbacks

  \brief MLConnections invitation callbacks.

  \apilevel 7
*/
typedef struct MLConnectionsInviteCallbacks {
  /*! Version of this structure. */
  uint32_t version;

/*!
  \brief On registration complete callback.

  This is the callback that will be called when Registration has completed,
  either successfully or not. It will be called only be called once.

  \param[in] result This is the result of the Registration. It will be MLResult_Ok in case of success, and an error otherwise.
  \param[in] context This is the context param that was passed to MLConnectionsRegisterForInvite().
*/
  void (*on_registration_complete)(MLResult result, void *context);

/*!
  \brief On invitaion callback.

  This is the callback that will be called every time an invitation comes in.
  If user_accepted is true, this means the user was shown a notification and accepted
  the invitation to launch the application. In this case, you should take the user
  directly to the invitation context. If it is false, you should display the notification
  yourself in a way that is most in-line with your application's style and design. The
  payload parameter will be the data sent from the inviting application on another device.
  The payload will contain the required session context.

  \param[in] user_accepted This is a boolean value that informs you whether the user has confirmed the invite notification.
  \param[in] payload This is the payload that was sent from the inviting application.
  \param[in] context This is the context param that was passed to MLConnectionsRegisterForInvite().
*/
  void (*on_invitation)(bool user_accepted, const char *payload, void *context);
} MLConnectionsInviteCallbacks;

/*!
  \brief Initializes default values for #MLConnectionsInviteCallbacks.

  \apilevel 7

  \param[in,out] inout_args.
*/
ML_STATIC_INLINE void MLConnectionsInviteCallbacksInit(MLConnectionsInviteCallbacks *inout_args) {
  inout_args->version = 1u;
  inout_args->on_registration_complete = NULL;
  inout_args->on_invitation = NULL;
}

/*!
  \struct MLConnectionsSelectionResult

  \brief Stores result of a selection \see MLConnectionsRequestSelection.

  \apilevel 8
*/
typedef struct MLConnectionsSelectionResult {
  /*! Version of this structure. */
  uint32_t version;

  /*! Status of operation. */
  MLConnectionsSelectionStatus selection_status;

  /*! Resultant list of connections. */
  MLConnectionsConnectionList result_list;
} MLConnectionsSelectionResult;

/*!
  \brief Initializes default values for MLConnectionsSelectionResult.

  \apilevel 8

  \param[in, out] inout_result The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLConnectionsSelectionResultInit(MLConnectionsSelectionResult *inout_result) {
  inout_result->version = 1u;
  inout_result->selection_status = MLConnectionsSelectionStatus_Pending;
  MLConnectionsConnectionListInit(&inout_result->result_list);
}

/*!
  \struct MLConnectionsSelectionArgs

  \brief Stores arguments for the sending invite process \see MLConnectionsRequestSelection.

  \apilevel 8
*/
typedef struct MLConnectionsSelectionArgs {
  /*! Version of this structure. */
  uint32_t version;

  /*! Max number of connections to be selected. Min limit is 1. */
  uint32_t max_selection_count;

  /*!
      Text prompt to be displayed to the user with selection dialog.
      Caller should allocate memory for this.
      Encoding should be in UTF8.
      This will be copied internally.
      The max length for the prompt is 40 characters.
  */
  const char *user_prompt;

  /*! Type of filter applied by default to ML connections list in selection dialog. */
  MLConnectionsSelectionFilter default_filter;

  /*! The context for on_selection callback. */
  void *context;

  /*!
    \brief On selection callback.

    This is the callback that will be called when selection has completed by the user.
    If this callback is supplied the handle is automatically cleaned up when selection is canceled,
    failed or confirmed.

    \param[in] result This is result of the selection process along with status and list of connections that were selected.
               The content is owned by the caller and will be released when the call returns.
    \param[in] context This is the context field that was passed to MLConnectionsRequestSelection().
  */
  void (*on_selection)(const MLConnectionsSelectionResult *result, void *context);

} MLConnectionsSelectionArgs;

/*!
  \brief Initializes default values for \see MLConnectionsSelectionArgs.

  \apilevel 8

  \param[in,out] inout_args.
*/
ML_STATIC_INLINE void MLConnectionsSelectionArgsInit(MLConnectionsSelectionArgs *inout_args) {
  inout_args->version = 1u;
  inout_args->max_selection_count = 5u;
  inout_args->user_prompt = NULL;
  inout_args->default_filter = MLConnectionsSelectionFilter_Following;
  inout_args->context = NULL;
  inout_args->on_selection = NULL;
}

/*!
  \brief Returns an ASCII string for MLConnectionsResult and MLResult codes.

  \apilevel 7

  \param[in] result_code The input MLResult enum from MLConnections functions.

  \return ASCII string containing readable version of result code.

  \priv None
*/
ML_API const char *ML_CALL MLConnectionsGetResultString(MLResult result);

/*!
  \brief Initialize all necessary resources for using MLConnections API.

  This function should be called before any connection functions are called.
  This function should be called along with MLConnectionsRegistrationStartup() &
  MLConnectionsRegisterForInvite() for any invitation to be delivered.

  \apilevel 7

  \retval MLResult_Ok if successfully initialized

  \priv None
*/
ML_API MLResult ML_CALL MLConnectionsStartup();

/*!
  \brief Deinitialize all resources used by MLConnections API.

  This function should be called prior to exiting the program
  if a call to MLConnectionsStartup() was called.
  This function should be called along with MLConnectionsRegistrationShutdown() if
  MLConnectionsRegistrationStartup() was called along with MLConnectionsStartup().


  \apilevel 7

  \retval MLResult_Ok if successfully shutdown.

  \priv None
*/
ML_API MLResult ML_CALL MLConnectionsShutdown();

/*!
  \brief Initialize all necessary resources for receiving an invite using MLConnectionsRegisterForInvite() API.

  This function should be called before any invite receiving functions are called.
  This function should be called along with MLConnectionsStartup() &
  MLConnectionsRegisterForInvite() for any invitation to be delivered.

  \apilevel 7

  \param[out] handle The handle that will be used for managing MLConnectionsRegisterForInvite().

  \retval MLResult_AllocFailed Indicates that resource allocation failed.
  \retval MLResult_InvalidParam Indicates the given argument is invalid.
  \retval MLResult_Ok if successfully initialized.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.

  \priv None
*/
ML_API MLResult ML_CALL MLConnectionsRegistrationStartup(MLHandle* out_handle);

/*!
  \brief Deinitialize all resources for receiving an invite using MLConnectionsRegisterForInvite() API.

  This should be called when you no longer want the MLConnections invitation
  callback to be activated, and prior to exiting the program if a call to
  MLConnectionsRegistrationStartup() was called.

  \apilevel 7

  \param[in] handle The handle that will be de-initialized.

  \retval MLResult_InvalidParam Indicates the given argument is invalid.
  \retval MLResult_Ok if successfully initialized.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.

  \priv None
*/
ML_API MLResult ML_CALL MLConnectionsRegistrationShutdown(MLHandle handle);

/*!
  \brief Register your application for receiving invites from other devices.

  This function should be called along with MLConnectionsStartup() &
  MLConnectionsRegistrationStartup() for any invitation to be delivered.

  \apilevel 7

  \param[in] handle The handle that was allocated with MLConnectionsRegistrationStartup().
  \param[in] callbacks This is the callbacks struct that contains the callbacks that will be called.
  \param[in] context This is the value that will be passed to both callbacks above.

  \retval MLResult_InvalidParam Indicates the given arguments are invalid.
  \retval MLResult_Ok If successfully initiated registration.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLConnectionsResult_AlreadyRegistered Indicates a duplicate registration.
  \retval MLConnectionsResult_NetworkFailure Indicates that communication to the network failed.
  \retval MLConnectionsResult_SystemFailure Indicates that there was system failure.

  \priv None
*/
ML_API MLResult ML_CALL MLConnectionsRegisterForInvite(MLHandle handle, MLConnectionsInviteCallbacks callbacks, void *context);

/*!
  \brief Submits a request to start the invite sending process.

  Request an invite to be sent for other connections to join a multi-user
  experience. This call will trigger a connections invite dialog to start the
  invitation process. There are two ways to invite users:

  [1] Invite users from your social graph.
  [2] Invite users using a join code.

  The system will then initiate a push notification to other online devices, start
  a copy of the application requesting the invite and deliver the given payload.
  The payload should contain the session context.

  When inviting users in the social graph the initiator selects the invitees from
  invite picker.  The payload is delivered to the remote users via the on_invitation
  callback. This can be used to create a session.  You cannot invite users in
  your social graph who are not following the current user.

  When inviting users using a join code the initiator first creates the room by
  calling this API. The remote user has to join the room before the payload can
  be delivered via the on_invitation callback. In order to join the room the
  remote user has to call this API.

  \apilevel 8

  \param[in] args Sender invite arguments.
  \param[out] out_request_handle This is the handle used to check the status of the invite sending
              process.

  \retval MLResult_InvalidParam Indicates the given arguments are invalid.
  \retval MLResult_Ok If successfully requested the start of the invite dialog.
  \retval MLResult_PrivilegeDenied Indicates permissions haven't been granted to make this API call.
  \retval MLResult_Timeout Indicates the request to request sending an invite timed out.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLConnectionsResult_IllegalState Indicates an issue with the connections system, e.g. service is not available for any reason.
  \retval MLConnectionsResult_InvalidInviteeCount Indicates number of invitees is invalid.

  \priv SocialConnectionsInvitesAccess
*/
ML_API MLResult ML_CALL MLConnectionsRequestInvite(const MLConnectionsInviteArgs *args, MLHandle *out_request_handle);

/*!
  \brief Checks the status of an invite sending process handle.

  This API call may be repeated if it returns MLResult_Pending for a request handle.

  \apilevel 7

  \param[in] invite_request_handle This is the handle created by MLConnectionsRequestInvite().
  \param[out] out_status Given that MLResult_Ok is returned, this will contain the status of the
              sending process for the invite.

  \retval MLResult_Ok Indicates polling for the invite status was successful, see out_status for invite status.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure in getting the invite status.
  \retval MLConnectionsResult_InvalidHandle If input handle is invalid.

  \priv None
*/
ML_API MLResult ML_CALL MLConnectionsTryGetInviteStatus(MLHandle invite_request_handle, MLConnectionsInviteStatus *out_status);

/*!
  \brief Attempts to cancel a previously requested invite sending process.

  If invite dialog has not yet been completed by the user, this request will
  dismiss the dialog and cancel the invite sending process. Otherwise this
  operation will return an error.

  \apilevel 7

  \param[in] invite_request_handle This is the handle created by MLConnectionsRequestInvite().

  \retval MLResult_Ok If invite was successfully cancelled.
  \retval MLResult_PrivilegeDenied Indicates permissions haven't been granted to make this API call.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLConnectionsResult_CancellationPending Indicates invite request has been found and the
          system is attempting to cancel the process.
  \retval MLConnectionsResult_InvalidHandle If input handle is invalid.

  \priv SocialConnectionsInvitesAccess
*/
ML_API MLResult ML_CALL MLConnectionsCancelInvite(MLHandle invite_request_handle);


/*!
  \brief Request a subset of connections as manually selected by the user.

  The selection is done using a separate system application.
  The results are delivered via a callback specified in args. If the callback is not specified
  the results can be retrived using MLConnectionsTryGetSelectionResult().

  \apilevel 8

  \param[in] args \see MLConnectionsSelectionArgs.
  \param[out] out_request_handle A pointer to an #MLHandle which will contain the handle to the request.
                                 If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_Ok If successfully submitted.
  \retval MLResult_PrivilegeDenied If permissions haven't been granted to make this API call.

  \priv SocialConnectionsSelectAccess
*/
ML_API MLResult ML_CALL MLConnectionsRequestSelection(const MLConnectionsSelectionArgs *args, MLHandle *out_request_handle);

/*!
  \brief Try to get result of a selection request.

  This API call may be repeated if it returns MLResult_Pending for a request handle.

  \apilevel 8

  \param[in] request_handle A handle to the request.
  \param[out] out_result Result. \see MLConnectionsSelectionResult. The content will be freed by
              the MLConnectionsReleaseRequestResources() API.

  \retval MLResult_InvalidParam If the request handle doesn't correspond to this operation.
  \retval MLResult_Pending If the request is still pending.
  \retval MLConnectionsResult_InvalidHandle If the request corresponding to the handle was not found.

  \priv None
*/
ML_API MLResult ML_CALL MLConnectionsTryGetSelectionResult(MLHandle request_handle, MLConnectionsSelectionResult **out_result);

/*!
  \brief Attempts to cancel a previously requested selection process.

  If selection dialog has not yet been completed by the user, this request will
  dismiss the dialog and cancel the selection process. Otherwise this
  operation will return an error.

  \apilevel 8

  \param[in] selection_request_handle This is the handle created by MLConnectionsRequestSelection().

  \retval MLResult_Ok If selection was successfully cancelled.
  \retval MLResult_PrivilegeDenied Indicates permissions haven't been granted to make this API call.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLConnectionsResult_CancellationPending Indicates selection request has been found and the
          system is attempting to cancel the process.
  \retval MLConnectionsResult_InvalidHandle If input handle is invalid.

  \priv SocialConnectionsSelectAccess
*/
ML_API MLResult ML_CALL MLConnectionsCancelSelection(MLHandle selection_request_handle);

/*!
  \brief Free all resources for a request corresponding to the handle.

  \apilevel 7

  \param[in] request_handle.

  \retval MLResult_Ok If all resources for the request were successfully freed.
  \retval MLConnectionsResult_InvalidHandle If the request corresponding to the handle was not found.

  \priv None
*/
ML_API MLResult ML_CALL MLConnectionsReleaseRequestResources(MLHandle request_handle);

/*! \} */

ML_EXTERN_C_END
