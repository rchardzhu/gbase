# Bazel (http://bazel.io/) BUILD file for Protobuf.

package(default_visibility = ["//visibility:public"])

licenses(["notice"])
exports_files(["LICENSE"])

load(
    "//gbase:build_config.bzl",
    "tf_proto_library",
    "tf_proto_library_cc",
)

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

tf_proto_library_cc(
    name = "grpc_helloworld_protos",
    srcs = [
        "helloworld.proto",
    ],
    has_services = 1,
    cc_api_version = 2,
    cc_grpc_version = 1,
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "grpc_helloworld_client",
    srcs = ["greeter_client.cc"],
    includes = ["./"],
    copts = COPTS,
    linkopts = LINK_OPTS,
    deps = [
        ":grpc_helloworld_protos_cc",
        "@grpc//:grpc++_unsecure",
    ],
)

cc_binary(
    name = "grpc_helloworld_server",
    srcs = ["greeter_server.cc"],
    includes = ["./"],
    copts = COPTS,
    linkopts = LINK_OPTS,
    deps = [
        ":grpc_helloworld_protos_cc",
        "@grpc//:grpc++_unsecure",
    ],
)
