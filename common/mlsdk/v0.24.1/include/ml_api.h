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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#if defined(_MSC_VER) && _MSC_VER
#ifdef ML_EXPORT
#define ML_API __declspec(dllexport)
#else
#define ML_API __declspec(dllimport)
#endif
#define ML_CALL __cdecl
#if defined(ML_NO_DEPRECATION_WARNING)
#define ML_DEPRECATED_MSG(msg)
#define ML_DEPRECATED
#if !defined(ML_NO_DEPRECATION_DISABLED_MSG)
#pragma message("Lumin SDK - API deprecation warnings are disabled.")
#endif
#else
#define ML_DEPRECATED_MSG(msg) __declspec(deprecated(msg))
#define ML_DEPRECATED __declspec(deprecated)
#endif
#define ML_STATIC_INLINE __inline
#else
#define ML_API
#define ML_CALL
#if defined(ML_NO_DEPRECATION_WARNING)
#define ML_DEPRECATED_MSG(msg)
#define ML_DEPRECATED
#if !defined(ML_NO_DEPRECATION_DISABLED_MSG)
#pragma message("Lumin SDK - API deprecation warnings are disabled.")
#endif
#else
#define ML_DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))
#define ML_DEPRECATED __attribute__((deprecated))
#endif
#define ML_STATIC_INLINE static inline
#endif

#ifdef __cplusplus
#define ML_EXTERN_C_BEGIN extern "C" {
#define ML_EXTERN_C_END }
#else
#define ML_EXTERN_C_BEGIN
#define ML_EXTERN_C_END
#endif

#include "ml_coordinate_frame_uid.h"
#include "ml_version.h"

ML_EXTERN_C_BEGIN

/*!
  \brief Integer handle type used to reference many things returned by the API, instead of
  returning pointers directly. Use MLHandleIsValid() to check if a handle is valid or not.
*/
typedef uint64_t MLHandle;

enum {
  /*! Value of an invalid handle. To test if a handle is invalid use MLHandleIsValid(). */
  ML_INVALID_HANDLE = 0xFFFFFFFFFFFFFFFF
};

enum {
  /*! Maximum character length of MLVersion build ID. */
  MLVersion_BuildIdMaxLength = 128
};

/*! SDK version: '[major].[minor].[revision].[build_id]'. */
typedef struct MLVersion {
  /*! Major version. */
  uint32_t major;
  /*! Minor version. */
  uint32_t minor;
  /*! Revision. */
  uint32_t revision;
  /*! Build ID. */
  char build_id[MLVersion_BuildIdMaxLength];
} MLVersion;

/*! Macro to set a the prefix bytes of an API specific MLResult code. */
#define MLRESULT_PREFIX(val) (val << 16)

enum {
  /*! Defines the prefix for global MLResult codes. */
  MLResultAPIPrefix_Global = MLRESULT_PREFIX(0),
};

/*! Global MLResult codes that any API can optionally return. */
typedef enum MLResultGlobal {
  /*! Operation completed successfuly. */
  MLResult_Ok = MLResultAPIPrefix_Global,
  /*! Asynchronous operation has not completed. */
  MLResult_Pending,
  /*! Operation has timed out. */
  MLResult_Timeout,
  /*! Request to lock a shared resource that is already locked. */
  MLResult_Locked,
  /*! Operation failed due to an unspecified internal error. */
  MLResult_UnspecifiedFailure,
  /*! Operation failed due to an invalid parameter being supplied. */
  MLResult_InvalidParam,
  /*! Operation failed because memory failed to be allocated. */
  MLResult_AllocFailed,
  /*! Operation failed because a required privilege has not been granted. */
  MLResult_PrivilegeDenied,
  /*! Operation failed because it is not currently implemented. */
  MLResult_NotImplemented,
  /*! Ensure enum is represented as 32 bits. */
  MLResult_Ensure32Bits = 0x7FFFFFFF
} MLResultGlobal;

/*! MLResult type that all functions in the APIs return. */
typedef int32_t MLResult;

/*!
  \brief Returns an ASCII string for MLResultGlobal codes.
  \param[in] result_code The input MLResult enum from ML API functions.
  \return ASCII string containing readable version of result code.
  \priv None
*/
ML_STATIC_INLINE const char* ML_CALL MLGetResultString(MLResult result_code) {
  switch (result_code) {
    case MLResult_Ok: return "MLResult_Ok";
    case MLResult_Pending: return "MLResult_Pending";
    case MLResult_Timeout: return "MLResult_Timeout";
    case MLResult_Locked: return "MLResult_Locked";
    case MLResult_UnspecifiedFailure: return "MLResult_UnspecifiedFailure";
    case MLResult_InvalidParam: return "MLResult_InvalidParam";
    case MLResult_AllocFailed: return "MLResult_AllocFailed";
    case MLResult_PrivilegeDenied: return "MLResult_PrivilegeDenied";
    case MLResult_NotImplemented: return "MLResult_NotImplemented";
    default: return "Unknown";
  }
}

/*!
  \brief Check if a handle is valid.
  \param[in] handle The handle to check.
  \return \c true if the handle is valid.
  \priv None
*/
ML_STATIC_INLINE bool MLHandleIsValid(MLHandle handle) {
  return handle != ML_INVALID_HANDLE;
}

/*!
  \brief Get SDK version as c-style string.
  \return SDK version as c-style string.
  \priv None
*/
ML_STATIC_INLINE const char *ML_CALL MLGetVersionName() {
  return MLSDK_VERSION_NAME;
}

/*!
  \brief Get SDK version as a struct with major, minor, revision, and build_id.
  \return SDK version as a struct.
  \priv None
*/
ML_STATIC_INLINE MLVersion MLGetVersion() {
  MLVersion version = {MLSDK_VERSION_MAJOR,
                       MLSDK_VERSION_MINOR,
                       MLSDK_VERSION_REVISION,
                       MLSDK_VERSION_BUILD_ID};
  return version;
}

ML_EXTERN_C_END
