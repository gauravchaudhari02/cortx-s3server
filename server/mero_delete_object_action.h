/*
 * Copyright (c) 2020 Seagate Technology LLC and/or its Affiliates
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For any questions about this software or licensing,
 * please email opensource@seagate.com or cortx-questions@seagate.com.
 *
 */

#pragma once

#ifndef __MERO_DELETE_OBJECT_ACTION_H__
#define __MERO_DELETE_OBJECT_ACTION_H__

#include <gtest/gtest_prod.h>
#include <memory>

#include "s3_factory.h"
#include "mero_action_base.h"

class MeroDeleteObjectAction : public MeroAction {
  int layout_id;
  m0_uint128 oid;
  std::shared_ptr<S3ClovisWriter> clovis_writer;

  std::shared_ptr<S3ClovisWriterFactory> clovis_writer_factory;

 public:
  MeroDeleteObjectAction(std::shared_ptr<MeroRequestObject> req,
                         std::shared_ptr<S3ClovisWriterFactory> writer_factory =
                             nullptr);

  void setup_steps();
  void validate_request();

  void delete_object();
  void delete_object_successful();
  void delete_object_failed();

  void send_response_to_s3_client();
};
#endif