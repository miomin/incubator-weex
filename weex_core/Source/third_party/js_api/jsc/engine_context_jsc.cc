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

#include "third_party/js_api/jsc/engine_context_jsc.h"

#include <memory>
#include <utility>
#include "third_party/js_api/jsc/jsc_utils.h"
#include "third_party/js_api/jsc/binding_macro_jsc.h"
#include "third_party/js_api/runtime_values.h"
#include "third_party/js_api/runtime_vm.h"

namespace unicorn {

EngineContextJSC::EngineContextJSC(EngineContextDelegate* delegate,
                                   RuntimeVM* vm)
                : delegate_(delegate), vm_(vm) {
  InitializeContext();
}

EngineContextJSC::~EngineContextJSC() {
  JSGlobalContextRelease(context_);

  if (delegate_) delegate_->OnEngineContextFinalized();
  delegate_ = NULL;
}

void EngineContextJSC::InitializeContext() {
  JSContextGroupRef context_group =
      static_cast<JSContextGroupRef>(vm_->EngineVM());
  context_ = JSGlobalContextCreateInGroup(context_group, NULL);

  if (delegate_) delegate_->OnEngineContextInitialized();
}

void EngineContextJSC::RunJavaScript(const std::string& script) {
  JSStringRef source = JSStringCreateWithUTF8CString(script.c_str());
  JSValueRef exception = nullptr;

  // JSValueRef result =
  JSEvaluateScript(context_, source, NULL, NULL, 0, &exception);

  // release jsc string
  JSStringRelease(source);

  if (exception) {
    // to be done
  }
}

void EngineContextJSC::ThrowJSError(const std::string& error) {}

void EngineContextJSC::ThrowException(const std::string& error) {}

void EngineContextJSC::SetGlobalPropertyValue(const std::string& property_id,
                                              ScopeValues value) {
  JSObjectRef global = JSContextGetGlobalObject(context_);
  JSValueRef exc = nullptr;
  JSStringRef str_ref = JSStringCreateWithUTF8CString(property_id.c_str());
  JSObjectSetProperty(
      context_,
      global,
      str_ref,
      Conversion::RuntimeValueToJSValue(context_, nullptr, value.get()),
      kJSPropertyAttributeNone,
      &exc);
  JSStringRelease(str_ref);
}

ScopeValues EngineContextJSC::GetGlobalProperty(std::string property_id) {
  JSObjectRef global = JSContextGetGlobalObject(context_);
  JSValueRef exc = nullptr;
  JSStringRef str = JSStringCreateWithUTF8CString(property_id.c_str());
  JSValueRef result = JSObjectGetProperty(context_, global, str, &exc);
  if (exc) {
  }

  JSStringRelease(str);
  return Conversion::JSValueToRuntimeValue(context_, global, result);
}

EngineContext* EngineContext::CreateEngineContext(
    EngineContextDelegate* delegate, RuntimeVM* vm) {
  return new EngineContextJSC(delegate, vm);
}
}  // namespace unicorn
