// Copyright (c) 2012 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "bloom_filter.h"

#include "base/coding.h"
#include "gtest/gtest.h"

namespace gbase {

static const int kVerbose = 1;

static StringPiece Key(int i, char* buffer) {
  EncodeFixed32(buffer, i);
  return StringPiece(buffer, sizeof(uint32_t));
}

class BloomTest {
 private:
  const FilterPolicy* policy_;
  std::string filter_;
  std::vector<std::string> keys_;

 public:
  BloomTest() : policy_(NewBloomFilterPolicy(10)) { }

  ~BloomTest() {
    delete policy_;
  }

  void Reset() {
    keys_.clear();
    filter_.clear();
  }

  void Add(const StringPiece& s) {
    keys_.push_back(s.as_string());
  }

  void Build() {
    std::vector<StringPiece> key_StringPieces;
    for (size_t i = 0; i < keys_.size(); i++) {
      key_StringPieces.push_back(StringPiece(keys_[i]));
    }
    filter_.clear();
    policy_->CreateFilter(&key_StringPieces[0], static_cast<int>(key_StringPieces.size()),
                          &filter_);
    keys_.clear();
    if (kVerbose >= 2) DumpFilter();
  }

  size_t FilterSize() const {
    return filter_.size();
  }

  void DumpFilter() {
    fprintf(stderr, "F(");
    for (size_t i = 0; i+1 < filter_.size(); i++) {
      const unsigned int c = static_cast<unsigned int>(filter_[i]);
      for (int j = 0; j < 8; j++) {
        fprintf(stderr, "%c", (c & (1 <<j)) ? '1' : '.');
      }
    }
    fprintf(stderr, ")\n");
  }

  bool Matches(const StringPiece& s) {
    if (!keys_.empty()) {
      Build();
    }
    return policy_->KeyMayMatch(s, filter_);
  }

  double FalsePositiveRate() {
    char buffer[sizeof(int)];
    int result = 0;
    for (int i = 0; i < 10000; i++) {
      if (Matches(Key(i + 1000000000, buffer))) {
        result++;
      }
    }
    return result / 10000.0;
  }
};

TEST(BloomTest, EmptyFilter) {
  BloomTest bt;
  ASSERT_TRUE(! bt.Matches("hello"));
  ASSERT_TRUE(! bt.Matches("world"));
}

TEST(BloomTest, Small) {
  BloomTest bt;
  bt.Add("hello");
  bt.Add("world");
  ASSERT_TRUE(bt.Matches("hello"));
  ASSERT_TRUE(bt.Matches("world"));
  ASSERT_TRUE(! bt.Matches("x"));
  ASSERT_TRUE(! bt.Matches("foo"));
}

static int NextLength(int length) {
  if (length < 10) {
    length += 1;
  } else if (length < 100) {
    length += 10;
  } else if (length < 1000) {
    length += 100;
  } else {
    length += 1000;
  }
  return length;
}

TEST(BloomTest, VaryingLengths) {
  char buffer[sizeof(int)];

  // Count number of filters that significantly exceed the false positive rate
  int mediocre_filters = 0;
  int good_filters = 0;

  BloomTest bt;

  for (int length = 1; length <= 10000; length = NextLength(length)) {
    bt.Reset();
    for (int i = 0; i < length; i++) {
      bt.Add(Key(i, buffer));
    }
    bt.Build();

    ASSERT_LE(bt.FilterSize(), static_cast<size_t>((length * 10 / 8) + 40))
        << length;

    // All added keys must match
    for (int i = 0; i < length; i++) {
      ASSERT_TRUE(bt.Matches(Key(i, buffer)))
          << "Length " << length << "; key " << i;
    }

    // Check false positive rate
    double rate = bt.FalsePositiveRate();
    if (kVerbose >= 1) {
      fprintf(stderr, "False positives: %5.2f%% @ length = %6d ; bytes = %6d\n",
              rate*100.0, length, static_cast<int>(bt.FilterSize()));
    }
    ASSERT_LE(rate, 0.02);   // Must not be over 2%
    if (rate > 0.0125) mediocre_filters++;  // Allowed, but not too often
    else good_filters++;
  }
  if (kVerbose >= 1) {
    fprintf(stderr, "Filters: %d good, %d mediocre\n",
            good_filters, mediocre_filters);
  }
  ASSERT_LE(mediocre_filters, good_filters/5);
}

// Different bits-per-byte
}  // namespace gbase
