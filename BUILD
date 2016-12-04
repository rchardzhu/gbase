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


cc_library(name = "gbase",
     srcs = [
        "base/string_piece.cc",
        "base/mutex.cc",
        "base/singleton.cc",
        "base/flags.cc",
    ],
    hdrs = [
        "base/port.h",
        "base/string_piece.h",
        "base/thread_annotations.h",
        "base/mutex.h",
        "base/singleton.h",
        "base/flags.h",
    ],
    includes = ["./"],
    copts = COPTS,
)
