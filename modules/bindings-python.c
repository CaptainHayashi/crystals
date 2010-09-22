/*
* Crystals (working title)
*
* Copyright (c) 2010 Matt Windsor, Michael Walker and Alexander
* Preisinger.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
*
* * The names of contributors may not be used to endorse or promote
* products derived from this software without specific prior
* written permission.
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

/** @file bindings-python.c
 * @author Alexander Preisinger
 * @brief Python module for scripting
 */

#include "Python.h"


/** Initialise the python module. */

void
init (void);


/** De-initialise the python module. */

void
term (void);


/** Execute the file
 *  The file has to be a valid python source
 *
 *  @param path Path to the file.
 *
 *  @return Returns SUCCESS if success else FAILURE
 * */

int
run_file(const char* path);


/** Test the python module. */

void
test (void);

void
init (void)
{
    Py_Initialize();
}

void
term (void)
{
    Py_Finalize();
}

int
run_file(const char* path)
{
  FILE *stream;

  short EXIT;

  stream = fopen(path, "r");

  if (stream)
    {
      if (PyRun_SimpleFile(stream, path) == 0)
        EXIT = 1;
      else
        EXIT = 0;
    }
  else
    EXIT = 0;

  fclose(stream);
  return EXIT;
}

void
test (void)
{
    /* a simple test */
    PyRun_SimpleString("print 'Hello World'");
}
