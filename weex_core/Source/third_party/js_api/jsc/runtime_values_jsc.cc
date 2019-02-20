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

#include "third_party/js_api/jsc/runtime_values_jsc.h"

#include "third_party/js_api/engine_context.h"
#include "third_party/js_api/jsc/jsc_utils.h"
#include "third_party/js_api/runtime_object.h"
#include "third_party/js_api/runtime_values.h"
#include "third_party/js_api/runtime_vm.h"

namespace unicorn {

/////////////////////////////////////////////////////////////////////////////
/* class JSCFunction implemention */
std::unique_ptr<Function>
JSCFunction::CreateWithCallback(JSContextRef ctx, std::string name,
                                JSObjectRef thiz,
                                JSObjectCallAsFunctionCallback callback) {
  Function* ptr = static_cast<Function*>(new JSCFunction(ctx, name,
                                             thiz, callback));
  return std::unique_ptr<Function>(ptr);
}

std::unique_ptr<Function>
JSCFunction::Create(JSContextRef ctx, std::string name,
                    JSObjectRef thiz, JSObjectRef function) {
  Function* ptr = static_cast<Function*>(new JSCFunction(ctx, name,
                                             thiz, function));
  return std::unique_ptr<Function>(ptr);
}

JSCFunction::JSCFunction(JSContextRef ctx, std::string name, JSObjectRef thiz,
                        JSObjectCallAsFunctionCallback callback)
                        : Function(),
                          context_(ctx),
                          function_name_(name),
                          thiz_(thiz),
                          is_from_native_(true) {
  if (thiz_ == nullptr) {
    thiz_ = JSContextGetGlobalObject(ctx);
  }

  JSStringRef str = JSStringCreateWithUTF8CString(function_name_.c_str());
  function_ = JSObjectMakeFunctionWithCallback(context_, str, callback);
  JSObjectSetProperty(context_, thiz_, str, function_, 0, nullptr);
  JSStringRelease(str);

  JSGlobalContextRef global_ctx = JSContextGetGlobalContext(ctx);
  EngineContext* context =
        RuntimeVM::GetEngineContext(static_cast<const void*>(global_ctx));
  RuntimeObject* native_thiz = new RuntimeObject(context);
  native_thiz->SetJSObject(function_);
  Function::SetObject(native_thiz);

  MemberProtect();
}

JSCFunction::JSCFunction(JSContextRef ctx, std::string name, JSObjectRef thiz,
                        JSObjectRef function)
                        : context_(ctx),
                          function_name_(name),
                          thiz_(thiz),
                          function_(function),
                          is_from_native_(false) {
  JSGlobalContextRef global_ctx = JSContextGetGlobalContext(ctx);
  EngineContext* context =
        RuntimeVM::GetEngineContext(static_cast<const void*>(global_ctx));
  RuntimeObject* native_thiz = new RuntimeObject(context);
  native_thiz->SetJSObject(function_);
  Function::SetObject(native_thiz);

  MemberProtect();
}

JSCFunction::~JSCFunction() {
  MemberUnprotect();
}

void JSCFunction::MemberProtect() {
  JSValueProtect(context_, thiz_);
  JSValueProtect(context_, function_);
}

void JSCFunction::MemberUnprotect() {
  JSValueUnprotect(context_, thiz_);
  JSValueUnprotect(context_, function_);
}

std::unique_ptr<RuntimeValues> JSCFunction::Call(size_t argc,
                                    std::vector<ScopeValues>& argv) const {
  JSValueRef argv_js[argc];
  for (size_t i = 0; i < argc; i++) {
    argv_js[i] = Conversion::RuntimeValueToJSValue(context_, nullptr,
                                                   argv[i].get());
  }

  return JSUtils::CallAsFunction(static_cast<JSContextRef>(context_),
                                 static_cast<JSObjectRef>(thiz_),
                                 static_cast<JSObjectRef>(function_),
                                 argc, argv_js);
}

/////////////////////////////////////////////////////////////////////////////
/* class JSCMap implemention */
std::unique_ptr<Map> JSCMap::Create(JSContextRef ctx, JSObjectRef thiz) {
  Map* ptr = static_cast<Map*>(new JSCMap(ctx, thiz));
  return std::unique_ptr<Map>(ptr);
}

// Map create from native
std::unique_ptr<Map> Map::CreateFromNative(EngineContext* context,
                                           ScopeValues thiz) {
  JSContextRef ctx = static_cast<JSContextRef>(context->GetContext());
  JSObjectRef thiz_object = nullptr;
  if (thiz->IsObject()) {
    auto object = thiz->GetAsObject();
    RuntimeObject* native_thiz =
                   static_cast<RuntimeObject*>(object->GetDataPtr());
    thiz_object = native_thiz->GetJSObject();
  }
  return JSCMap::Create(ctx, thiz_object);
}

JSCMap::JSCMap(JSContextRef ctx, JSObjectRef thiz)
        : Map(),
          context_(ctx),
          thiz_(thiz) {
  JSValueProtect(context_, thiz_);
}

JSCMap::~JSCMap() {
  JSValueUnprotect(context_, thiz_);
}

/////////////////////////////////////////////////////////////////////////////
/* class JSCArray implemention */
std::unique_ptr<Array> JSCArray::Create(JSContextRef ctx, JSObjectRef thiz) {
  Array* ptr = static_cast<Array*>(new JSCArray(ctx, thiz));
  return std::unique_ptr<Array>(ptr);
}

JSCArray::JSCArray(JSContextRef ctx, JSObjectRef thiz)
          : Array(),
            thiz_(thiz) {
  JSValueProtect(context_, thiz_);
}

JSCArray::~JSCArray() {
  JSValueUnprotect(context_, thiz_);
}

}  // namespace unicorn
