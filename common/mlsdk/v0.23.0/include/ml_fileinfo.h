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
  \addtogroup FileInfo File Info
  \sharedobject ml_lifecycle
  \brief API to retrieve information from a file
  \{
*/

/*! This typedef can be used directly with POSIX APIs like open(), close() etc. */
typedef int32_t MLFileDescriptor;

/*!
  \brief Opaque structure containing file information like mime type, fd, file name.

  The fields in this structure can be obtained by:
  File descriptor - Can be obtained by calling MLFileInfoGetFD().
  File mime type  - Can be obtained by calling MLFileInfoGetMimeType().
  File name       - Can be obtained by calling MLFileInfoGetFileName().
*/
typedef struct MLFileInfo MLFileInfo;

/*!
  \brief This API allocates an empty #MLFileInfo structure.

  The mime type, file name fields will be set to NULL and file descriptor
  will be set to -1.

  \param[out] out_file_info Pointer to #MLFileInfo pointer. The caller should free the pointer by
              calling MLFileInfoRelease().

  \retval MLResult_Ok on success
  \retval MLResult_InvalidParam if input parameter is invalid.
  \retval MLResult_AllocFailed if memory allocation fails.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \priv none.
*/
ML_API MLResult ML_CALL MLFileInfoAllocateEmpty(MLFileInfo** out_file_info);

/*!
  \brief This API retrieves the mime type of the given #MLFileInfo.

  This returned mime type can be of the pattern <type/sub-type>.
  e.g application/xml where "application" is type and "xml" is sub-type.

  \param[in] file_info Pointer to #MLFileInfo struct pointer.
  \param[out] out_mime_type Address of pointer to mime type of the given #MLFileInfo.
              The caller should not free the pointer returned.

  \retval MLResult_Ok on success
  \retval MLResult_InvalidParam if input parameter is invalid.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \priv none.
*/
ML_API MLResult ML_CALL MLFileInfoGetMimeType(const MLFileInfo *file_info, const char** out_mime_type);

/*!
  \brief This API retrieves the file name associated with the given #MLFileInfo.

  This is for information purposes only and cannot be used to access the file
  itself. That can only be accessed via the file descriptor obtained from
  MLFileInfoGetFD().

  \param[in] file_info Pointer to #MLFileInfo struct pointer.
  \param[out] out_file_name Address of pointer to file name of the given #MLFileInfo.
              The caller should not free the pointer returned.

  \retval MLResult_Ok on success
  \retval MLResult_InvalidParam if input parameter is invalid.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \priv none.
*/
ML_API MLResult ML_CALL MLFileInfoGetFileName(const MLFileInfo *file_info, const char** out_file_name);

/*!
  \brief This API retrieves the file descriptor of the given #MLFileInfo.

  \param[in] file_info Pointer to #MLFileInfo struct pointer.
  \param[out] out_fd Pointer to variable that will hold the file descriptor of the given #MLFileInfo.

  \retval MLResult_Ok on success
  \retval MLResult_InvalidParam if input parameter is invalid.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \priv none.
*/
ML_API MLResult ML_CALL MLFileInfoGetFD(const MLFileInfo *file_info, MLFileDescriptor* out_fd);

/*!
  \brief Sets the file descriptor for the given #MLFileInfo structure.

  This makes a copy/dup of the file descriptor, so the caller has to
  close fd when no longer needed.

  \param[in] file_info #MLFileInfo structure whose file descriptor field will be set.
  \param[in] fd file descriptor that will be set in #MLFileInfo.

  \retval MLResult_Ok on success
  \retval MLResult_InvalidParam if input parameter is invalid.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \priv none.
*/
ML_API MLResult ML_CALL MLFileInfoSetFD(MLFileInfo *file_info, int fd);

/*!
  \brief Sets the file name for the given #MLFileInfo structure.

  The memory used to store the string will be released in MLFileInfoRelease().

  \param[in] file_info #MLFileInfo structure who's file name field will be set.
  \param[in] file_name file name associated with the fd that will be set in #MLFileInfo.

  \retval MLResult_Ok on success
  \retval MLResult_InvalidParam if input parameter is invalid.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \priv none.
*/
ML_API MLResult ML_CALL MLFileInfoSetFileName(MLFileInfo *file_info, const char *file_name);

/*!
  \brief Sets the file mime type for the given #MLFileInfo structure.

  The memory used to store the string will be released in MLFileInfoRelease().

  \param[in] file_info #MLFileInfo structure who's mime type field will be set.
  \param[in] mime_type mime type of the file that will be set in #MLFileInfo.

  \retval MLResult_Ok on success
  \retval MLResult_InvalidParam if input parameter is invalid.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \priv none.
*/
ML_API MLResult ML_CALL MLFileInfoSetMimeType(MLFileInfo *file_info, const char *mime_type);

/*!
  \brief Frees the pointer to #MLFileInfo that is allocated by MLFileInfoAllocateEmpty()

  The memory used to store the file info and mime type strings will be released in
  MLFileInfoRelease(). The file descriptor stored in the #MLFileInfo structure is also
  closed.

  \param[in] file_info pointer to #MLFileInfo structure pointer that will be freed.

  \retval MLResult_Ok on success
  \retval MLResult_InvalidParam if input parameter is invalid.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \priv none.
*/
ML_API MLResult ML_CALL MLFileInfoRelease(MLFileInfo **file_info);

/*! \} */

ML_EXTERN_C_END
