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
// --------------------------------------------------------------------*/
// %BANNER_END%

#pragma once

#include "ml_api.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup InAppPurchases
  \brief General result codes for Cloud API calls

  \{
*/


enum {
  /*! Defines the prefix for MLCloudResult codes. */
  MLResultAPIPrefix_Cloud = MLRESULT_PREFIX(0xc4e3)
};

/*! Return values for Cloud API calls. */
typedef enum MLCloudResult {
  /*! The requested resource was not found on the server. */
  MLCloudResult_NotFound =  MLResultAPIPrefix_Cloud,
  /*! The server failed to process the request. */
  MLCloudResult_ServerError,
  /*! The request may not have made it to the server. The service did not receive a response from the server. */
  MLCloudResult_NetworkError,
  /*!
    \brief Some unexpected system error prevented the request from getting to
    the cloud. This is most likely a bug in the API implementation.
   */
  MLCloudResult_SystemError,
  /*! The handle that was passed in is not a valid handle for the API call it was used in. */
  MLCloudResult_InvalidHandle,
  /*! The handle was used too soon. */
  MLCloudResult_BadHandleState,
  /*! Ensure enum is represented as 32 bits. */
  MLCloudResult_Ensure32Bits = 0x7FFFFFFF
} MLCloudResult;

/*! This represents the polling status of an asynchronous cloud call. */
typedef enum MLCloudStatus {
  /*! The current request is still in-progress. */
  MLCloudStatus_NotDone = 0,
  /*! The current request is complete. It may have succeeded or failed. */
  MLCloudStatus_Done,
  /*! Ensure enum is represented as 32 bits. */
  MLCloudStatus_Ensure32Bits = 0x7FFFFFFF
} MLCloudStatus;

/*! \} */

ML_EXTERN_C_END
