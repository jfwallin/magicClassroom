// %BANNER_BEGIN%
// ---------------------------------------------------------------------
// %COPYRIGHT_BEGIN%
//
// Copyright (c) 2018 Magic Leap, Inc. (COMPANY) All Rights Reserved.
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
#include "ml_music_service_common.h"

ML_EXTERN_C_BEGIN

/*!
  \defgroup Music Service
  \addtogroup MusicService
  \sharedobject ml_musicservice_provider
  \brief APIs for implementing background music service as provider compoment.

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.
  \{
*/

/*!
  \deprecated Deprecated since 0.21.0. Scheduled for removal. Replaced by #MLMusicServiceProviderImplementationEx.
  \brief List of music service provider APIs. User will need provide implementation for them.
*/
typedef ML_DEPRECATED_MSG("Replaced by MLMusicServiceProviderImplementationEx.")
struct MLMusicServiceProviderImplementation {
  /*!
    \brief Set the authentication token.
    \param[in] auth_string auth_string is used for session management.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_auth_string)(const char *auth_string, void *callback_context);

  /*!
    \brief Set an URL for the playback.
    \param[in] url URL of the media to be played back.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_url)(const char *url, void *callback_context);

  /*!
    \brief Set a list of URLs.
    \param[in] list List of URLs.
    \param[in] count Number of elements in the list.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_play_list)(const char **list, size_t count, void *callback_context);

  /*!
    \brief Start playback.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*start)(void *callback_context);

  /*!
    \brief Stop playback.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*stop)(void *callback_context);

  /*!
    \brief Pause playback.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*pause)(void *callback_context);

  /*!
    \brief Resume playback.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*resume)(void *callback_context);

  /*!
    \brief Seek playback to a position.
    \param[in] position Seek position in milli second resolution.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*seek)(uint32_t position, void *callback_context);

  /*!
    \brief Play next track in the playlist.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*next)(void *callback_context);

  /*!
    \brief Play previous track in the playlist.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*previous)(void *callback_context);

  /*!
    \brief Set shuffle mode.
    \param[in] mode Shuffle mode.
    \param[in] callback_context User pointer that was set through #MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_shuffle)(MLMusicServiceShuffleState mode, void *callback_context);

  /*!
    \brief Set repeat mode.
    \param[in] mode Repeat mode.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_repeat)(MLMusicServiceRepeatState mode, void *callback_context);

  /*!
    \brief Set volume.
    \param[in] volume Volume level.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_volume)(float volume, void *callback_context);

  /*!
    \brief Get length of current track.
    \param[out] out_length Length of current track.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*get_track_length)(uint32_t *out_length, void *callback_context);

  /*!
    \brief Get current position of the playback.
    \param[out] out_position Current position of the playback.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*get_current_position)(uint32_t *out_position, void *callback_context);

  /*!
    \brief Get Metadata for the prev/current/next media track.
    \param[in] track Enumaration for previous/current/next track.
    \param[out] out_metadata Metadata of current track.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*get_metadata)(MLMusicServiceTrackType track, MLMusicServiceMetadata *out_metadata, void *callback_context);

  /*!
    \brief Terminate the music provider service.
           After this call, the music service provider process will be terminated.
           This function should stop all the activities, and clean up all the resources
           to enure the gracefully termination of this process.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreate().
  */
  void (*on_end_service)(void *callback_context);

  /*!
    \brief User pointer that will be passed through the callback functions when fired.
  */
  void *callback_context;
} MLMusicServiceProviderImplementation;

/*!
  \brief Music Service provider callback functions.

  This structure must be initialized by calling MLMusicServiceProviderImplementationExInit() before use.

  \apilevel 5
*/
typedef struct MLMusicServiceProviderImplementationEx {
  uint32_t version;
  /*!
    \brief Set the authentication token.

    \param[in] auth_string auth_string is used for session management.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_auth_string)(const char *auth_string, void *callback_context);

  /*!
    \brief Set an URL for the playback.

    \param[in] url URL of the media to be played back.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_url)(const char *url, void *callback_context);

  /*!
    \brief Set a list of URLs.

    \param[in] list List of URLs.
    \param[in] count Number of elements in the list.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_play_list)(const char **list, size_t count, void *callback_context);

  /*!
    \brief Start playback.

    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*start)(void *callback_context);

  /*!
    \brief Stop playback.

    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*stop)(void *callback_context);

  /*!
    \brief Pause playback.

    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*pause)(void *callback_context);

  /*!
    \brief Resume playback.

    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*resume)(void *callback_context);

  /*!
    \brief Seek playback to a position.

    \param[in] position Seek position in milli second resolution.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*seek)(uint32_t position, void *callback_context);

  /*!
    \brief Play next track in the playlist.

    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*next)(void *callback_context);

  /*!
    \brief Play previous track in the playlist.

    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*previous)(void *callback_context);

  /*!
    \brief Set shuffle mode.

    \param[in] mode Shuffle mode.
    \param[in] callback_context User pointer that was set through #MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_shuffle)(MLMusicServiceShuffleState mode, void *callback_context);

  /*!
    \brief Set repeat mode.

    \param[in] mode Repeat mode.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_repeat)(MLMusicServiceRepeatState mode, void *callback_context);

  /*!
    \brief Set volume.

    \param[in] volume Volume level.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*set_volume)(float volume, void *callback_context);

  /*!
    \brief Get length of current track.

    \param[out] out_length Length of current track.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*get_track_length)(uint32_t *out_length, void *callback_context);

  /*!
    \brief Get current position of the playback.

    \param[out] out_position Current position of the playback.
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*get_current_position)(uint32_t *out_position, void *callback_context);

  /*!
    \brief Get Metadata for the media at relative position to the media track.

    \param[in] track_index Relative index of the track for which to retrieve metadata.
    \param[out] out_metadata a valid pointer to copy the music metadata
    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().

    \return MLResult_OK for success or one of the MLResult error code for failure.
  */
  MLResult (*get_metadata_for_index)(int32_t track_index, MLMusicServiceMetadata *out_metadata, void *callback_context);

  /*!
    \brief Terminate the music provider service.

    After this call, the music service provider process will be terminated.
    This function should stop all the activities, and clean up the resources
    to ensure the graceful termination of this process.

    \param[in] callback_context User pointer that was set through MLMusicServiceProviderCreateEx().
  */
  void (*on_end_service)(void *callback_context);

  /*!
    \brief Context pointer from the user that will be passed to callback functions.
  */
  void *callback_context;
} MLMusicServiceProviderImplementationEx;

/*!
  \brief Initializes MLMusicServiceProviderImplementationEx with default values.
  \apilevel 5

  \param[in,out] inout_callbacks ML Music Service Provider callbacks structure defined by
  #MLMusicServiceProviderImplementationEx that needs to be initialized.
*/
ML_STATIC_INLINE void MLMusicServiceProviderImplementationExInit(MLMusicServiceProviderImplementationEx *inout_callbacks) {
  if (inout_callbacks) {
    inout_callbacks->version = 1;
    inout_callbacks->set_auth_string = NULL;
    inout_callbacks->set_url = NULL;
    inout_callbacks->set_play_list = NULL;
    inout_callbacks->start = NULL;
    inout_callbacks->stop = NULL;
    inout_callbacks->pause = NULL;
    inout_callbacks->resume = NULL;
    inout_callbacks->seek = NULL;
    inout_callbacks->next = NULL;
    inout_callbacks->previous = NULL;
    inout_callbacks->set_shuffle = NULL;
    inout_callbacks->set_repeat = NULL;
    inout_callbacks->set_volume = NULL;
    inout_callbacks->get_track_length = NULL;
    inout_callbacks->get_current_position = NULL;
    inout_callbacks->get_metadata_for_index = NULL;
    inout_callbacks->on_end_service = NULL;
    inout_callbacks->callback_context = NULL;
  }
}

/*!
  \deprecated Deprecated since 0.21.0. Scheduled for removal. Replaced by \ref MLMusicServiceGetMetadataEx.

  \brief Create music service provider instance.

  \param[in] implementation Implementations provided by the client

  \retval MLResult_AllocFailed Creation failed with resource allocation failure.
  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Created Provider to MusicService successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv MusicService
*/
ML_DEPRECATED_MSG("Replaced by MLMusicServiceProviderCreateEx.")
ML_API MLResult ML_CALL MLMusicServiceProviderCreate(const MLMusicServiceProviderImplementation *implementation);

/*!
  \brief Create music service provider instance.
  \apilevel 5

  \param[in] implementation Music Service Provider Implementation.

  \retval MLResult_AllocFailed Creation failed with resource allocation failure.
  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Created Provider to MusicService successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceProviderCreateEx(const MLMusicServiceProviderImplementationEx *implementation);

/*!
  \brief Register with the background music service and start the music service provider.

  \param[in] music_service_name Name of the Music Service provider. This name should match
             the "ml:visible_name" in the manifest.xml of the C version of the Music
             Service provider component.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv RegisterBackgroundMusicService, MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceProviderStart(const char *music_service_name);

/*!
  \brief Configure the audio output to which the audio will be rendered.

  \param[in] sample_rate Sampling rate of the output channel(s).
  \param[in] channel_count Number of output channels.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceProviderSetAudioOutput(uint32_t sample_rate, uint32_t channel_count);

/*!
  \brief Write audio buffer to the output port.

  \param[in] buffer Audio data.
  \param[in] buffer_size Size of audio buffer.
  \param[in] is_sync \c true for synchronous operation and \c false otherwise.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceProviderWriteAudioOutput(const void *buffer, size_t buffer_size, bool is_sync);

/*!
  \brief Flush audio buffers.

  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceProviderFlushAudioOutput();

/*!
  \brief Release audio output port.

  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceProviderReleaseAudioOutput();

/*!
  \brief Set volume.

  \param[in] volume Volume to set.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_PrivilegeDenied Necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv MusicService
*/
ML_API MLResult ML_CALL MLMusicServiceProviderSetVolume(float volume);

/*!
  \brief Notify the music service UI/App about position change.

  \param[in] position New track position.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceProviderNotifyPositionChange(uint32_t position);

/*!
  \brief Notify the music service UI/App about the playback state change.

  \param[in] state New playback state.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceProviderNotifyPlaybackStateChange(MLMusicServicePlaybackState state);

/*!
  \brief Notify the music service UI/App about the repeat state change.

  \param[in] state New repeat state.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceProviderNotifyRepeatStateChange(MLMusicServiceRepeatState state);

/*!
  \brief Notify the music service UI/App about the shuffle state change.

  \param[in] state New shuffle state.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceProviderNotifyShuffleStateChange(MLMusicServiceShuffleState state);

/*!
  \brief Notify the music service UI/App about provider's status update.

  \param[in] status New music service provider status.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceProviderNotifyStatus(MLMusicServiceStatus status);

/*!
  \brief Notify the music service UI/App about the errors.

  \param[in] type Error type.
  \param[in] code Error code.
             For error types other than MLMusicServiceError_ServiceSpecific,
             the value of 'code' uses the same enumeration as 'type'.
             For MLMusicServiceError_ServiceSpecific, the 'code' is known only
             between the music service provider and the application, therefore
             the values are not documented in this file.

  \retval MLResult_InvalidParam One of the parameters is invalid.
  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceProviderNotifyError(MLMusicServiceError type, int32_t code);

/*!
  \brief Notify the music service UI/App about the metadata change.

  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceProviderNotifyMetadataChange();

/*!
  \brief Notify the music service UI/App about the volume change.

  \param[in] volume Volume level.

  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceProviderNotifyVolumeChange(float volume);

/*!
  \brief Terminates music service provider process.

  \retval MLResult_Ok Disconnected from MusicService successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Music service wasn't connected.

  \priv None
*/
ML_API MLResult ML_CALL MLMusicServiceProviderTerminate();

/*! \} */

ML_EXTERN_C_END
