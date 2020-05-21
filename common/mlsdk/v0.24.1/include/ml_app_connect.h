// %BANNER_BEGIN%
// ---------------------------------------------------------------------
// %COPYRIGHT_BEGIN%
//
// Copyright (c) 2019 Magic Leap, Inc. (COMPANY) All Rights Reserved.
// Magic Leap, Inc. Confidential and Proprietary
//
//  NOTICE:  All information contained herein is, and remains the property
//  of COMPANY. The intellectual and technical concepts contained herein
//  are proprietary to COMPANY and may be covered by U.S. and Foreign
//  Patents, patents in process, and are protected by trade secret or
//  copyright law.  Dissemination of this information or reproduction of
//  this material is strictly forbidden unless prior written permission is
//  obtained from COMPANY.  Access to the source code contained herein is
//  hereby forbidden to anyone except current COMPANY employees, managers
//  or contractors who have executed Confidentiality and Non-disclosure
//  agreements explicitly covering such access.
//
//  The copyright notice above does not evidence any actual or intended
//  publication or disclosure  of  this source code, which includes
//  information that is confidential and/or proprietary, and is a trade
//  secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION,
//  PUBLIC  PERFORMANCE, OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS
//  SOURCE CODE  WITHOUT THE EXPRESS WRITTEN CONSENT OF COMPANY IS
//  STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE LAWS AND
//  INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE
//  CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS
//  TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE,
//  USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.
//
// %COPYRIGHT_END%
// --------------------------------------------------------------------*/
// %BANNER_END%

#pragma once

#include "ml_api.h"
#include "ml_image.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup ACP

  \sharedobject ml_app_connect
  \brief APIs for Application Connectivity Platform (ACP)

  Application Connectivity Platform, or ACP, provides the ability for applications to share data
  over the network between multiple Magic Leap devices via an "ACP connection".
  This connection is initialized by an "initiator device". The OS provides an user interface
  to allow the users to invite their contacts (based on their social graph), called "invite dialog".

  Once one or more peers accept the invite from the initiator device, a connection is established
  and multiple data pipes, for different purposes, can be created.

  Please Note: Use of the Applications Connectivity Platform (ACP) API is currently provided free
  of charge, however Magic Leap reserves the right to charge for use of the ACP API in the future.
  Consumption of a disproportionate amount of available API network resources resulting in the
  potential to disrupt or degrade ACP usage by others is prohibited.

  \{
 */

/*!
  \brief Connection Name Key.

  Mandatory key in the key-value list returned by #MLAppConnectInviteCallback().
  An application invited to a new connection is supposed to obtain the connection name by reading
  the value associated with this key.

  Check #MLAppConnectCreateConnection() and #MLAppConnectCojoinConnection() for more details.
 */
#define MLAppConnectArgConnectionNameKey "connection_name"

enum {
  /*! Defines the prefix for MLAppConnectResult codes. */
  MLResultAPIPrefix_AppConnect = MLRESULT_PREFIX(0xebf7)
};

/*!
  \brief Return values for ACP API calls.

  \apilevel 8
 */
typedef enum MLAppConnectResult {
  /*! Unsupported operation. */
  MLAppConnectResult_UnsupportedOperation = MLResultAPIPrefix_AppConnect,
  /*! Callback not removed. */
  MLAppConnectResult_CallbackNotRemoved,
  /*! Application exists. */
  MLAppConnectResult_ApplicationExists,
  /*! Application does not exist. */
  MLAppConnectResult_ApplicationDoesNotExist,
  /*! Connection callback not registered. */
  MLAppConnectResult_ConnectionCallbackNotRegistered,
  /*! Connection exists. */
  MLAppConnectResult_ConnectionExists,
  /*! Connection does not exist. */
  MLAppConnectResult_ConnectionDoesNotExist,
  /*! Connections per app exceeded. */
  MLAppConnectResult_ConnectionsPerAppExceeded,
  /*! Connections per device exceeded. */
  MLAppConnectResult_ConnectionsPerDeviceExceeded,
  /*! Max data pipes for connection exceeded. */
  MLAppConnectResult_DataPipesPerConnectionExceeded,
  /*! Pipe exists. */
  MLAppConnectResult_PipeExists,
  /*! Pipe does not exist. */
  MLAppConnectResult_PipeDoesNotExist,
  /*! Pipe type mismatch. */
  MLAppConnectResult_PipeTypeMismatch,
  /*! Pipe closed. */
  MLAppConnectResult_PipeClosed,
  /*! Max pipes per connection exceeded. */
  MLAppConnectResult_PipesPerConnectionExceeded,
  /*! Pipe invalid operation. */
  MLAppConnectResult_PipeInvalidOperation,
  /*! Pipe invalid properties. */
  MLAppConnectResult_PipeInvalidProperties,
  /*! Pipe name reserved. */
  MLAppConnectResult_PipeNameReserved,
  /*! Pipe buffer requested size exceeded limit. */
  MLAppConnectResult_PipeBufferSizeExceeded,
  /*! Pipe buffer size can not be zero. */
  MLAppConnectResult_PipeBufferSizeZero,
  /*! Pipe buffer can't be initialized. */
  MLAppConnectResult_PipeBufferInitError,
  /*! Pipe buffer read overrun. */
  MLAppConnectResult_PipeBufferReadOverrun,
  /*! Pipe buffer read error. */
  MLAppConnectResult_PipeBufferReadError,
  /*! Pipe buffer write error. */
  MLAppConnectResult_PipeBufferWriteError,
  /*! Pipe unknown priority. */
  MLAppConnectResult_PipeUnknownPriority,
  /*! Invalid pipe large data header. */
  MLAppConnectResult_PipeLargeDataTimedOut,
  /*! Pipe large data doesn't exist. */
  MLAppConnectResult_PipeLargeDataDoesNotExist,
  /*! Microphone not enabled. */
  MLAppConnectResult_MicrophoneNotEnabled,
  /*! Video frame not ready. */
  MLAppConnectResult_VideoFrameNotReady,
  /*! Camera initialization fail. */
  MLAppConnectResult_CameraInitializationFail,
  /*! Cloud generic error. */
  MLAppConnectResult_CloudGenericError,
  /*! Users not invited. */
  MLAppConnectResult_CloudUsersNotInvited,
  /*! Credentials not valid. */
  MLAppConnectResult_CloudCredentialsNotValid,
  /*! Copresence session error. */
  MLAppConnectResult_CloudCopresenceSessionError,
  /*! Users Provider request time out. */
  MLAppConnectResult_UsersProviderRequestError,
  /*! User id already exists. */
  MLAppConnectResult_UserIdExists,
  /*! User id does not exist. */
  MLAppConnectResult_UserIdInvalid,
  /*! Invite dialog selection canceled. */
  MLAppConnectResult_InviteSelectionCancelledByUser,
  /*! Invalid Invite selection. */
  MLAppConnectResult_InviteSelectionInvalid,
  /*! Invite dialog launch failed. */
  MLAppConnectResult_InviteDialogLaunchFail,
  /*! Invite dialog launch time out. */
  MLAppConnectResult_InviteDialogLaunchRegisterTimeOut,
  /*! Invalid argument for invite dialog. */
  MLAppConnectResult_InviteDialogInvalidArg,
  /*! Ensure enum is represented as 32 bits. */
  MLAppConnectResult_Ensure32Bits = 0x7FFFFFFF
} MLAppConnectResult;

/*!
  \brief Connection type.

  \apilevel 8
 */
typedef enum MLAppConnectConnectionType {
  /*! Unknown connection type. */
  MLAppConnectConnectionType_UnknownConnectionType,
  /*! Loopback Connetion. */
  MLAppConnectConnectionType_Loopback,
  /*! Default connection (device to device). */
  MLAppConnectConnectionType_Default,
  /*! Ensure enum is represented as 32 bits. */
  MLAppConnectConnectionType_Ensure32Bits = 0x7FFFFFFF
} MLAppConnectConnectionType;

/*!
  \brief Pipe type.

  \apilevel 8
 */
typedef enum MLAppConnectPipeType {
  /*! Unknown Pipe type. */
  MLAppConnectPipeType_UnknownPipeType,
  /*! Pipe type for Audio transfer. */
  MLAppConnectPipeType_AudioPipe,
  /*! Pipe type for Data transfer. */
  MLAppConnectPipeType_DataPipe,
  /*! Pipe type for Video transfer. */
  MLAppConnectPipeType_VideoPipe,
  /*! Ensure enum is represented as 32 bits. */
  MLAppConnectPipeType_Ensure32Bits = 0x7FFFFFFF
} MLAppConnectPipeType;

/*!
  \brief Pipe reliability.

  \apilevel 8
 */
typedef enum MLAppConnectPipeReliability {
  /*! Unknown reliability. */
  MLAppConnectPipeReliability_UnknownPipeReliability,
  /*! Reliable data transfer. */
  MLAppConnectPipeReliability_Reliable,
  /*! Unreliable data transfer. */
  MLAppConnectPipeReliability_Unreliable,
  /*! Ensure enum is represented as 32 bits. */
  MLAppConnectPipeReliability_Ensure32Bits = 0x7FFFFFFF
} MLAppConnectPipeReliability;

/*!
  \brief Pipe ordering.

  \apilevel 8
 */
typedef enum MLAppConnectPipeOrdering {
  /*! Unknown order. */
  MLAppConnectPipeOrdering_UnknownPipeOrdering,
  /*! Data transfer in order. */
  MLAppConnectPipeOrdering_Ordered,
  /*! Data transfer not in order. */
  MLAppConnectPipeOrdering_NotOrdered,
  /*! Ensure enum is represented as 32 bits. */
  MLAppConnectPipeOrdering_Ensure32Bits = 0x7FFFFFFF
} MLAppConnectPipeOrdering;

/*!
  \brief Pipe direction.

  \apilevel 8
 */
typedef enum MLAppConnectPipeDirection {
  /*! Pipe direction, Unknown. */
  MLAppConnectPipeDirection_UnknownPipeDirection,
  /*! Pipe direction, UniDirection out. */
  MLAppConnectPipeDirection_UniDirectionalOut,
  /*! Pipe direction, UniDirection in. */
  MLAppConnectPipeDirection_UniDirectionalIn,
  /*! Pipe direction, BiDirection. */
  MLAppConnectPipeDirection_BiDirectional,
  /*! Ensure enum is represented as 32 bits. */
  MLAppConnectPipeDirection_Ensure32Bits = 0x7FFFFFFF
} MLAppConnectPipeDirection;

/*!
  \brief Priority.

  Before the data is actually sent to other users, it gets queued according to a priority.
  If 2 pipes with different priorities have queued data at the same time, then the pipe
  with the higher priority will flush its data before the pipe with lower priority.

  \apilevel 8
 */
typedef enum MLAppConnectPriority {
  /*! High priority. */
  MLAppConnectPriority_High,
  /*! Normal priority. */
  MLAppConnectPriority_Normal,
  /*! Low priority. */
  MLAppConnectPriority_Low,
  /*! Ensure enum is represented as 32 bits. */
  MLAppConnectPriority_Ensure32Bits = 0x7FFFFFFF
} MLAppConnectPriority;

/*!
  \brief Event type.

  \apilevel 8
 */
typedef enum MLAppConnectConnectionEventType {
  /*! Unknown Event. */
  MLAppConnectConnectionEventType_UnknownEvent,
  /*! Pipe Created. */
  MLAppConnectConnectionEventType_PipeCreated,
  /*! Pipe Creation Faile. */
  MLAppConnectConnectionEventType_PipeFailed,
  /*! Pipe Deleted. */
  MLAppConnectConnectionEventType_PipeDeleted,
  /*! Pipe received large data packet(s). */
  MLAppConnectConnectionEventType_PipeLargeData,
  /*! Mic Muted. */
  MLAppConnectConnectionEventType_MicMuted,
  /*! Mic Unmuted. */
  MLAppConnectConnectionEventType_MicUnmuted,
  /*! User joined the connection. */
  MLAppConnectConnectionEventType_UserJoined,
  /*! User left the connection. */
  MLAppConnectConnectionEventType_UserExited,
  /*! A new user got invited. */
  MLAppConnectConnectionEventType_UserInvite,
  /*! User decided to abort the "invite dialog". */
  MLAppConnectConnectionEventType_UsersInviteAbort,
  /*! Ensure enum is represented as 32 bits. */
  MLAppConnectConnectionEventType_Ensure32Bits = 0x7FFFFFFF
} MLAppConnectConnectionEventType;

/*!
  \brief Video format.

  \apilevel 8
 */
typedef enum MLAppConnectVideoFormat {
  /*! RGBA video format. */
  MLAppConnectVideoFormat_RGBA,
  /*! I420 video format. */
  MLAppConnectVideoFormat_I420,
  /*! Ensure enum is represented as 32 bits. */
  MLAppConnectVideoFormat_Ensure32Bits = 0x7FFFFFFF
} MLAppConnectVideoFormat;

/* Maximum number of participants in a session */
const uint32_t MLAppConnectConnectionMaxParticipants = 10;

/*!
  \brief Connection properties.

  \apilevel 8
 */
typedef struct MLAppConnectConnectionProperties {
  /*! Version of this structure. */
  uint32_t version;
  /*! The connection type to create a connection. */
  MLAppConnectConnectionType type;
  /*! The priority level of this connection. */
  MLAppConnectPriority priority;
  /*! Max participants per connection. It may be any number between 2 and #MLAppConnectConnectionMaxParticipants */
  uint32_t max_participants;
  /*! Lock remote users from inviting user. */
  bool lock_users;
  /*! Lock remote users from creating/deleting pipes. */
  bool lock_pipes;
} MLAppConnectConnectionProperties;

/*!
  \brief Initializes a new instance of #MLAppConnectConnectionProperties.

  \apilevel 8

  \param[in,out] inout_connection_properties Instance of MLAppConnectConnectionProperties object.
 */
ML_STATIC_INLINE void MLAppConnectConnectionPropertiesInit(MLAppConnectConnectionProperties *inout_connection_properties) {
  if (inout_connection_properties) {
    inout_connection_properties->version = 1;
    inout_connection_properties->type = MLAppConnectConnectionType_Default;
    inout_connection_properties->priority = MLAppConnectPriority_Normal;
    inout_connection_properties->max_participants = MLAppConnectConnectionMaxParticipants;
    inout_connection_properties->lock_users = true;
    inout_connection_properties->lock_pipes = true;
  }
}

/*!
  \brief Pipe properties.

  \apilevel 8
 */
typedef struct MLAppConnectPipeProperties {
  /*! The pipe type to create a pipe. */
  MLAppConnectPipeType type;
  /*! The directionality (bi, uni-in, out) of the pipe. */
  MLAppConnectPipeDirection direction;
  /*! The priority level of this pipe (relative to connection). */
  MLAppConnectPriority priority;
} MLAppConnectPipeProperties;

/*!
  \brief Data Pipe properties.

  \apilevel 8
 */
typedef struct MLAppConnectDataPipeProperties {
  /*! Version of this structure. */
  uint32_t version;
  /*! The base pipe properties. */
  MLAppConnectPipeProperties base_pipe;
  /*! The reliability requirements of the pipe. */
  MLAppConnectPipeReliability reliability;
  /*! The packet ordering requirements of the pipe. */
  MLAppConnectPipeOrdering ordering;
  /*! The buffer size (memory allocation) for the pipe's queue(s). */
  size_t buffer_size;
} MLAppConnectDataPipeProperties;

/*!
  \brief Initializes a new instance of #MLAppConnectDataPipeProperties.

  \apilevel 8

  \param[in,out] inout_data_pipe_properties Instance of MLAppConnectDataPipeProperties object.
 */
ML_STATIC_INLINE void MLAppConnectDataPipePropertiesInit(MLAppConnectDataPipeProperties *inout_data_pipe_properties) {
  if (inout_data_pipe_properties) {
    inout_data_pipe_properties->version = 1;
    inout_data_pipe_properties->base_pipe.type = MLAppConnectPipeType_DataPipe;
    inout_data_pipe_properties->base_pipe.direction = MLAppConnectPipeDirection_BiDirectional;
    inout_data_pipe_properties->base_pipe.priority = MLAppConnectPriority_Normal;
    inout_data_pipe_properties->reliability = MLAppConnectPipeReliability_Reliable;
    inout_data_pipe_properties->ordering = MLAppConnectPipeOrdering_Ordered;
    inout_data_pipe_properties->buffer_size = 4096;
  }
}

/*!
  \brief Video pipe properties.

  \apilevel 8
 */
typedef struct MLAppConnectVideoPipeProperties {
  /*! Version of this structure. */
  uint32_t version;
  /*! The base pipe properties. */
  MLAppConnectPipeProperties base_pipe;
  /*! Preferred width of the frame. */
  uint32_t width;
  /*! Preferred height of the frame. */
  uint32_t height;
  /*! Maximum width supported by the pipe. It must be a multiple of 16. */
  uint32_t max_width;
  /*! Maximum height supported by the pipe. It must be a multiple of 16. */
  uint32_t max_height;
  /*! Rate at which frames are send in this pipe. */
  uint32_t framerate;
  /*! Video format. This is ignored for video capture pipe. */
  MLAppConnectVideoFormat format;
} MLAppConnectVideoPipeProperties;

/*!
  \brief Initializes a new instance of #MLAppConnectVideoPipeProperties.

  \apilevel 8

  \param[in,out] inout_video_pipe_properties Instance of #MLAppConnectVideoPipeProperties.
 */
ML_STATIC_INLINE void MLAppConnectVideoPipePropertiesInit(MLAppConnectVideoPipeProperties *inout_video_pipe_properties) {
  if (inout_video_pipe_properties) {
    inout_video_pipe_properties->version = 1;
    inout_video_pipe_properties->base_pipe.type = MLAppConnectPipeType_VideoPipe;
    inout_video_pipe_properties->base_pipe.direction = MLAppConnectPipeDirection_BiDirectional;
    inout_video_pipe_properties->base_pipe.priority = MLAppConnectPriority_Normal;
    inout_video_pipe_properties->width = 640;
    inout_video_pipe_properties->height = 480;
    inout_video_pipe_properties->max_width = 640;
    inout_video_pipe_properties->max_height = 480;
    inout_video_pipe_properties->framerate = 30;
    inout_video_pipe_properties->format = MLAppConnectVideoFormat_RGBA;
  }
}

/*!
  \brief Pipe event info.

  Information about a pipe from the connection event callback (#MLAppConnectEventCallback()).

  \apilevel 8
 */
typedef struct MLAppConnectPipeEventInfo {
  /*! Version of this structure. */
  uint32_t version;
  /*! Name of a pipe. */
  const char *pipe_name;
  /*! Owner of a pipe. */
  const char *pipe_owner;
  /*! Type of a pipe. */
  MLAppConnectPipeType pipe_type;
  /*! Direction of a pipe. */
  MLAppConnectPipeDirection pipe_direction;
  /*! Status of the pipe. */
  const char *status;
} MLAppConnectPipeEventInfo;

/*!
  \brief User event info.

  Information about a user from the connection event callback (#MLAppConnectEventCallback()).

  \apilevel 8
 */
typedef struct MLAppConnectUserEventInfo {
  /*! Version of this structure. */
  uint32_t version;
  /*! Name of the user for which the changes have occcured. */
  const char *user_name;
  /*! Status of the user. */
  const char *status;
} MLAppConnectUserEventInfo;

/*!
  \brief Microphone event info.

  Information about microphone from the connection event callback (#MLAppConnectEventCallback()).

  \apilevel 8
 */
typedef struct MLAppConnectMicrophoneEventInfo {
  /*! Version of this structure. */
  uint32_t version;
  /*! Name of the user for which the changes have occcured. */
  const char *user_name;
} MLAppConnectMicrophoneEventInfo;

/*!
  \brief Event info.

  Top level structure with information from the connection event callback (#MLAppConnectEventCallback()).
  It contains child strutures with specific information for each type of event.

  \apilevel 8
 */
typedef struct MLAppConnectEventInfo {
  /*! Version of this structure. */
  uint32_t version;
  /*! Event type. */
  MLAppConnectConnectionEventType event;
  /*! Connection name. */
  const char *connection_name;
  /*! Pipe Information. */
  MLAppConnectPipeEventInfo *pipe_info;
  /*! User Information. */
  MLAppConnectUserEventInfo *user_info;
  /*! Microphone Information. */
  MLAppConnectMicrophoneEventInfo *microphone_info;
} MLAppConnectEventInfo;

/*!
  \brief Key-value pair.

  This can be used to create key-value maps.

  \apilevel 8
 */
typedef struct MLAppConnectKeyValue {
  /*! Key. */
  char *key;
  /*! Value. */
  char *value;
} MLAppConnectKeyValue;

/*!
  \brief Invite callback.

  Parameters to be used by #MLAppConnectRegisterInviteCallback().
  It contains a callback function and a custom user data to be returned as argument in the callback itself.

  \apilevel 8
 */
typedef struct MLAppConnectInviteCallback {
  /*! Version of this structure. */
  uint32_t version;
  /*!
    \brief Connection callback function.

    \param[in] connection_name Name of the connection.
    \param[in] out_arguments A list of arguments.
    \param[in] out_count Number of arguments in the list.
    \param[in] user_data User provided data.

    \attention All resources in event_info will be cleaned up once the callback returns.
  */
  void (*callback_function)(const char *connection_name, MLAppConnectKeyValue arguments[], uint32_t arguments_count, void *user_data);
  /*! User provided data. */
  void *user_data;
} MLAppConnectInviteCallback;

/*!
  \brief Initializes a new instance of #MLAppConnectInviteCallback.

  \apilevel 8

  \param[in,out] inout_invite_callback Instance of #MLAppConnectInviteCallback.
 */
ML_STATIC_INLINE void MLAppConnectInviteCallbackInit(MLAppConnectInviteCallback *inout_invite_callback) {
  if (inout_invite_callback) {
    inout_invite_callback->version = 1;
    inout_invite_callback->callback_function = NULL;
    inout_invite_callback->user_data = NULL;
  }
}

/*!
  \brief Connection callback.

  Parameters to be used by #MLAppConnectRegisterConnectionCallback().
  It contains a callback function and a custom user data to be returned as argument in the callback itself.

  \apilevel 8
 */
typedef struct MLAppConnectConnectionCallback {
  /*! Version of this structure. */
  uint32_t version;
  /*!
    \brief Connection callback function.

    \param[in] result The result status of the connection.
    \param[in] connection_name Name of the connection.
    \param[in] user_data User provided data.

    \attention All resources in event_info will be cleaned up once the callback returns.
  */
  void (*callback_function)(MLResult result, const char *connection_name, void *user_data);
  /*! User provided data. */
  void *user_data;
} MLAppConnectConnectionCallback;

/*!
  \brief Initializes a new instance of #MLAppConnectConnectionCallback.

  \apilevel 8

  \param[in,out] inout_connection_callback Instance of #MLAppConnectConnectionCallback.
 */
ML_STATIC_INLINE void MLAppConnectConnectionCallbackInit(MLAppConnectConnectionCallback *inout_connection_callback) {
  if (inout_connection_callback) {
    inout_connection_callback->version = 1;
    inout_connection_callback->callback_function = NULL;
    inout_connection_callback->user_data = NULL;
  }
}

/*!
  \brief Connection event callback.

  Parameters to be used by #MLAppConnectRegisterEventCallback().
  It contains a callback function and a custom user data to be returned as argument in the callback itself.

  \apilevel 8
 */
typedef struct MLAppConnectEventCallback {
  /*! Version of this structure. */
  uint32_t version;
  /*!
    \brief Connection event callback function.

    \param[in] event_info Information related to the event.
    \param[in] user_data User provided data.

    \attention All resources in event_info will be cleaned up once the callback returns.
  */
  void (*callback_function)(MLAppConnectEventInfo *event_info, void *user_data);
  /*! User provided data. */
  void *user_data;
} MLAppConnectEventCallback;

/*!
  \brief Initializes a new instance of #MLAppConnectEventCallback.

  \apilevel 8

  \param[in,out] inout_event_callback Instance of #MLAppConnectEventCallback.
 */
ML_STATIC_INLINE void MLAppConnectEventCallbackInit(MLAppConnectEventCallback *inout_event_callback) {
  if (inout_event_callback) {
    inout_event_callback->version = 1;
    inout_event_callback->callback_function = NULL;
    inout_event_callback->user_data = NULL;
  }
}

/*!
  \brief MLAppConnectReadData parameters.

  Information about the received data from #MLAppConnectReadData() and the amount of bytes to read.

  \apilevel 8
 */
typedef struct MLAppConnectReadDataParameters {
  /*! Version of this structure. */
  uint32_t version;
  /* The minimum number of bytes to read before returning. */
  uint32_t min_bytes;
  /* The size of the data that was read. */
  uint64_t out_data_size;
  /* The data read from the pipe. */
  uint8_t *out_data;
} MLAppConnectReadDataParameters;

/*!
  \brief Initializes a new instance of #MLAppConnectReadDataParameters.

  \apilevel 8

  \param[in,out] inout_read_data_parameters Instance of #MLAppConnectReadDataParameters.
 */
ML_STATIC_INLINE void MLAppConnectReadDataParametersInit(MLAppConnectReadDataParameters *inout_read_data_parameters) {
  if (inout_read_data_parameters) {
    inout_read_data_parameters->version = 1;
    inout_read_data_parameters->min_bytes = 0;
    inout_read_data_parameters->out_data_size = 0;
    inout_read_data_parameters->out_data = NULL;
  }
}

/*!
  \brief MLAppConnectReadLargeDataParameters parameters.

  Information about the received data from #MLAppConnectReadLargeDataParameters().

  \apilevel 8
 */
typedef struct MLAppConnectReadLargeDataParameters {
  /*! Version of this structure. */
  uint32_t version;
  /* The number of bytes to read from the pipe. */
  uint64_t out_data_size;
  /* Address of data read from the pipe. */
  uint8_t *out_data;
  /* The number of packets remaining. */
  uint64_t out_data_packets_remaining;
} MLAppConnectReadLargeDataParameters;

/*!
  \brief Initializes a new instance of #MLAppConnectReadLargeDataParameters.

  \apilevel 8

  \param[in,out] inout_read_large_data_parameters Instance of #MLAppConnectReadLargeDataParameters.
 */
ML_STATIC_INLINE void MLAppConnectReadLargeDataParametersInit(MLAppConnectReadLargeDataParameters *inout_read_large_data_parameters) {
  if (inout_read_large_data_parameters) {
    inout_read_large_data_parameters->version = 1;
    inout_read_large_data_parameters->out_data_size = 0;
    inout_read_large_data_parameters->out_data = NULL;
    inout_read_large_data_parameters->out_data_packets_remaining = 0;
  }
}

/*!
  \brief Register invite callback.

  \apilevel 8

  \param[in] invite_callback The callback function.
  \param[out] out_callback_handle a callback handle, used in #MLAppConnectUnregisterInviteCallback()

  \retval MLResult_Ok if successfully registered, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectRegisterInviteCallback(const MLAppConnectInviteCallback *invite_callback,
                                                           MLHandle *out_callback_handle);

/*!
  \brief Unregister connection callback.

  \apilevel 8

  \param[in] callback_handle The callback handle obtained by #MLAppConnectRegisterInviteCallback().

  \retval MLResult_Ok if successfully unregistered, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess

  \attention This function should be called after ceasing to use connection related functions.
 */
ML_API MLResult ML_CALL MLAppConnectUnregisterInviteCallback(MLHandle callback_handle);

/*!
  \brief Register connection callback.

  \apilevel 8

  \param[in] connection_callback The callback function.
  \param[out] out_callback_handle a callback handle, used in #MLAppConnectUnregisterConnectionCallback()

  \retval MLResult_Ok if successfully registered, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectRegisterConnectionCallback(const MLAppConnectConnectionCallback *connection_callback,
                                                               MLHandle *out_callback_handle);

/*!
  \brief Unregister connection callback.

  \apilevel 8

  \param[in] callback_handle The callback handle obtained by #MLAppConnectRegisterConnectionCallback().

  \retval MLResult_Ok if successfully unregistered, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess

  \attention This function should be called after ceasing to use connection related functions.
 */
ML_API MLResult ML_CALL MLAppConnectUnregisterConnectionCallback(MLHandle callback_handle);

/*!
  \brief Create connection.

  Initial step to create a new connection.

  Invoking this function will start by displaying the "invite dialog" interface in the "initiator device".
  The user will be able to invite one or more of their users to the connection. Each one of the invited
  user will then receive a system notification to be accepted or declined.

  A connection callback (registered by #MLAppConnectRegisterConnectionCallback()) should be
  set up before calling this function.

  \apilevel 8

  \param[in] connection_name The name of the connection to create (used for identification while
             managing multiple connections).
  \param[in] connection_properties The properties of the connection to create from #MLAppConnectConnectionProperties.

  \retval MLResult_Ok if successfully created a connection, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectCreateConnection(const char *connection_name,
                                                     const MLAppConnectConnectionProperties *connection_properties);

/*!
  \brief Cojoin connection.

  Once a user is invited to join some specific connection, a system notification will be displayed
  and the user will be able to accept or decline.

  If the invite is accepted, the corresponding app will load and be ready to join the connection.
  The callback #MLAppConnectInviteCallback will then be triggered and bring any information related
  to the connection. In special, the "connection name" used by #MLAppConnectCojoinConnection().

  A connection callback (registered by #MLAppConnectRegisterConnectionCallback()) should be
  set up before calling this function.

  \apilevel 8

  \param[in] connection_name The name of the connection to cojoin.

  \retval MLResult_Ok if successfully cojoined a connection, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectCojoinConnection(const char *connection_name);

/*!
  \brief Invite users.

  This function is similar #MLAppConnectCreateConnection(), but works with an ongoing connection.
  It will also display the "invite dialog" and allow the user to invite more contacts.

  A connection event callback (registered by #MLAppConnectRegisterEventCallback()) should be
  set up before calling this function.

  \apilevel 8

  \param[in] connection_name The name of the ongoing connection.

  \retval MLResult_Ok if successful, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectInviteUsers(const char *connection_name);

/*!
  \brief Delete connection.

  \apilevel 8

  \param[in] connection_name The name of the connection to delete.

  \retval MLResult_Ok if successfully deleted a connection, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectDeleteConnection(const char *connection_name);

/*!
  \brief Register connection event callback.

  \apilevel 8

  \param[in] connection_name Name of the connection to operate on.
  \param[in] event_callback Event callback function.
  \param[out] out_callback_handle A callback handle, used in #MLAppConnectUnregisterEventCallback().

  \retval MLResult_Ok if successfully registered, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectRegisterEventCallback(const char *connection_name,
                                                          const MLAppConnectEventCallback *event_callback,
                                                          MLHandle *out_callback_handle);

/*!
  \brief Unregister connection event callback.

  \apilevel 8

  \param[in] connection_name The connection name.
  \param[in] callback_handle The callback handle obtained by #MLAppConnectRegisterEventCallback().

  \retval MLResult_Ok if successfully unregistered, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectUnregisterEventCallback(const char *connection_name, MLHandle callback_handle);

/*!
  \brief Create data pipe.

  Creates a "general purpose" data pipe. The functions #MLAppConnectSendData() and #MLAppConnectReadData() should
  be used to write to and read from this pipe.

  \apilevel 8

  \param[in] connection_name The name of the connection to add the pipe.
  \param[in] pipe_name The name of the pipe to create.
  \param[in] pipe_properties The data pipe properties for this pipe.

  \retval MLResult_Ok if successfully created a pipe, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectCreateDataPipe(const char *connection_name,
                                                   const char *pipe_name,
                                                   const MLAppConnectDataPipeProperties *pipe_properties);

/*!
  \brief Create video pipe.

  Creates a "video specialized" data pipe. The functions #MLAppConnectSendVideoFrame() and
  #MLAppConnectReadVideoFrame() should be used to write to and read from this pipe.

  \apilevel 8

  \param[in] connection_name The name of the connection to add the pipe.
  \param[in] pipe_name The name of the pipe to create.

  \retval MLResult_Ok if successfully created a pipe, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectCreateVideoPipe(const char *connection_name,
                                                    const char *pipe_name,
                                                    const MLAppConnectVideoPipeProperties *video_pipe_properties);

/*!
  \brief Delete pipe.

  This function should be called to delete a "data pipe" or a "video pipe" or an "audio pipe".

  This function should NOT be used to delete "video capture pipe" or "mic audio pipe".
  For these, use #MLAppConnectDeleteVideoCapturePipe() and #MLAppConnectDeleteMicAudioPipe() instead.

  \apilevel 8

  \param[in] connection_name The name of the connection to delete the pipe from.
  \param[in] pipe_name The name of the pipe to delete.

  \retval MLResult_Ok if successfully deleted the pipe, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectDeletePipe(const char *connection_name, const char *pipe_name);

/*!
  \brief Create video capture pipe.

  Creates a "video capture specialized" data pipe.
  No function should be used to write to this pipe. It is managed and fed automatically with frames
  from the device's camera.

  The other peers of the connection should use #MLAppConnectReadVideoFrame() to read the video frames
  from this pipe. From the receiver side, a "video capture pipe" is no different than a "regular video pipe".

  \apilevel 8

  \param[in] connection_name The name of the connection to add the pipe.

  \retval MLResult_Ok if successfully created a pipe, error code from #MLAppConnectResult otherwise.

  \attention User has to explicitly grant access to the device's camera before calling this function.
             Check the Privilege-checking system API.

  \priv ConnectionVideoCaptureStreaming
 */
ML_API MLResult ML_CALL MLAppConnectCreateVideoCapturePipe(const char *connection_name,
                                                           const MLAppConnectVideoPipeProperties *video_pipe_properties);

/*!
  \brief Delete video capture pipe.

  \apilevel 8

  \param[in] connection_name The name of the connection to delete the pipe from.

  \retval MLResult_Ok if successfully deleted a pipe, error code from #MLAppConnectResult otherwise.

  \priv ConnectionVideoCaptureStreaming
 */
ML_API MLResult ML_CALL MLAppConnectDeleteVideoCapturePipe(const char *connection_name);

/*!
  \brief Create mic audio pipe.

  Add an audio pipe (managed internally) for the device microphone to the connection.

  \apilevel 8

  \param[in] connection_name The name of the connection to add the pipe.

  \retval MLResult_Ok if successfully created a pipe, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAudioCaptureStreaming
 */
ML_API MLResult ML_CALL MLAppConnectCreateMicAudioPipe(const char *connection_name);

/*!
  \brief Delete mic audio pipe.

  Delete the audio pipe (internally managed) for the device microphone from the connection.

  \apilevel 8

  \param[in] connection_name The name of the connection to delete the pipe from.

  \retval MLResult_Ok if successfully deleted a pipe, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAudioCaptureStreaming
 */
ML_API MLResult ML_CALL MLAppConnectDeleteMicAudioPipe(const char *connection_name);

/*!
  \brief Mute mic.

  Mute the microphone for the connection.

  \apilevel 8

  \param[in] connection_name The name of the connection.

  \retval MLResult_Ok if successfully muted a pipe, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAudioCaptureStreaming
 */
ML_API MLResult ML_CALL MLAppConnectMuteMic(const char *connection_name);

/*!
  \brief Unmute mic.

  Unmute the microphone for the connection.

  \apilevel 8

  \param[in] connection_name The name of the connection.

  \retval MLResult_Ok if successfully unmmuted a pipe, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAudioCaptureStreaming
 */
ML_API MLResult ML_CALL MLAppConnectUnmuteMic(const char *connection_name);

/*!
  \brief Is mic Muted.

  Check if the microphone is muted in the connection.

  \apilevel 8

  \param[in] connection_name The name of the connection.
  \param[out] out_mic_muted A flag that indicates if microphone is muted.

  \retval MLResult_Ok if successfully returned the value, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAudioCaptureStreaming
 */
ML_API MLResult ML_CALL MLAppConnectIsMicMuted(const char *connection_name, bool *out_mic_muted);

/*!
  \brief Mute user.

  Mute audio from another user.

  \apilevel 8

  \param[in] connection_name The name of the connection.
  \param[in] user The user name.

  \retval MLResult_Ok if successfully returned the value, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectMuteUser(const char *connection_name, const char *user);

/*!
  \brief Unmute user.

  Unmute audio from another user.

  \apilevel 8

  \param[in] connection_name The name of the connection.
  \param[in] user The user name.

  \retval MLResult_Ok if successfully returned the value, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectUnmuteUser(const char *connection_name, const char *user);

/*!
  \brief Get a list of strings with the pipe names.

  \apilevel 8

  \param[in] connection_name The name of the connection.
  \param[out] out_pipe_names The list of pipe names.
  \param[out] out_count Number of elements in the list of pipe names.

  \retval MLResult_Ok if successfully returned the value, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectGetPipeNames(const char *connection_name,
                                                 char **out_pipe_names[],
                                                 uint32_t *out_count);

/*!
  \brief Free a list of pipe names.

  Deallocates a list of pipe names returned by #MLAppConnectGetPipeNames().

  \apilevel 8

  \param[in] pipe_names An array of C-Strings containing the list of pipe names.
  \param[in] count Number of elements in the array.

  \retval MLResult_Ok if successful in getting the value, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectFreePipeNames(char *pipe_names[], uint32_t count);

/*!
  \brief Send data over a pipe.

  This function should be called to send data over a pipe.

  \apilevel 8

  \param[in] connection_name The name of the connection to send the data.
  \param[in] pipe_name The name of the pipe to send the data.
  \param[in] data The data to be sent.
  \param[in] data_size The size of the data to be sent.

  \retval MLResult_Ok if successfully sent data, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectSendData(const char *connection_name,
                                             const char *pipe_name,
                                             const uint8_t *data,
                                             const uint64_t data_size);

/*!
  \brief Send Large data over a pipe.

  Send large data (>IP MTU) to a pipe of this connection.

  \apilevel 8

  \param[in] connection_name The name of the connection to send the data.
  \param[in] pipe_name The name of the pipe to send the data.
  \param[in] data The data to be sent.
  \param[in] data_size The size of the data to be sent.

  \retval MLResult_Ok if successfully sent data, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectSendLargeData(const char *connection_name,
                                                  const char *pipe_name,
                                                  const uint8_t *data,
                                                  const uint64_t data_size);

/*!
  \brief Send video frame.

  This function should be called to send video data over a pipe.

  \apilevel 8

  \param[in] connection_name The name of the connection to send the data.
  \param[in] pipe_name The name of the pipe to send the data.
  \param[in] video_frame The video frame.

  \retval MLResult_Ok if successfully sent data, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectSendVideoFrame(const char *connection_name,
                                                   const char *pipe_name,
                                                   const MLImage *video_frame);

/*!
  \brief Read data.

  This function will block the execution until at least #min_bytes are read from the pipe.
  #min_bytes can also be set to 0 and this function will return immediately with or without
  any incoming data.

  The receiving data must be freed using #MLAppConnectFreeData().

  \apilevel 8

  \param[in] connection_name The name of the connection to read the data.
  \param[in] pipe_name The name of the pipe to read the data.
  \param[in,out] inout_read_data_parameters Information about the received data and the amount of bytes to read.

  \retval MLResult_Ok if successfully read data, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectReadData(const char *connection_name,
                                             const char *pipe_name,
                                             MLAppConnectReadDataParameters *inout_read_data_parameters);

/*!
  \brief Free data.

  Free data buffers allocated by MLAppConnectReadData()

  \param[in] data Address of a data buffer.

  \retval MLResult_Ok if successfully deallocates the data, MLResult_UnspecifiedFailure otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectFreeData(const uint8_t *data);

/*!
  \brief Read Large.

  Read "large" data from a pipe of this connection.
  The receiving data must be freed using #MLAppConnectFreeLargeData().

  \apilevel 8

  \param[in] connection_name The name of the connection to read the data.
  \param[in] pipe_name The name of the pipe to read the data.
  \param[in,out] inout_read_large_data_parameters Information about the received data.

  \retval MLResult_Ok if successfully read data, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectReadLargeData(const char *connection_name,
                                                  const char *pipe_name,
                                                  MLAppConnectReadLargeDataParameters *out_read_large_data_parameters);

/*!
  \brief Free large data

  Free data buffers allocated by MLAppConnectReadLargeData()

  \param[in] data Address of a data buffer.

  \retval MLResult_Ok if successfully deallocates the data, MLResult_UnspecifiedFailure otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectFreeLargeData(const uint8_t *data);

/*!
  \brief Read a video frame from a video pipe.

  This will fill up out_video_frame structure with data related to the video frame.
  The data can be cleaned by #MLAppConnectFreeVideoFrame() after already used.

  \apilevel 8

  \param[in] connection_name The name of the connection to read the data.
  \param[in] pipe_name The name of the pipe to read the data.
  \param[in] wait_for_next_frame Number of frames to read.
  \param[out] out_video_frame Pointer to an existing MLImage structure.

  \retval MLResult_Ok if successfully read data, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectReadVideoFrame(const char *connection_name,
                                                   const char *pipe_name,
                                                   bool wait_for_next_frame,
                                                   MLImage *out_video_frame);

/*!
  \brief Free video frame

  Free data buffers allocated by #MLAppConnectReadVideoFrame().

  \param[in] in_video_frame Address of a data buffer.

  \retval MLResult_Ok if successfully deallocates the data, MLResult_UnspecifiedFailure otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectFreeVideoFrame(MLImage *in_video_frame);

/*!
  \brief Unblocks a blocked pipe.

  Pipes can be blocked by #MLAppConnectReadData(), #MLAppConnectReadLargeData() or #MLAppConnectReadVideoFrame().
  This happens, for example, when the application requests to read an amount of bytes and this amount isn't avaible yet.
  In this scenario the reader function will block the execution. Another thread can then use #MLAppConnectUnblockRead()
  to unblock it.

  \apilevel 8

  \param[in] connection_name The connection name.
  \param[in] pipe_name The pipe name.

  \retval MLResult_Ok if successfully unblocks read data, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectUnblockRead(const char *connection_name, const char *pipe_name);

/*!
  \brief Get Participants.

  Get a list of participants in an ongoing connection.

  \apilevel 8

  \param[in] connection_name The name of the connection.
  \param[out] out_participants The list of participants in this connection.
  \param[out] out_count Number of entries in the out_participants list.

  \retval MLResult_Ok if successfully able to get the participants, error code from #MLAppConnectResult otherwise.
  otherwise.

  \priv ConnectionAccess

  \attention out_participants is allocated by this function. Call free function MLAppConnectFreeParticipants() after usage.
 */
ML_API MLResult ML_CALL MLAppConnectGetParticipants(const char *connection_name,
                                                    char **out_participants[],
                                                    uint32_t *out_count);

/*!
  \brief Free a list of participants.

  Deallocates a list of participants returned by #MLAppConnectGetParticipants().

  \apilevel 8

  \param[in] participants_list An array of C-Strings containing the list of participants.
  \param[in] count Number of elements in the array.

  \retval MLResult_Ok if successful in getting the value, error code from #MLAppConnectResult otherwise.

  \priv ConnectionAccess
 */
ML_API MLResult ML_CALL MLAppConnectFreeParticipants(char *participants_list[], uint32_t count);

/*!
  \brief Returns an ASCII string for MLAppConnectResult/MLResultGlobal codes.

  \param[in] result_code The input MLAppConnectResult/MLResultGlobal result code.

  \return ASCII string containing readable version of result code.

  \priv None
 */
ML_API const char *ML_CALL MLAppConnectGetResultString(MLResult result_code);

ML_EXTERN_C_END
