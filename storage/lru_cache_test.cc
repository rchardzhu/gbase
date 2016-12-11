// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "storage/lru_cache.h"

#include <vector>
#include "base/coding.h"

#include "gtest/gtest.h"

namespace gbase {

// Conversions between numeric keys/values and the types expected by Cache.
static std::string EncodeKey(int k) {
  std::string result;
  PutFixed32(&result, k);
  return result;
}
static int DecodeKey(const StringPiece& k) {
  assert(k.size() == 4);
  return DecodeFixed32(k.data());
}
static void* EncodeValue(uintptr_t v) { return reinterpret_cast<void*>(v); }
static int DecodeValue(void* v) { return reinterpret_cast<uintptr_t>(v); }

class CacheTest {
 public:
  static CacheTest* current_;

  static void Deleter(const StringPiece& key, void* v) {
    current_->deleted_keys_.push_back(DecodeKey(key));
    current_->deleted_values_.push_back(DecodeValue(v));
  }

  static const int kCacheSize = 1000;
  std::vector<int> deleted_keys_;
  std::vector<int> deleted_values_;
  Cache* cache_;

  CacheTest() : cache_(NewLRUCache(kCacheSize)) {
    current_ = this;
  }

  ~CacheTest() {
    delete cache_;
  }

  int Lookup(int key) {
    Cache::Handle* handle = cache_->Lookup(EncodeKey(key));
    const int r = (handle == NULL) ? -1 : DecodeValue(cache_->Value(handle));
    if (handle != NULL) {
      cache_->Release(handle);
    }
    return r;
  }

  void Insert(int key, int value, int charge = 1) {
    cache_->Release(cache_->Insert(EncodeKey(key), EncodeValue(value), charge,
                                   &CacheTest::Deleter));
  }

  Cache::Handle* InsertAndReturnHandle(int key, int value, int charge = 1) {
    return cache_->Insert(EncodeKey(key), EncodeValue(value), charge,
                          &CacheTest::Deleter);
  }

  void Erase(int key) {
    cache_->Erase(EncodeKey(key));
  }
};
CacheTest* CacheTest::current_;

TEST(CacheTest, HitAndMiss) {
  CacheTest ct;
  ASSERT_EQ(-1, ct.Lookup(100));

  ct.Insert(100, 101);
  ASSERT_EQ(101, ct.Lookup(100));
  ASSERT_EQ(-1,  ct.Lookup(200));
  ASSERT_EQ(-1,  ct.Lookup(300));

  ct.Insert(200, 201);
  ASSERT_EQ(101, ct.Lookup(100));
  ASSERT_EQ(201, ct.Lookup(200));
  ASSERT_EQ(-1,  ct.Lookup(300));

  ct.Insert(100, 102);
  ASSERT_EQ(102, ct.Lookup(100));
  ASSERT_EQ(201, ct.Lookup(200));
  ASSERT_EQ(-1,  ct.Lookup(300));

  ASSERT_EQ(1, ct.deleted_keys_.size());
  ASSERT_EQ(100, ct.deleted_keys_[0]);
  ASSERT_EQ(101, ct.deleted_values_[0]);
}

TEST(CacheTest, Erase) {
  CacheTest ct;
  ct.Erase(200);
  ASSERT_EQ(0, ct.deleted_keys_.size());

  ct.Insert(100, 101);
  ct.Insert(200, 201);
  ct.Erase(100);
  ASSERT_EQ(-1,  ct.Lookup(100));
  ASSERT_EQ(201, ct.Lookup(200));
  ASSERT_EQ(1, ct.deleted_keys_.size());
  ASSERT_EQ(100, ct.deleted_keys_[0]);
  ASSERT_EQ(101, ct.deleted_values_[0]);

  ct.Erase(100);
  ASSERT_EQ(-1,  ct.Lookup(100));
  ASSERT_EQ(201, ct.Lookup(200));
  ASSERT_EQ(1, ct.deleted_keys_.size());
}

TEST(CacheTest, EntriesArePinned) {
  CacheTest ct;
  ct.Insert(100, 101);
  Cache::Handle* h1 = ct.cache_->Lookup(EncodeKey(100));
  ASSERT_EQ(101, DecodeValue(ct.cache_->Value(h1)));

  ct.Insert(100, 102);
  Cache::Handle* h2 = ct.cache_->Lookup(EncodeKey(100));
  ASSERT_EQ(102, DecodeValue(ct.cache_->Value(h2)));
  ASSERT_EQ(0, ct.deleted_keys_.size());

  ct.cache_->Release(h1);
  ASSERT_EQ(1, ct.deleted_keys_.size());
  ASSERT_EQ(100, ct.deleted_keys_[0]);
  ASSERT_EQ(101, ct.deleted_values_[0]);

  ct.Erase(100);
  ASSERT_EQ(-1, ct.Lookup(100));
  ASSERT_EQ(1, ct.deleted_keys_.size());

  ct.cache_->Release(h2);
  ASSERT_EQ(2, ct.deleted_keys_.size());
  ASSERT_EQ(100, ct.deleted_keys_[1]);
  ASSERT_EQ(102, ct.deleted_values_[1]);
}

TEST(CacheTest, EvictionPolicy) {
  CacheTest ct;
  ct.Insert(100, 101);
  ct.Insert(200, 201);
  ct.Insert(300, 301);
  Cache::Handle* h = ct.cache_->Lookup(EncodeKey(300));

  // Frequently used entry must be kept around,
  // as must things that are still in use.
  for (int i = 0; i < ct.kCacheSize + 100; i++) {
    ct.Insert(1000+i, 2000+i);
    ASSERT_EQ(2000+i, ct.Lookup(1000+i));
    ASSERT_EQ(101, ct.Lookup(100));
  }
  ASSERT_EQ(101, ct.Lookup(100));
  ASSERT_EQ(-1, ct.Lookup(200));
  ASSERT_EQ(301, ct.Lookup(300));
  ct.cache_->Release(h);
}

TEST(CacheTest, UseExceedsCacheSize) {
  // Overfill the cache, keeping handles on all inserted entries.
  std::vector<Cache::Handle*> h;
  CacheTest ct;
  for (int i = 0; i < CacheTest::kCacheSize + 100; i++) {
    h.push_back(ct.InsertAndReturnHandle(1000+i, 2000+i));
  }

  // Check that all the entries can be found in the cache.
  for (int i = 0; i < h.size(); i++) {
    ASSERT_EQ(2000+i, ct.Lookup(1000+i));
  }

  for (int i = 0; i < h.size(); i++) {
    ct.cache_->Release(h[i]);
  }
}

TEST(CacheTest, HeavyEntries) {
  // Add a bunch of light and heavy entries and then count the combined
  // size of items still in the cache, which must be approximately the
  // same as the total capacity.
  const int kLight = 1;
  const int kHeavy = 10;
  int added = 0;
  int index = 0;
  CacheTest ct;
  while (added < 2*ct.kCacheSize) {
    const int weight = (index & 1) ? kLight : kHeavy;
    ct.Insert(index, 1000+index, weight);
    added += weight;
    index++;
  }

  int cached_weight = 0;
  for (int i = 0; i < index; i++) {
    const int weight = (i & 1 ? kLight : kHeavy);
    int r = ct.Lookup(i);
    if (r >= 0) {
      cached_weight += weight;
      ASSERT_EQ(1000+i, r);
    }
  }
  ASSERT_LE(cached_weight, ct.kCacheSize + ct.kCacheSize/10);
}

TEST(CacheTest, NewId) {
  CacheTest ct;
  uint64_t a = ct.cache_->NewId();
  uint64_t b = ct.cache_->NewId();
  ASSERT_NE(a, b);
}

TEST(CacheTest, Prune) {
  CacheTest ct;
  ct.Insert(1, 100);
  ct.Insert(2, 200);

  Cache::Handle* handle = ct.cache_->Lookup(EncodeKey(1));
  ASSERT_TRUE(handle);
  ct.cache_->Prune();
  ct.cache_->Release(handle);

  ASSERT_EQ(100, ct.Lookup(1));
  ASSERT_EQ(-1, ct.Lookup(2));
}

}  // namespace gbase
