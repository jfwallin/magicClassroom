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
  \brief APIs for CEA708 Closed Captions Parser.

  Stringify the error codes returned by these APIs, call MLMediaResultGetString.

  \{
*/

/*!
  \brief Cea708 Caption maximum windows constant.

  \apilevel 8
*/
enum {
  MLCea708CaptionWindows_Max = 8
};

/*!
  \brief Maximum size for Cea708 Caption Emit command buffers.

  \apilevel 8
*/
enum {
  MLCea708CaptionEmitCommandBuffer_MaxSize = 20,
};

/*!
  \brief Cea708 Caption Emit Commands constants.

  \apilevel 8
*/
typedef enum MLCea708CaptionEmitCommand {
  /*! Buffer. */
  MLCea708CaptionEmitCommand_Buffer     = 1,
  /*! Control. */
  MLCea708CaptionEmitCommand_Control    = 2,
  /*!
  \brief SetCurrentWindow tells the caption decoder which window the following commands describe:
   - SetWindowAttributes
   - SetPenAttributes
   - SetPenColor
   - SetPenLocation.
   If the window specified has not already been created with a DefineWindow command then,
   SetCurrentWindow and the window property commands can be safely ignored.
  */
  MLCea708CaptionEmitCommand_CWX        = 3,
  /*! ClearWindows clears all the windows specified in the 8 bit window bitmap. */
  MLCea708CaptionEmitCommand_CLW        = 4,
  /*! DisplayWindows displays all the windows specified in the 8 bit window bitmap. */
  MLCea708CaptionEmitCommand_DSW        = 5,
  /*! HideWindows hides all the windows specified in the 8 bit window bitmap. */
  MLCea708CaptionEmitCommand_HDW        = 6,
  /*! ToggleWindows hides all displayed windows, and displays all hidden windows specified in the 8 bit window bitmap. */
  MLCea708CaptionEmitCommand_TGW        = 7,
  /*!
  /brief DeleteWindows deletes all the windows specified in the 8 bit window bitmap.
   If the current window, as specified by the last SetCurrentWindow command,
   is deleted then the current window becomes undefined and the window attribute commands
   should have no effect until after the next SetCurrentWindow or DefineWindow command.
  */
  MLCea708CaptionEmitCommand_DLW        = 8,
  /*! Delay suspends all processing of the current service, except for DelayCancel and Reset scanning. */
  MLCea708CaptionEmitCommand_DLY        = 9,
  /*! DelayCancel terminates any active delay and resumes normal command processing. DelayCancel should be scanned for during a Delay. */
  MLCea708CaptionEmitCommand_DLC        = 10,
  /*! Reset deletes all windows, cancels any active delay, and clears the buffer before the Reset command. Reset should be scanned for during a Delay. */
  MLCea708CaptionEmitCommand_RST        = 11,
  /*! The SetPenAttributes command specifies how certain attributes of subsequent characters are to be rendered in the current window, until the next SetPenAttributes command. */
  MLCea708CaptionEmitCommand_SPA        = 12,
  /*! SetPenColor sets the foreground, background, and edge color for the subsequent characters. */
  MLCea708CaptionEmitCommand_SPC        = 13,
  /*! SetPenLocation sets the location of for the next bit of appended text in the current window. It has two parameters, row and column. */
  MLCea708CaptionEmitCommand_SPL        = 14,
  /*! SetWindowAttributes Sets the window attributes of the current window. */
  MLCea708CaptionEmitCommand_SWA        = 15,
  /*! DefineWindow0-7 creates one of the eight windows used by a caption decoder. */
  MLCea708CaptionEmitCommand_DFX        = 16,
  /*! Ensure enum is represented as 32 bits. */
  MLCea708CaptionEmitCommand_Ensure32Bits = 0x7FFFFFFF
} MLCea708CaptionEmitCommand;

/*!
  \brief Cea708 Caption Emit Command Control constants.

  \apilevel 8
*/
typedef enum MLCea708CaptionEmitCommandControl {
  /*! End of Text. */
  MLCea708CaptionEmitCommandControl_ETX    = 0x03,
  MLCea708CaptionEmitCommandControl_BS     = 0x08,
  MLCea708CaptionEmitCommandControl_FF     = 0x0c,
  MLCea708CaptionEmitCommandControl_HCR    = 0x0e,
  /*! Ensure enum is represented as 32 bits. */
  MLCea708CaptionEmitCommandControl_Ensure32Bits = 0x7FFFFFFF
} MLCea708CaptionEmitCommandControl;

/*!
  \brief Cea708 Caption Color Opacity constants.

  \apilevel 8
*/
typedef enum MLCea708CaptionColorOpacity {
  MLCea708CaptionColorOpacity_SOLID           = 0,
  MLCea708CaptionColorOpacity_FLASH           = 1,
  MLCea708CaptionColorOpacity_TRANSLUCENT     = 2,
  MLCea708CaptionColorOpacity_TRANSPARENT     = 3,
  /*! Ensure enum is represented as 32 bits. */
  MLCea708CaptionColorOpacity_Ensure32Bits = 0x7FFFFFFF
} MLCea708CaptionColorOpacity;

/*!
  \brief Cea708 Caption Color.

  \apilevel 8
*/
typedef struct MLCea708CaptionColor {
  MLCea708CaptionColorOpacity opacity;
  int32_t red;
  int32_t green;
  int32_t blue;
} MLCea708CaptionColor;

/*!
  \brief Cea708 Caption Pen Size constants.

  \apilevel 8
*/
typedef enum MLCea708CaptionPenSize {
  MLCea708CaptionPenSize_SMALL     = 0,
  MLCea708CaptionPenSize_STANDARD  = 1,
  MLCea708CaptionPenSize_LARGE     = 2,
  /*! Ensure enum is represented as 32 bits. */
  MLCea708CaptionPenSize_Ensure32Bits = 0x7FFFFFFF
} MLCea708CaptionPenSize;

/*!
  \brief Cea708 Caption Pen Offset constants.

  \apilevel 8
*/
typedef enum MLCea708CaptionPenOffset {
  MLCea708CaptionPenOffset_SUBSCRIPT     = 0,
  MLCea708CaptionPenOffset_NORMAL        = 1,
  MLCea708CaptionPenOffset_SUPERSCRIPT   = 2,
  /*! Ensure enum is represented as 32 bits. */
  MLCea708CaptionPenOffset_Ensure32Bits = 0x7FFFFFFF
} MLCea708CaptionPenOffset;

/*!
  \brief Cea708 Caption Pen Attributes.

  \apilevel 8
*/
typedef struct MLCea708CaptionPenAttr {
  MLCea708CaptionPenSize pen_size;
  MLCea708CaptionPenOffset pen_offset;
  int32_t text_tag;
  int32_t font_tag;
  int32_t edge_type;
  bool underline;
  bool italic;
} MLCea708CaptionPenAttr;

/*!
  \brief Cea708 Caption Pen Color.

  \apilevel 8
*/
typedef struct MLCea708CaptionPenColor {
  MLCea708CaptionColor foreground_color;
  MLCea708CaptionColor background_color;
  MLCea708CaptionColor edge_color;
} MLCea708CaptionPenColor;

/*!
  \brief Cea708 Caption Pen Location.

  \apilevel 8
*/
typedef struct MLCea708CaptionPenLocation {
  int32_t row;
  int32_t column;
} MLCea708CaptionPenLocation;

/*!
  \brief Cea708 Caption Window Attributes.

  \apilevel 8
*/
typedef struct MLCea708CaptionWindowAttr {
  MLCea708CaptionColor fill_color;
  MLCea708CaptionColor border_color;
  int32_t border_type;
  bool word_wrap;
  int32_t print_direction;
  int32_t scroll_direction;
  int32_t justify;
  int32_t effect_direction;
  int32_t effect_speed;
  int32_t display_effect;
} MLCea708CaptionWindowAttr;

/*!
  \brief Cea708 Caption Window.

  \apilevel 8
*/
typedef struct MLCea708CaptionWindow {
  int32_t id;
  bool visible;
  bool row_lock;
  bool column_lock;
  int32_t priority;
  bool relative_positioning;
  int32_t anchor_vertical;
  int32_t anchor_horizontal;
  int32_t anchor_id;
  int32_t row_count;
  int32_t column_count;
  int32_t pen_style;
  int32_t window_style;
} MLCea708CaptionWindow;

/*!
  \brief Cea708 Caption Event.

  \apilevel 8
*/
typedef struct MLCea708CaptionEvent {
  /*! Emitted Cea708 caption event type. */
  MLCea708CaptionEmitCommand type;
  /*!
    \brief Emitted Cea708 caption event specific data, based on the event type.

    If the type is MLCea708CaptionEmitCommand_Buffer, obj will point to a null terminated string of maximum size MLCea708CaptionEmitCommandBuffer_MaxSize bytes.
    If the type is MLCea708CaptionEmitCommand_Control, obj will point to one byte character.
    If the type is MLCea708CaptionEmitCommand_CWX, obj will point to an integer.
    If the type is MLCea708CaptionEmitCommand_CLW, obj will point to an integer.
    If the type is MLCea708CaptionEmitCommand_DSW, obj will point to an integer.
    If the type is MLCea708CaptionEmitCommand_HDW, obj will point to an integer.
    If the type is MLCea708CaptionEmitCommand_TGW, obj will point to an integer.
    If the type is MLCea708CaptionEmitCommand_DLW, obj will point to an integer.
    If the type is MLCea708CaptionEmitCommand_DLY, obj will point to an integer.
    If the type is MLCea708CaptionEmitCommand_DLC, obj will be NULL.
    If the type is MLCea708CaptionEmitCommand_RST, obj will be NULL.
    If the type is MLCea708CaptionEmitCommand_SPA, obj will point to MLCea708CaptionPenAttr.
    If the type is MLCea708CaptionEmitCommand_SPC, obj will point to MLCea708CaptionPenColor.
    If the type is MLCea708CaptionEmitCommand_SPL, obj will point to MLCea708CaptionPenLocation.
    If the type is MLCea708CaptionEmitCommand_SWA, obj will point to MLCea708CaptionWindowAttr.
    If the type is MLCea708CaptionEmitCommand_DFX, obj will point to MLCea708CaptionWindow.
  */
  const void *obj;
} MLCea708CaptionEvent;

/*!
  \brief Create Cea708 parser object.

  \apilevel 8

  \param[out] out_handle A pointer to an #MLHandle which will contain the handle to the Cea708 parser.
              If this operation fails, out_handle will be #ML_INVALID_HANDLE.

  \retval MLResult_Ok Internal Cea708 parser object was created successfully.
  \retval MLResult_AllocFailed The operation failed with memory allocation error.
  \retval MLResult_InvalidParam out_handle is null.
  \priv None
*/
ML_API MLResult ML_CALL MLMediaCea708ParserCreate(MLHandle *out_handle);

/*!
  \brief Provide Cea708 parser with subtitle raw data for parsing.

  \apilevel 8

  \param[in] cc_handle A handle to a Cea708 parser.
  \param[in] subtitle_data data of MLMediaPlayerSubtitleData
  \param[in] subtitle_data_size data_size of MLMediaPlayerSubtitleData

  \retval MLResult_Ok the operation was successful.
  \retval MLResult_InvalidParam the input parameter(s) is(are) wrong.
  \retval MLMediaGenericResult_NoInit Cea708 parser was not properly built or initialized.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCea708ParserParse(MLHandle cc_handle, const uint8_t *subtitle_data, uint32_t subtitle_data_size);

/*!
  \brief Release Cea708 parser object.

  \apilevel 8

  \param[in] cc_handle A handle to a Cea708 parser.

  \retval MLResult_Ok internal Cea708 parser object was released successfully.
  \retval MLResult_InvalidParam cc_handle is wrong or not found.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCea708ParserDestroy(MLHandle cc_handle);

/*!
  \brief Callback signature called when Cea708 parser emits event.

  \apilevel 8

  \param[in] cc_handle #MLHandle is a Cea708 Parser instance for which callback was called.
  \param[in] out_event #MLCea708CaptionEvent conveys caption event to listener.
  \param[in] data Custom data to be returned when callback is fired.
*/
typedef void(*MLMediaCea708ParserEmitEventCallback)(MLHandle cc_handle, const MLCea708CaptionEvent *out_event, void *data);

/*!
  \brief Set callback to get notified when a caption event has generated.

  \apilevel 8

  \param[in] cc_handle A handle to a Cea708 parser.
  \param[in] on_emit_event_callback function pointer to be called.
            See MLMediaCea708ParserEmitEventCallback()) callback typedef declaration and comments
            for explanation on expected callback parameters.
  \param[in] data Custom data to be returned when callback is fired.

  \retval MLResult_InvalidParam cc_handle handle was not found or on_emit_event_callback is NULL.
  \retval MLResult_Ok Callback was successfully set.

  \priv None
*/
ML_API MLResult ML_CALL MLMediaCea708ParserSetEmitEventCallback(MLHandle cc_handle, MLMediaCea708ParserEmitEventCallback on_emit_event_callback, void *data);

/*! \} */

ML_EXTERN_C_END
