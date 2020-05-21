// %BANNER_BEGIN%
//---------------------------------------------------------------------
// %COPYRIGHT_BEGIN%
//
// Copyright (c) 2019 Magic Leap, Inc. (COMPANY) All Rights Reserved.
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
// ---------------------------------------------------------------------
// %BANNER_END%

#pragma once

#include "ml_api.h"

ML_EXTERN_C_BEGIN

/*!
  \defgroup Locale Locale
  \addtogroup Locale
  \sharedobject ml_locale
  \brief APIs for Locale.
  \{
*/

/*!
  \brief Reads the language code of the system locale.

  Example : "en" and "fr".

  \apilevel 6

  \param[out] out_language Language code defined in ISO 639. It is a pointer to
                           a string literal, and should not be freed.
                           Valid only if MLResult_Ok is returned.

  \retval MLResult_Ok The language code was retrieved successfully.
  \retval MLResult_UnspecifiedFailure Failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLLocaleGetSystemLanguage(const char **out_language);

/*!
  \brief Reads the country code of the system locale.

  Example : "us", "gb", and "fr".

  \apilevel 6

  \param[out] out_country Country code defined in ISO 3166, or an empty string.
                          It is a pointer to a string literal, and should not be freed.
                          Valid only if MLResult_Ok is returned.

  \retval MLResult_Ok The country code was retrieved successfully.
  \retval MLResult_UnspecifiedFailure Failed with an unspecified error.

  \priv None
*/
ML_API MLResult ML_CALL MLLocaleGetSystemCountry(const char **out_country);

/*! \} */

ML_EXTERN_C_END
