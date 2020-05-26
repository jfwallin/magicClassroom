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

ML_EXTERN_C_BEGIN

/*!
  \addtogroup Planes
  \sharedobject ml_perception_client
  \brief APIs for the Planes system.
  \{
*/

/*! Control flags for plane queries. */
typedef enum MLPlanesQueryFlags {
  /*! Include planes whose normal is perpendicular to gravity. */
  MLPlanesQueryFlag_Vertical         = 1 << 0,
  /*! Include planes whose normal is parallel to gravity. */
  MLPlanesQueryFlag_Horizontal       = 1 << 1,
  /*! Include planes with arbitrary normals. */
  MLPlanesQueryFlag_Arbitrary        = 1 << 2,
  /*! Include all plane orientations. */
  MLPlanesQueryFlag_AllOrientations  = MLPlanesQueryFlag_Vertical |
                                       MLPlanesQueryFlag_Horizontal |
                                       MLPlanesQueryFlag_Arbitrary,
  /*!
    \brief For non-horizontal planes, setting this flag will result in the top of
    the plane rectangle being perpendicular to gravity.
  */
  MLPlanesQueryFlag_OrientToGravity  = 1 << 3,
  /*!
    \brief If this flag is set, inner planes will be returned; if it is not set,
    outer planes will be returned.
  */
  MLPlanesQueryFlag_Inner            = 1 << 4,
  /*!
    \deprecated Deprecated since 0.15.0.

    \brief Instructs the plane system to ignore holes in planar surfaces. If set,
      planes can patch over holes in planar surfaces. Otherwise planes will
      be built around holes.
      The expected behavior is - As long as a hole is big enough (diameter
      of the hole ~16cm), the inner planes will avoid covering the holes.
      The outer planes by definition will cover all the holes.
  */
  MLPlanesQueryFlag_IgnoreHoles      = 1 << 5,
  /*! Include planes semantically tagged as ceiling. */
  MLPlanesQueryFlag_Semantic_Ceiling = 1 << 6,
  /*! Include planes semantically tagged as floor. */
  MLPlanesQueryFlag_Semantic_Floor   = 1 << 7,
  /*! Include planes semantically tagged as wall. */
  MLPlanesQueryFlag_Semantic_Wall    = 1 << 8,
  /*! Include all planes that are semantically tagged. */
  MLPlanesQueryFlag_Semantic_All     = MLPlanesQueryFlag_Semantic_Ceiling |
                                       MLPlanesQueryFlag_Semantic_Floor |
                                       MLPlanesQueryFlag_Semantic_Wall,
  /*!
    \brief Include polygonal planes.
      When this flag is set:
        - MLPlanesQueryGetResultsWithBoundaries returns polygons along with
          other applicable rectangular planes. MLPlanesReleaseBoundariesList
          MUST be called before the next call to MLPlanesQueryGetResultsWithBoundaries,
          otherwise UnspecifiedFailure will be returned.
      When this flag is not set:
        - MLPlanesQueryGetResultsWithBoundaries returns just rectangular planes.
          No need to call MLPlanesReleaseBoundariesList.
    \apilevel 2
  */
  MLPlanesQueryFlag_Polygons    = 1 << 9,
} MLPlanesQueryFlags;

/*! A plane with width and height */
typedef struct MLPlane {
  /*! Plane center. */
  MLVec3f position;
  /*! Plane rotation. */
  MLQuaternionf rotation;
  /*! Plane width. */
  float width;
  /*! Plane height. */
  float height;
  /*! Flags which describe this plane. */
  uint32_t flags;
  /*!
    \brief Plane ID. All inner planes within an outer plane will have the
    same ID (outer plane's ID). These IDs are persistent across
    plane queries unless a map merge occurs. On a map merge, IDs
    could be different.
  */
  MLHandle id;
} MLPlane;

/*!
  \brief Coplanar connected line segments representing the outer boundary of a polygon,
         an n sided polygon where n is the number of vertices.
  \apilevel 2
*/
typedef struct MLPolygon {
  /*! Vertices of all line segments. */
  MLVec3f *vertices;
  /*! Number of vertices. */
  uint32_t vertices_count;
} MLPolygon;

/*!
  \brief Type used to represent a region boundary on a 2D plane.
  \apilevel 2
*/
typedef struct MLPlaneBoundary {
  /*! The polygon that defines the region, the boundary vertices in MLPolygon will be in CCW order. */
  MLPolygon *polygon;
  /*! A polygon may contains multiple holes, the boundary vertices in MLPolygon will be in CW order. */
  MLPolygon *holes;
  /*! Count of the holes. */
  uint32_t holes_count;
} MLPlaneBoundary;

/*!
  \brief Type to represent multiple regions on a 2D plane.
  \apilevel 2
*/
typedef struct MLPlaneBoundaries {
  /*! Plane ID, the same value associating to the ID in #MLPlane if they belong to the same plane. */
  MLHandle id;
  /*! The boundaries in a plane. */
  MLPlaneBoundary *boundaries;
  /*! Count of boundaries. A plane may contain multiple boundaries each of which defines a region. */
  uint32_t boundaries_count;
} MLPlaneBoundaries;

/*!
  \brief Type to represent polygons of all returned planes.
  \apilevel 2
*/
typedef struct MLPlaneBoundariesList {
  uint32_t version;
  /*!
    \brief List of #MLPlaneBoundaries.
    \apilevel 2
  */
  MLPlaneBoundaries *plane_boundaries;
  /*!
    \brief Count of #MLPlaneBoundaries in the array.
    \apilevel 2
  */
  uint32_t plane_boundaries_count;
} MLPlaneBoundariesList;

/*!
  \brief Initializes an MLPlaneBoundariesList structure.
  \apilevel 2
*/
ML_STATIC_INLINE void MLPlaneBoundariesListInit(MLPlaneBoundariesList *boundaries_list) {
  boundaries_list->version = 1;
  boundaries_list->plane_boundaries = NULL;
  boundaries_list->plane_boundaries_count = 0u;
}

/*! Type used to represent a plane query. */
typedef struct MLPlanesQuery {
  /*! The flags to apply to this query. */
  uint32_t flags;
  /*! The center of the bounding box which defines where planes extraction should occur. */
  MLVec3f bounds_center;
  /*! The rotation of the bounding box where planes extraction will occur. */
  MLQuaternionf bounds_rotation;
  /*! The size of the bounding box where planes extraction will occur. */
  MLVec3f bounds_extents;
  /*!
    \brief The maximum number of results that should be returned. This is also
    the minimum expected size of the array of results passed to the
    MLPlanesGetResult function.
  */
  uint32_t max_results;
  /*!
    \deprecated Deprecated since 0.15.0.

    \brief If #MLPlanesQueryFlag_IgnoreHoles is set to false, holes with a perimeter
    (in meters) smaller than this value will be ignored, and can be part of the
    plane. This value cannot be lower than 0 (lower values will be capped to
    this minimum). A good default value is 0.5.
  */
  float min_hole_length;
  /*!
    \brief The minimum area (in squared meters) of planes to be returned. This value
    cannot be lower than 0.04 (lower values will be capped to this minimum).
    A good default value is 0.25.
  */
  float min_plane_area;
} MLPlanesQuery;

/*!
  \brief Create a planes tracker.
  \param[out] out_handle A pointer to an #MLHandle which will contain the handle to the planes tracker.
              If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam The parameter out_handle was not valid (null).
  \retval MLResult_Ok The planes tracker was created successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv WorldReconstruction
*/
ML_API MLResult ML_CALL MLPlanesCreate(MLHandle *out_handle);

/*!
  \brief Destroy a planes tracker.

  \param[in] planes_tracker #MLHandle to planes tracker to destroy.

  \retval MLResult_Ok The planes tracker was successfully destroyed.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv WorldReconstruction
*/
ML_API MLResult ML_CALL MLPlanesDestroy(MLHandle planes_tracker);

/*!
  \brief Initiates a plane query.

  \param[in] planes_tracker Handle produced by MLPlanesCreate().
  \param[in] query Pointer to #MLPlanesQuery structure containing query parameters.
  \param[out] out_handle A pointer to an #MLHandle which will contain the handle to the query.
              If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_InvalidParam A parameter was not valid (null).
  \retval MLResult_Ok Planes query was successfully submitted.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv WorldReconstruction
*/
ML_API MLResult ML_CALL MLPlanesQueryBegin(MLHandle planes_tracker, const MLPlanesQuery *query, MLHandle *out_handle);

/*!
  \brief Gets the result of a plane query with boundaries on each plane.
  After this function has returned successfully, the handle is invalid and should be discarded.
  Also check MLPlanesQueryFlag_Polygons description for this API's further behavior.

  \apilevel 2

  \param[in] planes_tracker Handle produced by MLPlanesCreate().
  \param[in] planes_query Handle produced by MLPlanesQueryBegin().
  \param[out] out_results An array of #MLPlane structures.
  \param[out] out_num_results The count of results pointed to by out_results.
  \param[out] out_boundaries A pointer to #MLPlaneBoundariesList for the returned polygons.
              If out_boundaries is NULL, the function call will not return any polygons,
              otherwise *out_boundaries must be zero initialized.

  \retval MLResult_Ok Query completed and out_results has been populated.
  \retval MLResult_Pending Query has not yet completed. This is not a failure.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv WorldReconstruction
*/
ML_API MLResult ML_CALL MLPlanesQueryGetResultsWithBoundaries(MLHandle planes_tracker, MLHandle planes_query, MLPlane *out_results, uint32_t *out_num_results, MLPlaneBoundariesList *out_boundaries);

/*!
  \brief Release the polygons data owned by the #MLPlaneBoundariesList.
  Also, check MLPlanesQueryFlag_Polygons description for this API's further behavior.

  \apilevel 2

  \param[in] plane_boundaries Polygons Pointer to the #MLPlaneBoundariessList.

  \retval MLResult_InvalidParam A parameter was not valid (null).
  \retval MLResult_Ok memory pointed by plane_boundaries is released successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv WorldReconstruction
*/
ML_API MLResult ML_CALL MLPlanesReleaseBoundariesList(MLHandle planes_tracker, MLPlaneBoundariesList *plane_boundaries);

/*! \} */

ML_EXTERN_C_END
