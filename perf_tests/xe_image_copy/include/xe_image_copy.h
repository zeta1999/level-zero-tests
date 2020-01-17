/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2019-2020 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to the
 * source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 */

#ifndef XE_IMAGE_COPY_H
#define XE_IMAGE_COPY_H

#include "common.hpp"
#include "ze_api.h"
#include "ze_cmdqueue.h"
#include "xe_app.hpp"

#include <assert.h>
#include <iomanip>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include "utils/utils.hpp"

namespace po = boost::program_options;
namespace pt = boost::property_tree;
using namespace std;
using namespace pt;


class XeImageCopy {
public:
  uint32_t width = 2048;
  uint32_t height = 2048;
  uint32_t depth  = 1;
  uint32_t xOffset = 0;
  uint32_t yOffset = 0;
  uint32_t zOffset = 0;
  uint32_t number_iterations = 50;
  uint32_t warm_up_iterations = 10;
  uint32_t data_validation = 0;
  bool validRet = false;
  long double gbps;
  long double latency;
  ptree param_array;
  ze_image_format_layout_t Imagelayout = ZE_IMAGE_FORMAT_LAYOUT_32;
  ze_image_flag_t Imageflags = ZE_IMAGE_FLAG_PROGRAM_READ;
  ze_image_type_t Imagetype = ZE_IMAGE_TYPE_2D;
  ze_image_format_type_t  Imageformat = ZE_IMAGE_FORMAT_TYPE_FLOAT;
  string JsonFileName;
  XeImageCopy();
  ~XeImageCopy();
  void measureHost2Device2Host();
  void measureHost2Device();
  void measureDevice2Host();
  int parse_command_line(int argc, char **argv);
  bool is_json_output_enabled();

private:
  void initialize_buffer(void);
  void test_initialize(void);
  void test_cleanup(void);
  void validate_data_buffer(void);

  XeApp *benchmark;
  ze_command_queue_handle_t command_queue;
  ze_command_list_handle_t command_list;
  ze_command_list_handle_t command_list_a;
  ze_command_list_handle_t command_list_b;
  ze_image_handle_t image;

  ze_image_region_t region;
  ze_image_format_desc_t formatDesc;
  ze_image_desc_t imageDesc;
  uint8_t *srcBuffer;
  uint8_t *dstBuffer;
  size_t buffer_size;
};

class XeImageCopyLatency : public XeImageCopy {
public:
  XeImageCopyLatency();
};

#endif  /* XE_IMAGE_COPY_H */
