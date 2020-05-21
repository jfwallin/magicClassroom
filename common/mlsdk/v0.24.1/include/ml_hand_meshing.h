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
// --------------------------------------------------------------------*/
// %BANNER_END%

#pragma once

#include "ml_api.h"
#include "ml_types.h"

ML_EXTERN_C_BEGIN

/*!
  \addtogroup HandMeshing
  \sharedobject ml_perception_client
  \brief APIs for the HandMeshing system.

  - The HandMeshing system is for generating a mesh representation of the user's hands.

  \attention EXPERIMENTAL
  \{
*/

/*!
  \brief Stores a hand mesh's vertices and indices.
  \apilevel 6
*/
typedef struct MLHandMeshBlock {
  /*! The number of indices in index buffer. */
  uint16_t index_count;
  /*! The number of vertices in vertex buffer. */
  uint32_t vertex_count;
  /*! Pointer to the vertex buffer. */
  MLVec3f *vertex;
  /*!
    \brief Pointer to index buffer. In the index buffer each value
    is the index of a vertex in the vertex buffer. Three indices
    define one triangle. For example, the first triangle will have
    the vertices: vertex[index[0]], vertex[index[1]], vertex[index[2]].
    Index order is clockwise.
  */
  /*! Pointer to the index buffer. */
  uint16_t *index;
} MLHandMeshBlock;

/*!
  \brief Stores MLHandMeshBlock data.
  \apilevel 6
*/
typedef struct MLHandMesh {
  /*! Version of this structure. */
  uint32_t version;
  /*! Number of meshes available in data */
  uint32_t data_count;
  /*! The mesh data */
  MLHandMeshBlock *data;
} MLHandMesh;

/*!
  \brief Initializes default values for MLHandMesh.
  \apilevel 6

  \param[in, out] inout_attr The object to initialize with default settings.
*/
ML_STATIC_INLINE void MLHandMeshInit(MLHandMesh *inout_attr) {
  inout_attr->version = 1u;
  inout_attr->data_count = 0;
  inout_attr->data = NULL;
}

/*!
  \brief Create the hand meshing client

  Note that this will be the only function in the HandMeshing API that will return MLResult_PrivilegeDenied.
  Trying to call the other functions with an invalid MLHandle will result in MLResult_InvalidParam.

  \apilevel 6

  \param[out] out_client_handle The handle to the created client.

  \retval MLResult_InvalidParam HandMeshing Client was not created due to an invalid parameter.
  \retval MLResult_Ok HandMeshing Client was created successfully.
  \retval MLResult_PrivilegeDenied HandMeshing Client was not created due to insufficient privilege.
  \retval MLResult_UnspecifiedFailure HandMeshing Client was not created due to an unknown error.

  \priv HandMesh
*/
ML_API MLResult ML_CALL MLHandMeshingCreateClient(MLHandle *out_client_handle);

/*!
  \brief Destroy the hand meshing client.
  \apilevel 6

  \param[in,out] inout_client_handle The client to destroy.

  \retval MLResult_InvalidParam HandMeshing Client was not destroyed due to an invalid parameter.
  \retval MLResult_Ok HandMeshing Client was destroyed successfully.

  \priv HandMesh
*/
ML_API MLResult ML_CALL MLHandMeshingDestroyClient(MLHandle *inout_client_handle);

/*!
  \brief Request the hand mesh.
  \apilevel 6

  \param[in] client_handle The handle to the created client.
  \param[out] out_request_handle The handle for the current request. Needs to be passed to query the result of the request.

  \retval MLResult_InvalidParam Mesh was not requested due to an invalid parameter.
  \retval MLResult_Ok Mesh was requested successfully.

  \priv HandMesh
*/
ML_API MLResult ML_CALL MLHandMeshingRequestMesh(MLHandle client_handle, MLHandle *out_request_handle);

/*!
  \brief Get the Result of a previous hand mesh request.
  \apilevel 6

  \param[in] client_handle The handle to the created client
  \param[in] request_handle The handle received from a previous MLHandMeshingRequestMesh call.
  \param[out] out_mesh The final result which will be populated only if the result is successful.

  \retval MLResult_InvalidParam Mesh was not updated due to an invalid parameter.
  \retval MLResult_Ok Mesh was populated successfully.
  \retval MLResult_Pending Mesh pending update.

  \priv HandMesh
*/
ML_API MLResult ML_CALL MLHandMeshingGetResult(MLHandle client_handle, MLHandle request_handle, MLHandMesh *out_mesh);

/*!
  \brief Free resources created by the hand meshing APIS. Needs to be called whenever MLHandMeshingGetResult,
         returns a success.
  \apilevel 6

  \param[in] client_handle The handle to the created client.
  \param[out] out_request_handle The handle received from a previous MLHandMeshingRequestMesh call.

  \retval MLResult_InvalidParam Resources were not freed due to an invalid parameter.
  \retval MLResult_Ok Resources were freed successfully.

  \priv HandMesh
*/
ML_API MLResult ML_CALL MLHandMeshingFreeResource(MLHandle client_handle, MLHandle *out_request_handle);

/*! \} */

ML_EXTERN_C_END
