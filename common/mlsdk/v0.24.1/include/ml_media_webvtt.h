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
  \brief Cue orientation setting.
  \apilevel 4

*/
typedef enum MLWebVTTOrientation {
  MLWebVTTOrientation_Horizontal   = 0,
  MLWebVTTOrientation_Vertical     = 1,
  /*! Ensure enum is represented as 32 bits. */
  MLWebVTTOrientation_Ensure32Bits = 0x7FFFFFFF
} MLWebVTTOrientation;

/*!
  \brief Cue direction setting.
  \apilevel 4

*/
typedef enum MLWebVTTDirection {
  MLWebVTTDirection_Default      = 0,
  MLWebVTTDirection_LeftToRight  = 1,
  MLWebVTTDirection_RightToLeft  = 2,
  /*! Ensure enum is represented as 32 bits. */
  MLWebVTTDirection_Ensure32Bits = 0x7FFFFFFF
} MLWebVTTDirection;

/*!
  \brief Cue aligment setting.
  \apilevel 4

*/
typedef enum MLWebVTTAlign {
  MLWebVTTAlign_Start        = 0,
  MLWebVTTAlign_Middle       = 1,
  MLWebVTTAlign_End          = 2,
  MLWebVTTAlign_Left         = 3,
  MLWebVTTAlign_Right        = 4,
  /*! Ensure enum is represented as 32 bits. */
  MLWebVTTAlign_Ensure32Bits = 0x7FFFFFFF
} MLWebVTTAlign;

/*!
  \brief WebVTT data structure.
  \apilevel 4

*/
typedef struct MLWebVTTData {
  /*! Track cue start time in milliseconds. */
  int64_t             start_time_ms;
  /*! Track cue end time in milliseconds. */
  int64_t             end_time_ms;
  /*!  WebVTT file body encoded as UTF-8. */
  char*               body;
  /*! A sequence of characters unique amongst all the WebVTT cue identifiers. */
  char*               id;
  /*! A boolean indicating whether the line is an integer number of lines. */
  bool                snap_to_lines;
  /*! Orientation of the cue. */
  MLWebVTTOrientation orientation;
  /*! The writing direction. */
  MLWebVTTDirection   direction;
  /*! Relative cue line position. */
  float               relative_line_position;
  /*! WebVTT cue line number. */
  int32_t             line_number;
  /*! The indent of the cue box in the direction defined by the writing direction. */
  float               text_position;
  /*! WebVTT cue size. */
  float               size;
  /*! WebVTT cue text alignment. */
  MLWebVTTAlign       align;
} MLWebVTTData;

ML_EXTERN_C_END
