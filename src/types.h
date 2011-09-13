/*
 * Crystals (working title)
 *
 * Copyright (c) 2010 Matt Windsor, Michael Walker and Alexander
 *                    Preisinger.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *   * The names of contributors may not be used to endorse or promote
 *     products derived from this software without specific prior
 *     written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * AFOREMENTIONED COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @file    src/types.h
 *  @author  Matt Windsor
 *  @brief   Type definitions.
 *
 *  Crystals uses
 */


#ifndef _TYPES_H
#define _TYPES_H


/* Windows systems */

#ifdef PLATFORM_WINDOWS

#define TYPES_DEFINED

#include "platform/w32-types.h"

#endif /* PLATFORM_WINDOWS */

#ifndef TYPES_DEFINED
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# else
/* Hopefully configure will have implemented the types we want to use.
 * If not, then we'll roll our own.
 */
#  ifndef int8_t

typedef unsigned char  uint8_t;   /**< Unsigned 8-bit integer. */
typedef unsigned short uint16_t;  /**< Unsigned 16-bit integer. */
typedef unsigned int   uint32_t;  /**< Unsigned 32-bit integer. */

typedef signed char    int8_t;    /**< Signed 8-bit integer. */
typedef signed short   int16_t;   /**< Signed 16-bit integer. */
typedef signed int     int32_t;   /**< Signed 32-bit integer. */

enum
  {
    UINT8_MAX  = 255,         /**< Maximum value of uint8_t.  */
    UINT16_MAX = 65535,       /**< Maximum value of uint16_t. */
    UINT32_MAX = 4294967295,  /**< Maximum value of uint32_t. */
    INT8_MIN   = -128,        /**< Minimum value of int8_t.   */
    INT16_MIN  = -32768,      /**< Minimum value of int16_t.  */
    INT32_MIN  = -2147483648, /**< Minimum value of int32_t.  */
    INT8_MAX   = 127,         /**< Maximum value of int8_t.   */
    INT16_MAX  = 32767,       /**< Maximum value of int16_t.  */
    INT32_MAX  = 2147483647   /**< Maximum value of int32_t.  */
  };
#  endif /* !defined int8_t */
# endif /* HAVE_STDINT_H */

# ifdef HAVE_STDBOOL_H
#  include <stdbool.h>
# else
#  ifndef HAVE__BOOL
#   ifdef __cplusplus
typedef bool _Bool;
#   else
#    define _Bool signed char
#   endif
#  endif
#  define bool _Bool
#  define false 0
#  define true 1
#  define __bool_true_false_are_defined 1
# endif /* HAVE_STDBOOL_H */

typedef _Bool bool_t; /* To be removed later */

#endif /* not TYPES_DEFINED */

#endif /* _TYPES_H */
