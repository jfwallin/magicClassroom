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
#include "ml_media_common.h"
#include "ml_media_data_source.h"
#include "ml_media_error.h"
#include "ml_media_stream_source.h"
#include "ml_types.h"
#include "ml_media_types.h"
#include "ml_media_drm.h"
#include "ml_media_webvtt.h"
#include <stddef.h>

ML_EXTERN_C_BEGIN

/*!
  \defgroup MediaPlayer Media Player
  \addtogroup MediaPlayer
  \sharedobject ml_mediaplayer
  \brief APIs for high level management of media playback.

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.

  \{
*/

/*! Codes for setting video scaling mode. */
typedef enum MLMediaPlayerVideoScalingMode {
  MLMediaPlayerVideoScalingMode_ScaleToFit = 1,
  MLMediaPlayerVideoScalingMode_ScaleToFitWithCropping = 2,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaPlayerVideoScalingMode_Ensure32Bits = 0x7FFFFFFF
} MLMediaPlayerVideoScalingMode;

/*! The state to be polled */
typedef enum MLMediaPlayerPollingStateFlags {
  /*! Playing. */
  MLMediaPlayerPollingStateFlag_IsPlaying            = 1 << 0,
  /*! Looping. */
  MLMediaPlayerPollingStateFlag_IsLooping            = 1 << 1,
  /*! Buffer available. */
  MLMediaPlayerPollingStateFlag_IsBufferAvailable    = 1 << 2,
  /*! Prepared. */
  MLMediaPlayerPollingStateFlag_HasBeenPrepared      = 1 << 3,
  /*! Playback completed. */
  MLMediaPlayerPollingStateFlag_HasPlaybackCompleted = 1 << 4,
  /*! Seek completed. */
  MLMediaPlayerPollingStateFlag_HasSeekCompleted     = 1 << 5,
  /*! Size changed. */
  MLMediaPlayerPollingStateFlag_HasSizeChanged       = 1 << 6,
  /*! Buffering updated. */
  MLMediaPlayerPollingStateFlag_HasBufferingUpdated  = 1 << 7,
  /*! Received info. */
  MLMediaPlayerPollingStateFlag_HasReceivedInfo      = 1 << 8,
  /*! Recieved error. */
  MLMediaPlayerPollingStateFlag_HasReceivedError     = 1 << 9,
  /*! Timed text updated. */
  MLMediaPlayerPollingStateFlag_HasTimedTextUpdated  = 1 << 10,
  /*! Subtitle updated. */
  MLMediaPlayerPollingStateFlag_HasSubtitleUpdated   = 1 << 11,
  /*! Metadata updated. */
  MLMediaPlayerPollingStateFlag_HasMetadataUpdated   = 1 << 12,
  /*! DRM info updated. */
  MLMediaPlayerPollingStateFlag_HasDrmInfoUpdated    = 1 << 13,
  /*! Async Reset completed. */
  MLMediaPlayerPollingStateFlag_HasResetCompleted    = 1 << 14,
} MLMediaPlayerPollingStateFlags;

/*!
  \brief Info and warning codes for the media player framework.

  These are non fatal, the playback is going on but there might be some user visible issues.

  Info and warning messages are communicated back to the client using the on_info callback.
  In this situation, this method is invoked with the following:

  'what'  should be a value from the MLMediaPlayerInfo.
  'extra' contains an implementation dependent info code to provide more details.
  Should default to 0 when not used.
*/
typedef enum MLMediaPlayerInfo {
  /*! Unknown */
  MLMediaPlayerInfo_Unknown           = 1,
  /*! The player was started because it was used as the next player. */
  MLMediaPlayerInfo_StartedAsNext     = 2,
  /*! The player just pushed the very first video frame for rendering. */
  MLMediaPlayerInfo_RenderingStart    = 3,
  /*! The player just reached EOS and started from beginning (loop). */
  MLMediaPlayerInfo_Looping           = 4,
  /*! The player acknowledgement that it has started playing. */
  MLMediaPlayerInfo_Started           = 5,
  /*! The player acknowledgement that it has paused. */
  MLMediaPlayerInfo_Paused            = 6,
  /*! The player acknowledgement that it has stopped playing. */
  MLMediaPlayerInfo_Stopped           = 7,
  /*! The player acknowledgement that it has started playing as result of shared player's request. */
  MLMediaPlayerInfo_StartedBySharedPlayer = 8,
  /*! The player acknowledgement that it has paused playing as result of shared player's request. */
  MLMediaPlayerInfo_PausedBySharedPlayer  = 9,
  /*! The player acknowledgement that it is seeking as result of shared player's request. */
  MLMediaPlayerInfo_SeekBySharedPlayer    = 10,
  /*! The player acknowledgement that it has stopped playing as result of shared player's request. */
  MLMediaPlayerInfo_StoppedBySharedPlayer = 11,
  /*! The player has started sync'ing with other shared players. */
  MLMediaPlayerInfo_SyncStart = 12,
  /*! The player has completed sync'ing with other shared players. */
  MLMediaPlayerInfo_SyncComplete = 13,
  /*! The video is too complex for the decoder, it can't decode frames fast enough. */
  MLMediaPlayerInfo_VideoTrackLagging = 700,
  /*! Media player is temporarily pausing playback. */
  MLMediaPlayerInfo_BufferingStart    = 701,
  /*! Media player is resuming playback after filling buffers. */
  MLMediaPlayerInfo_BufferingEnd      = 702,
  /*! Network bandwidth info. */
  MLMediaPlayerInfo_NetworkBandwidth  = 703,
  /*! Dash Live stream is Behind Live Window. */
  MLMediaPlayerInfo_BehindLiveWindow  = 704,
  /*! Playback Paused because of Power State Transition. */
  MLMediaPlayerInfo_PowerStatePause   = 705,
  /*! Playback Resumed because of Power State Transition. */
  MLMediaPlayerInfo_PowerStateResume  = 706,
  /*! Duration in milliseconds of buffered content. */
  MLMediaPlayerInfo_BufferedDuration  = 707,
  /*! Bad interleaving means that a media has been improperly interleaved. */
  MLMediaPlayerInfo_BadInterleaving   = 800,
  /*! The media is not seekable (e.g live stream). */
  MLMediaPlayerInfo_NotSeekable       = 801,
  /*! New media metadata is available. */
  MLMediaPlayerInfo_MetadataUpdate    = 802,
  /*! Media timed text error. */
  MLMediaPlayerInfo_TimedTextError    = 900,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaPlayerInfo_Ensure32Bits      = 0x7FFFFFFF
} MLMediaPlayerInfo;

/*! MediaTrack types returned by MLMediaPlayerGetTrackType(). */
typedef enum MLMediaPlayerTrackType {
  /*! Unknown. */
  MediaPlayerTrackType_Unknown      = 0,
  /*! Video. */
  MediaPlayerTrackType_Video        = 1,
  /*! Audio. */
  MediaPlayerTrackType_Audio        = 2,
  /*! Timed text. */
  MediaPlayerTrackType_TimedText    = 3,
  /*! Subtitle. */
  MediaPlayerTrackType_Subtitle     = 4,
  /*! Metadata. */
  MediaPlayerTrackType_Metadata     = 5,
  /*! Ensure enum is represented as 32 bits. */
  MediaPlayerTrackType_Ensure32Bits = 0x7FFFFFFF
} MLMediaPlayerTrackType;

typedef MLPSSHEntry MLMediaPlayerPSSHEntry;

/*!
  \deprecated Deprecated since 0.17.0. Scheduled for removal. Replaced by MLMediaPlayerTrackDRMInfo.

  \brief MediaPlayer DRM Info.
*/
typedef ML_DEPRECATED_MSG("Replaced by MLMediaPlayerTrackDRMInfo.") struct MLMediaPlayerDRMInfo {
  /*! Number of PSSH entries. */
  size_t pssh_info_count;
  /*! Pointer to array of #MLMediaPlayerPSSHEntry of size pssh_info_count. */
  MLMediaPlayerPSSHEntry *pssh_info;
  /*! Number of supported DRM UUID entries. */
  size_t uuid_count;
  /*! Pointer to array of #MLUUID of size uuid_count. */
  MLUUID *UUIDs;
} MLMediaPlayerDRMInfo;

/*!
  \brief MediaPlayer DRM Info for a Media Track.

  \apilevel 2
*/
typedef struct MLMediaPlayerTrackDRMInfo {
  /*! Number of PSSH entries. */
  size_t pssh_info_count;
  /*! Pointer to array of #MLMediaPlayerPSSHEntry of size pssh_info_count. */
  MLMediaPlayerPSSHEntry *pssh_info;
  /*! Number of supported DRM UUID entries. */
  size_t uuid_count;
  /*! Pointer to array of #MLUUID of size uuid_count. */
  MLUUID *UUIDs;
  /*! Media track type, can be either audio or video. */
  MLMediaPlayerTrackType track_type;
} MLMediaPlayerTrackDRMInfo;

/*!
  \deprecated Deprecated since 0.20.0. Scheduled for removal. Please use ml_media_format.h to get subtitle mime type.

  \brief MediaPlayer subtitle format.
 */
typedef enum MLMediaPlayerSubtitleFormat {
  /*! CEA-608. */
  MLMediaPlayerSubtitleFormat_Cea608 = 0,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaPlayerSubtitleFormat_Ensure32Bits = 0x7FFFFFFF
} MLMediaPlayerSubtitleFormat;

/*! MediaPlayer subtitle data. */
typedef struct MLMediaPlayerSubtitleData {
  /*!
    \deprecated Deprecated since 0.20.0. Scheduled for removal. Please use ml_media_format.h to get subtitle mime type.
  */
  MLMediaPlayerSubtitleFormat format;
  uint32_t track_index;
  uint64_t start_time_us;
  uint64_t duration_us;
  uint32_t data_size;
  uint8_t *data;
} MLMediaPlayerSubtitleData;

/*! Indicate the mode of buffering setttings. */
typedef enum MLMediaPlayerBufferingMode {
  /*! Do not support buffering. */
  MLMediaPlayerBufferingMode_None         = 0,
  /*! Support only time based buffering. */
  MLMediaPlayerBufferingMode_TimeOnly     = 1,
  /*! Support only size based buffering. */
  MLMediaPlayerBufferingMode_SizeOnly     = 2,
  /*!
    \brief Support both time and size based buffering, time based calculation precedes
    size based. Size based calculation will be used only when time information is not
    available for the stream.
  */
  MLMediaPlayerBufferingMode_TimeThenSize = 3,
  /*! Number of modes. */
  MLMediaPlayerBufferingMode_Count        = 4,
  /*! Ensure enum size is 32 bits. */
  MLMediaPlayerBufferingMode_Ensure32Bits = 0x7FFFFFFF
} MLMediaPlayerBufferingMode;

/*! Indicate the buffering setttings. */
typedef struct MLMediaPlayerBufferingSettings {
    /*! For prepare. */
    MLMediaPlayerBufferingMode initial_buffering_mode;
    /*! For playback. */
    MLMediaPlayerBufferingMode rebuffering_mode;
    /*! Time based. */
    int32_t initial_watermark_ms;
    /*! Size based. */
    int32_t initial_watermark_kb;
    /*!
      \brief When cached data is below this mark, playback will be paused for buffering
      until data reaches |mRebufferingWatermarkHighMs| or end of stream.
    */
    int32_t rebuffering_watermark_low_ms;
    /*! When cached data is above this mark, buffering will be paused. */
    int32_t rebuffering_watermark_high_ms;
    /*!
      \brief When cached data is below this mark, playback will be paused for buffering
      until data reaches |mRebufferingWatermarkHighKB| or end of stream. */
    int32_t rebuffering_watermark_low_kb;
    /*! When cached data is above this mark, buffering will be paused. */
    int32_t rebuffering_watermark_high_kb;
} MLMediaPlayerBufferingSettings;

/*!
  \brief MediaShared types to be set in MLMediaPlayerSetSharingInfo().

  \apilevel 8
*/
typedef enum MLMediaPlayerSharedType {
  /*! Unknown. */
  MLMediaPlayerSharedType_Unknown   = -1,
  /*! Stop sharing if being shared. */
  MLMediaPlayerSharedType_None      = 0,
  /*! Share as initiator. */
  MLMediaPlayerSharedType_Initiator = 1,
  /*! Share as follower. */
  MLMediaPlayerSharedType_Follower  = 2,
  /*! Ensure enum is represented as 32 bits. */
  MLMediaPlayerSharedType_Ensure32Bits = 0x7FFFFFFF
} MLMediaPlayerSharedType;

/*!
  \deprecated Deprecated since 0.17.0. Scheduled for removal. Replaced by MLMediaPlayerEventCallbacksEx.

  \brief Callbacks for notifying client about \ref MediaPlayer error events.
  Those are async mechanisms that should not be used in parallel with their
  sync counterpart, i.e. poll-based queries for the same events using
  MLMediaPlayerPollStatespoll().
*/
typedef ML_DEPRECATED_MSG("Replaced by MLMediaPlayerEventCallbacksEx.") struct MLMediaPlayerEventCallbacks {
    /*!
      \brief This callback function is invoked when buffered contents percentage changed.
      \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
      \param[in] percent The new percentage of buffered content.
      \param[in] data User data as passed to MLMediaPlayerSetEventCallbacks().
    */
    void (*on_buffering_update)(MLHandle media_player, int percent, void *data);
    /*!
      \brief This callback function is invoked when media player played back until end of
      media and has now come to a stop.

      \note This callback does not fire when 'looping = true',
      because \ref MediaPlayer does not "stop" in that case, but rather
      loops to beginning of media.

      To get notified when EOS is reached (when looping = true),
      set a proper callback function for the below on_info and look for the
      #MEDIA_PLAYER_INFO_LOOPING info code.

      \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
      \param[in] data User data as passed to MLMediaPlayerSetEventCallbacks().
    */
    void (*on_completion)(MLHandle media_player, void *data);
    /*!
      \brief This callback function is invoked when media player encounters an error.
      \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
      \param[in] result error/result code indicating failure reason.
      \param[in] data User data as passed to MLMediaPlayerSetEventCallbacks().
    */
    void (*on_error)(MLHandle media_player, MLResult result, void *data);
    /*!
      \brief This callback function is invoked when \ref MediaPlayer generates informational events.
      \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
      \param[in] info Type of informational event.
      \param[in] extra MLMediaPlayerInfo type specific extra information.
                 When info is MLMediaPlayerInfo_NetworkBandwidth, this holds bandwidth
                 in kbps. It is 0 for others.
      \param[in] data User data as passed to MLMediaPlayerSetEventCallbacks().
    */
    void (*on_info)(MLHandle media_player, MLMediaPlayerInfo info, int extra, void *data);
    /*!
      \brief This callback function is invoked when the player has finished preparing
      media and is ready to playback.
      \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
      \param[in] data User data as passed to MLMediaPlayerSetEventCallbacks().
    */
    void (*on_prepared)(MLHandle media_player, void *data);
    /*!
      \brief This callback function is invoked when a seek operation has completed.
      \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
      \param[in] data User data as passed to MLMediaPlayerSetEventCallbacks().
    */
    void (*on_seek_complete)(MLHandle media_player, void *data);
    /*!
      \brief This callback function is invoked when the internal surface has changed size.
      \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
      \param[in] width The new surface width.
      \param[in] height The new surface height.
      \param[in] data User data as passed to MLMediaPlayerSetEventCallbacks().
    */
    void (*on_video_size_changed)(MLHandle media_player, int width, int height, void *data);
    /*!
      \brief This callback function is invoked when source is DRM protected.
      \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
      \param[in] pointer to MLMediaPlayerDRMInfo.
    */
    void (*on_drm_info)(MLHandle media_player, struct MLMediaPlayerDRMInfo *data);
} MLMediaPlayerEventCallbacks;

/*!
  \brief MediaPlayer Buffering update callback Info.

  \apilevel 2
*/
typedef struct MLMediaPlayerOnBufferingUpdateInfo {
  /*! The new percentage of buffered content. */
  int percent;
  /*! User data as passed to MLMediaPlayerSetEventCallbacksEx(). */
  void *data;
} MLMediaPlayerOnBufferingUpdateInfo;

/*!
  \brief MediaPlayer Playback completion callback Info.

  \apilevel 2
*/
typedef struct MLMediaPlayerOnCompletionInfo {
  /*! User data as passed to MLMediaPlayerSetEventCallbacksEx(). */
  void *data;
} MLMediaPlayerOnCompletionInfo;

/*!
  \brief MediaPlayer Error callback Info.

  \apilevel 2
*/
typedef struct MLMediaPlayerOnErrorInfo {
  /*! Error/result code indicating failure reason. */
  MLResult result;
  /*! User data as passed to MLMediaPlayerSetEventCallbacksEx(). */
  void *data;
} MLMediaPlayerOnErrorInfo;

/*!
  \brief MediaPlayer Information callback Info.

  \apilevel 2
*/
typedef struct MLMediaPlayerOnInfoInfo {
  /*! Type of informational event. */
  MLMediaPlayerInfo info;
  /*!
    \brief MLMediaPlayerInfo type specific extra information.

    When info is MLMediaPlayerInfo_NetworkBandwidth, this holds bandwidth in kbps. It is 0 for others.
  */
  int extra;
  /*! User data as passed to MLMediaPlayerSetEventCallbacksEx(). */
  void *data;
} MLMediaPlayerOnInfoInfo;

/*!
  \brief MediaPlayer Playback prepared callback Info.

  \apilevel 2
*/
typedef struct MLMediaPlayerOnPreparedInfo {
  /*! User data as passed to MLMediaPlayerSetEventCallbacksEx(). */
  void *data;
} MLMediaPlayerOnPreparedInfo;

/*!
  \brief MediaPlayer Playback seek completion callback Info.

  \apilevel 2
*/
typedef struct MLMediaPlayerOnSeekCompleteInfo {
  /*! User data as passed to MLMediaPlayerSetEventCallbacksEx(). */
  void *data;
} MLMediaPlayerOnSeekCompleteInfo;

/*!
  \brief MediaPlayer Video Size Changed callback Info.

  \apilevel 2
*/
typedef struct MLMediaPlayerOnVideoSizeChangedInfo {
  /*! The new surface width. */
  int width;
  /*! The new surface height. */
  int height;
  /*! User data as passed to MLMediaPlayerSetEventCallbacksEx(). */
  void *data;
} MLMediaPlayerOnVideoSizeChangedInfo;

/*!
  \brief MediaPlayer Track DRM Info callback Info.

  \apilevel 2
*/
typedef struct MLMediaPlayerOnTrackDRMInfo {
  /*! Pointer to MLMediaPlayerTrackDRMInfo. */
  MLMediaPlayerTrackDRMInfo *track_info;
  /*! User data as passed to MLMediaPlayerSetEventCallbacksEx(). */
  void* data;
} MLMediaPlayerOnTrackDRMInfo;

/*!
  \brief MediaPlayer reset completion callback Info.

  \apilevel 8
*/
typedef struct MLMediaPlayerOnResetCompleteInfo {
  /*!
    \brief User data as passed to MLMediaPlayerSetEventCallbacksEx().

    \apilevel 8
  */
  void *data;
} MLMediaPlayerOnResetCompleteInfo;

/*!
  \brief Frame Packing Arrangement for stereoscopic videos.

  \apilevel 8
*/
typedef enum {
  /*! Not a stereoscopic video. */
  MLMediaFramePackingMode_None = 0,
  /*! Checkboard. */
  MLMediaFramePackingMode_Checkboard,
  /*! Column interleaved. */
  MLMediaFramePackingMode_ColumnInterleaved,
  /*! Row interleaved. */
  MLMediaFramePackingMode_RowInterleaved,
  /*! Side by side quincunx. */
  MLMediaFramePackingMode_SideBySideQuincunx,
  /*! Side by side. */
  MLMediaFramePackingMode_SideBySide,
  /*! Top bottom. */
  MLMediaFramePackingMode_TopBottom,
  /*! Multiview frame by frame. */
  MLMediaFramePackingMode_MultiviewFrameByFrame,
  /*! Frame by frame. */
  MLMediaFramePackingMode_FrameByFrame,
  /*! Anaglyph. */
  MLMediaFramePackingMode_Anaglyph,
} MLMediaFramePackingMode;

/*!
  \brief Frame Packing Arrangement flags for stereoscopic videos.

  \apilevel 8
*/
typedef enum {
  /*! No flags. */
  MLMediaFramePackingFlags_None = 0,
  /*! Right view first. */
  MLMediaFramePackingFlags_RightViewFirst = 1 << 0,
  /*! Left view flipped. */
  MLMediaFramePackingFlags_LeftFlipped = 1 << 1,
  /*! Right view flipped. */
  MLMediaFramePackingFlags_RightFlipped = 1 << 2,
  /*! Anaglyph red/cyan. */
  MLMediaFramePackingFlags_Anaglyph_RedCyan = 1 << 3,
  /*! Anaglyph green/magenta. */
  MLMediaFramePackingFlags_Anaglyph_GreenMagenta = 1 << 4,
} MLMediaFramePackingFlags;

/*!
  \brief MediaPlayer frame packing callback Info.

  \apilevel 8
*/
typedef struct MLMediaPlayerOnFramePackingInfo {
  /*! Frame packing more. */
  MLMediaFramePackingMode mode;
  /*! Frame packing flag. */
  MLMediaFramePackingFlags flags;
  /*! User data as passed to MLMediaPlayerSetEventCallbacksEx(). */
  void *data;
} MLMediaPlayerOnFramePackingInfo;

/*!
  \brief Callbacks for notifying client about \ref MediaPlayer error events.

  Those are async mechanisms that should not be used in parallel with their
  sync counterpart, i.e. poll-based queries for the same events using
  MLMediaPlayerPollStatespoll().

  \apilevel 8
*/
typedef struct MLMediaPlayerEventCallbacksEx {
  uint32_t version;
  /*!
    \brief This callback function is invoked when buffered contents percentage changed.

    \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
    \param[in] update_info Pointer to MLMediaPlayerOnBufferingUpdateInfo.
  */
  void (*on_buffering_update)(MLHandle media_player, const MLMediaPlayerOnBufferingUpdateInfo *update_info);
  /*!
    \brief This callback is invoked when media player played back until end of media and has now come to a stop.

    Note that this callback does not fire when 'looping = true',
    because \ref MediaPlayer does not "stop" in that case, but rather
    loops to beginning of media.

    To get notified when EOS is reached (when looping = true),
    set a proper callback function for the below on_info and look for the
    #MEDIA_PLAYER_INFO_LOOPING info code.

    \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
    \param[in] completion_info Pointer to  MLMediaPlayerOnCompletionInfo.
  */
  void (*on_completion)(MLHandle media_player, const MLMediaPlayerOnCompletionInfo *completion_info);
  /*!
    \brief This callback function is invoked when media player encounters an error.

    \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
    \param[in] erro_info Pointer to MLMediaPlayerOnErrorInfo.
  */
  void (*on_error)(MLHandle media_player, const MLMediaPlayerOnErrorInfo *error_info);
  /*!
    \brief This callback function is invoked when \ref MediaPlayer generates informational events.

    \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
    \param[in] info Pointer to MLMediaPlayerOnInfoInfo.
  */
  void (*on_info)(MLHandle media_player, const MLMediaPlayerOnInfoInfo *info);
  /*!
    \brief This callback is invoked when the player has finished preparing media and is ready to playback.

    \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
    \param[in] prepared_info Pointer to MLMediaPlayerOnPreparedInfo.
  */
  void (*on_prepared)(MLHandle media_player, const MLMediaPlayerOnPreparedInfo *prepared_info);
  /*!
    \brief This callback function is invoked when a seek operation has completed.

    \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
    \param[in] seek_complete_info Pointer to MLMediaPlayerOnSeekCompleteInfo.
  */
  void (*on_seek_complete)(MLHandle media_player, const MLMediaPlayerOnSeekCompleteInfo *seek_complete_info);
  /*!
    \brief This callback function is invoked when the internal surface has changed size.

    \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
    \param[in] video_size_info Pointer to MLMediaPlayerOnVideoSizeChangedInfo.
  */
  void (*on_video_size_changed)(MLHandle media_player, const MLMediaPlayerOnVideoSizeChangedInfo *video_size_info);
  /*!
    \brief This callback function is invoked when source has DRM protected media track(s).

    \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
    \param[in] track_drm_info Pointer to MLMediaPlayerOnTrackDRMInfo.
  */
  void (*on_track_drm_info)(MLHandle media_player, const MLMediaPlayerOnTrackDRMInfo *track_drm_info);
  /*!
    \brief This callback function is invoked when an async reset operation has completed.

    \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
    \param[in] reset_complete_info pointer to MLMediaPlayerOnResetCompleteInfo.
  */
  void (*on_reset_complete)(MLHandle media_player, const MLMediaPlayerOnResetCompleteInfo *reset_complete_info);
  /*!
    \brief This callback function is invoked when a stereoscopic video frame packing change.

    \param[in] media_player #MLHandle is a \ref MediaPlayer instance on which callback was set.
    \param[in] mode A MLMediaFramePackingMode.
    \param[in] flags A MLMediaFramePackingFlags.
  */
  void (*on_frame_packing)(MLHandle media_player, const MLMediaPlayerOnFramePackingInfo *frame_packing_info);
} MLMediaPlayerEventCallbacksEx;

/*!
  \brief Initializes MediaPlayerEventCallbacks with default values.

  \param[in,out] inout_callbacks Media Player Callback structure defined by #MLMediaPlayerEventCallbacksEx that needs to be initialized.
*/
ML_STATIC_INLINE void MLMediaPlayerEventCallbacksExInit(MLMediaPlayerEventCallbacksEx *inout_callbacks) {
  if (inout_callbacks) {
    inout_callbacks->version = 2;
    inout_callbacks->on_buffering_update = NULL;
    inout_callbacks->on_completion = NULL;
    inout_callbacks->on_error = NULL;
    inout_callbacks->on_info = NULL;
    inout_callbacks->on_prepared = NULL;
    inout_callbacks->on_seek_complete = NULL;
    inout_callbacks->on_video_size_changed = NULL;
    inout_callbacks->on_track_drm_info = NULL;
    inout_callbacks->on_reset_complete = NULL;
    inout_callbacks->on_frame_packing = NULL;
  }
}

/*!
  \brief MediaPlayer Track Information.

  \apilevel 2
*/
typedef struct MLMediaPlayerTrackInfo {
  uint32_t version;
  /*! Media track type, can be either audio or video. */
  MLMediaPlayerTrackType track_type;
} MLMediaPlayerTrackInfo;

/*!
  \brief Initializes Media Track Information with default values.

  \param[in,out] inout_track_info Media track information defined by #MLMediaPlayerTrackInfo that needs to be initialized.
*/
ML_STATIC_INLINE void MLMediaPlayerTrackInfoInit(MLMediaPlayerTrackInfo *inout_track_info) {
  if (inout_track_info) {
    inout_track_info->version = 1;
    inout_track_info->track_type = MediaPlayerTrackType_Unknown;
  }
}

/*!
  \brief DRM Session information for a MediaPlayer Track.

  \apilevel 2
*/
typedef struct MLMediaPlayerTrackDRMSessionInfo {
  uint32_t version;
  /*! Media track type, can be either audio or video. */
  MLMediaPlayerTrackType track_type;
  /*! UUID of the DRM Scheme of type #MLUUID. */
  const MLUUID *uuid;
  /*! DRM Session ID of type #MLMediaDRMByteArray. */
  const MLMediaDRMByteArray *session_id;
} MLMediaPlayerTrackDRMSessionInfo;

/*!
  \brief Initializes Media Track DRM Session Information with defult values.

  \param[in,out] inout_drm_session_info Media track DRM Session information defined by #MLMediaPlayerTrackDRMSessionInfo that needs to be initialized
*/
ML_STATIC_INLINE void MLMediaPlayerTrackDRMSessionInfoInit(MLMediaPlayerTrackDRMSessionInfo *inout_drm_session_info) {
  if (inout_drm_session_info) {
    inout_drm_session_info->version = 1;
    inout_drm_session_info->track_type = MediaPlayerTrackType_Unknown;
    inout_drm_session_info->uuid = NULL;
    inout_drm_session_info->session_id = NULL;
  }
}

/*!
  \brief Callback signature called when decoded frame is available for consumption.

  \note Client should *not* call MLMediaPlayer APIs from within this callback but rather
  set internal flags or use synchronization mechanisms to wake up another thread that
  will acquire the frame.

  \param[in] media_player #MLHandle is a \ref MediaPlayer instance for which callback was called.
  \param[in,out] data Custom data to be returned when callback is fired.
*/
typedef void(*MLMediaPlayerOnFrameAvailableCallback)(MLHandle media_player, void *data);

/*!
  \brief Create a new \ref MediaPlayer object.

  \note This is a blocking sync method. As creating a MediaPlayer object might take some time,
        make sure to *not* call this API from your main UI thread.

  \param[out] out_handle The #MLHandle to the new \ref MediaPlayer object created. Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Passed out_handle was NULL.
  \retval MLResult_Ok MediaPlayer was created successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit Could not initialize internal MediaPlayer.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerCreate(MLHandle *out_handle);

/*!
  \brief Destroy a MediaPlayer object.

  \note This is a blocking sync method. As destroying a MediaPlayer object might take some time,
        make sure to *not* call this API from your main UI thread.

  \param[in] media_player #MLHandle to the \ref MediaPlayer object to destroy.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok MediaPlayer was destroyed successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerDestroy(MLHandle media_player);

/*!
  \brief Set a file descriptor as the data source.

  The file descriptor must be seekable. It is the caller's responsibility to
  close the file descriptor. It is safe to do so as soon as this call returns.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] fd The file descriptor for the file you want to play.

  \retval MLResult_InvalidParam Passed fd does no refer to a valid,open file descriptor or 'media_player' was NULL.
  \retval MLResult_Ok Data source was correctly set to passed file descriptor.
  \retval MLResult_PrivilegeDenied Failed as necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called in the wrong state.
          SetDataSource must be called immediately after creation, or after a media player reset.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetDataSourceForFD(MLHandle media_player, int32_t fd);

/*!
  \brief Set a file descriptor as the data source with offset.

  The file descriptor must be seekable. It is the caller's responsibility to
  close the file descriptor. It is safe to do so as soon as this call returns.
  This API is useful for specifying playable media located in resource files.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] fd The file descriptor for the file you want to play.
  \param[in] offset The offset (in bytes) of the source's start.
  \param[in] length The number of bytes of media to load after offset.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Data source was correctly set to passed file descriptor.
  \retval MLResult_PrivilegeDenied Failed as necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called in the wrong state.
          SetDataSource must be called immediately after creation, or after a media player reset.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetOffsetDataSourceForFD(MLHandle media_player, int32_t fd, int64_t offset, int64_t length);

/*!
  \brief Set a local file path as the data source.

  The path should be an absolute path and should reference a world-readable
  file.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] path The linux-style path of the file.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Data source was correctly set to passed local file path.
  \retval MLResult_PrivilegeDenied Failed as necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called in the wrong state.
          SetDataSource must be called immediately after creation, or after a media player reset.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetDataSourceForPath(MLHandle media_player, const char *path);

/*!
  \brief Set a URI string as the data source.

  Supported URI schemes are `file`, `http`, `https`, and `rtsp`. If looking to
  provide headers, use MLMediaPlayerSetRemoteDataSourceForURI().

  \param[in] uri The C-style string representing the URI complete string.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Data source was correctly set to passed URI.
  \retval MLResult_PrivilegeDenied Failed as necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called in the wrong state.
          SetDataSource must be called immediately after creation, or after a media player reset.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv Internet
*/
ML_API MLResult ML_CALL MLMediaPlayerSetDataSourceForURI(MLHandle media_player, const char *uri);

/*!
  \brief Set a URI string as the remote data source.

  Supported URI schemes are `file`, `http`, `https`, and `rtsp`.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] uri The C-style string representing the complete URL.
  \param[in] headers Array of char * headers to be sent.
  \param[in] len Number of headers passed.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Data source was correctly set to passed URI.
  \retval MLResult_PrivilegeDenied Failed as necessary privilege is missing.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called in the wrong state.
          SetDataSource must be called immediately after creation, or after a media player reset.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv Internet
*/
ML_API MLResult ML_CALL MLMediaPlayerSetRemoteDataSourceForURI(MLHandle media_player, const char *uri, char **headers, size_t len);

/*!
  \brief Set the data source to use.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] media_data_source #MLHandle returned by MLMediaDataSourceCreate().

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Data source was correctly set.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called in the wrong state.
          SetDataSource must be called immediately after creation, or after a media player reset.
  \retval MLMediaGenericResult_NameNotFound No valid source could be found for the passed media_data_source handle.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetMediaDataSource(MLHandle media_player, MLHandle media_data_source);

/*!
  \brief Set the data source to use.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] media_stream_source #MLHandle returned by MLMediaStreamSourceCreate().

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Data source was correctly set.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called in the wrong state.
          SetDataSource must be called immediately after creation, or after a media player reset.
  \retval MLMediaGenericResult_NameNotFound No valid source could be found for the passed media_stream_source handle.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetMediaStreamSource(MLHandle media_player, MLHandle media_stream_source);

/*!
  \brief Add a timedtext source from a URI.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] uri The C-style string representing the complete URI.
  \param[in] mimeType The MIME type of the file.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok TimedTextSource was correctly added from passed URI.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerAddTimedTextSourceForURI(MLHandle media_player, const char *uri, const char *mimeType);

/*!
  \brief Add a timedtext source from a file descriptor.

  The file descriptor must be seekable. It is the caller's responsibility to
  close the file descriptor. It is safe to do so as soon as this call returns.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] fd The file descriptor for the timed text source.
  \param[in] offset The offset (in bytes) of the source's start. Must be >= 0
  \param[in] length The number of bytes of media to load after offset. Must be >= 0;
  \param[in] mimeType The MIME type of the file.

  \retval MLResult_InvalidParam Passed \c media_player handle or 'fd' was invalid, mimetype is NULL
          or other params are outside the valid range.
  \retval MLResult_Ok TimedTextSource was correctly added from passed file descriptor.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerAddTimedTextSourceForFD(MLHandle media_player, int32_t fd, int64_t offset, int64_t length, const char *mimeType);

/*!
  \brief Set the 'next' \ref MediaPlayer to automatically 'start' when current \ref MediaPlayer playback completes.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] next_media_player #MLHandle of an already created \ref MediaPlayer instance.
          To 'remove' the next player, set next_media_player = 0.

  \retval MLResult_InvalidParam One of the MLHandle was invalid or could not be found.
  \retval MLResult_Ok Next \ref MediaPlayer was correctly registered.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called in the wrong state.
          Can be called only from the 'prepared', 'paused' or 'playback-complete' states.
  \retval MLMediaGenericResult_NameNotFound No valid MediaPlayer could be found for the passed next_media_player handle.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetNextPlayer(MLHandle media_player, MLHandle next_media_player);

/*!
  \brief Count the number of tracks found in the data source.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_track_count The number of tracks found in the player source.

  \retval MLResult_InvalidParam \c media_player handle was invalid or not found or out_track_count was set to NULL.
  \retval MLResult_Ok Number of tracks was returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          Can only be called after one of the setDataSource methods.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetTrackCount(MLHandle media_player, uint32_t *out_track_count);

/*!
  \brief Get the type of a track.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] track The track number for which type is to be returned. Has to be < TrackCount.
  \param[out] out_track_type The returned type for specified track number.

  \retval MLResult_InvalidParam \c media_player was invalid, track did not refer to a valid track number,
          was out of range or out_track_type was NULL.
  \retval MLResult_Ok Type for passed track was returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state
         Can only be called after one of the setDataSource methods.
  \retval MLMediaGenericResult_NameNotFound Found track's type was unknown.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetTrackType(MLHandle media_player, uint32_t track, MLMediaPlayerTrackType *out_track_type);

/*!
  \brief Get the language of a track.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] track The track number for which language is to be returned. Has to be < TrackCount.
  \param[out] out_track_language The returned language of the track. Caller has ownership of that param
              and has to free it when no longer needed.

  \retval MLResult_InvalidParam \c media_player was invalid, track did not refer to a valid track number,
          was out of range or out_track_language was NULL.
  \retval MLResult_Ok Language for passed track was returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          Can only be called after one of the setDataSource methods.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetTrackLanguage(MLHandle media_player, uint32_t track, char **out_track_language);

/*!
  \brief Get the Media Format of a track.

  \apilevel 2

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] track The track number for which Media Format is to be returned. Has to be < TrackCount.
  \param[out] out_media_format #MLHandle to \ref MediaFormat of the track.
              Caller has ownership of this handle and should call #MLMediaFormatDestroy to free it when no longer needed.

  \retval MLResult_InvalidParam \c media_player was invalid, track did not refer to a valid track number,
          was out of range or out_media_format was NULL.
  \retval MLResult_Ok if operation succeeded.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          Can only be called after one of the setDataSource methods.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetTrackMediaFormat(MLHandle media_player, uint32_t track, MLHandle *out_media_format);

/*!
  \brief Select a track.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] track Number of the track to select. Has to be < TrackCount.

  \retval MLResult_InvalidParam \c media_player was invalid, track did not refer to a valid track number,
          was out of range or out_track_language was NULL.
  \retval MLResult_Ok Track was correctly selected.
  \retval MLResult_UnspecifiedFailure Method failed internally.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          Can only be called after one of the setDataSource methods.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSelectTrack(MLHandle media_player, uint32_t track);

/*!
  \brief Unselect a track.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] track Number of the track to unselect. Has to be < TrackCount.

  \retval MLResult_InvalidParam \c media_player was invalid, track did not refer to a valid track number.
  \retval MLResult_Ok Track was correctly unselected.
  \retval MLResult_UnspecifiedFailure Method failed internally.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          Can only be called after one of the setDataSource methods.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerUnselectTrack(MLHandle media_player, uint32_t track);

/*!
  \brief Prepare the player for playback, synchronously.

  After setting the data source and the #GraphicBufferProducer, you need to
  either call prepare() or prepareAsync(). For files, it is OK to call
  prepare(), which blocks until \ref MediaPlayer is ready for playback.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Player was prepared successfully.
  \retval MLResult_UnspecifiedFailure Unknown internal error prevented MediaPlayer from preparing.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          Can only be called after one of the setDataSource methods or in the 'stopped' state.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.
  \retval MLMediaResult_CannotConnect MediaPlayer failed to connect with the given URL.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerPrepare(MLHandle media_player);

/*!
  \brief Prepare the player for playback, asynchronously.

  After setting the data's source and the #GraphicBufferProducer, you need to
  either call prepare() or prepareAsync(). For streams, you should call
  prepareAsync(), which returns immediately, rather than blocking until
  enough data has been buffered. Prepared state will then be obtained via the
  'on_prepared' callback if already registered (see MLMediaPlayerSetEventCallbacksEx),
  or polled for via the MLMediaPlayerPollStates() call with the
  MLMediaPlayerPollingStateFlag_HasBeenPrepared flag set.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.


  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok MediaPlayer started preparing in an async. fashion.
          For result, poll for MLMediaPlayerPollingStateFlag_HasBeenPrepared state
          or wait for 'on_prepared' callback if already set.
  \retval MLResult_UnspecifiedFailure Unknown internal error prevented MediaPlayer from preparing.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          Can only be called after one of the setDataSource methods or in the 'stopped' state.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerPrepareAsync(MLHandle media_player);

/*!
  \brief Start or resumes playback.

  If playback had previously been paused, playback will continue from where
  it was paused. If playback had been stopped, or never started before,
  playback will start at the beginning.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Playback was successfully started or resumed.
  \retval MLResult_UnspecifiedFailure Internal error prevented MediaPlayer from starting.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          Can only be called in one of the following states : 'prepared', 'playback complete', 'paused', 'stopped'.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerStart(MLHandle media_player);

/*!
  \brief Stop playback after playback has been stopped or paused.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Playback got successfully stopped.
  \retval MLResult_UnspecifiedFailure Internal error prevented MediaPlayer from stopping.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          Can only be called in one of the following states : 'playing', 'paused', 'stopped', 'prepared', 'playback complete'.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerStop(MLHandle media_player);

/*!
  \brief Pause playback.

  Calling pause() is a NOOP if \ref MediaPlayer is in state #MEDIA_PLAYER_PAUSED
  or #MEDIA_PLAYER_PLAYBACK_COMPLETE.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Playback got successfully paused.
  \retval MLResult_UnspecifiedFailure Internal error prevented MediaPlayer from pausing.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          Can only be called in one of the following states : 'playing', 'paused', 'playback complete'.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerPause(MLHandle media_player);

/*!
  \brief Reset the \ref MediaPlayer to its uninitialized state.

  After calling this method, you will have to initialize it again by setting the
  data source and calling prepare().

  \note This is a blocking sync method, make sure to *not* call this API
        from your main UI thread.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok MediaPlayer was reset successfully.
  \retval MLResult_UnspecifiedFailure Internal error prevented MediaPlayer from resetting.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          This method can be called anytime except when in 'preparing async'.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerReset(MLHandle media_player);

/*!
  \brief Seek to specified time position.

  Note that SeekTo is an async. function and returns immediately.
  Successful seek result has to be obtained either via the 'on_seek_complete' if
  already registered (see MLMediaPlayerSetEventCallbacksEx) or by polling for the flag
  'MLMediaPlayerPollingStateFlag_HasSeekCompleted' when calling MLMediaPlayerPollStates().

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] msec The offset in milliseconds from the start, to seek to
  \param[in] mode Seek mode defined by MLMediaSeekMode.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Seek operation was successfully launched in an async. fashion.
          For result, poll for MLMediaPlayerPollingStateFlag_HasSeekCompleted state or
          wait for the 'on_seek_complete' callback if already set.
  \retval MLResult_UnspecifiedFailure Internal error prevented MediaPlayer from seeking.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          This method can only be called from the following states :
          'started', 'prepared', 'paused' and 'playback complete'.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSeekTo(MLHandle media_player, int msec, MLMediaSeekMode mode);

/*!
  \brief Set callback to get notified when a decoded frame is available.

  When MLMediaPlayerOnFrameAvailableCallback() function gets called-back because
  there is a buffer available for consumption, one should immediately call
  MLMediaPlayerAcquireNextAvailableBuffer(...) to get the buffer and
  render from it.

  Setting this callback does not prevent caller from using the the poll-based
  buffer interrogation API (MLMediaPlayerPollStates() with the
  #MLMediaPlayerPollingState_IsBufferAvailable flag).

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] on_buffer_available Function pointer to be called.
            This can be 'NULL' to unset the callback. See MLMediaPlayerOnFrameAvailable().
            Callback typedef declaration and comments for explanation on expected callback parameters.

  \param[in] data Custom data to be returned when callback is fired.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Callback was successfully set.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetOnBufferAvailableCallback(MLHandle media_player, MLMediaPlayerOnFrameAvailableCallback on_buffer_available, void *data);

/*!
  \brief Consume next available buffer.

  Note: This function is non-blocking and should only be called after
  MLMediaPlayerPollStates() with #MLMediaPlayerPollingState_IsBufferAvailable
  mask returned true, or when registered callback MLMediaPlayerOnFrameAvailableCallback
  just fired.

  Note: The returned buffer's color format is multi-planar YUV420. Since our
  underlying hardware interops do not support multiplanar formats, advanced
  texture features like mipmaping, anisotropic filtering and MSAA will *not*
  work on those returned buffers. If such advanced texturing features are
  required, one has to acquire the YUV buffers as usual and *render* to an
  RGB texture and only then, will one be able to enable hardware optimization
  for such advanced texture features.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_native_buffer The handle to the acquired native buffer
              When buffer is not required anymore, caller must call
              MLMediaPlayerReleaseBuffer() with this nativeBuffer handle to free up
              queuing space for upcoming buffers.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found or out_native_buffer was NULL.
  \retval MLResult_Ok A valid buffer handle was returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.
  \retval MLMediaGenericResult_NotEnoughData No buffer available internally.
          Make sure you call this function after MLMediaPlayerPollStates() with
          #MLMediaPlayerPollingState_IsBufferAvailable mask returned true, or when
          registered callback MLMediaPlayerOnFrameAvailableCallback just fired.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerAcquireNextAvailableBuffer(MLHandle media_player, uint64_t *out_native_buffer);

/*!
  \brief Release a native buffer previously acquired.

  \param[in] media_player MLHandle to the MediaPlayer instance.
  \param[in] native_buffer The android native buffer to release.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok The specified native buffer was successfully released.
  \retval MLResult_UnspecifiedFailure Internal error prevented MediaPlayer from releasing the buffer.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NameNotFound native_buffer handle was wrong, or already released.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerReleaseBuffer(MLHandle media_player, uint64_t native_buffer);

/*!
  \brief Retrieves the 4x4 texture coordinate transform matrix associated with the last acquired Frame.

  This transform matrix maps 2D homogeneous texture coordinates of the form
  (s, t, 0, 1) with s and t in the inclusive range [0, 1] to the texture
  coordinate that should be used to sample that location from the texture.
  Sampling the texture outside of the range of this transform is undefined.

  This transform is necessary to compensate for transforms that the stream
  content producer may implicitly apply to the content. By forcing users of
  a GLConsumer to apply this transform we avoid performing an extra
  copy of the data that would be needed to hide the transform from the
  user.

  The matrix is stored in column-major order so that it may be passed
  directly to OpenGL ES via the glLoadMatrixf or glUniformMatrix4fv
  functions.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_mtx A pointer to an array of 16 floats that will receive retrieved
              transformation matrix.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok The matrix was returned.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetFrameTransformationMatrix(MLHandle media_player, float out_mtx[16]);

/*!
  \brief Retrieves the timestamp associated with the last acquired Frame
  The timestamp is in nanoseconds, and is monotonically increasing. Its
  other semantics (zero point, etc) are codec-dependent and should be
  documented by the codec vendor.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_timestamp A pointer to user variable for storing retrieved timestamp.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok The timestamp was returned.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetFrameTimestamp(MLHandle media_player, int64_t *out_timestamp);

/*!
  \brief Retrieves the QueueBuffer timestamp associated with the last acquired Frame
  This timestamp is for profiling purposes only and corresponds to the time when this
  frame was queued by internal Codec's producer code. The timestamp is in nanoseconds,
  and is monotonically increasing. Its other semantics (zero point, etc) are codec-
  dependent and should by documented by the codec vendor.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_timestamp A pointer to user variable for storing retrieved timestamp.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok The timestamp was returned.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetFrameQueueBufferTimestamp(MLHandle media_player, int64_t *out_timestamp);

/*!
  \brief Retrieves the last acquired frame number.

  The frame number is an incrementing counter set to 0 at Codec configuration time.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_number A pointer to user variable for storing retrieved frame number.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok The frame number was returned.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetFrameNumber(MLHandle media_player, uint64_t *out_number);

/*!
  \deprecated Deprecated since 0.17.0. Scheduled for removal. Replaced by MLMediaPlayerSetEventCallbacksEx().

  \brief Set callbacks to notify clients about player events.

  Client needs to implement the callback functions declared in
  #MLMediaPlayerEventCallbacks, but can set to NULL the ones he does not
  care about.

  \param[in] media_player #MLHandle to the \ref MediaCodec.
  \param[in] callbacks Set of event callbacks. Can be set to NULL to unset the callbacks altogether.
  \param[in] data Custom data to be returned when any callback is fired.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok The specified callbacks were successfully set/unset.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_DEPRECATED_MSG("Replaced by MLMediaPlayerSetEventCallbacksEx.")
ML_API MLResult ML_CALL MLMediaPlayerSetEventCallbacks(MLHandle media_player, MLMediaPlayerEventCallbacks *callbacks, void *data);

/*!
  \brief Set callbacks to notify clients about player events.

  This structure must be initialized by calling MLMediaPlayerEventCallbacksExInit() before use.

  Client needs to implement the callback functions declared in
  #MLMediaPlayerEventCallbacksEx, but can set to NULL the ones he does not care about.

  \apilevel 2

  \param[in] media_player #MLHandle to the \ref MediaCodec.
  \param[in] callbacks Set of event callbacks. Can be set to NULL to unset
             the callbacks altogether.
  \param[in] data Custom data to be returned when any callback is fired.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok The specified callbacks were successfully set/unset.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetEventCallbacksEx(MLHandle media_player, const MLMediaPlayerEventCallbacksEx *callbacks, void *data);

/*!
  \brief Return the size of the video frame.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_width The returned video width.
  \param[out] out_height The returned video height.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Video size was returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetVideoSize(MLHandle media_player, int32_t *out_width, int32_t *out_height);

/*!
  \brief Return current position of playback.

  \param[out] out_msec The returned position in milliseconds.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found or 'out_msec' was set to NULL by caller.
  \retval MLResult_Ok Current position was successfully returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetCurrentPosition(MLHandle media_player, int32_t *out_msec);

/*!
  \brief Return media duration.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_msec The returned duration in milliseconds.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found or 'out_msec' was set to NULL by caller.
  \retval MLResult_Ok Duration was successfully returned.
  \retval MLResult_UnspecifiedFailure Internal error prevented MediaPlayer from fetching a valid media duration.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          This method can only be called from the following states :
          'prepared', 'started', 'paused', 'stopped' and 'playback complete'.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetDuration(MLHandle media_player, int32_t *out_msec);

/*!
  \brief Return current buffering percentage.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_percentage The returned buffering percentage in the range [0, 100].

  \retval MLResult_InvalidParam Passed \c media_player handle was not found or 'out_percentage' was set to NULL by caller.
  \retval MLResult_Ok Current buffering percentage was successfully returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetBufferingUpdate(MLHandle media_player, int32_t *out_percentage);

/*!
  \brief Return last info received by internal \ref MediaPlayer.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_code The main info code.
  \param[out] out_extra The secondary code/data.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Last MediaPlayer info was successfully returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetInfo(MLHandle media_player, int32_t *out_code, int32_t *out_extra);

/*!
  \brief Return last error received by internal \ref MediaPlayer.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_result The result code.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Last MediaPlayer error was successfully returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetError(MLHandle media_player, MLResult *out_result);

/*!
  \brief Return last timed text event information.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_text The text of the timed text event.
  \param[out] out_start The start time.
  \param[out] out_end The end time.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Last timedtext entry was successfully returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetTimedText(MLHandle media_player, const char **out_text, int32_t *out_start, int32_t *out_end);

/*!
  \deprecated Deprecated. Replaced by MLMediaPlayerGetSubtitleEx().

  \brief Return last subtitle event information.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_track The return Track for the subtitle event.
  \param[out] out_time The returned start time for that track.
  \param[out] out_duration The returned duration of the event.
  \param[out] out_buffer The returned event buffer. This buffer
              is only valid until the next metadata event arrives, so make a copy if you
              need to use the buffer later.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Last subtitle entry was successfully returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_DEPRECATED_MSG("Replaced by MLMediaPlayerGetSubtitleEx")
ML_API MLResult ML_CALL MLMediaPlayerGetSubtitle(MLHandle media_player, int32_t *out_track, int64_t *out_time, int64_t *out_duration, uint8_t **out_buffer);

/*!
  \brief Return last subtitle event information.

  \apilevel 2

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_subtitle_args The returned MLMediaPlayerGetSubtitleExArgs pointer.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Last subtitle entry was successfully returned.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetSubtitleEx(MLHandle media_player, MLMediaPlayerSubtitleData **out_subtitle_data);

/*!
  \brief Release last subtitle event information.

  \apilevel 2

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Last subtitle entry was successfully released.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerReleaseSubtitleEx(MLHandle media_player);

/*!
  \brief Callback signature called when subtitle update is available.

  \param[in] data Custom data to be returned when callback is fired.
  \param[in] media_player #MLHandle is a \ref MediaPlayer instance for which callback was called.
  \param[in] subtitle_data #MLMediaPlayerSubtitleData is the raw subtitle data.
*/
typedef void(*MLMediaPlayerOnMediaSubtitleUpdateCallback)(MLHandle media_player, MLMediaPlayerSubtitleData subtitle_data, void *data);

/*!
  \brief Set callback to get notified when a subtitle update is available along with its data.

  \apilevel 4

  Setting this callback disables use of the poll-based API to query for the
  #MLMediaPlayerPollingStateFlag_HasSubtitleUpdated flag.
  MLMediaPlayerPollStates() will ignore checks for this bit.

  This also makes MLMediaPlayerGetSubtitleEx() redundant as the subtitle data is provided with the callback.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] on_media_subtitle_callback function pointer to be called.
            This can be 'NULL' to unset the callback.
            See MLMediaPlayerOnMediaSubtitleUpdateCallback() callback typedef declaration and comments
            for explanation on expected callback parameters.
  \param[in] data Custom data to be returned when callback is fired.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Callback was successfully set.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetOnMediaSubtitleUpdateCallback(MLHandle media_player, MLMediaPlayerOnMediaSubtitleUpdateCallback on_media_subtitle_callback, void *data);

/*!
  \brief Return last metadata event information.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_time The returned event time.
  \param[out] out_size The returned event buffer size.
  \param[out] out_buffer The returned event buffer.
              This buffer is only valid until the next metadata event arrives,
              so make a copy if you need to use the buffer later.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Last metadata entry was successfully returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetMetadata(MLHandle media_player, int64_t *out_time, int32_t *out_size, uint8_t **out_buffer);

/*!
  \brief Set video scaling mode.

  \param[in] media_player MLHandle to the MediaPlayer instance
  \param[in] mode The video scaling mode to set.
             For a list of valid values, see #media_video_scaling_mode_type declaration.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Current video scaling mode was successfully returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          Can only be called after one of the setDataSource methods.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetVideoScalingMode(MLHandle media_player, MLMediaPlayerVideoScalingMode mode);

/*!
  \brief Set the looping mode of the player.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] loop The looping mode.
             true if player should loop to beginning of \ref MediaPlayer when EOS reached.
             false if \ref MediaPlayer should simply stop playback at end of media.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Current looping mode was successfully returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetLooping(MLHandle media_player, bool loop);

/*!
  \brief Set the volume on this player.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] volume The new volume in the [0.0, 1.0] range.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok Volume was successfully set.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetVolume(MLHandle media_player, float volume);

/*!
  \brief Get the handle of the audio stream.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_handle The #MLHandle to the returned Audio handle. Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found or 'out_handle' parameter was set to NULL by caller.
  \retval MLResult_Ok Current audio stream handle was successfully returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetAudioHandle(MLHandle media_player, MLHandle *out_handle);

/*!
  \brief Poll the desired states from internal \ref MediaPlayer.

  Desired states have to be selected by passing a bitwise OR'ed mask of
  #MLMediaPlayerPollingStateFlags flags.

  The "is_XXX"  states can be polled multiples times and the return value will
  be the same if internal state hasn't changed since last call.

  When polling the "has_XXX" states however, internal state is set to false,
  since they mean : has <state> happened/changed since I last polled <state>

  Two-step state polling : The following states, when returning '1', indicate
                           there is some data ready to be retrieved, by calling
                           a getter function, as explained below :

  For '#MLMediaPlayerPollingState_IsBufferAvailable',
  call MLMediaPlayerAcquireNextAvailableBuffer() to get new buffer handle.

  For '#MLMediaPlayerPollingState_HasSizeChanged',
  call MLMediaPlayerGetVideoSize() to get the new size.

  For '#MLMediaPlayerPollingState_HasBufferingUpdated',
  call MLMediaPlayerGetBufferingUpdate() to get buffering progress value.

  For '#MLMediaPlayerPollingState_HasReceivedInfo,
  call MLMediaPlayerGetError() to get last info code and data.

  For '#MLMediaPlayerPollingState_HasReceivedError,
  call MLMediaPlayerGetError() to get last error code and data.

  Note: This API can still be used even if an #OnBufferAvailable callback has
  been set using MLMediaPlayerSetOnBufferAvailableCallback() method.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] mask Selection mask for picking which states to retrieve.
  \param[out] out_polled_states Bitwise OR'ed integer of all the chosen states from bitwise 'mask'
              and in the range [0 = false : 1 = true].

  \retval MLResult_InvalidParam Passed \c media_player handle was not found or 'out_polled_states' parameter was set to NULL by caller.
  \retval MLResult_Ok Selected states were successfully polled and returned.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerPollStates(MLHandle media_player, uint16_t mask, uint16_t *out_polled_states);

/*!
  \deprecated Deprecated since 0.17.0. Scheduled for removal. Replaced by MLMediaPlayerGetTrackDRMInfo().

  \brief Get the DRM info of the video source

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_drm_info pointer to the pointer to #MLMediaPlayerDRMInfo struct.
              The caller should not free the pointer returned.
              The memory will be released in the call to MLMediaPlayerReleaseDRM().

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Current DRM Info was successfully returned.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_DEPRECATED_MSG("Replaced by MLMediaPlayerGetTrackDRMInfo.")
ML_API MLResult ML_CALL MLMediaPlayerGetDRMInfo(MLHandle media_player, MLMediaPlayerDRMInfo **out_drm_info);

/*!
  \deprecated Deprecated since 0.17.0. Scheduled for removal. Replaced by MLMediaPlayerPrepareTrackDRM().

  \brief Prepare DRM.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] uuid UUID of DRM scheme.
  \param[in] session_id pointer to MLMediaDRMByteArray, stands for Session ID.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok DRM was prepared successfully.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_DEPRECATED_MSG("Replaced by MLMediaPlayerPrepareTrackDRM.")
ML_API MLResult ML_CALL MLMediaPlayerPrepareDRM(MLHandle media_player, const uint8_t uuid[16], const MLMediaDRMByteArray *session_id);

/*!
  \brief Get the DRM info of the selected media (either audio or video) track.

  This function has to be called only after DataSource has been set and the MediaPlayer is completely prepared.

  \apilevel 2

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] track_info Media track information defined by #MLMediaPlayerTrackInfo
  \param[out] out_drm_info pointer to the pointer to #MLMediaPlayerTrackDRMInfo struct.
              The caller should not free the pointer returned.
              The memory will be released in the call to MLMediaPlayerReleaseDRM().

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Current DRM Info was successfully returned.
  \retval MLResult_Pending Media Source is not prepared yet. Wait for on_prepared callback and call again.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.
  \retval MLMediaGenericResult_NotAvailable DRM Information not available for the given track.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetTrackDRMInfo(MLHandle media_player, const MLMediaPlayerTrackInfo *track_info, MLMediaPlayerTrackDRMInfo **out_drm_info);

/*!
  \brief Prepare DRM for the selected media (either audio or video) track

  \apilevel 2

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] drm_session_info DRM Session info for the Media track, as defined by #MLMediaPlayerTrackDRMSessionInfo.
  \param[in] uuid UUID of DRM scheme.
  \param[in] session_id pointer to MLMediaDRMByteArray, stands for Session ID.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok DRM was prepared successfully.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerPrepareTrackDRM(MLHandle media_player, const MLMediaPlayerTrackDRMSessionInfo *drm_session_info);

/*!
  \brief Release DRM.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.

  \retval MLResult_InvalidParam \c media_player was invalid.
  \retval MLResult_Ok DRM was released successfully.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerReleaseDRM(MLHandle media_player);

/*!
  \brief Get default Buffering settings.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_buf_settings A pointer of #MLMediaPlayerBufferingSettings.

  \retval MLResult_InvalidParam \c media_player was invalid.
  \retval MLResult_Ok Default BufferSetting was returned successfully.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetDefaultBufferingSettings(MLHandle media_player, MLMediaPlayerBufferingSettings *out_buf_settings);

/*!
  \brief Get current Buffering settings.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_buf_settings A pointer of #MLMediaPlayerBufferingSettings.

  \retval MLResult_InvalidParam \c media_player was invalid.
  \retval MLResult_Ok Current BufferSetting was returned successfully.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetBufferingSettings(MLHandle media_player, MLMediaPlayerBufferingSettings *out_buf_settings);

/*!
  \brief Set Buffering settings.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] buf_settings A reference of #MLMediaPlayerBufferingSettings.

  \retval MLResult_InvalidParam \c media_player was invalid.
  \retval MLResult_Ok Set BufferSetting successfully.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetBufferingSettings(MLHandle media_player, MLMediaPlayerBufferingSettings buf_settings);

/*!
  \brief Return last Timed Text event information.

  \apilevel 4

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] out_timed_text The returned Timed Text handle pointer.
  When handle is not required anymore, the caller should call MLMediaPlayerReleaseTimedTextEx()
  with the returned handle.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Last subtitle entry was successfully returned.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetTimedTextEx(MLHandle media_player, MLHandle *out_timed_text);

/*!
  \brief Release last Timed Text event information.

  \apilevel 4

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] timed_text Timed Text #MLHandle to release.

  \retval MLResult_InvalidParam One of the parameters was NULL or invalid.
  \retval MLResult_Ok Last Timed Text entry was successfully released.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerReleaseTimedTextEx(MLHandle media_player, MLHandle timed_text);

/*!
  \brief Callback signature called when Timed Text update is available.

  \apilevel 4

  \param[in] media_player #MLHandle is a \ref MediaPlayer instance for which callback was called.
  \param[in] timed_text #MLHandle is the handle for timed_text data.
  \param[in] data Custom data to be delivered when callback is invoked.

  \priv None
*/
typedef void(*MLMediaPlayerOnMediaTimedTextUpdateCallback)(MLHandle media_player, MLHandle timed_text, void *data);

/*!
  \brief Set callback to get invoked when a Timed Text update is available along with its data.

  \apilevel 4

  Setting this callback disables use of the poll-based API to query for the
  #MLMediaPlayerPollingStateFlag_HasTimedTextUpdated flag.
  MLMediaPlayerPollStates() will ignore checks for this bit.

  This also makes MLMediaPlayerGetTimedTextEx() redundant as the Timed Text data is provided with the callback.

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] on_media_timed_text_callback function pointer to be called.
            This can be 'NULL' to unset the callback.
  \param[in] data Custom data to be returned when callback is fired.

  \retval MLResult_Ok Callback was successfully set.
  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetOnMediaTimedTextUpdateCallback(MLHandle media_player, MLMediaPlayerOnMediaTimedTextUpdateCallback on_media_timed_text_callback, void *data);

/*!
  \brief Get WebVTT data represented by a timed text handle.

  \apilevel 4

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] timed_text The Timed Text handle.
  \param[out] out_webvtt_data The WebVTT data structure to be returned.
  The returned structure is only valid until the corresponding timed_text handle is released
  using MLMediaPlayerReleaseTimedTextEx().

  \retval MLResult_InvalidParam One of the parameters was invalid.
  \retval MLMediaGenericResult_BadType The timed_text handle was not in the WebVTT format. The timed text track's mime type must be "text/vtt".
  \retval MLResult_Ok WebVTT data was successfully returned.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetWebVTTData(MLHandle media_player, MLHandle timed_text, MLWebVTTData** out_webvtt_data);

/*!
  \brief Reset the \ref MediaPlayer to its uninitialized state asynchronously.

  This method is asynchronous and will return immediately. After invocation,
  you will have to initialize it again by setting the data source and calling prepare()
  or prepareAsync().

  A successful reset result can be obtained either by registering the 'on_reset_complete'
  callback (see MLMediaPlayerSetEventCallbacksEx) or by polling for the
  'MLMediaPlayerPollingStateFlag_HasResetCompleted' flag when calling MLMediaPlayerPollStates().

  \apilevel 8

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.

  \retval MLResult_InvalidParam Passed media_player handle was not found.
  \retval MLResult_Ok MediaPlayer reset operation was successfully launched asynchronously.
          For result, poll for MLMediaPlayerPollingStateFlag_HasResetCompleted or
          use the 'on_reset_complete' callback if already set.
  \retval MLResult_UnspecifiedFailure Internal error prevented MediaPlayer from resetting.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
          This method can be called anytime except when in 'preparing async'.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerResetAsync(MLHandle media_player);

/*!
  \brief MediaPlayer metrics data.

  \apilevel 8
*/
typedef struct MLMediaPlayerMetrics {
  /*! The average framerate (fps) of video playback */
  float average_framerate;
  /*! The average bitrate (bps) of video playback*/
  int average_video_bitrate;
  /*! The average bitrate (bps) of audio playback*/
  int average_audio_bitrate;
} MLMediaPlayerMetrics;

/*!
  \brief Retrieve metrics recorded on the MediaPlayer.

  \apilevel 8

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[out] metrics See \ref MLMediaPlayerMetrics.

  \retval MLResult_InvalidParam Passed media_player handle was not found.
  \retval MLResult_Ok Metrics received successfully
  \retval MLResult_UnspecifiedFailure Internal error prevented MediaPlayer from retreiving metrics.
  \retval MLMediaGenericResult_InvalidOperation Method was called from the wrong state.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerGetMetrics(MLHandle media_player, MLMediaPlayerMetrics *metrics);

/*!
  \brief Set the unique id on this player.
  This function needs to be called immediatly after creating the media player,
  before making any further operations on the media player. The id should be unique
  across all media player sessions that are being shared.

  Once the id is set, then it can not be changed. If called multiple times,
  the call returns an error and keeps the id set at the first call.

  \apilevel 8

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] id unique Id for this player.

  \retval MLResult_InvalidParam Passed \c media_player handle was not found.
  \retval MLResult_Ok id is successfully set.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_AlreadyExists If the id is already set.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetID(MLHandle media_player, uint32_t id);

/*!
  \brief Sets sharing information for the media player being shared and enables only functionality
  for synchronize the content playback.

  This function needs to be called only after Id is set on media player using MLMediaPlayerSetID().

  For sharing the encrypted/subscribed content, application needs to make sure that all necessary
  setups for DRM/Subscription/account validation etc are already done, before calling this function
  to share.

  This function does not resolve any DRM/subscription/account issues occurred during the sharing
  by itself.

  For an initiator instance of the player this is called when the application is shared.
  For a follower instance of the player, this function must be called immediatly after
  setting the id and before setting data source by calling one of setDataSource methods.

  \apilevel 8

  \param[in] media_player #MLHandle to the \ref MediaPlayer instance.
  \param[in] shared_type The shared type for the current media player from enum #MLMediaPlayerSharedType.
  \param[in] session_id  Unique Identifier of the sharing session in which the media players are being shared.
                         Sessions can be created by different sharing protocols.

                         Current supported sharing protocols:
                         Application Connectivity Platform (ACP)

                         If Application Connectivity Platform (ml_app_connect.h) is used as protocol to share, then
                         this parameter holds the connection_name as used in MLAppConnectCreateConnection,
                         which is used to create ACP connection.

  \retval MLResult_InvalidParam for invalid parameters.
  \retval MLResult_Ok shared information is successfully set.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit MediaPlayer was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaPlayerSetSharingInfo(MLHandle media_player, MLMediaPlayerSharedType shared_type, const char *session_id);

/*! \} */

ML_EXTERN_C_END
