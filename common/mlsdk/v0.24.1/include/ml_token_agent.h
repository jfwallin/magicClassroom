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
#include "ml_invoke_async.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Identity
  \sharedobject ml_identity
  \{
*/

/*! The credentials that can be used to for a user to access a particular service (Audience). */
typedef struct MLTokenAgentCredentials {
  /*! Null terminated string allocated and released by the library. */
  const char *access_key_id;
  /*! Null terminated string allocated and released by the library. */
  const char *secret_key;
  /*! Null terminated string allocated and released by the library. */
  const char *session_token;
} MLTokenAgentCredentials;

/*! Contains tokens that are used to read and modify the user profile. */
typedef struct MLTokenAgentTokens {
  /*!
    \brief The id_token contains information from the user profile.
           It is a null terminated string that is allocated and released by the library.
  */
  const char *id_token;
  /*!
    \brief The access_token is the token that can be used to modify attributes of the user profile.
           It is a null terminated string that is allocated and released by the library.
  */
  const char *access_token;
} MLTokenAgentTokens;

/*! Represents the credentials and tokens of the User-Audience pair that is associated with the calling service. */
typedef struct MLTokenAgentClientCredentials {
  /*! The credentials that can be used to access a particular service (Audience).*/
  MLTokenAgentCredentials credentials;
  /*! The tokens that can be used to manage the user profile for a particular Audience. */
  MLTokenAgentTokens tokens;
} MLTokenAgentClientCredentials;

enum {
  /*! Defines the prefix for MLTokenAgentResult codes. */
  MLResultAPIPrefix_TokenAgent = MLRESULT_PREFIX(0x37ee)
};

/*! Return values for Token Agent API calls. */
typedef enum MLTokenAgentResult {
  /*! The local binder service was not found.*/
  MLTokenAgentResult_FailedToConnectToLocalService = MLResultAPIPrefix_TokenAgent,
  /*! The local binder service is running but has not been registered by the login service.*/
  MLTokenAgentResult_ServiceNotStarted,
  /*! The local binder service failed to connect to the cloud service.*/
  MLTokenAgentResult_FailedToConnectToCloud,
  /*! The cloud service rejected the request due to inappropriate credentials.*/
  MLTokenAgentResult_CloudAuthentication,
  /*! The local server failed to log in with the cloud.*/
  MLTokenAgentResult_FailedToLogin,
  /*! An attempt to complete the login was begun without starting the login.*/
  MLTokenAgentResult_LoginNotBegun,
  /*! The operation was rejected because the local server was not logged in with the cloud. */
  MLTokenAgentResult_NotLoggedIn,
  /*! The login request failed because the local server is already logged in with the cloud. */
  MLTokenAgentResult_AlreadyLoggedIn,
  /*! The login request request failed because a login is already in progress. */
  MLTokenAgentResult_LoginInProgress,
  /*!
    \brief Having completed the login process with the cloud successfully, the service failed to
          start the local identity service.
  */
  MLTokenAgentResult_FailedToStartIdentityService,
  /*! The serial number of the device is not recognized by the cloud so login failed. */
  MLTokenAgentResult_DeviceNotRegistered,
  /*! The device is not yet connected to the cloud so login failed. */
  MLTokenAgentResult_DeviceNotConnected,
  /*! The device is not connected to a network. */
  MLTokenAgentResult_NetworkError,
  /*! The user is not registered on the website. */
  MLTokenAgentResult_UserIsNotRegistered,
  /*! The user's credentials have expired. */
  MLTokenAgentResult_ExpiredCredentials,
  /*! The cloud rejected the operation because the user is not authorized to execute it. */
  MLTokenAgentResult_Unauthorized,
  /*! The device failed to authenticate the server. */
  MLTokenAgentResult_CertificateError,
  /*! The cloud rejected the operation. */
  MLTokenAgentResult_RejectedByCloud,
  /*! The user must accept the terms of use on the website. */
  MLTokenAgentResult_UserMustAcceptTerms,
  /*! The user's account has been locked and can only be unlocked on the website. */
  MLTokenAgentResult_AccountLocked,
  /*! Ensure enum is represented as 32 bits. */
  MLTokenAgentResult_Ensure32Bits = 0x7FFFFFFF
} MLTokenAgentResult;

/*!
  \brief MLTokenAgentGetClientCredentials() is a blocking function that accesses the cloud and
  returns a pointer to an #MLTokenAgentClientCredentials structure containing the users credentials and
  tokens for a particular service (Audience).

  The library deduces the Audience being requested from the name of the calling service.

  \param[out] out_credentials is a pointer to a #MLTokenAgentClientCredentials pointer which was allocated
              with all its fields by the library. It must only be released
              using MLTokenAgentReleaseClientCredentials().

  \retval MLResult_AllocFailed The operation failed to allocate memory.
  \retval MLResult_InvalidParam out_credentials was 0 (null).
  \retval MLResult_Ok The operation completed successfully.
  \retval MLResult_PrivilegeDenied The caller does not have the ClientCredentialsRead privilege.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLTokenAgentResult A token agent specific failure occurred during the operation.

  \priv ClientCredentialsRead
*/
ML_API MLResult ML_CALL MLTokenAgentGetClientCredentials(const MLTokenAgentClientCredentials **out_credentials);

/*!
  \brief MLTokenAgentGetClientCredentialsAsync() invokes the MLTokenAgentGetClientCredentials()
  function asynchronously (in a different thread).

  \param[out] out_future
  \parblock
  A pointer to an #MLInvokeFuture pointer which provides the means to poll for completion and
  to retrieve the credentials returned by MLTokenAgentGetClientCredentials().

  Note: This pointer will be freed by the library before returning from the first (and last)
  call to MLTokenAgentGetClientCredentialsWait(), after the asynchronous call completed, that is
  after MLTokenAgentGetClientCredentialsWait() returns any value that is not MLResult_Pending
  \endparblock

  \retval MLResult_AllocFailed The operation failed to allocate memory.
  \retval MLResult_InvalidParam profile or out_future were 0 (null).
  \retval MLResult_Ok The operation completed successfully and out_future points
          to an allocated MLInvokeFuture. If any other value is returned, the location pointed to
          by out_future is set to 0 (null).
  \retval MLResult_PrivilegeDenied The caller does not have the ClientCredentialsRead privilege.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLTokenAgentResult A token agent specific failure occurred during the operation.

  \priv ClientCredentialsRead
*/
ML_API MLResult ML_CALL MLTokenAgentGetClientCredentialsAsync(MLInvokeFuture** out_future);

/*!
  \brief Having made a call to MLTokenAgentGetClientCredentialsAsync(), the user can call
  MLTokenAgentGetClientCredentialsWait() to detect whether the asynchronous call completed and if
  so, to retrieve the credentials in out_credentials.

  The call to MLTokenAgentGetClientCredentialsWait() blocks until either
  msec_timeout msec elapses or the asynchronous function completes.

  Note:
  1. To poll without blocking use msec_timeout = 0.
  2. After the function returns true, future is freed by the library and must not be used again in
     a call to MLTokenAgentGetClientCredentialsWait().

  \param[in] future The pointer returned by the MLTokenAgentGetClientCredentialsAsync() function.
  \param[in] msec_timeout The timeout in milliseconds.
  \param[out] out_credentials The location in memory where the pointer to the credentials
              structure allocated by the library will be copied, if the asynchronous call completed,
              or 0 (null) if not.

  \retval MLResult_AllocFailed The operation failed to allocate memory.
  \retval MLResult_InvalidParam future or out_credentials were 0 (null).
  \retval MLResult_Ok The operation completed successfully before the timeout elapsed.
          The location pointed to by out_credentials was set to the address of the
          #MLTokenAgentClientCredentials structure allocated by the library.
          The returned credentils must be released when no longer needed by calling
          MLTokenAgentReleaseClientCredentials().
          If any other value is returned, the location pointed to by out_credentials is set to 0 (null).
  \retval MLResult_Pending The timeout elapsed before the asynchronous call completed.
  \retval MLResult_PrivilegeDenied The caller does not have the ClientCredentialsRead privilege.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLTokenAgentResult A token agent specific failure occurred during the operation.

  \priv ClientCredentialsRead
*/
ML_API MLResult ML_CALL MLTokenAgentGetClientCredentialsWait(MLInvokeFuture *future, uint32_t msec_timeout, const MLTokenAgentClientCredentials **out_credentials);

/*!
  \brief MLTokenAgentReleaseClientCredentials() releases all resources associated with the
  #MLTokenAgentClientCredentials structure that was returned by the library.

  \param[in] credentials a pointer to a library allocated structure received using one of:
             MLTokenAgentGetClientCredentials() or MLTokenAgentGetClientCredentialsWait().

  \retval MLResult_InvalidParam credentials was 0 (null).
  \retval MLResult_Ok The operation completed successfully.

  \priv ClientCredentialsRead
*/
ML_API MLResult ML_CALL MLTokenAgentReleaseClientCredentials(const MLTokenAgentClientCredentials *credentials);

/*!
  \brief Returns an ASCII string for MLTokenAgentResult and MLResultGlobal codes.

  \param[in] result_code The input MLResult enum from MLTokenAgent functions.

  \return ASCII string containing readable version of result code.

  \priv None
*/
ML_API const char* ML_CALL MLTokenAgentGetResultString(MLResult result_code);

/*! \} */

ML_EXTERN_C_END
