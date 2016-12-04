// Copyright 2010-2016, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef GBASE_BASE_UTIL_H_
#define GBASE_BASE_UTIL_H_

#ifdef OS_WIN
#include <Windows.h>
#include <WinCrypt.h>
#include <time.h>
#include <stdio.h>  // MSVC requires this for _vsnprintf
#else  // OS_WIN

#ifdef OS_MACOSX
#include <mach/mach.h>
#include <mach/mach_time.h>

#elif defined(OS_NACL)  // OS_MACOSX
#include <irt.h>
#endif  // OS_MACOSX or OS_NACL
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#endif  // OS_WIN

#include "base/port.h"

namespace gbase {

class Util {
 public:
  static void Sleep(uint32 msec) {
  #ifdef OS_WIN
    ::Sleep(msec);
  #else  // OS_WIN
    usleep(msec * 1000);
  #endif  // OS_WIN
  }

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(Util);
}; // class Util

}  // namespace gbase

#endif  // GBASE_BASE_UTIL_H_
