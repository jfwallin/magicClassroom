// %BANNER_BEGIN%
// ---------------------------------------------------------------------
// %COPYRIGHT_BEGIN%
//
// Copyright (c) 2019 Magic Leap, Inc. (COMPANY) All Rights Reserved.
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

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Avatar
  \sharedobject ml_avatar
  \brief APIs for Magic Leap Avatars.

  The Avatar API provides the following capabilities:
  (1) Getting the local user’s avatar settings.
  (2) Avatar Engine initialization and animation update loop.
  (3) Getting avatars’ asset data (meshes, materials, textures) from Avatar Engine,
      so the avatar model can be created in other rendering engines.
  (4) Getting the avatar’s material properties (e.g., model opacity, current hair color)
      and textures (e.g., tattoos), so they can be applied to the avatar model.
  (5) Serialization/deserialization of avatar attributes and shared events,
      to be used in multi-peer applications.

  \{
*/

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//                       AVATAR DEFINITIONS
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

/*!
  \defgroup AvatarDefs Avatar Definitions
  \ingroup Avatar
  \addtogroup AvatarDefs
  \{
*/

enum {
  /*! Peer ID that corresponds to the local user. */
  MLAvatarPeerID_Local = 0,
  /*! Invalid peer ID. An avatar created with this peer ID will be static, and will not be animated. */
  MLAvatarPeerID_Invalid = 0xFFFFFFFF
};

enum {
  /*! An invalid avatar or attachment ID. */
  MLAvatarObjectID_Invalid = 0xFFFFFFFF
};

enum {
  /*! The maximum number of bones that can influence each skinned mesh's vertex in MLAvatarMeshCreateData. */
  MLAvatarMaxBoneWeights = 4
};

enum {
  /*! The maximum size of the data buffer in MLAvatarSerializedData. */
  MLAvatarMaxSerializedDataSize = 4096
};

/*!
    \brief Unique identifier that corresponds to an MLAvatar object, which is either a base avatar or an attachment.

    An avatar consists of a base avatar with its own MLAvatarObjectID, as well as attachments that each have their
    own MLAvatarObjectIDs.
*/
typedef uint32_t MLAvatarObjectID;

/*!
    \brief ID that corresponds to a peer or user.

    MLAvatarPeerID_Local should be used when creating the user's local avatar.
    Note that Peer IDs are relative to the user. For instance in a 2 user application,
    here's what the peer IDs will look like on both applications:
    * Application running on User 1's device:
      (Peer ID = MLAvatarPeerID_Local): User 1's local avatar
      (Peer ID = 1): User 2's avatar.
    * Application running on User 2's device:
      (Peer ID = MLAvatarPeerID_Local): User 2's local avatar
      (Peer ID = 1): User 1's avatar.
*/
typedef uint32_t MLAvatarPeerID;

/*! The render layer ID of the material used for back-to-front sorting of transparent materials. */
typedef uint8_t MLAvatarRenderingLayer;

/*! A list of MLAvatarObjectIDs. */
typedef MLAvatarObjectID* MLAvatarObjectIDList;

/////////////////////
//  RESULT CODES
/////////////////////

/*!
  \defgroup DefResultCodes Result Codes
  \ingroup AvatarDefs
  \addtogroup DefResultCodes
  \{
*/

enum {
  /*! Defines the prefix for MLAvatarResult codes. */
  MLResultAPIPrefix_Avatar = MLRESULT_PREFIX(0x2f70)
};

/*!
  \brief Return result values for API calls that are relevant to avatars.

  \apilevel 8
*/
typedef enum MLAvatarResult {
  /*! Avatar Engine is not initialized. */
  MLAvatarResult_NotInitialized = MLResultAPIPrefix_Avatar,

  /*! The avatar configuration file was not loaded. */
  MLAvatarResult_AvatarConfigLoadFailed = MLResultAPIPrefix_Avatar + 1,

  /*! Expressions service initialization failed. */
  MLAvatarResult_ExpressionsInitFailed = MLResultAPIPrefix_Avatar + 2,

  /*! Expressions service startup failed. */
  MLAvatarResult_ExpressionsStartFailed = MLResultAPIPrefix_Avatar + 3,

  /*! The EMotionFX animation system was not initialized. */
  MLAvatarResult_EMotionFXInitFailed = MLResultAPIPrefix_Avatar + 4,

  /*! The object ID provided doesn't refer to an object that exists. */
  MLAvatarResult_ObjectIDInvalid = MLResultAPIPrefix_Avatar + 5,

  /*! Ensure enum is represented as 32 bits. */
  MLAvatarResult_Ensure32Bits = 0x7FFFFFFF
} MLAvatarResult;
/*! \} */

/*!
  \brief Defines the type of a shader.

  \apilevel 8
*/
typedef enum MLAvatarShaderType {
  /*! Avatar unlit shader. */
  MLAvatarShader_AvatarUnlit = 0,
  /*! Avatar PBR shader. */
  MLAvatarShader_AvatarPBR,
  /*! Unlit shader. */
  MLAvatarShader_Unlit,
  /*! PBR shader. */
  MLAvatarShader_PBR,
  /*! Unknown shader type. */
  MLAvatarShader_Unknown,
  /*! Ensure enum is represented as 32 bits. */
  MLAvatarShader_Ensure32Bits = 0x7FFFFFFF
} MLAvatarShaderType;

/*!
  \brief Defines the blend mode of a material.

  The blend mode determines how the pixels belonging to this material will be blended over what's already in the background.

  \apilevel 8
*/
typedef enum MLAvatarBlendMode {
  /*! Used for opaque objects. */
  MLAvatarBlendMode_Opaque = 0,
  /*!
    \brief Used for transparent objects.

    Pixels belonging to this material will be blended with the background pixel values, using the alpha values from the diffuse texture or the uniform material color.
  */
  MLAvatarBlendMode_Transparent,
  /*! Ensure enum is represented as 32 bits. */
  MLAvatarBlendMode_Ensure32Bits = 0x7FFFFFFF
} MLAvatarBlendMode;

/*!
  \brief Defines the format of a texture's data.

  \apilevel 8
*/
typedef enum MLAvatarTextureFormatType {
  /*! R8 format. */
  MLAvatarTextureFormat_R8 = 0,
  /*! RGB8 format. */
  MLAvatarTextureFormat_RGB8,
  /*! RGBA8 format. */
  MLAvatarTextureFormat_RGBA8,
  /*! BC1 format. */
  MLAvatarTextureFormat_BC1,
  /*! BC3 format. */
  MLAvatarTextureFormat_BC3,
  /*! BC7 format. */
  MLAvatarTextureFormat_BC7,
  /*! Unknown texture format. */
  MLAvatarTextureFormat_Unknown,
  /*! Ensure enum is represented as 32 bits. */
  MLAvatarTextureFormat_Ensure32Bits = 0x7FFFFFFF
} MLAvatarTextureFormatType;

/*!
  \brief Defines the type of a texture.

  \apilevel 8
*/
typedef enum MLAvatarTextureType {
  /*! Diffuse texture. */
  MLAvatarTexture_Diffuse = 0,
  /*! Normal texture. */
  MLAvatarTexture_Normal,
  /*! Opacity texture. */
  MLAvatarTexture_Opacity,
  /*! Ambient texture. */
  MLAvatarTexture_Ambient,
  /*! The maximum number of texture types. */
  MLAvatarTexture_Max,
  /*! Ensure enum is represented as 32 bits. */
  MLAvatarTexture_Ensure32Bits = 0x7FFFFFFF
} MLAvatarTextureType;

/*!
  \brief The creation data of a submesh.

  A submesh represents a part of the parent or base mesh that's rendered using a single material.

  \apilevel 8
*/
typedef struct MLAvatarSubmeshCreateData {
  /*! The name of the material this submesh should be rendered with. */
  char* material_name;
  /*!
    \brief The start index.

    This is the offset in the vertex indices array of the parent mesh where the data for this submesh starts.
  */
  uint32_t start_vertex_index;
  /*! The indices of the vertices of the parent or base mesh that make up this submesh. */
  uint32_t* vertex_indices;
  /*! The number of vertex indices contained in the submesh. */
  uint32_t vertex_index_count;
} MLAvatarSubmeshCreateData;

/*!
  \brief The creation data of a mesh.

  \apilevel 8
*/
typedef struct MLAvatarMeshCreateData {
  /*! The index of the base avatar or attachment node where this mesh should be attached to. */
  uint32_t node_index;
  /*! The positions of the vertices that make up this mesh. */
  MLVec3f* vertex_positions;
  /*! The normals of the vertices that make up this mesh. */
  MLVec3f* vertex_normals;
  /*! The uv values of the vertices that make up this mesh. */
  MLVec2f* vertex_uv;
  /*! The colors of the vertices that make up this mesh. */
  MLColor4f* vertex_colors;
  /*!
    \brief The bone indices of the vertices in a skinned mesh.

    Will be null if the mesh isn't skinned. Each vertex has MLAvatarMaxBoneWeights number of bone indices.
  */
  uint32_t* bone_indices;
  /*!
    \brief The bone weights of the vertices in a skinned mesh.

    Will be null if the mesh isn't skinned. Each vertex has MLAvatarMaxBoneWeights number of bone weights.
  */
  float* bone_weights;
  /*! The number of vertices in the mesh. */
  uint32_t vertex_count;
  /*! The submesh datas of this mesh. */
  MLAvatarSubmeshCreateData** submesh_datas;
  /*! The number of submeshes. */
  uint32_t submesh_count;
  /*! Whether the mesh is skinned or not. */
  bool skinned;
} MLAvatarMeshCreateData;

/*!
  \brief Describes the creation data of an avatar node.

  A node is a connection point in the hierarchy of the avatar or attachment model.
  Each node can have one parent, but multiple child nodes. An example of a nodes is a bone, like an upper or lower arm arm.

  \apilevel 8
*/
typedef struct MLAvatarNodeCreateData {
  /*! The name of the node. */
  char* node_name;
  /*! The bind pose transform of the node. */
  MLMat4f bind_pose_transform;
  /*! The inverse bind pose transform of the node. */
  MLMat4f inv_bind_pose_transform;
  /*! The index of the parent node. */
  uint32_t parent_node_index;
  /*! The indices of this node's children nodes. */
  uint32_t* child_node_indices;
  /*! The number of children nodes this node has. */
  uint32_t child_count;
} MLAvatarNodeCreateData;

/*!
  \brief Describes a vertex delta caused by a mesh deformation.

  \apilevel 8
*/
typedef struct MLAvatarMorphTargetVertexDelta {
  /*! The index of the vertex being modified. */
  uint32_t vertex_index;
  /*! The position delta or offset. */
  MLVec3f position_delta;
  /*! The normal delta or offset. */
  MLVec3f normal_delta;
} MLAvatarMorphTargetVertexDelta;

/*!
  \brief Describes a node transformation applied by a standard morph target.

  \apilevel 8
*/
typedef struct MLAvatarMorphTargetTransform {
  /*! The index of the transformed node. */
  uint32_t node_index;
  /*! The position delta of the transformation. */
  MLVec3f position_delta;
  /*! The rotation of the transformation. Note that this is not a delta, it is the rotation itself. */
  MLQuaternionf rotation;
  /*! The delta or change in scale applied of the transformation. */
  MLVec3f scale_delta;
} MLAvatarMorphTargetTransform;

/*!
  \brief The data that describes a mesh deformation.

  \apilevel 8
*/
typedef struct MLAvatarMeshDeformCreateData {
  /*! The node on which the deformation takes place. */
  uint32_t node_index;
  /*! The vertex delta datas. */
  MLAvatarMorphTargetVertexDelta* vertex_deltas;
  /*! The number of vertex deltas. */
  uint32_t vertex_delta_count;
} MLAvatarMeshDeformCreateData;

/*!
  \brief The creation data of a standard morph target.

  A standard morph target can apply modifications to node transformations, and/or meshes.

  \apilevel 8
*/
typedef struct MLAvatarMorphTargetCreateData {
  /*! The name of the morph target. */
  char* morph_target_name;
  /*! The id of the morph target. */
  uint32_t morph_target_id;
  /*! The weight of the morph target. */
  float morph_target_weight;
  /*! The mesh deformation datas of the morph target. */
  MLAvatarMeshDeformCreateData** mesh_deform_datas;
  /*! The number of mesh deformations this morph target has. */
  uint32_t mesh_deform_count;
  /*! The node transformation datas of the morph target. */
  MLAvatarMorphTargetTransform* node_transform_datas;
  /*! The number of node transformations this morph target has. */
  uint32_t transform_data_count;
} MLAvatarMorphTargetCreateData;

/*!
  \brief The creation data of a composite morph target.

  A composite morph target maps its weight to an array of standard morph targets. When the weight is mapped to a
  standard morph target, it needs to also be multiplied by the reference weight of the standard target.

  \apilevel 8
*/
typedef struct MLAvatarCompositeMorphTargetCreateData {
  /*! The name of the morph target. */
  char* morph_target_name;
  /*! The name id the morph target. */
  uint32_t morph_target_id;
  /*! The name weight the morph target. */
  float morph_target_weight;
  /*! The ids of the standard morph targets this composite morph target maps to. */
  uint32_t* mapped_morph_target_ids;
  /*!
    \brief The reference weights of the standard morph targets this composite morph target maps to.

    When setting the weight of a mapped standard morph target, multiply the corresponding reference weight by
    the weight of the composite morph target.
  */
  float* mapped_ref_weights;
  /*! The number of standard morph targets this composite morph target maps to. */
  uint32_t mapped_morph_target_count;
} MLAvatarCompositeMorphTargetCreateData;

/*!
  \brief he creation data of a material used on an avatar or attachment model.

  \apilevel 8
*/
typedef struct MLAvatarMaterialCreateData {
  /*! The material name. */
  char* material_name;
  /*! The type of the shader this material uses. */
  MLAvatarShaderType shader;
  /*! The blend mode of this material. */
  MLAvatarBlendMode blend_mode;
  /*! The rendering layer of the material. */
  MLAvatarRenderingLayer rendering_layer;
  /*! The texture paths of the material. */
  char* texture_paths[MLAvatarTexture_Max];
  /*! Material color. */
  MLColor4f color;
  /*! The metalness of the material. */
  float metallic;
  /*! The roughness of the material. */
  float roughness;
} MLAvatarMaterialCreateData;

/*!
  \brief The creation data of a texture belonging to an avatar or attachment model.

  \apilevel 8
*/
typedef struct MLAvatarTextureCreateData {
  /*! The path of the texture. */
  char* texture_path;
  /*! The texture data. */
  uint8_t* data;
  /*! The size of the texture data. */
  uint32_t data_size;
  /*! The texture width. */
  uint32_t width;
  /*! The texture height. */
  uint32_t height;
  /*! Whether the texture has mipmaps or not. */
  bool has_mipmaps;
  /*! The format of the texture data. */
  MLAvatarTextureFormatType format;
} MLAvatarTextureCreateData;

/*!
  \brief The creation data of an avatar or attachment model.

  Contains all the necessary mesh, skinning, material and texture data to create this model in runtime.

  \apilevel 8
*/
typedef struct MLAvatarModelCreateData {
  /*! The path of this model asset. */
  char* model_path;
  /*! The mesh creation datas. */
  MLAvatarMeshCreateData** mesh_datas;
  /*! The number of meshes. */
  uint32_t mesh_count;
  /*! The node creation datas. */
  MLAvatarNodeCreateData** node_datas;
  /*! The number of nodes. */
  uint32_t node_count;
  /*! Whether the dual quaternion skinning method should be used for this model. */
  bool use_dual_quaternion_skinning;
  /*! The morph target creation datas. */
  MLAvatarMorphTargetCreateData** morph_target_datas;
  /*! The number of morph targets. */
  uint32_t morph_target_count;
  /*! The composite morph target creation datas. */
  MLAvatarCompositeMorphTargetCreateData** composite_morph_target_datas;
  /*! The number of composite morph targets. */
  uint32_t composite_morph_target_count;
  /*! The material creation datas. */
  MLAvatarMaterialCreateData** material_datas;
  /*! The number of materials. */
  uint32_t material_count;
  /*! The texture creation datas. */
  MLAvatarTextureCreateData** texture_datas;
  /*! The number of textures belonging to this model. */
  uint32_t texture_count;
  /*! Whether the object should be shown or not upon creation. */
  bool show_object;
  /*! The initial opacity of the model. */
  float opacity;
} MLAvatarModelCreateData;

/*!
  \brief The creation data of an attachment. Every attachment has a base avatar that it is attached to.

  \apilevel 8
*/
typedef struct MLAvatarAttachmentCreateData {
  /*! The id of the attachment. */
  MLAvatarObjectID attachment_id;
  /*! The creation data of the attachment model. */
  MLAvatarModelCreateData* model_data;
  /*! The id of the base avatar that this attachment is attached to. */
  MLAvatarObjectID base_avatar_id;
  /*! The name of the node in the base avatar that this attachment is attached to. */
  char* attachment_node_name;
} MLAvatarAttachmentCreateData;

/*!
  \brief The creation data of an avatar, containing all the data necessary to construct the avatar assets in runtime.

  \apilevel 8
*/
typedef struct MLAvatarCreateData {
  /*! The id of the avatar. */
  MLAvatarObjectID avatar_id;
  /*! The id of the peer this avatar belongs to. */
  MLAvatarPeerID peer_id;
  /*! Whether the avatar is a portal avatar or not. */
  bool is_portal;
  /*! The creation data of the base avatar model. */
  MLAvatarModelCreateData* base_model_data;
  /*! The attachment creation datas. */
  MLAvatarAttachmentCreateData** attachment_datas;
  /*! The number of attachments this avatar has. */
  uint32_t attachment_count;
} MLAvatarCreateData;

/*!
  \brief The animation frame of a base avatar or attachment object.

  \apilevel 8
*/
typedef struct MLAvatarAnimationFrame {
  /*! Root transform of the object. */
  MLMat4f root_transform;
  /*! The transforms of nodes belonging to the object. */
  MLMat4f* transforms;
  /*! Array specifying which transforms have changed. */
  bool* transform_dirty;
  /*! Number of transforms. */
  uint32_t transform_count;
  /*! The morph target weights of the object. */
  float* morph_target_weights;
  /*!Array specifying which morph target weights have changed. */
  bool* morph_target_dirty;
  /*! Number of morph targets. */
  uint32_t morph_target_count;
  /*! Whether the object should be shown or not. */
  bool show_object;
  /*! The opacity of the object. */
  float opacity;
} MLAvatarAnimationFrame;

/*!
  \brief The animation data of the avatar, including the base avatar model and all of its attachments.

  \apilevel 8
*/
typedef struct MLAvatarAnimationData {
  /*! Animation frame of the base avatar model. */
  MLAvatarAnimationFrame* base_avatar_frame;
  /*! Animation frames of all attachments belonging to this avatar. */
  MLAvatarAnimationFrame** attachment_frames;
  /*! Number of attachments. */
  uint32_t attachment_count;
} MLAvatarAnimationData;

/*!
  \brief Structure specificying avatar creation parameters.

  Needs to be initialized with MLAvatarCreateArgsInit.

  \apilevel 8
*/
typedef struct MLAvatarCreateArgs {
  /*! Version of the struct. */
  uint32_t version;
  /*! The user's username. */
  const char* username;
  /*! The user's avatar settings. */
  const char* avatar_settings;
  /*! The peer id of the avatar. */
  MLAvatarPeerID peer_id;
  /*!
    \brief If true, a portal icon version of the avatar will be created, meant to be used as a 3D UI element.

    For more information on portal icons, refer to: https://developer.magicleap.com/learn/guides/avatarmod-portal-icons.
  */
  bool as_portal;
} MLAvatarCreateArgs;

/*!
  \brief Serialized data structure used for serialization and deserialization of avatar attributes and shared events.

  \apilevel 8
*/
typedef struct MLAvatarSerializedData {
  /*! The serialized data buffer. Max size is MLAvatarMaxSerializedDataSize. */
  uint8_t data[MLAvatarMaxSerializedDataSize];
  /*! The actual size of the data that was serialized into the buffer. */
  uint32_t data_size;
} MLAvatarSerializedData;
/*! \} */
/*! \} */

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//                         AVATAR ENGINE MANAGEMENT
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

/*!
  \defgroup AvatarEngine Avatar Engine
  \ingroup Avatar
  \addtogroup AvatarEngine
  \{
*/

/*!
  \brief Returns an ASCII string for MLAvatarResult and MLResult codes.

  \apilevel 8

  \param[in] result_code The input MLResult enum from MLAvatar functions.

  \return ASCII string containing readable version of result code.

  \priv None
*/
ML_API const char* ML_CALL MLAvatarGetResultString(MLResult result_code);

/*!
  \brief Initialize Avatar Engine.

  The caller needs to also call MLAvatarShutdown when the app shuts down, or when the caller wants to stop all avatars
  related functionality.

  \apilevel 8

  \retval MLResult_Ok Avatar Engine initialized successfully.
  \retval MLResult_PrivilegeDenied Expressions privilege was not granted to the app.
  \retval MLResult_UnspecifiedFaiure Internal error.
  \retval MLAvatarResult_AvatarConfigLoadFailed Failed to load avatar configuration file.
  \retval MLAvatarResult_EMotionFXInitFailed Failed to initialize the EMotionFX animation engine.
  \retval MLAvatarResult_ExpressionsInitFailed Failed to initialize avatar expressions service.
  \retval MLAvatarResult_ExpressionsStartFailed Failed to start avatar expressions service.
                                                    prior to calling the function.

  \priv Expressions
*/
ML_API MLResult ML_CALL MLAvatarInit();

/*!
  \brief Updates Avatar Engine.

  \apilevel 8

  \param[in] delta_time Delta time, in seconds, used to update Avatar Engine.

  \retval MLResult_Ok Avatar Engine updated.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarUpdate(float delta_time);

/*!
  \brief Shut down avatar engine.

  \apilevel 8

  \retval MLResult_Ok The avatar engine is shut down.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarShutdown();
/*! \} */

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//                         AVATAR MANAGEMENT
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

/*!
  \defgroup AvatarManagement Avatar Management
  \ingroup Avatar
  \addtogroup AvatarManagement
  \{
*/

/*!
  \brief Initializes default values for MLAvatarCreateArgs.

  \apilevel 8

  \param[in,out] inout_args.
*/
ML_STATIC_INLINE void MLAvatarCreateArgsInit(MLAvatarCreateArgs *create_args) {
  create_args->version = 1u;
  create_args->username = NULL;
  create_args->avatar_settings = NULL;
  create_args->peer_id = MLAvatarPeerID_Invalid;
  create_args->as_portal = false;
}

/*!
  \brief Create an avatar with the specified arguments.

  The app is responsible for deleting create_data after making use of the data, by calling MLAvatarDestroyCreateData.

  \apilevel 8

  \param[in] create_args Specifies arguments for avatar creation. includes information such as username (optional),
             peer_id (optional, specify MLAvatarPeerID_Invalid for a static avatar), avatar_settings
             (avatar personalization settings, specifying avatar look), and as_portal (if true, a portal icon version
             of the avatar will be created).
  \param[out] create_data Avatar creation data populated by the function, which the app can use to
              load avatar assets for rendering.

  \retval MLResult_InvalidParam An invalid or null avatar_settings or username was passed as a parameter in create_args.
  \retval MLResult_Ok Avatar created successfully.
  \retval MLResult_UnspecifiedFailure Avatar creation failed.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarCreateAvatar(const MLAvatarCreateArgs* create_args, MLAvatarCreateData** create_data);

/*!
  \brief Destroy create_data structure created by MLAvatarCreateAvatar.

  \apilevel 8

  \param[in] create_data The structure that will be destroyed.

  \retval MLResult_Ok The MLAvatarCreateData structure was destroyed successfully.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarDestroyCreateData(MLAvatarCreateData** create_data);

/*!
  \brief Check if the specified avatar exists.

  \apilevel 8

  \param[in] avatar_id The id of the avatar that is being checked.
  \param[out] has_avatar Whether the specified avatar exists or not.

  \retval MLResult_Ok The check was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarHasAvatar(MLAvatarObjectID avatar_id, bool* has_avatar);

/*!
  \brief Get ID of the peer that controls the specified avatar.

  \apilevel 8

  \param[in] avatar_id The id of the avatar whose peer id is being checked.
  \param[out] peer_id The peer ID of the specified avatar.

  \retval MLResult_Ok The check was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.
  \retval MLAvatarResult_ObjectIDInvalid The passed in avatar id doesn't refer to an existing avatar.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarGetAvatarPeerId(MLAvatarObjectID avatar_id, MLAvatarPeerID* peer_id);

/*!
  \brief Check if the avatar with the specified ID represents the local user.

  \apilevel 8

  \param[in] avatar_id The id of the avatar that is being checked.
  \param[out] is_local Whether the specified avatar represents the local user or not.

  \retval MLResult_Ok The check was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.
  \retval MLAvatarResult_ObjectIDInvalid The passed in avatar id doesn't refer to an existing avatar.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarIsLocalAvatar(MLAvatarObjectID avatar_id, bool* is_local);

/*!
  \brief Get IDs of avatars for the specified peer.

  The allocated MLAvatarObjectIDList needs to be destroyed with MLAvatarDestroyObjectIDList once the caller is done using it.

  \apilevel 8

  \param[in] peer_id The specified peer.
  \param[out] avatar_id_list An array of avatar IDs of all the avatars belonging to the specified peer.
                             It is allocated and filled by the function, but the app is responsible
                             for deallocating it by calling MLAvatarDestroyAvatarObjectIDList.
  \param[out] avatar_id_count The total number of avatars belonging to the specified peer.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarGetAvatarsForPeer(MLAvatarPeerID peer_id, MLAvatarObjectIDList* avatar_id_list, uint32_t* avatar_id_count);

/*!
  \brief Get IDs of avatars for the local user.

  The allocated MLAvatarObjectIDList needs to be destroyed with MLAvatarDestroyObjectIDList once the caller is done using it.

  \apilevel 8

  \param[out] avatar_id_list An array of avatar IDs of all the avatars belonging to local user.
                             It is allocated and filled by the function, but the app is responsible
                             for deallocating it by calling MLAvatarDestroyAvatarObjectIDList.
  \param[out] avatar_id_count The total number of avatars belonging to local user.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarGetLocalAvatars(MLAvatarObjectIDList* avatar_id_list, uint32_t* avatar_id_count);

/*!
  \brief Get IDs of avatars for the specified user.

  The allocated MLAvatarObjectIDList needs to be destroyed with MLAvatarDestroyObjectIDList once the caller is done using it.

  \apilevel 8

  \param[in] username The specified user name.
  \param[out] avatar_id_list An array of avatar IDs of all the avatars belonging to the specified user.
                             It is allocated and filled by the function, but the app is responsible
                             for deallocating it by calling MLAvatarDestroyAvatarObjectIDList.
  \param[out] avatar_id_count The total number of avatars belonging to the specified user.

  \retval MLResult_InvalidParam The passed in username was null or empty.
  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarGetAvatarsForUser(const char* username, MLAvatarObjectIDList* avatar_id_list, uint32_t* avatar_id_count);

/*!
  \brief Destroy an array of avatar object IDs.

  \apilevel 8

  \param[in] avatar_id_list The avatar object ID array that will be destroyed.

  \retval MLResult_InvalidParam The passed in avatar_id_list was invalid or null.
  \retval MLResult_Ok The operation was successful.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarDestroyObjectIDList(MLAvatarObjectIDList* avatar_id_list);

/*!
  \brief Destroy the specified avatar.

  \apilevel 8

  \param[in] avatar_id The id of the avatar.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.
  \retval MLAvatarResult_ObjectIDInvalid The passed in avatar id doesn't refer to an existing avatar.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarDestroyAvatar(MLAvatarObjectID avatar_id);

/*!
  \brief Destroy all created avatars and their attachments.

  \apilevel 8

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarDestroyAllAvatars();
/*! \} */

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//                         AVATAR ANIMATION
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

/*!
  \defgroup AvatarAnimation Avatar Animation
  \ingroup Avatar
  \addtogroup AvatarAnimation
  \{
*/

/*!
  \brief Create an initial animation data snapshot from Avatar Engine for the specified avatar.

  This can be reused frame to frame for any number of MLAvatarGetAnimationData calls, but it needs to be destroyed
  using MLAvatarDestroyAnimationData before the avatar gets destroyed.

  \apilevel 8

  \param[in] avatar_id The id of the avatar.
  \param[out] animation_data The created animation data.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.
  \retval MLAvatarResult_ObjectIDInvalid The passed in avatar id doesn't refer to an existing avatar.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarCreateAnimationData(MLAvatarObjectID avatar_id, MLAvatarAnimationData** animation_data);

/*!
  \brief Destroy the animation data snapshot from Avatar Engine for the specified avatar.

  This should be done before the avatar gets destroyed.

  \apilevel 8

  \param[in] avatar_id The id of the avatar.
  \param[in] animation_data The animation data that will be destroyed.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarDestroyAnimationData(MLAvatarObjectID avatar_id, MLAvatarAnimationData** animation_data);

/*!
  \brief Get latest animation data snapshot from Avatar Engine for the specified avatar.

  \apilevel 8

  \param[in] avatar_id The id of the avatar.
  \param[out] animation_data The animation data that will be populated using the latest avatar animation snapshot.
              This data needs to be created first using MLAvatarCreateAnimationData before this function is used.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarGetAnimationData(MLAvatarObjectID avatar_id, MLAvatarAnimationData* animation_data);

/*!
  \brief Trigger OnHover animation state on the portal avatar.

  \apilevel 8

  \param[in] avatar_id The id of the avatar.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.
  \retval MLAvatarResult_ObjectIDInvalid The passed in avatar id doesn't refer to an existing avatar.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarOnPortalHoverEnter(MLAvatarObjectID avatar_id);

/*!
  \brief Go back to idle animation state on the portal avatar.

  \apilevel 8

  \param[in] avatar_id The id of the avatar.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.
  \retval MLAvatarResult_ObjectIDInvalid The passed in avatar id doesn't refer to an existing avatar.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarOnPortalHoverExit(MLAvatarObjectID avatar_id);
/*! \} */

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//                         AVATAR SERIALIZATION
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

/*!
  \defgroup AvatarSerialization Avatar Serialization
  \ingroup Avatar
  \addtogroup AvatarSerialization
  \{
*/

/*!
  \brief Create serialized data buffer structure.

  \apilevel 8

  \param[out] data The created data.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarCreateSerializedData(MLAvatarSerializedData** data);

/*!
  \brief Destroy serialized data buffer structure.

  \apilevel 8

  \param[in] data The serialized data that will be destroyed.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarDestroySerializedData(MLAvatarSerializedData** data);

/*!
  \brief Serialize attribute values for the specified peer into a buffer of bytes.

  \apilevel 8

  \param[out] attribute_data  The attribute data that will be serialized, which contains the buffer.
              It needs to be created first using MLAvatarCreateSerializedData before this function is used.


  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarSerializeAttributes(MLAvatarSerializedData* attribute_data);

/*!
  \brief Deserialize attribute values for the specified peer from a buffer of bytes.

  \apilevel 8

  \param[in] peer_id The specified peer.
  \param[in] attribute_data The attribute event data that will be deserialized, which contains the buffer.
             It needs to be created first using MLAvatarCreateSerializedData before this function is used.
  \param[in] time_diff The difference between the current time on the local peer and remote peer,
                       needed to convert timestamps on attribute values.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarDeserializeAttributes(MLAvatarPeerID peer_id, MLAvatarSerializedData* attribute_data, float time_diff);

/*!
  \brief Enable/disable muting of local user’s lip-sync attributes.

  Can be used to implement a self-mute function to disable the local user's lip-sync capability, so their avatar's lips
  won't move while they're muted. If muted, lip movement attributes also won't be serialized with the rest of the avatar's attributes.

  \apilevel 8

  \param[in] mute If true, the user's lip-sync attributes will be muted.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarSetLipSyncMute(bool mute);

/*!
  \brief Serialize shared events scheduled for dispatch into a buffer of bytes.

  \apilevel 8

  \param[out] shared_event_data  The shared event data that will be serialized, which contains the buffer.
              It needs to be created first using MLAvatarCreateSerializedData before this function is used.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarSerializeSharedEvents(MLAvatarSerializedData* shared_event_data);

/*!
  \brief Deserialize shared events for the specified peer from a buffer of bytes.

  \apilevel 8

  \param[in] peer_id The specified peer.
  \param[in] shared_event_data The shared event data that will be deserialized, which contains the buffer.
             It needs to be created first using MLAvatarCreateSerializedData before this function is used.
  \param[in] time_diff The difference between the current time on the local peer and remote peer,
                       needed to convert timestamps on attribute values.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarDeserializeSharedEvents(MLAvatarPeerID peer_id, MLAvatarSerializedData* shared_event_data, float time_diff);

/*!
  \brief Check if shared event serialization/deserialization is enabled.

  \apilevel 8

  \param[out] enabled Enabled state of shared event serialization.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarGetSharedEventSyncEnabled(bool* enabled);

/*!
  \brief Enable/disable shared event serialization/deserialization.
         This needs to be enabled in order for serialization and deserialization functions to do anything.

  \apilevel 8

  \param[in] enabled Enabled state of shared event serialization.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarSetSharedEventSyncEnabled(bool enabled);

/*!
  \brief Get current time in seconds since Avatar Engine was initialized.

  This is also the local time of all attribute values and shared events. Use this value received from local and remote
  peers to calculate the time differences that are passed into the MLAvatarDeserializeAttributes and
  MLAvatarDeserializeSharedEvents functions.

  \apilevel 8

  \param[out] current_time The current time in seconds.

  \retval MLResult_Ok The operation was successful.
  \retval MLAvatarResult_NotInitialized The avatar client was not properly initialized with MLAvatarInit.

  \priv None
*/
ML_API MLResult ML_CALL MLAvatarGetCurrentTime(float* current_time);
/*! \}*/
/*! \} */

ML_EXTERN_C_END