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

#include "third_party/js_api/runtime_context_android.h"

#include <cstdlib>

namespace unicorn {

RuntimeContextAndroid::RuntimeContextAndroid(RuntimeVM* vm)
    : initialized_(false), vm_(vm), script_url_("") {
}

RuntimeContextAndroid::~RuntimeContextAndroid() {
  if (engine_context_) engine_context_.reset();
}

void RuntimeContextAndroid::CreateJavaScriptContext() {
  engine_context_.reset(EngineContext::CreateEngineContext(this, vm_));
  RuntimeVM::NotifyContextCreated(this);
}

void RuntimeContextAndroid::ReleaseJavaScriptContext() {
  RuntimeVM::NotifyContextDestorying(this);
  engine_context_.reset();
}

void RuntimeContextAndroid::SetName(const std::string& name) {}

void RuntimeContextAndroid::InitializeContext() {}

void RuntimeContextAndroid::UpdateSetting(const std::string& name) {}

bool RuntimeContextAndroid::ExecuteJavaScript(const std::string& content) {
  engine_context_->RunJavaScript(content);
  return true;
}

std::unique_ptr<RuntimeValues>
RuntimeContextAndroid::ExecuteJavaScriptWithResult(const std::string& content) {
  // TODO(unicorn): execute javascript then return result
  return std::unique_ptr<RuntimeValues>();
}

void RuntimeContextAndroid::OnEngineContextInitialized() {
  if (!initialized_) initialized_ = true;
}

void RuntimeContextAndroid::OnEngineContextFinalized() { initialized_ = false; }

void RuntimeContextAndroid::NotifyIdle() {
  // TODO(unicorn): we can do gc worker when idle
}

bool RuntimeContextAndroid::Available() { return initialized_; }

void RuntimeContextAndroid::Destroy() {}

std::unique_ptr<RuntimeContext> RuntimeContext::Create(RuntimeVM* vm) {
  return std::unique_ptr<RuntimeContext>(
      static_cast<RuntimeContext*>(new RuntimeContextAndroid(vm)));
}

}  // namespace unicorn
