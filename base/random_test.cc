// Copyright 2010-2016, Google Inc.
// All rights reserved.

#include "base/random.h"

#include <climits>

#include "gtest/gtest.h"

namespace gbase {

TEST(RandomTest, RandomSeedTest) {
  Random::SetRandRandomSeed(0);
  const int first_try = Random::RandRandom(INT_MAX);
  const int second_try = Random::RandRandom(INT_MAX);
  EXPECT_NE(first_try, second_try);

  // Reset the seed.
  Random::SetRandRandomSeed(0);
  EXPECT_EQ(first_try, Random::RandRandom(INT_MAX));
}

}  // namespace gbase
