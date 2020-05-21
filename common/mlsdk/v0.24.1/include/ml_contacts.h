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
// --------------------------------------------------------------------
// %BANNER_END%

#pragma once

#include "ml_api.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Contacts
  \sharedobject ml_contacts
  \brief MLContacts APIs provide address-book style functionality for storing, editing, deleting,
  and retrieving contacts from on-device storage. This API does not expose access to user's
  connections with other magic leap users (e.g. social graph).

  This API enables interaction with on-device contact storage in an asynchronous manner. API users
  can submit a requests for operation and receive a corresponding handle, which can be used to
  retrieve the result at a later time. Handles can be used to poll for operation result using a
  TryGet function. These functions are non-blocking and will either return request not ready status
  or request results once ready.

  API users are also responsible for calling MLContactsReleaseRequestResources to free any
  resources associated with request handle once it is no longer needed.
  \{
*/

/*!
  \brief Stores a tagged value, such as phone number or email address.
  Optional tag indicates what type of value is stored, e.g. "home", "work", etc.
  \apilevel 2
*/
typedef struct MLContactsTaggedAttribute {
  /*! Version of this structure. */
  uint32_t version;

  /*! Name of the Tag. */
  const char *tag;

  /*! Value of this attribute. */
  const char *value;
} MLContactsTaggedAttribute;

/*!
  \brief Initializes default values for MLContactsTaggedAttribute.
  \apilevel 2
  \param[in, out] in_out_attr The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLContactsTaggedAttributeInit(MLContactsTaggedAttribute *in_out_attr) {
  in_out_attr->version = 1u;
  in_out_attr->tag = NULL;
  in_out_attr->value = NULL;
}

/*!
  \brief Stores a list of MLContactsTaggedAttribute instances.
  \apilevel 2
*/
typedef struct MLContactsTaggedAttributeList {
  /*! Version of this structure. */
  uint32_t version;

  /*! Number of tagged attributes in this list. */
  size_t count;

  /*! Pointer referring to the array of tagged attributes. */
  MLContactsTaggedAttribute *const *items;
} MLContactsTaggedAttributeList;

/*!
  \brief Initializes default values for MLContactsTaggedAttributeList.
  \apilevel 2
  \param[in, out] in_out_attr_list The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLContactsTaggedAttributeListInit(MLContactsTaggedAttributeList *in_out_attr_list) {
  in_out_attr_list->version = 1u;
  in_out_attr_list->count = 0;
  in_out_attr_list->items = NULL;
}

/*!
  \brief Stores a list of phone numbers.
  \apilevel 2
*/
typedef MLContactsTaggedAttributeList MLContactsPhoneNumberList;

/*!
  \brief Stores a list of email addresses.
  \apilevel 2
*/
typedef MLContactsTaggedAttributeList MLContactsEmailAddressList;

/*!
  \brief Stores a contact identifier.
  \apilevel 2
*/
typedef const char *MLContactsID;

/*!
  \brief Representation of available information for a single contact in address book.
  \apilevel 2
*/
typedef struct MLContactsContact {
  /*! Version of this structure. */
  uint32_t version;

  /*! Locally-unique contact identifier. Generated by the system. May change across reboots. */
  MLContactsID id;

  /*! Contacts's name. */
  const char *name;

  /*!
    \brief List of tagged phone numbers. Tag-value pairs are not required to be unique. Tags are
    optional (empty strings allowed).
  */
  MLContactsPhoneNumberList phone_number_list;

  /*!
    \brief List of tagged email addresses. Tag-value pairs are not required to be unique. Tags are
    optional (empty strings allowed).
  */
  MLContactsEmailAddressList email_address_list;
} MLContactsContact;

/*!
  \brief Initializes default values for MLContactsContact.
  \apilevel 8

  \param[in, out] in_out_contact The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLContactsContactInit(MLContactsContact *in_out_contact) {
  in_out_contact->version = 2u;
  in_out_contact->id = NULL;
  in_out_contact->name= NULL;
  MLContactsTaggedAttributeListInit(&in_out_contact->phone_number_list);
  MLContactsTaggedAttributeListInit(&in_out_contact->email_address_list);
}

/*!
  \brief Stores a list of MLContactsContact.
  \apilevel 2
*/
typedef struct MLContactsContactList {
  /*! Version of this structure. */
  uint32_t version;

  /*! Number of contacts. */
  size_t count;

  /*! Pointer referring to array of contacts. */
  MLContactsContact *const *contacts;
} MLContactsContactList;

/*!
  \brief Initializes default values for MLContactsContactList.
  \apilevel 2

  \param[in, out] in_out_contact_list The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLContactsContactListInit(MLContactsContactList *in_out_contact_list) {
  in_out_contact_list->version = 1u;
  in_out_contact_list->count = 0;
  in_out_contact_list->contacts = NULL;
}

enum {
  /*! Defines the prefix for MLContactsResult codes. */
  MLResultAPIPrefix_Contacts = MLRESULT_PREFIX(0x94a0)
};

/*!
  \brief Return values for Contacts API calls.
  \apilevel 2
*/
typedef enum MLContactsResult {
  /*! This MLHandle is not yet recognized. */
  MLContactsResult_HandleNotFound = MLResultAPIPrefix_Contacts,
  /*!
    /brief Request is completed, its corresponding result has been returned, and its related resources
    are pending to be freed. See MLContactsReleaseRequestResources().
  */
  MLContactsResult_Completed,
  /*! Request is successfully cancelled. */
  MLContactsResult_Cancelled,
  /*! Request failed due to system being in an illegal state, e.g., when the user hasn't successfully logged-in. */
  MLContactsResult_IllegalState,
  /*! Ensure enum is represented as 32 bits. */
  MLContactsResult_Ensure32Bits = 0x7FFFFFFF
} MLContactsResult;

enum {
  /*!
    \brief Default fetch limit for retrieval based operations.
    \apilevel 2
  */
  MLContacts_DefaultFetchLimit = 250
};

/*!
  \brief Offset to be used to get the next batch of results.
  \apilevel 2
*/
typedef const char *MLContactsOffset;

/*!
  \brief Stores arguments for a List request (MLContactsRequestList()).
  \apilevel 2
*/
typedef struct MLContactsListArgs {
  /*! Version of this structure. */
  uint32_t version;

  /*! Request results offset by this amount assuming contacts are ordered in a consistent way. */
  MLContactsOffset offset;

  /*! Limit the number of results returned by list operation. Min limit is 1. */
  uint32_t limit;
} MLContactsListArgs;

/*!
  \brief Initializes default values for MLContactsListArgs.
  \apilevel 8

  \param[in, out] in_out_args The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLContactsListArgsInit(MLContactsListArgs *in_out_args) {
  in_out_args->version = 2u;
  in_out_args->limit = MLContacts_DefaultFetchLimit;
  in_out_args->offset = NULL;
}

/*!
  \brief Fields that will be used for search operations.
  \apilevel 2
*/
typedef enum MLContactsSearchField {
  /*! Search field for nickname. */
  MLContactsSearchField_Name = 1u,
  /*! Search field for phone. */
  MLContactsSearchField_Phone = 1u << 1u,
  /*! Search field for email. */
  MLContactsSearchField_Email = 1u << 2u,
  /*! Search across all fields. */
  MLContactsSearchField_All = MLContactsSearchField_Name | MLContactsSearchField_Phone | MLContactsSearchField_Email,
 /*! Ensure enum is represented as 32 bits. */
  MLContactsSearchField_Ensure32Bits = 0x7FFFFFFF
} MLContactsSearchField;

/*!
  \brief Stores arguments for a search request. See MLContactsRequestSearch().
  \apilevel 2
*/
typedef struct MLContactsSearchArgs {
  /*! Version of this structure. */
  uint32_t version;

  /*! Request results offset by this amount assuming contacts are ordered in a consistent way. */
  MLContactsOffset offset;

  /*! Limit the number of results returned by search operation. Min limit is 1. */
  uint32_t limit;

  /*! Query text/keywords. */
  const char *query;

  /*! Bitwise mask of fields where to search. See MLContactsSearchField. */
  MLContactsSearchField fields;
} MLContactsSearchArgs;

/*!
  \brief Initializes default values for MLContactsSearchArgs.
  \apilevel 8

  \param[in, out] in_out_args The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLContactsSearchArgsInit(MLContactsSearchArgs *in_out_args) {
  in_out_args->version = 2u;
  in_out_args->offset = NULL;
  in_out_args->limit = MLContacts_DefaultFetchLimit;
  in_out_args->query = NULL;
  in_out_args->fields = MLContactsSearchField_All;
}

/*!
  \brief Defines possible status values for an operation performed on a MLContactsContact.
  \apilevel 2
 */
typedef enum MLContactsOperationStatus {
  /*! Operation succeeded. */
  MLContactsOperationStatus_Success,

  /*! Operation failed. */
  MLContactsOperationStatus_Fail,

  /*! MLContactsContact with the details specified for an insert already exists. */
  MLContactsOperationStatus_Duplicate,

  /*! MLContactsContact to be deleted/updated doesn't exist. */
  MLContactsOperationStatus_NotFound,

  /*! Ensure enum is represented as 32 bits. */
  MLContactsOperationStatus_Ensure32Bits = 0x7FFFFFFF
} MLContactsOperationStatus;

/*!
  \brief Fields that will be fetched for a selection of contacts in MLContactsRequestSelection().

  \apilevel 7
*/
typedef enum MLContactsSelectionField {
  /*! Select nickname. */
  MLContactsSelectionField_Name = 1u,
  /*! Select phone. */
  MLContactsSelectionField_Phone = 1u << 1u,
  /*! Select email. */
  MLContactsSelectionField_Email = 1u << 2u,
  /*! Select all fields. */
  MLContactsSelectionField_All = MLContactsSelectionField_Name | MLContactsSelectionField_Phone |
                                 MLContactsSelectionField_Email,
  /*! Ensure enum is represented as 32 bits. */
  MLContactsSelectionField_Ensure32Bits = 0x7FFFFFFF
} MLContactsSelectionField;

/*!
  \brief Stores the result of an operation performed on a MLContactsContact.
  \apilevel 2
*/
typedef struct MLContactsOperationResult {
  /*! Version of this structure. */
  uint32_t version;

  /*! Status of the operation. */
  MLContactsOperationStatus operation_status;

  /*!
    Resultant contact with updated fields, for e.g., the 'id' of the contact would be available
    in this resultant contact for MLContactsRequestInsert().
  */
  const MLContactsContact *contact;
} MLContactsOperationResult;

/*!
  \brief Initializes default values for MLContactsOperationResult.
  \apilevel 2

  \param[in, out] in_out_result The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLContactsOperationResultInit(MLContactsOperationResult *in_out_result) {
  in_out_result->version = 1u;
  in_out_result->operation_status = MLContactsOperationStatus_Fail;
  in_out_result->contact = NULL;
}

/*!
  \brief Stores result of an operation that returns a list of contacts.
  \apilevel 2
*/
typedef struct MLContactsListResult {
  /*! Version of this structure. */
  uint32_t version;

  /*! Status of operation. */
  MLContactsOperationStatus operation_status;

  /*! Resultant list of contacts. */
  MLContactsContactList result_list;

  /*! Offset to be used to get the next batch of results for this operation. */
  MLContactsOffset offset;

  /*! Total number of hits for this request. */
  size_t total_hits;
} MLContactsListResult;

/*!
  \brief Initializes default values for MLContactsListResult.
  \apilevel 2

  \param[in, out] in_out_result The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLContactsListResultInit(MLContactsListResult *in_out_result) {
  in_out_result->version = 1u;
  in_out_result->operation_status = MLContactsOperationStatus_Fail;
  MLContactsContactListInit(&in_out_result->result_list);
  in_out_result->offset = NULL;
  in_out_result->total_hits = 0;
}

/*!
  \brief Stores arguments for a selection request (MLContactsRequestSelection()).

  \apilevel 7
*/
typedef struct MLContactsSelectionArgs {
  /*! Version of this structure. */
  uint32_t version;

  /*! Limit the number of selections in the operation. Min limit is 1. */
  uint32_t limit;

  /*! Bitwise mask of fields to be fetched for contacts. See MLContactsSelectionField. */
  MLContactsSelectionField fields;
} MLContactsSelectionArgs;

/*!
  \brief Initializes default values for MLContactsSelectionArgs.

  \apilevel 8

  \param[in,out] inout_args The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLContactsSelectionArgsInit(MLContactsSelectionArgs *inout_args) {
  inout_args->version = 2u;
  inout_args->limit = 1u;
  inout_args->fields = MLContactsSelectionField_All;
}

/*!
  \brief Serialize MLResult.

  Serialize MLResult (including MLContactsResult).

  \apilevel 2

  \param[in] result MLResult to be serialized.

  \return Serialized human-readable version of MLResult.

  \priv None
*/
ML_API const char *ML_CALL MLContactsGetResultString(MLResult result);

/*!
  \brief Initialize all necessary resources for using MLContacts API.
  \apilevel 2

  \retval MLResult_Ok If successfully initialized.
*/
ML_API MLResult ML_CALL MLContactsStartup();

/*!
  \brief Deinitialize all resources for this API.
  \apilevel 2

  \retval MLResult_Ok If successfully shutdown.
*/
ML_API MLResult ML_CALL MLContactsShutdown();

/*!
  \brief Insert a MLContactsContact.

  Request add operation for new contact. To get result of this operation,
  see MLContactsTryGetOperationResult(). Operation will fail for a contact that does not
  contain a name and at least one email address or phone numbers or for a contact that is a
  duplicate of existing one. Contact id is assigned upon successful completion of add operation; any
  id specified for input parameter here will be ignored.

  \apilevel 8

  \param[in] contact Contact info.
  \param[out] out_request_handle A pointer to an #MLHandle which will contain the handle to the request.
                                 If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam If either of the parameters are invalid.
  \retval MLResult_Ok If successfully submitted.
  \retval MLResult_PrivilegeDenied If permissions haven't been granted to make this API call.
  \retval MLContactsResult_IllegalState If the device / system is in an invalid state.

  \priv AddressBookWrite
*/
ML_API MLResult ML_CALL MLContactsRequestInsert(const MLContactsContact *contact, MLHandle *out_request_handle);

/*!
  \brief Update an existing MLContactsContact.

  Request edit operation for existing contact. To get result of this operation,
  see MLContactsTryGetOperationResult(). Contacts are matched by id and input contact must
  contain a valid id. Operation will fail for a contact that does not contain a name and at least
  one email address or phone numbers or for a contact that is a duplicate of existing one.

  \apilevel 8

  \param[in] contact Contact info.
  \param[out] out_request_handle A pointer to an #MLHandle which will contain the handle to the request.
                                 If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam If either of the parameters are invalid.
  \retval MLResult_Ok If successfully submitted.
  \retval MLResult_PrivilegeDenied If permissions haven't been granted to make this API call.
  \retval MLContactsResult_IllegalState If the device / system is in an invalid state.

  \priv AddressBookWrite
*/
ML_API MLResult ML_CALL MLContactsRequestUpdate(const MLContactsContact *contact, MLHandle *out_request_handle);

/*!
  \brief Delete an existing MLContactsContact.

  Request delete operation for existing contact. To get result of this operation,
  see MLContactsTryGetOperationResult(). Contacts are matched by id and input contact must
  contain a valid id.

  \apilevel 8

  \param[in] contact Contact info.
  \param[out] out_request_handle A pointer to an #MLHandle which will contain the handle to the request.
                                 If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam If either of the parameters are invalid.
  \retval MLResult_Ok If successfully submitted.
  \retval MLResult_PrivilegeDenied If permissions haven't been granted to make this API call.
  \retval MLContactsResult_IllegalState If the device / system is in an invalid state.

  \priv AddressBookWrite
*/
ML_API MLResult ML_CALL MLContactsRequestDelete(const MLContactsContact *contact, MLHandle *out_request_handle);

/*!
  \brief Try to get result of an operation that was previously requested on a single MLContactsContact
  (i.e. Insert, Update, or Delete).

  This API call may be repeated if it returns MLResult_Pending for a request handle.

  \apilevel 2

  \param[in] request_handle A handle to the request.
  \param[out] out_result Result. See MLContactsOperationResult.

  \retval MLResult_InvalidParam If the request handle doesn't correspond to this operation.
  \retval MLResult_Pending If the request is still pending.
  \retval MLContactsResult_Completed If the request is completed.
  \retval MLContactsResult_HandleNotFound If the request corresponding to the handle was not found.

  \priv None
*/
ML_API MLResult ML_CALL MLContactsTryGetOperationResult(MLHandle request_handle, MLContactsOperationResult **out_result);

/*!
  \brief List available contacts.

  Request list operation. To get result of this operation, see MLContactsTryGetListResult.
  Contacts are listed in lexicographical order based on serialization of name, [tag, email] list,
  and [tag, phone_number] list.

  \apilevel 8

  \param[in] args See MLContactsListArgs.
  \param[out] out_request_handle A pointer to an #MLHandle which will contain the handle to the request.
                                 If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam Invalid input parameter.
  \retval MLResult_Ok If successfully submitted.
  \retval MLContactsResult_IllegalState If the device / system is in an invalid state.

  \priv AddressBookRead
*/
ML_API MLResult ML_CALL MLContactsRequestList(const MLContactsListArgs *args, MLHandle *out_request_handle);

/*!
  \brief Search for contacts with a given query across specified fields.

  Request search operation. To get result of this operation, see MLContactsTryGetListResult.
  Search results will be listed in lexicographical order based on serialization of name, [tag,
  email] list, and [tag, phone_number] list. Partial matching of search keywords is supported.

  \apilevel 8

  \param[in] args See MLContactsSearchArgs.
  \param[out] out_request_handle A pointer to an #MLHandle which will contain the handle to the request.
                                 If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam Invalid input parameter.
  \retval MLResult_Ok If successfully submitted.
  \retval MLContactsResult_IllegalState If the device / system is in an invalid state.

  \priv AddressBookRead
*/
ML_API MLResult ML_CALL MLContactsRequestSearch(const MLContactsSearchArgs *args, MLHandle *out_request_handle);

/*!
  \brief Try to get result of a request which is expected to return MLContactsListResult (i.e .List or Search).
  \apilevel 2

  This API call may be repeated if it returns MLResult_Pending for a request handle.

  \param[in] request_handle A handle to the request.
  \param[out] out_result Result. See MLContactsListResult.

  \retval MLResult_InvalidParam If the request handle doesn't correspond to this operation.
  \retval MLResult_Pending If the request is still pending.
  \retval MLContactsResult_Completed If the request is completed.
  \retval MLContactsResult_HandleNotFound If the request corresponding to the handle was not found.

  \priv None
*/
ML_API MLResult ML_CALL MLContactsTryGetListResult(MLHandle request_handle, MLContactsListResult **out_result);

/*!
  \brief Cancel a request corresponding to the handle. Request cancellation will also release
  resources associated with request handle.

  This can be used in dismissing a MLContactsRequestSelection() request.

  \apilevel 8

  \param[in] request_handle A handle to the request.

  \return MLResult_OK If request was successfully cancelled.
  \retval MLContactsResult_HandleNotFound If the request corresponding to the handle was not found.

  \priv None
*/
ML_API MLResult ML_CALL MLContactsCancelRequest(MLHandle request_handle);

/*!
  \brief Free all resources for a request corresponding to the handle. MLContacts API users are
  expected to free resources for all handles.

  \apilevel 2

  \param[in] request_handle A handle to the request.

  \retval MLResult_Ok If all resources for the request were successfully freed.
  \retval MLContactsResult_HandleNotFound If the request corresponding to the handle was not found.

  \priv None
*/
ML_API MLResult ML_CALL MLContactsReleaseRequestResources(MLHandle request_handle);

/*!
  \brief Request a subset of contacts as manually selected by the user via separate system application.
  Request selection operation. To get result of this operation, MLContactsTryGetListResult().

  \apilevel 8

  \param[in] args See MLContactsSelectionArgs.
  \param[out] out_request_handle.

  \retval MLResult_Ok If successfully submitted.
  \retval MLResult_PrivilegeDenied If permissions haven't been granted to make this API call.
  \retval MLContactsResult_IllegalState If the device / system is in an invalid state.

  \priv AddressBookBasicAccess
*/
ML_API MLResult ML_CALL MLContactsRequestSelection(const MLContactsSelectionArgs *args, MLHandle *out_request_handle);

/*! \} */

ML_EXTERN_C_END
