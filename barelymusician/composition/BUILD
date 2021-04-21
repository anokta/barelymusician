load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "note",
    hdrs = ["note.h"],
)

cc_library(
    name = "note_utils",
    srcs = ["note_utils.cpp"],
    hdrs = ["note_utils.h"],
    deps = ["//barelymusician/common:logging"],
)

cc_test(
    name = "note_utils_test",
    srcs = ["note_utils_test.cpp"],
    deps = [
        ":note_utils",
        "@googletest//:gtest_main",
    ],
)
