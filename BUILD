# Bazel (http://bazel.io/) BUILD file for Protobuf.

package(default_visibility = ["//visibility:public"])

licenses(["notice"])
exports_files(["LICENSE"])

COPTS = [
    "-DOS_LINUX",
    "-DHAVE_PTHREAD",
    "-Wall",
    "-Wwrite-strings",
    "-Woverloaded-virtual",
    "-Wno-sign-compare",
    "-Wno-error=unused-function",
]

LINK_OPTS = ["-lpthread"]

cc_library(
    name = "base",
    srcs = [
        "base/string_piece.cc",
        "base/mutex.cc",
        "base/singleton.cc",
        "base/flags.cc",
        "base/clock.cc",
        "base/clock_mock.cc",
        "base/logging.cc",
        "base/thread.cc",
        "base/hash.cc",
        "base/util.cc",
        "base/cpu_stats.cc",
        "base/scoped_handle_win.cc",
        "base/mmap.cc",
        "base/number_util.cc",
        "base/file_stream.cc",
        "base/file_util.cc",
        "base/stopwatch.cc",
        "base/unnamed_event.cc",
        "base/process_mutex.cc",
        "base/coding.cc",
        "base/bloom_filter.cc",
        "base/multifile.cc",
        "base/version.cc",
        "base/scheduler.cc",
        "base/scheduler_stub.cc",
        "base/serialized_string_array.cc",
        "base/arena.cc",
        "base/random.cc",
    ],
    hdrs = [
        "base/atomic_pointer.h",
        "base/port.h",
        "base/string_piece.h",
        "base/thread_annotations.h",
        "base/mutex.h",
        "base/singleton.h",
        "base/flags.h",
        "base/clock.h",
        "base/clock_mock.h",
        "base/logging.h",
        "base/thread.h",
        "base/util.h",
        "base/hash.h",
        "base/bitarray.h",
        "base/trie.h",
        "base/cpu_stats.h",
        "base/scoped_handle_win.h",
        "base/mmap.h",
        "base/number_util.h",
        "base/file_stream.h",
        "base/file_util.h",
        "base/stl_util.h",
        "base/iterator_adapter.h",
        "base/stopwatch.h",
        "base/unnamed_event.h",
        "base/process_mutex.h",
        "base/coding.h",
        "base/bloom_filter.h",
        "base/multifile.h",
        "base/version_def.h",
        "base/version.h",
        "base/scheduler.h",
        "base/scheduler_stub.h",
        "base/serialized_string_array.h",
        "base/arena.h",
        "base/random.h",
        "base/skiplist.h"
    ],
    includes = ["./"],
    copts = COPTS,
)

cc_test(
    name = "base_test",
    srcs = [
        "base/string_piece_test.cc",
        "base/clock_mock_test.cc",
        "base/clock_test.cc",
        "base/thread_test.cc",
        "base/flags_test.cc",
        "base/singleton_test.cc",
        "base/mutex_test.cc",
        "base/logging_test.cc",
        "base/hash_test.cc",
        "base/bitarray_test.cc",
        "base/trie_test.cc",
        "base/cpu_stats_test.cc",
        "base/number_util_test.cc",
        "base/mmap_test.cc",
        "base/file_util_test.cc",
        "base/stl_util_test.cc",
        "base/iterator_adapter_test.cc",
        "base/stopwatch_test.cc",
        "base/unnamed_event_test.cc",
        "base/process_mutex_test.cc",
        "base/util_test.cc",
        "base/coding_test.cc",
        "base/bloom_filter_test.cc",
        "base/multifile_test.cc",
        "base/version_test.cc",
        "base/scheduler_test.cc",
        "base/scheduler_stub_test.cc",
        "base/serialized_string_array_test.cc",
        "base/arena_test.cc",
        "base/random_test.cc",
        "base/skiplist_test.cc",
    ],
    includes = ["./"],
    copts = COPTS,
    deps = [
        ":base",
        "//external:gtest",
    ],
)

cc_binary(
    name = "stopwatch_main",
    srcs = ["base/stopwatch_main.cc"],
    copts = COPTS,
    linkopts = LINK_OPTS,
    deps = [":base",],
)

cc_binary(
    name = "cpu_stats_main",
    srcs = ["base/cpu_stats_main.cc"],
    copts = COPTS,
    linkopts = LINK_OPTS,
    deps = [":base",],
)

cc_library(
    name = "storage",
    srcs = [
        "storage/lru_storage.cc",
    ],
    hdrs= [
        "storage/simple_lru_cache.h",
        "storage/lru_storage.h",
    ],
    copts = COPTS,
    linkopts = LINK_OPTS,
    deps = [":base",],
)

cc_test(
    name = "storage_test",
    srcs = [
        "storage/lru_storage_test.cc",
    ],
    includes = ["./"],
    copts = COPTS,
    deps = [
        ":storage",
        "//external:gtest",
    ],
)

cc_binary(
    name = "simple_lru_cache_app",
    srcs = ["storage/simple_lru_cache_main.cc"],
    copts = COPTS,
    linkopts = LINK_OPTS,
    deps = [":storage",],
)

cc_binary(
    name = "lru_storage_app",
    srcs = ["storage/lru_storage_main.cc"],
    copts = COPTS,
    linkopts = LINK_OPTS,
    deps = [":storage",],
)
