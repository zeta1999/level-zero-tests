/*
 *
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gtest/gtest.h"

#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "logging/logging.hpp"

namespace lzt = level_zero_tests;

#include <level_zero/ze_api.h>

namespace {

class CooperativeKernelTests : public ::testing::Test,
                               public ::testing::WithParamInterface<int> {};

TEST_P(
    CooperativeKernelTests,
    GivenCooperativeKernelWhenAppendingLaunchCooperativeKernelThenSuccessIsReturnedAndOutputIsCorrect) {
  uint32_t max_group_count = 0;
  ze_module_handle_t module = nullptr;
  ze_kernel_handle_t kernel = nullptr;
  auto device = lzt::zeDevice::get_instance()->get_device();
  auto flags = ZE_COMMAND_QUEUE_FLAG_SUPPORTS_COOPERATIVE_KERNELS;
  auto mode = ZE_COMMAND_QUEUE_MODE_DEFAULT;
  auto priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL;
  auto ordinal = 0;
  auto command_queue =
      lzt::create_command_queue(device, flags, mode, priority, ordinal);
  auto command_list = lzt::create_command_list();

  // Set up input vector data
  const size_t data_size = 4096;
  uint64_t kernel_data[data_size] = {0};
  auto input_data = lzt::allocate_shared_memory(sizeof(uint64_t) * data_size);
  memcpy(input_data, kernel_data, data_size * sizeof(uint64_t));

  auto row_num = GetParam();
  uint32_t groups_x = 1;

  module = lzt::create_module(device, "cooperative_kernel.spv");
  kernel = lzt::create_function(module, "module_cooperative_pascal");

  // Use a small group size in order to use more groups
  // in order to stress cooperation
  zeKernelSetGroupSize(kernel, 1, 1, 1);
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeKernelSuggestMaxCooperativeGroupCount(kernel, &groups_x));
  ASSERT_GT(groups_x, 0);
  // We've set the number of workgroups to the max
  // so check that it is sufficient for the input,
  // otherwise just clamp the test
  if (groups_x < row_num) {
    row_num = groups_x;
  }
  ASSERT_EQ(ZE_RESULT_SUCCESS, zeKernelSetArgumentValue(
                                   kernel, 0, sizeof(input_data), &input_data));
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeKernelSetArgumentValue(kernel, 1, sizeof(row_num), &row_num));

  ze_group_count_t args = {groups_x, 1, 1};
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendLaunchCooperativeKernel(
                command_list, kernel, &args, nullptr, 0, nullptr));

  lzt::close_command_list(command_list);
  lzt::execute_command_lists(command_queue, 1, &command_list, nullptr);
  lzt::synchronize(command_queue, UINT32_MAX);

  // Validate the kernel completed successfully and correctly
  uint64_t prev_val = 1;
  uint64_t val = 1;
  for (int i = 0; i <= row_num / 2; i++) {
    val = i ? prev_val * ((row_num + 1 - i) / i) : 1;
    EXPECT_EQ(static_cast<uint64_t *>(input_data)[i], val);
    EXPECT_EQ(static_cast<uint64_t *>(input_data)[row_num - i], val);
    prev_val = val;
  }

  lzt::free_memory(input_data);
  lzt::destroy_command_list(command_list);
  lzt::destroy_command_queue(command_queue);
}

INSTANTIATE_TEST_CASE_P(GroupNumbers, CooperativeKernelTests,
                        ::testing::Values(0, 1, 5, 10, 100, 500));

} // namespace