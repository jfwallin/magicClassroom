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

/*!
  \addtogroup Privileges Privileges
  \sharedobject ml_privileges
  \brief APIs to check or request privileges from the system.

  \attention NOTE: Do not directly include this header. Include 'ml_privileges.h'
  \{
*/

ML_EXTERN_C_BEGIN

typedef struct MLPrivilegesAsyncRequest MLPrivilegesAsyncRequest;

enum {
  /*! Defines the prefix for MLPrivilegesResult codes. */
  MLResultAPIPrefix_Privileges = MLRESULT_PREFIX(0xcbcd)
};

/*! Return values for Privileges API calls. */
typedef enum MLPrivilegesResult {
  MLPrivilegesResult_Granted = MLResultAPIPrefix_Privileges,
  MLPrivilegesResult_Denied,
} MLPrivilegesResult;


/*!
  \brief Start the privilege-checking system. This function should be called
  before any privilege-checking functions are called.

  \retval MLResult_Ok If the privilege system startup succeeded.
  \retval MLResult_UnspecifiedFailure If the privilege system failed to startup.

  \priv None
*/
ML_API MLResult ML_CALL MLPrivilegesStartup();

/*!
  \brief Shut down and clean all resources used by the privilege-checking
  system. This function should be called prior to exiting the program if
  a call to MLPrivilegesInit() was made.

  \retval MLResult_Ok If the privilege system shutdown succeeded.
  \retval MLResult_UnspecifiedFailure If the privilege system failed to shutdown.

  \priv None
*/
ML_API MLResult ML_CALL MLPrivilegesShutdown();

/*!
  \brief Check whether the application has the specified privileges.
  This does not solicit consent from the end-user.

  \param[in] privilege_id The privilege to check.

  \retval MLResult_UnspecifiedFailure If the privilege system was not started.
  \retval MLPrivilegesResult_Denied If the privilege is denied.
  \retval MLPrivilegesResult_Granted If the privilege is granted.

  \priv None
*/
ML_API MLResult ML_CALL MLPrivilegesCheckPrivilege(MLPrivilegeID privilege_id);

/*!
  \brief Request the specified privilege. This may solicit consent from the end-user.
  When waiting for user consent, this function blocks.
  For a non-blocking option, use MLPrivilegesRequestPrivilegeAsync().

  \param[in] privilege_id The privilege to request.

  \retval MLResult_UnspecifiedFailure If the privilege system was not started.
  \retval MLPrivilegesResult_Denied If the privilege is denied.
  \retval MLPrivilegesResult_Granted If the privilege is granted.

  \priv None
*/
ML_API MLResult ML_CALL MLPrivilegesRequestPrivilege(MLPrivilegeID privilege_id);

/*!
  \brief Request the specified privileges. This may solicit consent from the end-user.

  \param[in] privilege_id The privilege to request.
  \param[out] request The request object. Poll using MLPrivilegesRequestPrivilegeTryGet().

  \retval MLResult_InvalidParam If request is a null pointer.
  \retval MLResult_Ok If the privilege request is in progress.
  \retval MLResult_UnspecifiedFailure If the privilege system was not started.

  \priv None
*/
ML_API MLResult ML_CALL MLPrivilegesRequestPrivilegeAsync(MLPrivilegeID privilege_id, MLPrivilegesAsyncRequest** request);

/*!
  \brief Try to get the result from a privilege request. The request object is internally freed when
  the result is ready (i.e. retval is either MLPrivilegesResult_Granted or MLPrivilegesResult_Denied).

  \param[in] request The request created in MLPrivilegesRequestPrivilegeAsync().

  \retval MLResult_InvalidParam If request is a null pointer.
  \retval MLResult_Pending If the privilege request is in progress.
  \retval MLResult_UnspecifiedFailure If the privilege system was not started.
  \retval MLPrivilegesResult_Denied If the privilege is denied.
  \retval MLPrivilegesResult_Granted If the privilege is granted.

  \priv None
*/
ML_API MLResult ML_CALL MLPrivilegesRequestPrivilegeTryGet(MLPrivilegesAsyncRequest* request);

/*!
  \brief Returns an ASCII string for MLPrivilegesResult and MLResultGlobal codes.

  \param[in] result_code The input MLResult enum from MLPrivileges functions.

  \return ASCII string containing readable version of result code.

  \priv None
*/
ML_API const char* ML_CALL MLPrivilegesGetResultString(MLResult result_code);

/*! \} */

ML_EXTERN_C_END
