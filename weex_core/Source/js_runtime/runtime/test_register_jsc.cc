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

#include <API/JSObjectRef.h>
#include <JavaScriptCore/JavaScript.h>

#include "flutter/unicorn/js_runtime/runtime/runtime_context.h"
#include "flutter/unicorn/js_runtime/runtime/test.h"

JSValueRef AddNum(JSContextRef ctx, JSObjectRef function,
                  JSObjectRef thisObject, size_t argumentCount,
                  const JSValueRef arguments[], JSValueRef* exception) {
  unicorn::Test* t =
      static_cast<unicorn::Test*>(JSObjectGetPrivate(thisObject));
  return JSValueMakeUndefined(ctx);
}

JSValueRef GetNum(JSContextRef ctx, JSObjectRef function,
                  JSObjectRef thisObject, size_t argumentCount,
                  const JSValueRef arguments[], JSValueRef* exception) {
  unicorn::Test* t =
      static_cast<unicorn::Test*>(JSObjectGetPrivate(thisObject));
  return JSValueMakeNumber(ctx, t->GetNum());
}

JSClassRef CreateTest() {
  static JSStaticValue TestMembers[] = {
      {"number", GetNum, 0, kJSPropertyAttributeNone}, {0, 0, 0, 0}};

  static JSStaticFunction TestFunctions[] =
  { {"addNum", AddNum, kJSPropertyAttributeNone},
    {0, 0, 0},
  }

  static JSClassDefinition TestDefinition =
  { 0,
    kJSClassAttributeNone,
    "test",
    0,
    testValues,
    testFunctions,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0 }

  static JSClassRef t = JSClassCreate(&TestDefinition);
  return t;
}

void RegisterObject(Test test, RuntimeContext* runtime_context) {
  if (!runtime_context) return;
  JSContextRef* ctx =
      static_cast<JSContextRef*>(runtime_context->GetCurrentPlatformContext());
  JSObjectRef classObj = JSObjectMake(*ctx, testClass(), &test);
  JSObjectRef globalObj = JSContextGetGlobalObject(*ctx);
  JSStringRef objName = JSStringCreateWithUTF8CString("test");
  JSObjectSetProperty(ctx, globalObj, objName, classObj,
                      kJSPropertyAttributeNone, NULL);
}
