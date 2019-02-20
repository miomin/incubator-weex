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

#ifndef FLUTTER_UNICORN_RUNTIME_RUNTIME_OBJECT_H_
#define FLUTTER_UNICORN_RUNTIME_RUNTIME_OBJECT_H_

#include <string>
#include <vector>
#include "third_party/js_api/engine_context.h"
#include "third_party/js_api/runtime_values.h"

namespace unicorn {

class RuntimeObject {
 public:
  explicit RuntimeObject(EngineContext* context, JSContext js_ctx = nullptr)
                        : engine_context_(context) {
    // if js_ctx is null, so we use global context
    if (js_ctx == nullptr && engine_context_)
      context_ = static_cast<JSContext>(engine_context_->GetContext());
    else
      context_ = js_ctx;
  }
  virtual ~RuntimeObject() {}

  virtual void SetEngineContext(EngineContext* context) {
    engine_context_ = context;
    if (context_ == nullptr && context)
      context_ = static_cast<JSContext>(engine_context_->GetContext());
  }
  virtual EngineContext* GetEngineContext() const { return engine_context_; }
  virtual void SetParentJSClass(JSClass parent_class);
  virtual JSClass ParentJSClass() { return parent_class_; }
  virtual void SetJSClass(JSClass class_ref) { class_ = class_ref; }
  virtual JSClass GetJSClass() { return class_; }
  virtual void SetJSObject(JSObject object) { object_ = object;}
  virtual JSObject GetJSObject() { return object_; }
  virtual bool HasProperty(const std::string& name);
  virtual void SetProperty(const std::string& name, JSClass clz,
                           ScopeValues value);
  virtual void SetProperty(const std::string& name, RuntimeObject* value);
  virtual ScopeValues GetPropertyValue(const std::string& name);
  virtual void GetPropertyNameArray(std::vector<std::string>& array);
  virtual bool DeleteProperty(const std::string& name);
  // this function is used after your member functuon called by javascript
  // if you want to do something to the jsobject, please override this
  // funciton
  virtual void OnMethodCalledByJS() {}

 private:
  EngineContext* engine_context_;
  JSClass parent_class_{nullptr};
  JSClass class_{nullptr};
  JSObject object_{nullptr};
  JSContext context_{nullptr};
};

}  // namespace unicorn

#endif  // FLUTTER_UNICORN_RUNTIME_RUNTIME_OBJECT_H_
