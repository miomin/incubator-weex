/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/**
# Copyright 2018 Taobao (China) Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
**/

#ifndef FLUTTER_UNICORN_RUNTIME_TEST_H_
#define FLUTTER_UNICORN_RUNTIME_TEST_H_

#include <vector>
#include "flutter/fml/logging.h"
#include "flutter/unicorn/js_runtime/runtime/engine_context.h"
#include "flutter/unicorn/js_runtime/runtime/runtime_object.h"
#include "flutter/unicorn/js_runtime/runtime/runtime_values.h"
#include "js_js_runtime/runtime/js_runtime/runtime/binding_macro.h"

namespace unicorn {

class Address : public RuntimeObject {
 public:
  DECLARE_CLASS_REGISTER_OP()

  static Address* JSCreate(std::vector<unicorn::ScopeValues> args) {
    return new Address();
  }
  static void JSFinalize(Address* thiz) {
    FML_LOG(ERROR) << "Address::JSFinalize";
  }

  Address();
  Address(const Address&);
  ~Address() {}

  ScopeValues Path(const std::vector<unicorn::ScopeValues>& args) {
      Path();
      return RuntimeValues::MakeUndefined();
  }
  void Path();
};

class Base {
 public:
  DECLARE_CLASS_REGISTER_OP();
  Base() = default;
  ~Base() = default;
  ScopeValues Change(std::vector<unicorn::ScopeValues>& args);
};

class RuntimeTest : public Base, public RuntimeObject {
 public:
  DECLARE_CLASS_REGISTER_OP()

  static RuntimeTest* JSCreate(std::vector<unicorn::ScopeValues>& args) {
    return new RuntimeTest(nullptr);
  }

  static void JSFinalize(RuntimeTest* thiz) {
    FML_LOG(ERROR) << "RuntimeTest::JSFinalize";
  }

  static int staticNumber;

  explicit RuntimeTest(EngineContext* context);
  ~RuntimeTest() = default;

  static void Register(EngineContext*);

  // member get
  ScopeValues GetNumber();
  ScopeValues GetLocation();

  // member set
  void SetNumber(ScopeValues value);
  void SetLocation(unicorn::ScopeValues value);

  // static method
  static ScopeValues Printf(std::vector<unicorn::ScopeValues>& args);
  // method
  ScopeValues AddNum(const std::vector<unicorn::ScopeValues>& args);
  ScopeValues Location(const std::vector<unicorn::ScopeValues>& args);
  void MakeObject(const char* name);

 private:
  void AddNum();
  int num_;
  Address location_;
};

}  // namespace unicorn
#endif  // FLUTTER_UNICORN_RUNTIME_TEST_H_
