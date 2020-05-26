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
#include "ml_graphics.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Remote
  \sharedobject ml_remote
  \brief APIs for the ML Remote on Host OS Platforms.

  \attention These APIs are meant to enable a better integration to ML Remote for
             engines running on Host OS Platforms. The library is not available on
             device. On Host OS Platforms, most APIs are provided by the ml_remote
             library, with some being extensions of other existing modules.
             MLRemote* - symbols located in ml_remote module
             MLRemoteGraphics* - Symbols located in ml_graphics module
  \{
*/

/*!
  \brief Checks to see if ML Remote server is running.

  \param[out] out_is_configured Pointer to a bool indicating whether the remote
              server is running and is configured properly

  \retval MLResult_InvalidParam is_configured parameter is not valid (null).
  \retval MLResult_Ok If query was successful.
  \retval MLResult_Timeout The ML Remote server could not be reached.
  \retval MLResult_UnspecifiedFailure There was an unknown error submitting the query.

  \priv None
*/

ML_API MLResult ML_CALL MLRemoteIsServerConfigured(bool *out_is_configured);

#ifdef VK_VERSION_1_0
/*!
  \brief Returns a list of required VkInstance extensions.

  If out_required_extension_properties is null then the number of required extension is returned in out_extension_property_count.
  Otherwise, out_extension_property_count must point to a variable set to the number of elements in the out_required_extension_properties
  array, and on return the variable is overwritten with the number of strings actually written to out_required_extension_properties.

  \param[out] out_required_extension_properties Either null or a pointer to an array of VkExtensionProperties.
  \param[out] out_extension_property_count Is a pointer to an integer related to the number of extensions required or queried.

  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok If query was successful.
  \retval MLResult_UnspecifiedFailure There was an unknown error submitting the query.

  \priv None
*/
ML_API MLResult ML_CALL MLRemoteEnumerateRequiredVkInstanceExtensions(VkExtensionProperties *out_required_extension_properties, uint32_t *out_extension_property_count);

/*!
  \brief Returns a list of required VkDevice extensions.

  If out_required_extension_properties is null then the number of required extension is returned in out_extension_property_count.
  Otherwise, out_extension_property_count must point to a variable set to the number of elements in the out_required_extension_properties
  array, and on return the variable is overwritten with the number of strings actually written to out_required_extension_properties.

  \param[out] out_required_extension_properties Either null or a pointer to an array of VkExtensionProperties.
  \param[out] out_extension_property_count Is a pointer to an integer related to the number of extensions required or queried.

  \retval MLResult_InvalidParam If input parameter is invalid.
  \retval MLResult_Ok If query was successful.
  \retval MLResult_UnspecifiedFailure There was an unknown error submitting the query.

  \priv None
*/
ML_API MLResult ML_CALL MLRemoteEnumerateRequiredVkDeviceExtensions(VkExtensionProperties *out_required_extension_properties, uint32_t *out_extension_property_count);
#endif

#ifdef __OBJC__
#import <Metal/Metal.h>

/*!
  \brief The client creation parameters for MLRemoteGraphicsCreateClientMTL().
  \apilevel 5
*/
typedef struct MLRemoteGraphicsCreateClientMTLParams {
  /*! Struct API version. */
  uint32_t version;
  /*! The graphics options for resource creation. */
  MLGraphicsOptions *options;
  /*! A pointer to a id<MTLDevice>, the Metal(R) device to use for resource creation. */
  id<MTLDevice> *metal_device;
} MLRemoteGraphicsCreateClientMTLParams;

/*!
  \brief Initializes a MLRemoteGraphicsCreateClientMTLParams with the default parameters.
  \apilevel 5

  \param[in] params The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLRemoteGraphicsCreateClientMTLParamsInit(MLRemoteGraphicsCreateClientMTLParams *params) {
  if (!params) return;
  params->version      = 1;
  params->options      = NULL;
  params->metal_device = NULL;
}

/*!
  \brief Create the Metal(R) graphics client for the macOS(R) platform integration only.

  Create a graphics client for use with Metal(R) API.
  The created client handle complies the usage pattern defined in ml_graphics.h on macOS(R) platform.

  \apilevel 5

  \param[in] params The graphics parameters for client creation.
  \param[out] out_graphics_client The handle to the created graphics client. Only valid if result is MLResult_Ok.

  \retval MLResult_InvalidParam Graphics Client was not created due to an invalid parameter.
  \retval MLResult_Ok Graphics Client was created successfully.
  \retval MLResult_UnspecifiedFailure Graphics Client was not created due to an unknown error.

  \priv None
*/
ML_API MLResult ML_CALL MLRemoteGraphicsCreateClientMTL(const MLRemoteGraphicsCreateClientMTLParams *params, MLHandle *out_graphics_client);

/*!
  \brief Convert MLSurfaceFormat to Metal(R) internal format.
  \apilevel 5

  \param[in] format The MLSurfaceFormat to convert.

  \return The returned internal format.

  \priv None
*/
ML_API MTLPixelFormat ML_CALL MLRemoteGraphicsMTLFormatFromMLSurfaceFormat(MLSurfaceFormat format);

/*!
  \brief Convert Metal(R) internal format to MLSurfaceFormat.
  \apilevel 5

  \param[in] format The internal format to convert.

  \return The returned MLSurfaceFormat.

  \priv None
*/
ML_API MLSurfaceFormat ML_CALL MLRemoteGraphicsMLSurfaceFormatFromMTLFormat(MTLPixelFormat format);

/*!
  \brief Signal the sync object provided for each virtual camera for Metal(R) clients.

  Required for each frame, after rendering each virtual camera (or multiple if stereo rendering)
  to signal the sync object in order to inform the graphics system that the rendering for the
  camera(s) has completed. This function should be invoked in the completion callback of MTLCommandBuffer.
  All sync objects can be signaled after rendering all cameras, but results in lower fidelity in the graphics system.

  \apilevel 5

  \param[in] graphics_client The graphics client owning the sync object.
  \param[in] sync_object The sync object to be signaled.

  \retval MLResult_InvalidParam Sync Object was not signaled due to an invalid parameter.
  \retval MLResult_Ok Sync Object was signaled successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLRemoteGraphicsSignalSyncObjectMTL(MLHandle graphics_client, MLHandle sync_object);

#endif // __OBJC__

/*! \} */

ML_EXTERN_C_END
