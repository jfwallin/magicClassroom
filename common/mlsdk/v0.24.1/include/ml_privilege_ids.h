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

/*!
  \addtogroup Privileges Privileges
  \{
*/
typedef enum MLPrivilegeID {
    /*! Invalid privilege */
    MLPrivilegeID_Invalid = 0,

    /*!
      <b>Description:</b> Access a manually selected subset of contacts from address book<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 3
    */
    MLPrivilegeID_AddressBookBasicAccess = 305,

    /*!
      <b>Description:</b> Full read and search access to address book contacts<br/>
      <b>Type:</b> \c sensitive<br/>
      <b>Category:</b> \c Contacts
      \apilevel 2
    */
    MLPrivilegeID_AddressBookRead = 1,

    /*!
      <b>Description:</b> Ability to add, modify and delete address book contacts<br/>
      <b>Type:</b> \c sensitive<br/>
      <b>Category:</b> \c Contacts
      \apilevel 1
    */
    MLPrivilegeID_AddressBookWrite = 2,

    /*!
      <b>Description:</b> Open a microphone stream of the users voice or the ambient surroundings<br/>
      <b>Type:</b> \c reality<br/>
      <b>Category:</b> \c Microphone
      \apilevel 1
    */
    MLPrivilegeID_AudioCaptureMic = 49,

    /*!
      <b>Description:</b> Deprecated and scheduled for removal<br/>
      <b>Type:</b> \c reality<br/>
      <b>Category:</b> \c Microphone
      \apilevel 1
    */
    MLPrivilegeID_AudioRecognizer = 13,

    /*!
      <b>Description:</b> Download in the background<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_BackgroundDownload = 120,

    /*!
      <b>Description:</b> Upload in the background<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_BackgroundUpload = 121,

    /*!
      <b>Description:</b> Query battery status/percentage<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_BatteryInfo = 18,

    /*!
      <b>Description:</b> Access to Bluetooth adapter from external app<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 6
    */
    MLPrivilegeID_BluetoothAdapterExternalApp = 362,

    /*!
      <b>Description:</b> Limited access to bluetooth adapter service<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 6
    */
    MLPrivilegeID_BluetoothAdapterUser = 106,

    /*!
      <b>Description:</b> Write-Access bluetooth Generic Attribute Profile service<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 6
    */
    MLPrivilegeID_BluetoothGattWrite = 108,

    /*!
      <b>Description:</b> Take pictures and videos using camera<br/>
      <b>Type:</b> \c reality<br/>
      <b>Category:</b> \c Camera
      \apilevel 1
    */
    MLPrivilegeID_CameraCapture = 26,

    /*!
      <b>Description:</b> Get coarse location of the device.<br/>
      <b>Type:</b> \c sensitive<br/>
      <b>Category:</b> \c Location
      \apilevel 4
    */
    MLPrivilegeID_CoarseLocation = 323,

    /*!
      <b>Description:</b> SDK access CV related info from graph_pss<br/>
      <b>Type:</b> \c reality<br/>
      <b>Category:</b> \c CVCamera
      \apilevel 5
    */
    MLPrivilegeID_ComputerVision = 343,

    /*!
      <b>Description:</b> Connect to Background Music Service<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_ConnectBackgroundMusicService = 192,

    /*!
      <b>Description:</b> Access ACP connection APIs<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 8
    */
    MLPrivilegeID_ConnectionAccess = 350,

    /*!
      <b>Description:</b> Access ACP microphone audio streaming APIs<br/>
      <b>Type:</b> \c reality<br/>
      <b>Category:</b> \c Microphone
      \apilevel 8
    */
    MLPrivilegeID_ConnectionAudioCaptureStreaming = 351,

    /*!
      <b>Description:</b> Access ACP video capture streaming APIs<br/>
      <b>Type:</b> \c reality<br/>
      <b>Category:</b> \c Camera
      \apilevel 8
    */
    MLPrivilegeID_ConnectionVideoCaptureStreaming = 352,

    /*!
      <b>Description:</b> Access controller pose data<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c Perception
      \apilevel 1
    */
    MLPrivilegeID_ControllerPose = 263,

    /*!
      <b>Description:</b> Register with and use the Desktop Companion<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 7
    */
    MLPrivilegeID_DesktopCompanion = 369,

    /*!
      <b>Description:</b> Access shared documents<br/>
      <b>Type:</b> \c trusted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_Documents = 175,

    /*!
      <b>Description:</b> Provision and use DRM certficates<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_DrmCertificates = 51,

    /*!
      <b>Description:</b> Get user expression data for avatar animation<br/>
      <b>Type:</b> \c sensitive<br/>
      <b>Category:</b> \c AvatarExpressions
      \apilevel 8
    */
    MLPrivilegeID_Expressions = 312,

    /*!
      <b>Description:</b> Get fine location of the device.<br/>
      <b>Type:</b> \c reality<br/>
      <b>Category:</b> \c Location
      \apilevel 7
    */
    MLPrivilegeID_FineLocation = 367,

    /*!
      <b>Description:</b> Set/Get gesture configuration<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c Perception
      \apilevel 1
    */
    MLPrivilegeID_GesturesConfig = 269,

    /*!
      <b>Description:</b> Subscribe to gesture hand mask and config data<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c Perception
      \apilevel 1
    */
    MLPrivilegeID_GesturesSubscribe = 268,

    /*!
      <b>Description:</b> Access hand mesh features<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c Perception
      \apilevel 6
    */
    MLPrivilegeID_HandMesh = 315,

    /*!
      <b>Description:</b> Read user profile attributes<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c Identity
      \apilevel 1
    */
    MLPrivilegeID_IdentityRead = 113,

    /*!
      <b>Description:</b> Use the in-app purchase mechanism<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_InAppPurchase = 42,

    /*!
      <b>Description:</b> Access the internet (network nodes in other subnets as well as the gateway)<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c Internet
      \apilevel 1
    */
    MLPrivilegeID_Internet = 96,

    /*!
      <b>Description:</b> Access the local area network (other network nodes in the same subnet)<br/>
      <b>Type:</b> \c sensitive<br/>
      <b>Category:</b> \c LocalAreaNetwork
      \apilevel 1
    */
    MLPrivilegeID_LocalAreaNetwork = 171,

    /*!
      <b>Description:</b> Access Low Latency data from the Lightwear<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c Perception
      \apilevel 1
    */
    MLPrivilegeID_LowLatencyLightwear = 59,

    /*!
      <b>Description:</b> Access Media Services<br/>
      <b>Type:</b> \c trusted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_Media = 130,

    /*!
      <b>Description:</b> Access Media Drm Services<br/>
      <b>Type:</b> \c trusted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_MediaDrm = 129,

    /*!
      <b>Description:</b> Access Content Management media information<br/>
      <b>Type:</b> \c trusted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_MediaMetadata = 140,

    /*!
      <b>Description:</b> Access Music Service functionality<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_MusicService = 218,

    /*!
      <b>Description:</b> Post notifications for users to see, dismiss own notifications, listen for own notification events<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c Notifications
      \apilevel 1
    */
    MLPrivilegeID_NormalNotificationsUsage = 208,

    /*!
      <b>Description:</b> Access found object data from object-recognition pipeline<br/>
      <b>Type:</b> \c sensitive<br/>
      <b>Category:</b> \c ObjectData
      \apilevel 8
    */
    MLPrivilegeID_ObjectData = 394,

    /*!
      <b>Description:</b> Deprecated and scheduled for removal<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_Occlusion = 52,

    /*!
      <b>Description:</b> Read Persistent Coordinate Frames from Passable World<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 6
    */
    MLPrivilegeID_PcfRead = 201,
    MLPrivilegeID_PwFoundObjRead = 201,

    /*!
      <b>Description:</b> Get power information<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_PowerInfo = 150,

    /*!
      <b>Description:</b> Register with Background Music Service<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_RegisterBackgroundMusicService = 193,

    /*!
      <b>Description:</b> Create channels in the screens framework'<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_ScreensProvider = 264,

    /*!
      <b>Description:</b> Request a secure browser window<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 6
    */
    MLPrivilegeID_SecureBrowserWindow = 357,

    /*!
      <b>Description:</b> Ability to initiate invites to Social connections<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 5
    */
    MLPrivilegeID_SocialConnectionsInvitesAccess = 329,

    /*!
      <b>Description:</b> Select access to social connections<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 8
    */
    MLPrivilegeID_SocialConnectionsSelectAccess = 372,

    /*!
      <b>Description:</b> Receive voice input<br/>
      <b>Type:</b> \c reality<br/>
      <b>Category:</b> \c Microphone
      \apilevel 1
    */
    MLPrivilegeID_VoiceInput = 173,

    /*!
      <b>Description:</b> Get Wifi status<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 5
    */
    MLPrivilegeID_WifiStatusRead = 344,

    /*!
      <b>Description:</b> Access world reconstruction features (meshes, planes, raycast)<br/>
      <b>Type:</b> \c autogranted<br/>
      <b>Category:</b> \c None
      \apilevel 1
    */
    MLPrivilegeID_WorldReconstruction = 33,


    /*! Ensure that the enum is 32 bits */
    MLPrivilegeID_Ensure32Bits = 0x7FFFFFFF,
} MLPrivilegeID;
/*! \} */
