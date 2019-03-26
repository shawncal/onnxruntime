// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "gtest/gtest.h"
#include "test/providers/provider_test_utils.h"

namespace onnxruntime {
namespace test {

TEST(ReverseSequenceTest, BatchMajor) {
  OpTester test("ReverseSequence", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> input = {0, 1, 2, 3,
                                4, 5, 6, 7};
  std::vector<int32_t> sequence_lens = {4, 3};
  std::vector<int64_t> expected_output = {3, 2, 1, 0,
                                          6, 5, 4, 7};

  // test.AddAttribute("data_format", "batch_major");
  test.AddAttribute("batch_axis", int64_t(0));
  test.AddAttribute("seq_axis", int64_t(1));

  test.AddInput<int64_t>("input", {2, 4, 1}, input);
  test.AddInput<int32_t>("sequence_lens", {2}, sequence_lens);
  test.AddOutput<int64_t>("Y", {2, 4, 1}, expected_output);
  test.Run();
}

TEST(ReverseSequenceTest, TimeMajor) {
  OpTester test("ReverseSequence", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> input = {0, 4,
                                1, 5,
                                2, 6,
                                3, 7};

  std::vector<int32_t> sequence_lens = {4, 3};
  std::vector<int64_t> expected_output = {3, 6,
                                          2, 5,
                                          1, 4,
                                          0, 7};

  // test.AddAttribute("data_format", "time_major");
  test.AddAttribute("batch_axis", int64_t(1));
  test.AddAttribute("seq_axis", int64_t(0));

  test.AddInput<int64_t>("input", {4, 2, 1}, input);
  test.AddInput<int32_t>("sequence_lens", {2}, sequence_lens);
  test.AddOutput<int64_t>("Y", {4, 2, 1}, expected_output);
  test.Run();
}

TEST(ReverseSequenceTest, Strings) {
  OpTester test("ReverseSequence", 1, onnxruntime::kMSDomain);
  std::vector<std::string> input = {"0", "4 string longer than 16 chars that requires its own buffer",
                                    "1", "5",
                                    "2", "6",
                                    "3", "7"};

  std::vector<int32_t> sequence_lens = {4, 3};
  std::vector<std::string> expected_output = {"3", "6",
                                              "2", "5",
                                              "1", "4 string longer than 16 chars that requires its own buffer",
                                              "0", "7"};

  // test.AddAttribute("data_format", "time_major");
  test.AddAttribute("batch_axis", int64_t(1));
  test.AddAttribute("seq_axis", int64_t(0));

  test.AddInput<std::string>("input", {4, 2, 1}, input);
  test.AddInput<int32_t>("sequence_lens", {2}, sequence_lens);
  test.AddOutput<std::string>("Y", {4, 2, 1}, expected_output);
  test.Run();
}

TEST(ReverseSequenceTest, InvalidInput) {
  {
    int64_t batch_size = 2, seq_size = 4;

    // Bad axis valus
    auto check_bad_axis = [&](int64_t batch_dim, int64_t seq_dim,
                              const std::vector<int64_t>& input_shape,
                              const std::string err_msg) {
      OpTester test("ReverseSequence", 1, onnxruntime::kMSDomain);
      std::vector<int64_t> input(batch_size * seq_size, 0);
      std::vector<int32_t> sequence_lens(batch_size, 1);
      std::vector<int64_t> expected_output = input;

      test.AddAttribute("batch_axis", batch_dim);
      test.AddAttribute("seq_axis", seq_dim);

      test.AddInput<int64_t>("input", input_shape, input);
      test.AddInput<int32_t>("sequence_lens", {batch_size}, sequence_lens);
      test.AddOutput<int64_t>("Y", input_shape, expected_output);
      test.Run(test::OpTester::ExpectResult::kExpectFailure, err_msg);
    };

    check_bad_axis(2, 1, {1, seq_size, batch_size}, "batch_axis must be 0 or 1. Got:2");
    check_bad_axis(0, 2, {batch_size, 1, seq_size}, "seq_axis must be 0 or 1. Got:2");
    check_bad_axis(1, 1, {batch_size, seq_size, 1}, "seq_axis and batch_axis must have different values but both are 1");
  }

  // invalid sequence_lens size
  {
    OpTester test("ReverseSequence", 1, onnxruntime::kMSDomain);

    // Bad data_format value
    std::vector<int64_t> input = {0, 1, 2, 3,
                                  4, 5, 6, 7};
    std::vector<int32_t> sequence_lens = {4, 3, 4};
    std::vector<int64_t> expected_output = {3, 2, 1, 0,
                                            6, 5, 4, 7};

    // test.AddAttribute("data_format", "batch_major");
    test.AddAttribute("batch_axis", int64_t(0));
    test.AddAttribute("seq_axis", int64_t(1));

    test.AddInput<int64_t>("input", {2, 4, 1}, input);
    test.AddInput<int32_t>("sequence_lens", {3}, sequence_lens);
    test.AddOutput<int64_t>("Y", {2, 4, 1}, expected_output);
    test.Run(test::OpTester::ExpectResult::kExpectFailure, "Batch size mismatch for input and sequence_lens. 2 != 3");
  }
}

}  // namespace test
}  // namespace onnxruntime
