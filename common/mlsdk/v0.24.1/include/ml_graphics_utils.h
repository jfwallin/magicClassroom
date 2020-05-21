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
// --------------------------------------------------------------------*/
// %BANNER_END%

#pragma once

#include "ml_api.h"
#include "ml_types.h"
#include "ml_graphics.h"
#include "ml_meshing2.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup GraphicsUtilities Graphics Utilities
  \sharedobject ml_graphics_utils
  \brief Optional graphics utility functionality

  - The graphics utility library is not necessary to create an application using
    the ML SDK. It contains a collection of useful code that can optionally be used
    to make development easier.

  \attention EXPERIMENTAL
  \{
*/

/*! Flags for MLMeshingDepthBufferInfo. */
typedef enum MLMeshingPopulateDepthFlags {
  /*! None. */
  MLMeshingPopulateDepthFlags_None,
  /*! If set, will fill the color buffer. */
  MLMeshingPopulateDepthFlags_EnableVisualization,
  /*! Ensure enum is represented as 32 bits. */
  MLMeshingPopulateDepthFlags_Ensure32Bits = 0x7FFFFFFF
} MLMeshingPopulateDepthFlags;

#ifdef VK_VERSION_1_0
/*!
  \brief Handles to and information about an imported surface.

  Populated by #MLGraphicsImportVkImageFromMediaHandle. Provides all information,
  necessary for creating a VkImageView and VkSampler for the imported surface.

  Refer to the Vulkan specification's equivalent parameters in VkAndroidHardwareBufferFormatPropertiesANDROID.
  Also refer to usage of VkExternalFormatANDROID and VkSamplerYcbcrConversionCreateInfo.
*/
typedef struct MLGraphicsImportedMediaSurface {
  /*! The VkImage bound to the imported surface. */
  VkImage                       imported_image;
  /*! The VkDevice memory representing the imported surface. */
  VkDeviceMemory                imported_memory;
  /*!
    \brief The Vulkan format corresponding to the imported surface’s format,
           or VK_FORMAT_UNDEFINED if there isn’t an equivalent Vulkan format.
  */
  VkFormat                      format;
  /*! An implementation-defined external format identifier for use with VkExternalFormatANDROID. */
  uint64_t                      external_format;
  /*! Describes the capabilities of this external format. */
  VkFormatFeatureFlags          format_features;
  /*! The component swizzle that should be used in VkSamplerYcbcrConversionCreateInfo. */
  VkComponentMapping            sampler_ycbcr_conversion_components;
  /*! Suggested color model to use in the VkSamplerYcbcrConversionCreateInfo. */
  VkSamplerYcbcrModelConversion suggested_ycbcr_model;
  /*! Suggested numerical value range to use in VkSamplerYcbcrConversionCreateInfo. */
  VkSamplerYcbcrRange           suggested_ycbcr_range;
  /*! Suggested X chroma offset to use in VkSamplerYcbcrConversionCreateInfo. */
  VkChromaLocation              suggested_x_chroma_offset;
  /*! Suggested Y chroma offset to use in VkSamplerYcbcrConversionCreateInfo. */
  VkChromaLocation              suggested_y_chroma_offset;
} MLGraphicsImportedMediaSurface;
#endif

/*! Per virtual camera information needed to render depth. */
typedef struct MLMeshingVirtualCameraBufferInfo {
  /*! The projection matrix for the camera. */
  MLMat4f projection;
  /*! The local space (relative to world) camera transform needed to render content, inverse is view matrix. */
  MLTransform transform;
} MLMeshingVirtualCameraBufferInfo;

/*! To be passed to PopulateDepth. */
typedef struct MLMeshingDepthBufferInfo {
  /*! Optional flags for Populate Depth. */
  uint32_t flags;
  /*! The number of buffers to render to for the frame. */
  uint32_t buffer_count;
  /*!
    \brief [DEBUG_ONLY] The handle (ID) of the color surface to render to for the buffer.
           Expects handle of a Texture2d Array.
  */
  MLHandle color_id;
  /*! The handle (ID) of the depth surface to render to for the buffer. Expects handle of a Texture2d Array. */
  MLHandle depth_id;
  /*! The origin and extents to render to for the camera. */
  MLRectf viewport;
  /*! Controls use of far_clip and directionality of z in projection model. */
  MLGraphicsProjectionType projection_type;
  /*! The array of buffers to render to for the frame. */
  MLMeshingVirtualCameraBufferInfo buffers[MLGraphicsVirtualCameraName_Count];
} MLMeshingDepthBufferInfo;

#ifdef VK_VERSION_1_0
/*!
  \brief Returns a list of required VkDevice extension names.

  If out_required_extension_properties is NULL then the number of required extension is returned in
  out_extension_property_count. Otherwise, out_extension_property_count must point to a variable set
  to the number of elements in the out_required_extension_properties array, and on return the variable
  is overwritten with the number of strings actually written to out_required_extension_properties.

  \param[out] out_required_extension_properties Either NULL or a pointer to an array of VkExtensionProperties.
  \param[out] out_extension_property_count A pointer to an integer related to the number of extensions required or queried.

  \retval MLResult_InvalidParam The query failed due to an invalid parameter.
  \retval MLResult_Ok The list of extensions or number of required extensions was returned successfully.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsEnumerateRequiredVkDeviceExtensionsForMediaHandleImport(VkExtensionProperties *out_required_extension_properties, uint32_t *out_extension_property_count);

/*!
  \brief Import a surface as a VkImage from a media api handle.

  Creates VkImage and VkDeviceMemory objects refrencing a surface referred to by a handle obtained from a media API.
  The resulting image will be owned by the VkDevice provided.This VkDevice must be created with the extensions
  specified by MLGraphicsEnumerateRequiredVkDeviceExtensionsForMediaHandleImport enabled.
  This call does not depend on a Graphics Client and can be used independent of other graphics APIs.

  \param[in] vulkan_logical_device The Vulkan logical device to use for resource creation.
  \param[in] media_handle The media api handle referring to the surface to import.
  \param[out] out_imported_surface_info A struct containing the resulting resource handles and information for sampler creation.

  \retval MLResult_InvalidParam Import was not performed due to an invalid parameter.
  \retval MLResult_Ok Surface was imported successfully.
  \retval MLResult_UnspecifiedFailure Import was not performed due to an unknown error.

  \priv None
*/
ML_API MLResult ML_CALL MLGraphicsImportVkImageFromMediaHandle(const VkDevice vulkan_logical_device, uint64_t media_handle, MLGraphicsImportedMediaSurface *out_imported_surface_info);
#endif

/*!
  \brief Populates the depth buffer (passed as a Texture2D array) with the meshes given by a user.
         It has to be called after MLGraphicsBeginFrame which provides the handles for the depth/color buffers.

  \param[in] client_handle The handle to the created client.
  \param[in] info The per virtual camera info for rendering depth.
  \param[in] meshes The meshes for which the depth info needs to be populated.

  \retval MLResult_InvalidParam Depth buffers were not populated due to an invalid parameter.
  \retval MLResult_Ok Depth buffer was populated successfully.

  \priv WorldReconstruction
*/
ML_API MLResult ML_CALL MLMeshingPopulateDepth(MLHandle client_handle, const MLMeshingDepthBufferInfo *info, const MLMeshingMesh *meshes);

/*! \} */

ML_EXTERN_C_END
