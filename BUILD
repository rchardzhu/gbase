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


cc_library(
    name = "gbase",
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
    ],
    hdrs = [
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
    ],
    includes = ["./"],
    copts = COPTS,
)

cc_test(
    name = "gbase_test",
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
    ],
    includes = ["./"],
    copts = COPTS,
    deps = [
        ":gbase",
        "//external:gtest",
    ],
)
