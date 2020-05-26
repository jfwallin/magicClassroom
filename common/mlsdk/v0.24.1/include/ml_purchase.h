// %BANNER_BEGIN%
//---------------------------------------------------------------------
// %COPYRIGHT_BEGIN%
//
// Copyright (c) 2017-2018 Magic Leap, Inc. (COMPANY) All Rights Reserved.
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
#include "ml_cloud.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup InAppPurchases
  \sharedobject ml_purchase
  \brief APIs for the In-App Purchase flow.

  \attention THIS IS NOT CURRENTLY AVAILABLE
             FOR USE. ALL RESPONSES WILL RETURN MLCloudResult_NotFound UNTIL THE
             BACKEND SYSTEMS ARE IN PLACE.

  - All char*'s in this API are UTF-8.

  \attention Not expected to function at runtime. API for early review only.
  \{
 */

enum {
  /*! The prefix for MLPurchaseResult codes. */
  MLResultAPIPrefix_Purchase = MLRESULT_PREFIX(0xdf1d)
};

/*! Return values for Purchase API calls. */
typedef enum MLPurchaseResult {
  /*! The purchase was cancelled by the user through the system dialog. */
  MLPurchaseResult_UserCancelled = MLResultAPIPrefix_Purchase,
  /*! The purchase cannot be completed for an already-purchased item. */
  MLPurchaseResult_ItemAlreadyPurchased,
  /*! There was some issue with the source of payment. */
  MLPurchaseResult_PaymentSourceError,
  /*! The purchase token used in the call was not valid. */
  MLPurchaseResult_InvalidPurchaseToken,
  /*! The item selected for purchase is not available for purchasing. */
  MLPurchaseResult_ItemUnavailable,
  /*! The user has not setup a default payment method. */
  MLPurchaseResult_NoDefaultPayment,
  /*! Ensure enum is represented as 32 bits. */
  MLPurchaseResult_Ensure32Bits = 0x7FFFFFFF
} MLPurchaseResult;

enum {
  /*! The maximum number of ids that can be sent as MLPurchaseItemDetailsQuery. */
  MLPurchaseItemDetails_MaximumIdsPerRequest = 20
};

/*! Struct containing information required to query the MLPurchaseItemDetails API. */
typedef struct MLPurchaseItemDetailsQuery {
  /*!
    \brief The array of item IDs for which details are to be queried.
    This array should not contain more than
    MLPurchaseItemDetails_MaximumIdsPerRequest ids. The array must be allocated
    by the caller of the MLPurchaseItemDetails API.
  */
  const char **ids;
  /*!
    \brief The length of the above `ids` array (the number of item IDs to be
    queried.
  */
  uint32_t id_count;
} MLPurchaseItemDetailsQuery;

/*! The type of the item. */
typedef enum MLPurchaseType {
  /*!
    \brief Consumable items are items that can be purchased multiple times. The
    application is responsible for managing the consumption of consumable items.
  */
  MLPurchaseType_Consumable,
  /*!
    \brief Nonconsumable items can only be purchased once. This restriction will
    be enforced by the service.
  */
  MLPurchaseType_Nonconsumable,
  /*! Ensure enum is represented as 32 bits. */
  MLPurchaseType_Ensure32Bits = 0x7FFFFFFF
} MLPurchaseType;

/*!
  \brief MLPurchaseToken is an opaque type used to represent and submit
  a purchase during the purchase transaction process.
*/
typedef const char* MLPurchaseToken;

/*!
  \brief Represents the item details for a single in app purchase item.

  An array of these structs will be returned by the MLPurchaseItemDetailsQuery
  API calls. All strings in the struct are allocated by the API, and their
  lifetime is bound to the lifetime of the MLPurchaseItemDetailsQuery handle.
  Client code should read / copy the information in this struct before calling
  the MLPurchaseItemDetailsDestroy API as that will free the query handle and
  associated result data.
*/
typedef struct MLPurchaseItemDetailsResult {
  /*! Id of the in-app purchase item. */
  const char *iap_id;
  /*!
    \brief The formatted price for the item. This version only
    supports US purchases, so it will only show as a valid dollar amount,
    e.g. $0.00, $9.99, $123.45, etc.
  */
  const char *price;
  /*! The user-friendly name of the item. */
  const char *name;
  /*! The type of purchase. */
  MLPurchaseType type;
  /*!
    \brief The token to be used when submitting a purchase. It must be
    copied before calling MLPurchaseItemDetailsDestroy on the handle associated
    with this struct. For the purposes of copying, this can be treated as a null
    terminated character array.
  */
  MLPurchaseToken token;
  /*! A flag that signifies if the item is free or not. */
  bool is_free;
} MLPurchaseItemDetailsResult;

/*! MLPurchaseItemDetailsResults is the data response for the MLPurchaseItemDetailsQuery API call. */
typedef struct MLPurchaseItemDetailsResults {
  /*! The number of MLPurchaseItemDetailsResult structs in the array response. */
  uint32_t count;
  /*! An array of MLPurchaseItemDetailsResult structs. */
  MLPurchaseItemDetailsResult *item_details;
} MLPurchaseItemDetailsResults;

/*!
  \brief The MLPurchaseConfirmation struct contains the purchase confirmation
  information for a historical, successfully completed purchase. An array of
  these structs will be returned by the MLPurchaseHistoryQuery API call.
*/
typedef struct MLPurchaseConfirmation {
  /*! The unique ID for this purchase. */
  const char *order_id;
  /*! The package name of the application from which this purchase originated. */
  const char *package_name;
  /*! The time the product was purchased, in milliseconds since the epoch (Jan 1, 1970). */
  uint64_t purchase_time;
  /*! The signature of the purchase data, signed with the private key of the developer. */
  const char *signature;
  /*! The in app purchase ID of the item being purchased. */
  const char *iap_id;
  /*! The type of purchase. */
  MLPurchaseType type;
} MLPurchaseConfirmation;

/*! The results of a particular page request for purchase history. */
typedef struct MLPurchaseHistoryResult {
  /*! The number of purchase confirmations available in this page of purchase confirmations. */
  uint32_t count;
  /*!
    \brief The array of confirmed purchases made by this user for this
    application. Its memory is allocated by the
    MLPurchaseHistoryQueryGetPageResult call and is freed by the
    MLPurchaseHistoryQueryDestroy call.
  */
  MLPurchaseConfirmation *confirmations;
  /*!
    \brief If has_next_page is true, MLPurchaseHistoryQueryGetPageResult may
    be re-called to query for the next page of purchase confirmation data.
  */
  bool has_next_page;
  /*!
    \brief The status field Indicates whether or not the cloud call has
    completed or not, with the values MLCloudStatus_Done or
    MLCloudStatus_NotDone, respectively.
  */
  MLCloudStatus status;
} MLPurchaseHistoryResult;

/*!
  \brief Creates the handle to be used for future MLPurchaseItemDetails API calls.

  \param[out] out_handle The handle to the purchase item details query. The out_handle is only valid if return value is MLResult_Ok.

  \retval MLResult_AllocFailed If the handle cannot be allocated.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.

  \priv None
 */
ML_API MLResult ML_CALL MLPurchaseItemDetailsCreate(MLHandle *out_handle);

/*!
  \brief Submits the details query for the provided in-app purchase ID's.

  \param[in] item_details_handle This is the query handle created by MLPurchaseItemDetailsCreate.
  \param[in] request This is a query struct containing the list of items to request.

  \retval MLResult_InvalidParam If any input parameters are invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_PrivilegeDenied Indicates an privilege error with the purchase details system call.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLCloudResult_InvalidHandle Indicates the provided query handle is not valid.
  \retval MLCloudResult_SystemError Indicates an issue with the cloud service, e.g. service is not available for any reason.

  \priv InAppPurchase
 */
ML_API MLResult ML_CALL MLPurchaseItemDetailsGet(MLHandle item_details_handle, const MLPurchaseItemDetailsQuery *request);

/*!
  \brief Checks the status of the call to MLPurchaseItemDetails().
  This call is used to poll for the status of the query. Given that #MLResult_Ok is returned,
  the call is complete when the value for out_status is #MLCloudStatus_Done.

  \param[in] item_details_handle This is the handle that was used to call MLPurchaseItemDetails().
  \param[out] out_item_details_results This will contain the results of the call when the return value
              is #MLResult_Ok and out_status is #MLCloudStatus_Done.
  \param[out] out_status This will contain the status of the call, either #MLCloudStatus_Done or #MLCloudStatus_NotDone.

  \retval MLResult_InvalidParam If any input parameters are invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_PrivilegeDenied Indicates an privilege error with the purchase details system call.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLCloudResult_InvalidHandle Indicates the provided query handle is not valid.
  \retval MLCloudResult_NetworkError If there is a network connectivity error for the service call.
  \retval MLCloudResult_NotFound Indicates the requested resource was not found on the server.
  \retval MLCloudResult_ServerError If there is an unexepected server error.
  \retval MLCloudResult_SystemError Indicates an issue with the cloud service, e.g. if the service is not available for any reason.

  \priv None
 */
ML_API MLResult ML_CALL MLPurchaseItemDetailsGetResult(MLHandle item_details_handle, MLPurchaseItemDetailsResults *out_item_details_results, MLCloudStatus *out_status);

/*!
  \brief Frees the memory allocated for the handle in the call to MLPurchaseItemDetailsCreate().

  \param[in] item_details_handle This is a handle that was created by MLPurchaseItemDetailsCreate().

  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLCloudResult_InvalidHandle Indicates the provided query handle is not valid.

  \priv None
 */
ML_API MLResult ML_CALL MLPurchaseItemDetailsDestroy(MLHandle item_details_handle);

/*!
  \brief Creates the handle to be used for the MLPurchaseSubmit call to submit an in-app purchase.

  \param[out] out_handle The handle for the API call to submit the in-app purchase. The out_handle is only valid if return value is #MLResult_Ok.

  \retval MLResult_AllocFailed If the handle cannot be allocated.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.

  \priv None
 */
ML_API MLResult ML_CALL MLPurchaseCreate(MLHandle *out_handle);

/*!
  \brief Attempts to purchase the in-app purchase item represented by the MLPurchaseToken param.

  \param[in] purchase_handle This is a handle created by MLPurchaseCreate, used to identify this purchase attempt.
  \param[in] token This identifies the in-app purchase item to be purchased by this call. This should have been copied
                   from the struct MLPurchaseItemDetails before calling MLPurchaseItemDetailsDestroy, otherwise the token is invalid.
                   See MLPurchaseItemDetails for more information on copying.

  \retval MLResult_InvalidParam If any input parameters are invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_PrivilegeDenied Indicates an privilege error with the purchase details system call.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLCloudResult_InvalidHandle Indicates the provided query handle is not valid.
  \retval MLCloudResult_SystemError Indicates an issue with the cloud service, e.g. if the service is not available for any reason.

  \priv InAppPurchase
 */
ML_API MLResult ML_CALL MLPurchaseSubmit(MLHandle purchase_handle, MLPurchaseToken token);

/*!
  \brief Checks the status of the call to MLPurchaseSubmit().

  \param[in] purchase_handle This handle represents the in-app purchase submitted vai MLPurchaseSubmit().
  \param[out] out_confirmation This will be populated with the purchase confirmation
              information given that the call is successful (returns MLResult_Ok) and complete (out_status is MLCloudStatus_Done).
  \param[out] out_status Given that MLResult_Ok is returned, this represents the polling state of the request.
              It is either MLCloudStatus_Done or MLCLoudStatus_NotDone.

  \retval MLResult_InvalidParam If any input parameters are invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLCloudResult_InvalidHandle Indicates the provided query handle is not valid.
  \retval MLCloudResult_NetworkError If there is a network connectivity error for the service call.
  \retval MLCloudResult_NotFound Indicates the requested resource was not found on the server.
  \retval MLCloudResult_ServerError If there is an unexepected server error.
  \retval MLCloudResult_SystemError Indicates an issue with the cloud service, e.g. if the service is not available for any reason.
  \retval MLPurchaseResult_InvalidPurchaseToken If the token representing the in-app purchase is not valid.
  \retval MLPurchaseResult_ItemAlreadyPurchased Indicates the item has already been purchased.
  \retval MLPurchaseResult_ItemUnavailable Indicates the item is no longer available.
  \retval MLPurchaseResult_NoDefaultPayment If the user has not configured a default payment method.
  \retval MLPurchaseResult_PaymentSourceError Indicates there was a failure with the payment source.
  \retval MLPurchaseResult_UserCancelled Indicates the user cancelled the purchase via the purchase confirmation UI.

  \priv None
 */
ML_API MLResult ML_CALL MLPurchaseGetResult(MLHandle purchase_handle, MLPurchaseConfirmation *out_confirmation, MLCloudStatus *out_status);

/*!
  \brief Frees the memory being maintained by the handle created by MLPurchaseCreate().

  \param[in] purchase_handle This is a handle that was created by MLPurchaseCreate().

  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLCloudResult_InvalidHandle Indicates the provided query handle is not valid.

  \priv None
 */
ML_API MLResult ML_CALL MLPurchaseDestroy(MLHandle purchase_handle);

/*!
  \brief Creates the handle to be used to retrieve in-app purchase history via the MLPurchaseHistoryQueryGetPage() call.
  MLPurchaseHistoryQueryDestroy() must be called on the handle when done with the query and the returned data in order clean up the allocations.

  \param[out] out_handle The handle to the purchase item details query. The out_handle is only valid if return value is MLResult_Ok.

  \retval MLResult_AllocFailed If the handle cannot be allocated.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLResult_Ok On success.

  \priv None
 */
ML_API MLResult ML_CALL MLPurchaseHistoryQueryCreate(MLHandle *out_handle);

/*!
  \brief Request the next available page of purchase history information for the current user for the current application.

  The first time it is called, it will initiate the query for the first page of purchase
  history. The second time it is called with the same handle, it will initiate
  the query for the the second page, etc.

  \param[in] history_handle This is a handle that was created by MLPurchaseHistoryQueryCreate().
  \param[in] limit This is the maximum number of items that will be returned in a single page.

  \retval MLResult_Ok On success.
  \retval MLResult_PrivilegeDenied Indicates an privilege error with the purchase details system call.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLCloudResult_InvalidHandle Indicates the provided query handle is not valid.
  \retval MLCloudResult_SystemError Indicates an issue with the cloud service, e.g. if the service is not available for any reason.

  \priv InAppPurchase
 */
ML_API MLResult ML_CALL MLPurchaseHistoryQueryGetPage(MLHandle history_handle, uint32_t limit);

/*!
  \brief Used to obtain the in app purchase history via the MLPurchaseHistoryResult out param.

  \param[in] history_handle This is a handle that was used to call MLPurchaseHistoryQueryPage().
  \param[out] out_page_result This will be populated with the purchase history result for the queried
              page of history given that the call is successful (returns #MLResult_Ok) and
              complete (out_page_result->status is #MLCloudStatus_Done). The MLPurchaseHistoryResult data is
              allocated by the call and will be freed when MLPurchaseHistoryQueryDestroy is called OR *the next time
              MLPurchaseHistoryQueryGetPage is called*.

  \retval MLResult_AllocFailed If the memory for the response cannot be allocated.
  \retval MLResult_InvalidParam If any input parameters are invalid.
  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLCloudResult_InvalidHandle Indicates the provided query handle is not valid.
  \retval MLCloudResult_SystemError Indicates an issue with the cloud service, e.g. if the service is not available for any reason.

  \priv None
 */
ML_API MLResult ML_CALL MLPurchaseHistoryQueryGetPageResult(MLHandle history_handle, MLPurchaseHistoryResult *out_page_result);

/*!
  \brief Frees the memory being maintained by the handle created by MLPurchaseHistoryQueryCreate().

  \param[in] history_handle This is a handle that was created by MLPurchaseHistoryQueryCreate().

  \retval MLResult_Ok On success.
  \retval MLResult_UnspecifiedFailure Indicates an unexpected failure.
  \retval MLCloudResult_InvalidHandle Indicates the provided query handle is not valid.

  \priv None
 */
ML_API MLResult ML_CALL MLPurchaseHistoryQueryDestroy(MLHandle history_handle);

/*!
  \brief Returns an ASCII string for #MLPurchaseResult and MLResultGlobal codes.

  \param[in] result_code The input MLResult enum from MLPurchase functions.

  \return ASCII string containing readable version of result code.

  \priv None
*/
ML_API const char* ML_CALL MLPurchaseGetResultString(MLResult result_code);

/*! \} */

ML_EXTERN_C_END
