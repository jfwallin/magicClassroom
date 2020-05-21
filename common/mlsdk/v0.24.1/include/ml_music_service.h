// %BANNER_BEGIN%
// ---------------------------------------------------------------------
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
// ---------------------------------------------------------------------
// %BANNER_END%

#pragma once

#include "ml_api.h"
#include "ml_music_service_common.h"

ML_EXTERN_C_BEGIN

/*!
  \defgroup Music Service
  \addtogroup MusicService
  \sharedobject ml_musicservice
  \brief APIs for implementing background music service as client/application compoment.

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.
  \{
*/

/*!
 \brief Callbacks to be implemented by client to receive information from
        MusicService if callback mechanism is used. Note that the data passed
        into callbacks is destroyed upon return.
*/
typedef struct MLMusicServiceCallbacks {
  /*!
    \brief This callback is called when the playback state changes.
    \param[in] state Playback state.
    \param[in] data User pointer that was set through MLMusicServiceSetCallbacks().
  */
  void (*on_playback_state_change)(MLMusicServicePlaybackState state, void *data);
  /*!
    \brief This callback is called when the repeat state changes.
    \param[in] state Repeat state.
    \param[in] data User pointer that was set through MLMusicServiceSetCallbacks().
  */
  void (*on_repeat_state_change)(MLMusicServiceRepeatState state, void *data);
  /*!
    \brief This callback is called when the shuffle state changes.
    \param[in] state Shuffle state.
    \param[in] data User pointer that was set through MLMusicServiceSetCallbacks().
  */
  void (*on_shuffle_state_change)(MLMusicServiceShuffleState state, void *data);
  /*!
    \brief This callback is called when the metadata changes.
    \param[in] metadata Metadata.
    \param[in] data User pointer that was set through MLMusicServiceSetCallbacks().
  */
  void (*on_metadata_change)(const MLMusicServiceMetadata *metadata, void *data);
  /*!
    \brief This callback is called when the position changes.
    \param[in] metadata Position.
    \param[in] data User pointer that was set through MLMusicServiceSetCallbacks().
  */
  void (*on_position_change)(int32_t position, void *data);
  /*!
    \brief This callback is called when the music service encounters an error.
    \param[in] error_type Error type.
    \param[in] error_code Error code.
    \param[in] data User pointer that was set through MLMusicServiceSetCallbacks().
  */
  void (*on_error)(MLMusicServiceError error_type, int32_t error_code, void *data);
  /*!
    \brief This callback is called when the music service status changes.
    \param[in] status Status.
    \param[in] data User pointer that was set through MLMusicServiceSetCallbacks().
  */
  void (*on_status_change)(MLMusicServiceStatus status, void *data);
  /*!
    \brief This callback is called when the volume changes.
    \param[in] volume Volume level.
    \param[in] data User pointer that was set through MLMusicServiceSetCallbacks().
  */
  void (*on_volume_change)(float volume, void *data);
} MLMusicServiceCallbacks;

/*!
  \brief Connects to MusicService.

  \param[in] name Name of the MusicService provider (defined as the visible_name in the manifest).
                  NULL terminated ASCII character sequence.

  \retval MLResult_AllocFailed Connection failed with resource allocation failure.
  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Connected to MusicService successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_AlreadyExists Connection exists already.

  \priv ConnectBackgroundMusicService, MusicService
 */
ML_API MLResult ML_CALL MLMusicServiceConnect(const char *name);

/*!
  \brief Disconnects from MusicService.

  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
 */
ML_API MLResult ML_CALL MLMusicServiceDisconnect();

/*!
  \brief Sets the client-implemented callbacks to receive information from MusicService.
         Client needs to implement the callbacks defined by MLMusicServiceCallbacks.
         The library passes the MusicService info to the client via those callbacks.

  \param[in] callbacks Client-implemented callbacks.
  \param[in] data User metadata.

  \retval MLResult_Ok Callbacks set successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceSetCallbacks(MLMusicServiceCallbacks *callbacks, void *data);

/*!
  \brief Sets the authentication string on the MusicService.

  \param[in] auth_string The authentication string.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Authentication string set successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
 */
ML_API MLResult ML_CALL MLMusicServiceSetAuthString(const char *auth_string);

/*!
  \brief Sets a specified URL.

  \param[in] url The URL to play.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok URL set successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceSetURL(const char *url);

/*!
  \brief Sets the the playlist on the MusicService.

  \param[in] play_list List of URLs to play.
  \param[in] size Number of URLs in the list.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Play list set successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceSetPlayList(const char **play_list, uint64_t size);

/*!
  \brief Starts playing the current track.

  \retval MLResult_Ok Playback started successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceStart();

/*!
  \brief Stops playing the current track.

  \retval MLResult_Ok Playback stopped successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceStop();

/*!
  \brief Pauses the current track.

  \retval MLResult_Ok Playback resumed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServicePause();

/*!
  \brief Resumes playing the current track.

  \retval MLResult_Ok Playback resumed successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceResume();

/*!
  \brief Seeks to the desired position.
  \param[in] position The position(in milliseconds) to seek to

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Playback seeked successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceSeek(uint32_t position);

/*!
  \brief Advances to the next track in the playlist.

  \retval MLResult_Ok Next track is selected successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceNext();

/*!
  \brief Rewinds to the previous track in the playlist.

  \retval MLResult_Ok Previous track is selected successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServicePrevious();

/*!
  \brief Sets the shuffle mode.

  \param[in] mode Shuffle mode On/Off switch.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Playlist shuffling is set successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceSetShuffle(MLMusicServiceShuffleState mode);

/*!
  \brief Sets the repeat mode.

  \param[in] mode Repeat mode options.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Playlist repeat is set successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceSetRepeat(MLMusicServiceRepeatState mode);

/*!
  \brief Sets the volume.

  \param[in] volume The new volume in the [0.0, 1.0] range.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok volume is set successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceSetVolume(float volume);

/*!
  \brief Obtains the length of the current track.

  \param[out] out_length Length of the track in seconds.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Track duration is retrieved successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceGetTrackLength(uint32_t *out_length);

/*!
  \brief Obtains the position of the current track.

  \param[out] out_position Position in milliseconds.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Current Playback position is retrieved successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceGetCurrentPosition(uint32_t *out_position);

/*!
  \brief Obtains the MusicService status.

  \param[out] out_status MusicService status.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Current status is retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceGetStatus(MLMusicServiceStatus *out_status);

/*!
  \brief Obtains the error from MusicService.
  \param[out] out_type Error type.
  \param[out] out_code Error code.
              For error types other than MLMusicServiceError_ServiceSpecific,
              the value of out_code uses the same enumeration as out_type.
              For MLMusicServiceError_ServiceSpecific, the out_code is known only
              between the music streaming service and the application, therefore
              the values are not documented in this file.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Current error is retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceGetError(MLMusicServiceError *out_type, int32_t *out_code);

/*!
  \brief Polls the state of playback from MusicService.

  \param[out] out_state Playback state.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Current playback state is retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceGetPlaybackState(MLMusicServicePlaybackState *out_state);

/*!
  \brief Polls the state of repeat setting from MusicService.

  \param[out] out_state Repeat setting.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Current repeat state is retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceGetRepeatState(MLMusicServiceRepeatState *out_state);

/*!
  \brief Polls the state of shuffle setting from MusicService.

  \param[out] out_state shuffle setting.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Current shuffle state is retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceGetShuffleState(MLMusicServiceShuffleState *out_state);

/*!
  \brief Polls the volume level of MusicService Provider.

  \param[out] out_volume Volume level.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Current volume level is retrieved successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceGetVolume(float *out_volume);

/*!
  \deprecated Deprecated since 0.20.0. Scheduled for removal. Replaced by \ref MLMusicServiceGetMetadataForIndex.

  \brief Polls the metadata from MusicService for the current track.
         The library maintains the lifetime of the data. Call MLMusicServiceReleaseMetadata()
         to release them.

  \param[out] out_metadata Metadata received.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Current tracks's metadata is retrieved successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_DEPRECATED_MSG("Replaced by MLMusicServiceGetMetadataForIndex.")
ML_API MLResult ML_CALL MLMusicServiceGetMetadata(MLMusicServiceMetadata *out_metadata);

/*!
  \deprecated Deprecated since 0.21.0. Scheduled for removal. Replaced by \ref MLMusicServiceGetMetadataForIndex.

  \brief Polls the metadata from MusicService for the previous/current/next track.
         The library maintains the lifetime of the data. Call MLMusicServiceReleaseMetadata()
         to release them.

  \apilevel 4

  \param[in] track_type Track enumeration for previous/current/next track.
  \param[out] out_metadata Metadata received.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Tracks's metadata is retrieved successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService

*/
ML_DEPRECATED_MSG("Replaced by MLMusicServiceGetMetadataForIndex.")
ML_API MLResult ML_CALL MLMusicServiceGetMetadataEx(MLMusicServiceTrackType track_type, MLMusicServiceMetadata *out_metadata);

/*!
  \brief Get Metadata for the track at relative position to the current track.
         The library maintains the lifetime of the data. Call MLMusicServiceReleaseMetadata()
         to release them.

  \apilevel 5

  \param[in] track_index Relative index of the track for which to retrieve metadata
  \param[out] out_metadata Metadata to be returned

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Metadata was returned successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceGetMetadataForIndex(int32_t track_index, MLMusicServiceMetadata *out_metadata);

/*!
  \brief Releases the metadata.

  \param[in] metadata Metadata obtained by MLMusicServiceGetMetadata().

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Metadata object is released successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceReleaseMetadata(MLMusicServiceMetadata *metadata);

/*! \} */

ML_EXTERN_C_END
