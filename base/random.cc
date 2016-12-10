// Copyright (c) 2016 Richard zhu(rchardzhu@gmail.com). All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//

#include "base/random.h"

#include <string.h>
#include <fstream>

#include "base/logging.h"

namespace gbase {

namespace {
bool GetSecureRandomSequence(char *buf, size_t buf_size) {
  memset(buf, '\0', buf_size);
#ifdef OS_WIN
  HCRYPTPROV hprov;
  if (!::CryptAcquireContext(&hprov,
                             NULL,
                             NULL,
                             PROV_RSA_FULL,
                             CRYPT_VERIFYCONTEXT)) {
    return false;
  }
  if (!::CryptGenRandom(hprov,
                        static_cast<DWORD>(buf_size),
                        reinterpret_cast<BYTE *>(buf))) {
    ::CryptReleaseContext(hprov, 0);
    return false;
  }
  ::CryptReleaseContext(hprov, 0);
  return true;
#elif defined(OS_NACL)
  struct nacl_irt_random interface;

  if (nacl_interface_query(NACL_IRT_RANDOM_v0_1, &interface,
                           sizeof(interface)) != sizeof(interface)) {
    DLOG(ERROR) << "Cannot get NACL_IRT_RANDOM_v0_1 interface";
    return false;
  }

  size_t nread;
  const int error = interface.get_random_bytes(buf, buf_size, &nread);
  if (error != 0) {
    LOG(ERROR) << "interface.get_random_bytes error: " << error;
    return false;
  } else if (nread != buf_size) {
    LOG(ERROR) << "interface.get_random_bytes error. nread: " << nread
               << " buf_size: " << buf_size;
    return false;
  }
  return true;
#else  // !OS_WIN && !OS_NACL
  // Use non blocking interface on Linux.
  // Mac also have /dev/urandom (although it's identical with /dev/random)
  ifstream ifs("/dev/urandom", ios::binary);
  if (!ifs) {
    return false;
  }
  ifs.read(buf, buf_size);
  return true;
#endif  // OS_WIN or OS_NACL
}
}  // namespace

void Random::GetRandomSequence(char *buf, size_t buf_size) {
  if (GetSecureRandomSequence(buf, buf_size)) {
    return;
  }
  LOG(ERROR) << "Failed to generate secure random sequence. "
             << "Make it with Util::Random()";
  for (size_t i = 0; i < buf_size; ++i) {
    buf[i] = static_cast<char>(Random::RandRandom(256));
  }
}

void Random::GetRandomAsciiSequence(char *buf, size_t buf_size) {
  // We use this map to convert a random byte value to an ascii character.
  // Its size happens to be 64, which is just one fourth of the number of
  // values that can be represented by a single byte value. This accidental
  // coincidence makes implementation of the method quite simple.
  const char kCharMap[] =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_";
  GetRandomSequence(buf, buf_size);
  for (size_t i = 0; i < buf_size; ++i) {
    // The size of kCharMap is just one fourth of 256. So we don't need to
    // care if probability distribution over the characters is biased.
    buf[i] = kCharMap[static_cast<unsigned char>(buf[i]) % 64];
  }
}

int Random::RandRandom(int size) {
  DLOG_IF(FATAL, size < 0) << "|size| should be positive or 0. size: " << size;
  // Caveat: RAND_MAX is likely to be too small to achieve fine-grained
  // uniform distribution.
  // TODO(yukawa): Improve the resolution.
  return static_cast<int> (1.0 * size * rand() / (RAND_MAX + 1.0));
}

void Random::SetRandRandomSeed(uint32 seed) {
  ::srand(seed);
}

}  // namespace gbase
