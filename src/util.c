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

/**
 * @file    util.c
 * @author  Matt Windsor
 * @brief   Miscellaneous utility functions.
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#include "util.h"
#include "parser.h"
#include "main.h"


/* - DEFINITIONS - */

/* ~~ Safe type conversions */

/* Safely convert a long integer to a 16-bit unsigned integer. */

inline uint16_t
long_to_uint16 (long integer)
{
  g_assert (integer >= 0);
  g_assert (integer <= UINT16_MAX);

  return (uint16_t) integer;
}


/* Safely convert a long integer to a signed 16_bit integer. */

inline int16_t
long_to_int16 (long integer)
{
  g_assert (integer >= INT16_MIN);
  g_assert (integer <= INT16_MAX);

  return (int16_t) integer;
}


/* Safely convert an unsigned long integer to an unsigned 16-bit integer. */

inline uint16_t
ulong_to_uint16 (unsigned long integer)
{
  g_assert (integer <= UINT16_MAX);

  return (uint16_t) integer;
}


/* Safely convert an unsigned long integer to a signed 16-bit integer. */

inline int16_t
ulong_to_int16 (unsigned long integer)
{
  g_assert (integer <= INT16_MAX);

  return (int16_t) integer;
}


/* ~~ Memory management */

/* Calloc and check */
inline void*
xcalloc (size_t nmemb, size_t size)
{
  void *memory = calloc (nmemb, size);

  g_assert (memory);

  return memory;
}

/* ~~ Error reporting */

/* Fatal error. */

void
fatal (const char message[], ...)
{
  va_list ap;
  va_start (ap, message);
  g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, message, ap);
  
  cleanup ();
  exit (1);
}


/* Non-fatal error. */

void
error (const char message[], ...)
{
  va_list ap;
  va_start (ap, message);
  g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, message, ap);
}


