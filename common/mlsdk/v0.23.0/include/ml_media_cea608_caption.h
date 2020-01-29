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
  \defgroup MediaPlayer Media Player
  \addtogroup MediaPlayer
  \sharedobject ml_media_ccparser
  \brief APIs for CEA608 Closed Captions Parser

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.
  \{
*/

/*!
  \brief Cea608 Caption Dimension constants.
  \apilevel 2
*/
typedef enum MLCea608CaptionDimension {
  MLCea608_CCMaxRows = 15,
  MLCea608_CCMaxCols = 32,
  MLCea608_CCMaxRowsPlus2 = 17,
  MLCea608_CCMaxColsPlus2 = 34,
  /*! Ensure enum is represented as 32 bits. */
  MLCea608_CCMaxRowsCols_Ensure32Bits = 0x7FFFFFFF
} MLCea608CaptionDimension;

/*!
  \brief Cea608 caption color code.
  \apilevel 2
*/
typedef enum MLCea608CaptionColor {
  MLCea608CaptionColor_White        = 0,
  MLCea608CaptionColor_Green        = 1,
  MLCea608CaptionColor_Blue         = 2,
  MLCea608CaptionColor_Cyan         = 3,
  MLCea608CaptionColor_Red          = 4,
  MLCea608CaptionColor_Yellow       = 5,
  MLCea608CaptionColor_Magenta      = 6,
  MLCea608CaptionColor_Invalide     = 7,
  /*! Ensure enum is represented as 32 bits. */
  MLCea608CaptionColor_Ensure32Bits = 0x7FFFFFFF
} MLClosedCaptionColor;

/*!
  \brief Cea608 caption style code.
  \apilevel 2
*/
typedef enum MLCea608CaptionStyle {
  MLCea608CaptionStyle_Italics      = 0x00000001,
  MLCea608CaptionStyle_Underline    = 0x00000002,
  MLCea608CaptionStyle_Ensure32Bits = 0x7FFFFFFF
} MLCea608CaptionStyle;

/*!
  \brief Cea608 caption style and color.
  \apilevel 2
*/
typedef struct MLCea608CaptionStyleColor {
  uint32_t style;
  uint32_t color;
} MLCea608CaptionStyleColor;

/*!
  \brief Cea608 caption preamble address code.
  \apilevel 2
*/
typedef struct MLCea608CaptionPAC {
  MLCea608CaptionStyleColor styleColor;
  uint32_t row;
  uint32_t col;
} MLCea608CaptionPAC;

/*!
  \brief Cea608 caption line structure.
  \apilevel 2
*/
typedef struct MLCea608CaptionLine {
  char displayChars[MLCea608_CCMaxColsPlus2];
  MLCea608CaptionStyleColor *midRowStyles[MLCea608_CCMaxColsPlus2];
  MLCea608CaptionPAC *pacStyles[MLCea608_CCMaxColsPlus2];
} MLCea608CaptionLine;

/*!
  \brief Cea608 caption segment structure.
  \apilevel 2
*/
typedef struct MLCea608CaptionSegment {
  uint32_t version;
  MLCea608CaptionLine *ccLine[MLCea608_CCMaxRowsPlus2];
} MLCea608CaptionSegment;

/*!
  \brief Initialize an MLCea608CaptionSegment structure.
  \apilevel 2
*/
ML_STATIC_INLINE void MLCea608CaptionSegmentInit(MLCea608CaptionSegment *cc_seg) {
  if (cc_seg) {
    cc_seg->version = 1;
    for(uint32_t i = 0; i < MLCea608_CCMaxRowsPlus2; i++) {
      cc_seg->ccLine[i] = NULL;
    }
  }
}

/*!
  \brief Create internal closed caption parser object.
  \apilevel 2

  \param[out] out_handle A pointer to an #MLHandle which will contain the handle to the CC parser.
              If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_Ok Internal closed caption parser object was created successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCCParserCreate(MLHandle *out_handle);

/*!
  \brief Return closed caption for display.
  \apilevel 2

  \param[in] cc_handle A handle to a CC parser.
  \param[in] subtitle_data data of MLMediaPlayerSubtitleData structure returned from
             MLMediaPlayerGetSubtitleEx function
  \param[in] subtitle_data_size data_size of MLMediaPlayerSubtitleData structure returned
             from MLMediaPlayerGetSubtitleEx function
  \param[out] out_cc_seg The returned MLCea608CaptionSegment pointer for CC display.

  \retval MLResult_Ok CC is ready to display and content is stored in out_ccSeg.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.
  \retval MLMediaGenericResult_NoInit CC parser was not properly built or initialized.
  \retval MLMediaGenericResult_NotEnoughData more data is required and no displayable CC for now.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCCParserGetDisplayable(MLHandle cc_handle, uint8_t* subtitle_data, int32_t subtitle_data_size, MLCea608CaptionSegment *out_cc_seg);

/*!
  \brief Release memory held by last displayable CC segment.
  \apilevel 2

  \param[in] cc_handle A handle to a CC parser.
  \param[in] ccSeg MLCea608CaptionSegment pointer holding last displayable CC.

  \retval MLResult_Ok memeory was released successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCCParserReleaseSegment(MLHandle cc_handle, MLCea608CaptionSegment *cc_seg);

/*!
  \brief Release internal CC parser object.
  \apilevel 2

  \param[in] cc_handle A handle to a CC parser.

  \retval MLResult_Ok internal CC parser object was released successfully.
  \retval MLResult_UnspecifiedFailure The operation failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCCParserDestroy(MLHandle cc_handle);

ML_EXTERN_C_END
