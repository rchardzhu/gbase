// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef GBASE_BASE_RANDOM_H_
#define GBASE_BASE_RANDOM_H_

#include <stdint.h>

#include "base/port.h"

namespace gbase {

// A very simple random number generator.  Not especially good at
// generating truly random bits, but good enough for our needs in this
// package.
class Random {
 private:
  uint32_t seed_;
 public:
  explicit Random(uint32_t s) : seed_(s & 0x7fffffffu) {
    // Avoid bad seeds.
    if (seed_ == 0 || seed_ == 2147483647L) {
      seed_ = 1;
    }
  }
  uint32_t Next() {
    static const uint32_t M = 2147483647L;   // 2^31-1
    static const uint64_t A = 16807;  // bits 14, 8, 7, 5, 2, 1, 0
    // We are computing
    //       seed_ = (seed_ * A) % M,    where M = 2^31-1
    //
    // seed_ must not be zero or M, or else all subsequent computed values
    // will be zero or M respectively.  For all other values, seed_ will end
    // up cycling through every number in [1,M-1]
    uint64_t product = seed_ * A;

    // Compute (product % M) using the fact that ((x << 31) % M) == x.
    seed_ = static_cast<uint32_t>((product >> 31) + (product & M));
    // The first reduction may overflow by 1 bit, so we may need to
    // repeat.  mod == M is not possible; using > allows the faster
    // sign-bit-based test.
    if (seed_ > M) {
      seed_ -= M;
    }
    return seed_;
  }
  // Returns a uniformly distributed value in the range [0..n-1]
  // REQUIRES: n > 0
  uint32_t Uniform(int n) { return Next() % n; }

  // Randomly returns true ~"1/n" of the time, and false otherwise.
  // REQUIRES: n > 0
  bool OneIn(int n) { return (Next() % n) == 0; }

  // Skewed: pick "base" uniformly from range [0,max_log] and then
  // return "base" random bits.  The effect is to pick a number in the
  // range [0,2^max_log-1] with exponential bias towards smaller numbers.
  uint32_t Skewed(int max_log) {
    return Uniform(1 << Uniform(max_log + 1));
  }

  // Generate a random sequence. It uses secure method if possible, or Random()
  // as a fallback method.
  static void GetRandomSequence(char *buf, size_t buf_size);
  static void GetRandomAsciiSequence(char *buf, size_t buf_size);

  // Return random variable whose range is [0..size-1].
  // This function uses rand() internally, so don't use it for
  // security-sensitive purpose.
  // Caveats: The returned value does not have good granularity especially
  // when |size| is larger than |RAND_MAX|.
  // TODO(yukawa): Improve the granularity.
  // TODO(yukawa): Clarify the semantics when |size| is 0 or smaller.
  static int RandRandom(int size);

  // Set the seed of Random::RandRandom().
  static void SetRandRandomSeed(uint32 seed);
};

}  // namespace gbase

#endif  // GBASE_BASE_RANDOM_H_
