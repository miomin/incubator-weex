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

#ifndef FLUTTER_UNICORN_RUNTIME_JSC_ENGINE_CONTEXT_JSC_H_
#define FLUTTER_UNICORN_RUNTIME_JSC_ENGINE_CONTEXT_JSC_H_

#include <JavaScriptCore/JavaScript.h>
#include <memory>
#include <string>
#include "third_party/js_api/engine_context.h"
#include "third_party/js_api/runtime_values.h"

namespace unicorn {

class RuntimeVM;

class EngineContextJSC : public EngineContext {
 public:
  EngineContextJSC(EngineContextDelegate* delegate, RuntimeVM* vm);
  ~EngineContextJSC();

  void InitializeContext() override;
  void RunJavaScript(const std::string& script) override;
  void ThrowJSError(const std::string& error) override;
  void ThrowException(const std::string& error) override;
  void* GetContext() const override { return static_cast<void*>(context_); }
  void SetGlobalPropertyValue(const std::string& property_id,
                              ScopeValues value) override;
  ScopeValues GetGlobalProperty(std::string property_id) override;

 private:
  EngineContextDelegate* delegate_;
  RuntimeVM* vm_;

  JSGlobalContextRef context_;
};

}  // namespace unicorn

#endif  // FLUTTER_UNICORN_RUNTIME_JSC_ENGINE_CONTEXT_JSC_H_
