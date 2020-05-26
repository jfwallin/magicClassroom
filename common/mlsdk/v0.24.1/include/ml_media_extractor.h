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
#include "ml_types.h"
#include "ml_media_common.h"
#include "ml_media_data_source.h"
#include "ml_media_types.h"

ML_EXTERN_C_BEGIN

/*!
  \defgroup MediaPlayer Media Player
  \addtogroup MediaPlayer
  \sharedobject ml_mediaextractor
  \brief APIs for to extracting elementary track from media container.

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.
  \{
*/

/*! Sample flag output from #MLMediaExtractorGetSampleFlags. */
typedef enum MLMediaExtractorSampleFlag {
  MLMediaExtractorSampleFlag_Sync         = 1,
  MLMediaExtractorSampleFlag_Encrypted    = 2,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaExtractorSampleFlag_Ensure32Bits = 0x7FFFFFFF
} MLMediaExtractorSampleFlag;

/*! Seek mode used by #MLMediaExtractorSeekTo. */
typedef enum MLMediaExtractorSeekPostion {
  MLMediaExtractorSeekPosition_Previous_Sync,
  MLMediaExtractorSeekPosition_Next_Sync,
  MLMediaExtractorSeekPosition_Closest_Sync,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaExtractorSeekPostion_Ensure32Bits = 0x7FFFFFFF
} MLMediaExtractorSeekPostion;

typedef MLPSSHEntry MLMediaExtractorPSSHEntry;

/*! DRM Scheme initialization data. */
typedef struct MLMediaExtractorDRMSchemeInitData {
  /*! Mimetype of the DRM Scheme initialization data. */
  char *mime_type;
  /*! Size of the DRM Scheme initialization data. */
  size_t size;
  /*! DRM Scheme initialization data. */
  uint8_t *data;
} MLMediaExtractorDRMSchemeInitData;

/*!
  \brief Create a new \ref MLMediaExtractor.

  \param[out] out_handle A handle to the created \ref MLMediaExtractor.
              If there is an error during creation, it will
              return #ML_INVALID_HANDLE.

  \retval MLResult_AllocFailed If MediaExtractor handle cannot be allocated.
  \retval MLResult_InvalidParam If out_handle is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorCreate(MLHandle *out_handle);

/*!
  \brief Destroy a \ref MLMediaExtractor.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor object.

  \retval MLResult_InvalidParam If handle is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorDestroy(MLHandle handle);

/*!
  \brief Set a local file path as the data source.

  The path should be an absolute path and should
  reference a world-readable file.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[in] path Content path of the file.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorSetDataSourceForPath(MLHandle handle, const char *path);

/*!
  \brief Set a URI string as the data source.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[in] uri C-style string representing the URI complete string.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv Internet
*/
ML_API MLResult ML_CALL MLMediaExtractorSetDataSourceForURI(MLHandle handle, const char *uri);

/*!
  \brief Set a URI string as the data source.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[in] uri C-style string representing the complete URL.
  \param[in] headers Array of char * headers to be sent.
  \param[in] len Number of headers passed-in.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv Internet
*/
ML_API MLResult ML_CALL MLMediaExtractorSetDataSourceForURIWithHeaders(MLHandle handle, const char *uri, const char **headers, size_t len);

/*!
  \brief Set the data source as the file-descriptor to use.

  It is the caller's responsibility to close the file descriptor.
  It is safe to do so as soon as this call returns.

  \param[in] handle MLHandle to the MediaExtractor.
  \param[in] fd File-descriptor for the file to extract from.
  \param[in] offset Offset in bytes into the file where the data
             to be extracted starts.
  \param[in] length Length in bytes of the data to be extracted.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorSetDataSourceForFD(MLHandle handle, int fd, int64_t offset, int64_t length);

/*!
  \brief Sets the data source as user defined media data source.

  \param[in] handle MLHandle to the MediaExtractor.
  \param[in] media_data_source MLHandle returned by MLMediaDataSourceCreate().

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NameNotFound If media_data_source cannot be found.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorSetMediaDataSource(MLHandle handle, MLHandle media_data_source);

/*!
  \brief Extract DRM initialization data for a given DRM Scheme if it exists.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[in] drm_uuid UUID of the DRM scheme.
  \param[out] out_drm_init_data DRM initialization data pointer.
              To release the DRM initialization data upon successful return, call
              MLMediaExtractorReleaseDRMInitData().

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorGetDRMInitData(MLHandle handle, MLUUID drm_uuid, MLMediaExtractorDRMSchemeInitData *out_drm_init_data);

/*!
  \brief Release DRM initialization data which was previously retrieved.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[in] drm_init_data DRM initialization data that needs to be released.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorReleaseDRMInitData(MLHandle handle, MLMediaExtractorDRMSchemeInitData *drm_init_data);

/*!
  \brief Count the number of tracks found in the data source.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[out] out_track_count Number of tracks found in data source.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorGetTrackCount(MLHandle handle, uint64_t *out_track_count);

/*!
  \brief Get the track format at the specified index.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[in] index Index of the track.
  \param[out] out_track_handle \ref MLMediaFormat handle that represents track format.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorGetTrackFormat(MLHandle handle, size_t index, MLHandle *out_track_handle);

/*!
  \brief Select a track for the subsequent operations.

  Subsequent calls to MLMediaExtractorReadSampleData(), MLMediaExtractorGetSampleTrackIndex() and
  getSampleTime() only retrieve information for the subset of
  tracks selected. Selecting the same track multiple times has
  no effect, the track is only selected once.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[in] index Index of the track.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorSelectTrack(MLHandle handle, size_t index);

/*!
  \brief Unselect a track for the subsequent operations.

  Subsequent calls to MLMediaExtractorReadSampleData(), MLMediaExtractorGetSampleTrackIndex() and
  getSampleTime() only retrieve information for the subset of
  tracks selected.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[in] index Index of the track.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorUnselectTrack(MLHandle handle, size_t index);

/*!
  \brief All selected tracks seek near the requested time according to the specified mode.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[in] time_us Seek time.
  \param[in] mode Seek mode defined by MLMediaSeekMode.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorSeekTo(MLHandle handle, int64_t time_us, MLMediaSeekMode mode);

/*!
  \brief Advance to the next sample. Returns \c false if no more
  sample data is available (end of stream).

  \param[in] handle #MLHandle to the \ref MLMediaExtractor instance.

  \retval MLResult_InvalidParam If handle is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_FailedTransaction If Advance operation failed.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorAdvance(MLHandle handle);

/*!
  \brief Retrieve the current encoded sample and store it in the byte buffer starting at the given offset.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[in] byte_buf Destination byte buffer.
  \param[in] buf_size Size of the destination buffer.
  \param[in] offset Offset within the buffer.
  \param[out] out_data_size Sample size or -1 if no more samples are available.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorReadSampleData(MLHandle handle, uint8_t *byte_buf, size_t buf_size, size_t offset, int64_t *out_data_size);

/*!
  \brief Return the track index the current sample originates from.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor.
  \param[out] out_track_index Track index or -1 if no more samples are available.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorGetSampleTrackIndex(MLHandle handle, int64_t *out_track_index);

/*!
  \brief Return the current sample's presentation time in microseconds.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor instance.
  \param[out] out_sample_time Sample time or -1 if no more samples are available.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorGetSampleTime(MLHandle handle, int64_t *out_sample_time);

/*!
  \brief Return the current sample's flags.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor instance.
  \param[out] out_sample_flags Sample flag defined by #MLMediaExtractorSampleFlag, or -1 if on failure.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorGetSampleFlags(MLHandle handle, int *out_sample_flags);

/*!
  \brief Return the current sample's Crypto info if has encrypted sample.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor instance.
  \param[out] out_info_handle On success it returns handle to Crypto info of the sample.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorGetSampleCryptoInfo(MLHandle handle, MLHandle *out_info_handle);

/*!
  \brief Release the Crypto Info.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor instance.
  \param[in] info_handle handle to Crypto info that needs to be released.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorReleaseCryptoInfo(MLHandle handle, MLHandle *info_handle);

/*!
  \brief Return an estimate of how much data is presently cached in memoryexpressed in microseconds.

  \param[in] #handle MLHandle to the \ref MLMediaExtractor.
  \param[out] out_cached_duration Cached amount or -1 if that
              information is unavailable or not applicable (no cache).

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorGetCachedDuration(MLHandle handle, int64_t *out_cached_duration);

/*!
  \brief Return \c true if we are caching data and the cache has
  reached the end of the data stream (for now, a future
  seek may of course restart the fetching of data).

  This API only returns a meaningful result if getCachedDuration()
  indicates the presence of a cache, i.e. does NOT return -1.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor instance.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorHasCacheReachedEOS(MLHandle handle);

/*!
  \brief Get the Number of PSSH info if present.

  The PSSH(Protection System Specific Header)
  contains a UUID representing a DRM scheme along with opaque data blob
  used for creating crypto session, which in turn used to configure codec.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor instance.
  \param[out] out_index On successful return contains number of PSSH entries.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorGetPSSHInfoCount(MLHandle handle, size_t *out_index);

/*!
  \brief Get the PSSH info if present. The PSSH(Protection System Specific Header)
  contains a UUID representing a DRM scheme along with opaque data blob
  used for creating crypto session, which in turn used to configure codec.

  Upon successful return, if a #MLMediaExtractorPSSHEntry is returned,
  same should be released by calling MLMediaExtractorReleasePSSHInfo().

  \param[in] handle #MLHandle to the \ref MLMediaExtractor instance.
  \param[in] index Index of the PSSH info data element being retrieved.
  \param[out] out_pssh_entry Pointer to the PSSH info data element being retrieved.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorGetPSSHInfo(MLHandle handle, size_t index, MLMediaExtractorPSSHEntry *out_pssh_entry);

/*!
  \brief Clear the PSSH info data.

  Clears the entire cached PSSH info data in the library.

  \param[in] handle #MLHandle to the \ref MLMediaExtractor instance.
  \param[in] pssh_entry Pointer to the PSSH info data element that needs to be released.

  \retval MLResult_InvalidParam If one of parameters is invalid.
  \retval MLResult_Ok If operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaExtractorReleasePSSHInfo(MLHandle handle, MLMediaExtractorPSSHEntry *pssh_entry);

/*! \} */

ML_EXTERN_C_END
