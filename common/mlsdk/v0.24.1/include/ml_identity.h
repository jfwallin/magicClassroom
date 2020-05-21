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
  \brief APIs for the Identity system.
  \{
*/

/*!
  \brief #MLIdentityAttributeKey identifies an attribute in a user profile.
  Attributes that were not known at the time the library was built, are marked as
  #MLIdentityAttributeKey_Unknown
  Note: Attribute values that are marked 'Disabled' will silently be ignored
  by MLIdentityRequestAttributeValues and MLIdentityRequestAttributeValuesAsync,
*/
typedef enum MLIdentityAttributeKey {
  /*! Unknown. */
  MLIdentityAttributeKey_Unknown,
  /*! Given name - Disabled in 0.24.0. */
  MLIdentityAttributeKey_GivenName,
  /*! Family name - Disabled in 0.24.0. */
  MLIdentityAttributeKey_FamilyName,
  /*! Email - Disabled in 0.24.0. */
  MLIdentityAttributeKey_Email,
  /*! Bio - Disabled in 0.24.0. */
  MLIdentityAttributeKey_Bio,
  /*! Phone number - Disabled in 0.24.0. */
  MLIdentityAttributeKey_PhoneNumber,
  /*! 2D avatar - Disabled in 0.24.0. */
  MLIdentityAttributeKey_Avatar2D,
  /*! 3D avatar. */
  MLIdentityAttributeKey_Avatar3D,
  /*! Nickname */
  MLIdentityAttributeKey_Nickname,
  /*! The number of keys. */
  MLIdentityAttributeKey_Count,
  /*! Ensure enum is represented as 32 bits. */
  MLIdentityAttributeKey_Ensure32Bits = 0x7FFFFFFF
} MLIdentityAttributeKey;

/*!
  \brief #MLIdentityAttribute represents an attribute of a user's profile
  (for instance: name, address, email). Each attribute has a name (represented by key and value).
*/
typedef struct MLIdentityAttribute {
  /*! Identity attribute. */
  MLIdentityAttributeKey key;
  /*! Name. */
  const char *name;
  /*! Value. */
  const char *value;
  /*! /c true if requested. /c false otherwise. */
  bool is_requested;
  /*! /c true if granted. /c false otherwise. */
  bool is_granted;
} MLIdentityAttribute;

/*! #MLIdentityProfile represents a set of attribute of a user's profile. */
typedef struct MLIdentityProfile {
  /*! The number of #MLIdentityAttribute structures pointed to by attribute_ptrs. */
  uint32_t attribute_count;
  /*! An array of #MLIdentityAttributes containing user profile information. */
  MLIdentityAttribute *const *attribute_ptrs;
} MLIdentityProfile;

enum {
  /*! Defines the prefix for MLIdentityResult codes. */
  MLResultAPIPrefix_Identity = MLRESULT_PREFIX(0x7d4d)
};

/*! Return values for Identity API calls. */
typedef enum MLIdentityResult {

  /*! The local service is not running, or it cannot be accessed. */
  MLIdentityResult_FailedToConnectToLocalService = MLResultAPIPrefix_Identity,
  /*!
    \brief The service failed to access the cloud service.
    Either there is no IP connection or the cloud service is not available.
  */
  MLIdentityResult_FailedToConnectToCloudService,
  /*!
    \brief The user does not have the required privileges to use the requesting service
    or the refresh token used by the service is invalid.
  */
  MLIdentityResult_CloudAuthentication,
  /*! Signature verification failed on the information returned by the cloud or a parsing error occurred. */
  MLIdentityResult_InvalidInformationFromCloud,
  /*! The operation failed because the user is not logged in to the cloud. */
  MLIdentityResult_NotLoggedIn,
  /*! The user's credentials have expired. */
  MLIdentityResult_ExpiredCredentials,
  /*! Failed to retrieve attributes of the user's profile. */
  MLIdentityResult_FailedToGetUserProfile,
  /*! The cloud rejected the operation because the user is not authorized to execute it. */
  MLIdentityResult_Unauthorized,
  /*! The device failed to authenticate the server. */
  MLIdentityResult_CertificateError,
  /*! The cloud rejected the operation. */
  MLIdentityResult_RejectedByCloud,
  /*! The user is already logged in. */
  MLIdentityResult_AlreadyLoggedIn,
  /*! The cloud does not support modification of an attribute value. */
  MLIdentityResult_ModifyIsNotSupported,
  /*! The device is not connected to a network. */
  MLIdentityResult_NetworkError,
  /*! Ensure enum is represented as 32 bits. */
  MLIdentityResult_Ensure32Bits = 0x7FFFFFFF
} MLIdentityResult;

/*!
  \brief MLIdentityGetAttributeNames() is a blocking function that access the cloud and
  returns a #MLIdentityProfile structure containing all of the attributes that are available
  for the Audience of the calling service.

  The library deduces the Audience being requested from the name of the calling service.
  This method does not request access to the values of the attributes from the user and does
  not return the values of these attributes. Only the names are provided.
  In order to request access for the attributes values and to receive them, set the
  is_requested field of each required attribute in the profile that is returned and call
  one of the methods: MLIdentityRequestAttributeValues() or MLIdentityRequestAttributeValuesAsync().

  \param[out] out_profile
  \parblock
  A pointer to a #MLIdentityProfile pointer that is allocated by the library.
  In each attribute of the returned profile the name field will point to the name of the
  attribute, the value field will point to an empty string and the is_requested and is_granted
  flags will both be \c false.

  The returned profile structure and any of its fields must not be modified by the caller except
  to set the is_requested field. (See MLIdentityRequestAttributeValues())
  The returned profile must be released when no longer needed by calling MLIdentityReleaseUserProfile().
  \endparblock

  \retval MLResult_AllocFailed The operation failed to allocate memory.
  \retval MLResult_InvalidParam out_profile was 0 (null).
  \retval MLResult_Ok The operation completed successfully.
  \retval MLResult_PrivilegeDenied The caller does not have the IdentityRead privilege.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLIdentityResult An identity specific failure occurred during the operation.

  \priv IdentityRead
*/

ML_API MLResult ML_CALL MLIdentityGetAttributeNames(MLIdentityProfile **out_profile);

/*!
  \brief MLIdentityGetKnownAttributeNames() returns a #MLIdentityProfile structure containing
  attributes of a user's profile whose names are specified in the attributeNames array.

  Each element of the attributeNames array may only be one of the values specified above.
  This method does not access the cloud to discover which attribute names are actually
  available for the calling service, does not request access to the values of these
  attributes for the service by the user, and does not return the values of these attributes.
  In order to request access for the attributes and to receive their values, pass the profile
  that is returned by MLIdentityGetKnownAttributeNames() to one of the methods:
  MLIdentityRequestAttributeValues() or MLIdentityRequestAttributeValuesAsync().

  \param[in] keys is an array of distinct #MLIdentityAttributeKey values that
             are not equal #to MLIdentityAttributeKey_Unknown.
  \param[in] key_count is the number of elements in the #MLIdentityAttributeKey array.
  \param[out] out_profile
  \parblock
  A pointer to #MLIdentityProfile pointer which will be allocated by the library.
  The returned profile will contain an array of #MLIdentityAttribute structures such that the
  enumValue of each attribute is contained in the keys array

  If the attribute specified by any of the keys is not available, a corresponding
  MLIdentityAttribute will not appear in the #MLIdentityProfile.
  Not available in this context means that the user profile does not contain the attribute,
  not that the caller does not have permission to access its value.

  All attributes in the returned profile will have their is_requested field set to true and
  their is_granted field to false. The values of the attributes will point to empty strings.
  The returned profile must be released using #MLIdentityReleaseUserProfile when no longer needed.
  \endparblock

  \retval MLResult_AllocFailed The operation failed to allocate memory.
  \retval MLResult_InvalidParam either keys or out_profile were 0 (null).
  \retval MLResult_Ok The operation completed successfully.
  \retval MLResult_PrivilegeDenied The caller does not have the IdentityRead privilege.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLIdentityResult An identity specific failure occurred during the operation.

  \priv IdentityRead
*/
ML_API MLResult ML_CALL MLIdentityGetKnownAttributeNames(const MLIdentityAttributeKey *keys, uint32_t key_count, MLIdentityProfile **out_profile);

/*!
  \brief MLIdentityGetAttributeNamesAsync() invokes the MLIdentityGetAttributeNames()
  function asynchronously (in a different thread).

  \param[out] out_future
  \parblock
  A pointer to an #MLInvokeFuture pointer which provides the means to poll for completion and
  to retrieve the profile returned by MLIdentityGetAttributeNames().

  Note: This pointer will be freed by the library before returning from the first (and last)
  call to MLIdentityGetAttributeNamesWait(), after the asynchronous call completed, , that is
  after MLIdentityGetAttributeNamesWait() returns any value that is not MLResult_Pending
  \endparblock

  \retval MLResult_AllocFailed The operation failed to allocate memory.
  \retval MLResult_InvalidParam out_future was 0 (null).
  \retval MLResult_Ok The operation completed successfully.
  \retval MLResult_PrivilegeDenied The caller does not have the IdentityRead privilege.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLIdentityResult An identity specific failure occurred during the operation.

  \priv IdentityRead
*/
ML_API MLResult ML_CALL MLIdentityGetAttributeNamesAsync(MLInvokeFuture** out_future);

/*!
  \brief Having made a call to MLIdentityGetAttributeNamesAsync(), the user can call
  MLIdentityGetAttributeNamesWait() to detect whether the asynchronous call completed and if
  successful, to retrieve the profile in out_profile.

  The call to MLIdentityGetAttributeNamesWait() blocks until either msec_timeout msec elapses or the
  asynchronous function completes.

  Note:
  1. To poll without blocking use msec_timeout = 0.
  2. After the function returns a value other than MLResult_Pending, future is freed by the
     library and must not be used again in a call to MLIdentityGetAttributeNamesWait().

  \param[in,out] future The pointer returned by the MLIdentityGetAttributeNamesAsync() function.
  \param[in] msec_timeout The timeout in milliseconds.
  \param[out] out_profile The location in memory where the pointer to the profile structure allocated by
              the library will be copied, if the asynchronous call completed, or 0 (null) if not.

  \retval MLResult_AllocFailed The operation failed to allocate memory.
  \retval MLResult_InvalidParam future or out_profile were 0 (null).
  \retval MLResult_Ok The operation completed successfully before the timeout elapsed.
          The location pointed to by out_profile was set to the address of the #MLIdentityProfile
          structure allocated by the library. The returned profile must be released when no longer
          needed by calling MLIdentityReleaseUserProfile().
          If any other value is returned, the location pointed to by out_profile is set to 0 (null).
  \retval MLResult_Pending The timeout elapsed before the asynchronous call completed.
  \retval MLResult_PrivilegeDenied The caller does not have the IdentityRead privilege.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLIdentityResult An identity specific failure occurred during the operation.

  \priv IdentityRead
*/
ML_API MLResult ML_CALL MLIdentityGetAttributeNamesWait(MLInvokeFuture *future, uint32_t msec_timeout, MLIdentityProfile **out_profile);

/*!
  \brief MLIdentityRequestAttributeValues() is a blocking function that accesses the cloud
  and requests permission from the user in order to fill the attribute values that are marked as
  requested in the #MLIdentityProfile pointed to by the profile argument.

  Note: If any (even all) of the attributes passed in have is_requested set to false this will not
  cause the function to return an error.
  If any (even all) of the attributes passed in are no longer available for the user, this
  will not cause the function to return an error.

  \param[in,out] profile
  \parblock
  A pointer to the #MLIdentityProfile that was allocated by the library in which
  (none, ) one or more of the attributes have had their is_requested field set to true.
  Attributes for which the is_requested field is true, that are still available in the cloud and
  which the user has approved to make available for the calling service will have their
  values filled.
  The is_granted field will be set by the library to true in those attributes that were filled
  and false in the others.
  \endparblock

  \retval MLResult_AllocFailed The operation failed to allocate memory.
  \retval MLResult_InvalidParam profile was 0 (null).
  \retval MLResult_Ok The operation completed successfully.
  \retval MLResult_PrivilegeDenied The caller does not have the IdentityRead privilege.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLIdentityResult An identity specific failure occurred during the operation.

  \priv IdentityRead
*/
ML_API MLResult ML_CALL MLIdentityRequestAttributeValues(MLIdentityProfile *profile);

/*!
  \brief MLIdentityRequestAttributeValuesAsync() invokes the
  MLIdentityRequestAttributeValues() function in a different thread.

  \param[in,out] profile
                 \parblock
                 A pointer to the #MLIdentityProfile that was allocated by the library in which
                 (none, ) one or more of the attributes have had their is_requested field set to true.

                 Attributes for which the is_requested field is true, that are still available in the cloud and
                 which the user has approved to make available for the calling service will have their
                 values filled.

                 The is_granted field will be set by the library to \c true in those attributes that were filled
                 and \c false in the others.

                 profile must not be released (using MLIdentityReleaseUserProfile()) until
                 MLIdentityRequestAttributeValuesWait() returns a value other than MLResult_Pending, because
                 it will be written to asynchronously by MLIdentityRequestAttributeValuesAsync().
                 \endparblock
  \param[out] out_future
              \parblock
              A pointer to an #MLInvokeFuture pointer which provides the means to poll for completion and
              to retrieve the profile returned by MLIdentityRequestAttributeValues().

              Note: This pointer will be freed by the library before returning from the first (and last) call
              to MLIdentityRequestAttributeValuesWait() after the asynchronous call completed, that is
              after MLIdentityRequestAttributeValuesWait() returns any value that is not MLResult_Pending.
              \endparblock

  \retval MLResult_AllocFailed The operation failed to allocate memory.
  \retval MLResult_InvalidParam profile or out_future were 0 (null).
  \retval MLResult_Ok The operation completed successfully and out_future points
          to an allocated MLInvokeFuture. If any other value is returned, the location pointed to
          by out_future is set to 0 (null).
  \retval MLResult_PrivilegeDenied The caller does not have the IdentityRead privilege.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLIdentityResult An identity specific failure occurred during the operation.

  \priv IdentityRead
*/
ML_API MLResult ML_CALL MLIdentityRequestAttributeValuesAsync(MLIdentityProfile *profile, MLInvokeFuture** out_future);

/*!
  \brief Having made a call to MLIdentityRequestAttributeValuesAsync(), the user can call
  MLIdentityRequestAttributeValuesWait() to detect whether the asynchronous call completed.

  The call to MLIdentityRequestAttributeValuesWait() blocks until either msec_timeout msec elapses
  or the asynchronous function completes.

  Note:
  1. To poll without blocking use msec_timeout = 0.
  2. After the function returns a result other than MLResult_Pending, future is freed by the
     library and must not be used again in a call to MLIdentityRequestAttributeValuesWait().

  \param[in,out] future The pointer returned by the MLIdentityRequestAttributeValuesAsync() function.
  \param[in] msec_timeout The timeout in milliseconds.

  \retval MLResult_AllocFailed The operation failed to allocate memory.
  \retval MLResult_InvalidParam future was 0 (null).
  \retval MLResult_Ok The operation completed successfully before the timeout elapsed.
          The profile provided in the MLIdentityRequestAttributeValuesAsync call was updated as follows:
          Attributes for which the is_requested field is true, that are still available in the cloud and
          which the user has approved to make available for the calling service will have their
          values filled.
          The is_granted field will be set by the library to true in those attributes that were filled
          and false in the others.
          If any other value is returned, the profile provided in the
          MLIdentityRequestAttributeValuesAsync() call was not updated.
  \retval MLResult_Pending The timeout elapsed before the asynchronous call completed.
  \retval MLResult_PrivilegeDenied The caller does not have the IdentityRead privilege.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLIdentityResult An identity specific failure occurred during the operation.

  \priv IdentityRead
*/
ML_API MLResult ML_CALL MLIdentityRequestAttributeValuesWait(MLInvokeFuture *future, uint32_t msec_timeout);

/*!
  \brief MLIdentityReleaseUserProfile() releases all resources associated with the
  #MLIdentityProfile stucture that was returned by the library.

  \param[in] profile A pointer to a library allocated structure received using one of:
             MLIdentityGetAttributeNames(), MLIdentityGetAttributeNamesWait() or
             MLIdentityGetKnownAttributeNames().

  \retval MLResult_InvalidParam profile was 0 (null).
  \retval MLResult_Ok The operation completed successfully.

  \priv IdentityRead
*/
ML_API MLResult ML_CALL MLIdentityReleaseUserProfile(MLIdentityProfile *profile);

/*!
  \brief Returns an ASCII string for MLIdentityResult and MLResultGlobal codes.
  \param[in] result_code The input MLResult enum from MLIdentity functions.
  \return ASCII string containing readable version of result code
  \priv None
*/
ML_API const char* ML_CALL MLIdentityGetResultString(MLResult result_code);

/*! \} */

ML_EXTERN_C_END
