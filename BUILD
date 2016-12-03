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
    ],
    hdrs = [
        "base/port.h",
        "base/string_piece.h",
    ],
    includes = ["./"],
    copts = COPTS,
)
