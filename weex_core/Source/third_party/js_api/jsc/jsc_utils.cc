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

#include "third_party/js_api/jsc/jsc_utils.h"

#include <string>
#include <vector>

#include "third_party/js_api/jsc/runtime_values_jsc.h"
#include "third_party/js_api/runtime_object.h"
#include "third_party/js_api/runtime_values.h"

namespace unicorn {

static JSClassRef kDefaultClass = nullptr;

JSValueRef Conversion::RuntimeValueToJSValue(JSContextRef ctx,
                                             JSClassRef class_ref,
                                             const RuntimeValues* value) {
  if (value->IsUndefined()) {
    return JSValueMakeUndefined(ctx);
  } else if (value->IsNull()) {
    return JSValueMakeNull(ctx);
  } else if (value->IsBool()) {
    bool b = false;
    value->GetAsBoolean(&b);
    JSValueMakeBoolean(ctx, b);
  } else if (value->IsInt()) {
    int num = 0;
    value->GetAsInteger(&num);
    return JSValueMakeNumber(ctx, static_cast<double>(num));
  } else if (value->IsDouble()) {
    double num = 0.0;
    value->GetAsDouble(&num);
    return JSValueMakeNumber(ctx, num);
  } else if (value->IsString()) {
    std::string tmp;
    value->GetAsString(&tmp);
    JSStringRef str = JSStringCreateWithUTF8CString(tmp.c_str());
    return JSValueMakeString(ctx, str);
  } else if (value->IsObject()) {
    BaseObject* native_ob = value->GetAsObject();
    if (class_ref == nullptr) {
      auto clz = native_ob->GetRuntimeClass();
      if (clz)
        class_ref = clz->GetJSClass();
    }
    void* data = native_ob->GetDataPtr();
    if (data && !class_ref) {
      // if class_ref == null, so the js object can not store data
      // we use the default empty class instead
      if (!kDefaultClass) {
        JSClassDefinition class_definition = kJSClassDefinitionEmpty;
        kDefaultClass = JSClassCreate(&class_definition);
      }
      class_ref = kDefaultClass;
    }
    JSObjectRef js_obj = JSObjectMake(ctx, class_ref, nullptr);
    JSObjectSetPrivate(js_obj, data);
    return js_obj;
  } else if (value->IsMap()) {
    const JSCMap* local = static_cast<const JSCMap*>(value->GetAsMap());
    auto& map = local->GetMap();
    JSObjectRef thiz = local->GetThisObject();
    if (thiz == nullptr) {
      thiz = JSObjectMake(ctx, nullptr, nullptr);
    }
    for (auto& iter : map) {
      JSStringRef str = JSStringCreateWithUTF8CString(iter.first.c_str());
      JSValueRef value_ref = RuntimeValueToJSValue(ctx, nullptr, iter.second);
      JSObjectSetProperty(ctx, thiz, str, value_ref, 0, nullptr);
      JSStringRelease(str);
    }
    return thiz;
  } else if (value->IsFunction()) {
    const JSCFunction* local =
        static_cast<const JSCFunction*>(value->GetAsFunction());
    return local->GetFunction();
  }
// JSValueIsArray is only used in ios 9.0 or above, so we just enable
// array in android now
#if defined(OS_ANDROID)
  else if (value->IsArray()) {
    const JSCArray* local = static_cast<const JSCArray*>(value->GetAsArray());
    JSObjectRef thiz = local->GetThisObject();
    if (thiz == nullptr) {
      thiz = JSObjectMake(ctx, nullptr, nullptr);
    }
    auto& array = local->GetArray();
    size_t length = local->Size();
    JSStringRef len_str = JSStringCreateWithUTF8CString("length");
    JSObjectSetProperty(ctx, thiz, len_str, JSValueMakeNumber(ctx, length),
                        0, nullptr);
    JSStringRelease(len_str);
    for (size_t i = 0; i < length; i++) {
      JSValueRef value_ref = RuntimeValueToJSValue(ctx, nullptr, array[i]);
      JSObjectSetPropertyAtIndex(ctx, thiz, i, value_ref, nullptr);
    }
    return thiz;
  }
#endif

  return JSValueMakeUndefined(ctx);
}

ScopeValues Conversion::JSValueToRuntimeValue(JSContextRef ctx,
                                              JSObjectRef thiz,
                                              JSValueRef value) {
  if (JSValueIsNumber(ctx, value)) {
    double origin = JSValueToNumber(ctx, value, nullptr);
    if (origin == static_cast<int>(origin)) {
      return ScopeValues(new RuntimeValues(static_cast<int>(origin)));
    }
    return ScopeValues(new RuntimeValues(origin));
  } else if (JSValueIsBoolean(ctx, value)) {
    return ScopeValues(new RuntimeValues(JSValueToBoolean(ctx, value)));
  } else if (JSValueIsNull(ctx, value)) {
    return ScopeValues(new RuntimeValues(nullptr));
  } else if (JSValueIsUndefined(ctx, value)) {
    return ScopeValues(new RuntimeValues());
  } else if (JSValueIsString(ctx, value)) {
    JSStringRef str = JSValueToStringCopy(ctx, value, nullptr);
    std::string result;
    size_t max_bytes = JSStringGetMaximumUTF8CStringSize(str);
    result.resize(max_bytes);
    size_t bytes_written = JSStringGetUTF8CString(str, &result[0], max_bytes);
    if (max_bytes == 0)
        return RuntimeValues::MakeString("");
    result.resize(bytes_written - 1);
    JSStringRelease(str);
    return RuntimeValues::MakeString(result);
  } else if (JSValueIsObject(ctx, value)) {
    JSObjectRef ob = JSValueToObject(ctx, value, nullptr);
    auto native_ob = JSObjectGetPrivate(ob);
    if (native_ob) {
      return RuntimeValues::MakeCommonObject(native_ob, nullptr);
    } else if (JSObjectIsFunction(ctx, ob)) {
      auto func = JSCFunction::Create(ctx, "", thiz, ob);
      auto func_holder = RuntimeValues::MakeFunction(std::move(func));
      return func_holder;
    } else {
      std::vector<std::string> properties;
      JSUtils::GetPropertyNameArray(ctx, ob, properties);
      auto jsc_map = JSCMap::Create(ctx, ob);
      for (size_t i = 0; i < properties.size(); i++) {
         JSStringRef str_ref = JSStringCreateWithUTF8CString(
                                                    properties[i].c_str());
         JSValueRef val = JSObjectGetProperty(ctx, ob, str_ref, nullptr);
         auto native = Conversion::JSValueToRuntimeValue(ctx, ob, val);
         jsc_map->Insert(properties[i], native.release());
      }
      auto map_holder = RuntimeValues::MakeMap(std::move(jsc_map));
      return map_holder;
    }
  }
// JSValueIsArray is only used in ios 9.0 or above, so we just enable
// array in android now
#if defined(OS_ANDROID)
  else if (JSValueIsArray(ctx, value)) {
    JSObjectRef ob = JSValueToObject(ctx, value, nullptr);
    JSStringRef len_str = JSStringCreateWithUTF8CString("length");
    int length = static_cast<int>(JSValueToNumber(ctx,
                 JSObjectGetProperty(ctx, ob, len_str, nullptr), nullptr));
    JSStringRelease(len_str);
    auto holder = JSCArray::Create(ctx, ob);
    for (int i = 0; i < length; i++) {
      JSValueRef temp = JSObjectGetPropertyAtIndex(ctx, ob, i, nullptr);
      auto native = JSValueToRuntimeValue(ctx, nullptr, temp);
      holder->PushBack(native.release());
    }
    auto array_holder = RuntimeValues::MakeArray(std::move(holder));
    return array_holder;
  }
#endif

  return ScopeValues(new RuntimeValues());
}

void Conversion::JSValuesArrayToRuntimeValues(JSContextRef ctx,
                                       JSObjectRef thiz,
                                       size_t argc, const JSValueRef* argv,
                                       std::vector<ScopeValues>& output) {
  if (argc == 0 || argv == nullptr)
    return;
  for (size_t i = 0; i < argc; i++) {
    output.push_back(JSValueToRuntimeValue(ctx, thiz, argv[i]));
  }
}

/////////////////////////////////////////////////////////////////////////////
/*JSUtils implemention*/
bool JSUtils::HasProperty(JSContextRef ctx, JSObjectRef ob,
                          const std::string& name) {
  if (!ctx || !ob)
    return false;

  JSContextRef context = static_cast<JSContextRef>(ctx);
  JSObjectRef object = static_cast<JSObjectRef>(ob);
  JSStringRef str = JSStringCreateWithUTF8CString(name.c_str());
  bool ret = JSObjectHasProperty(context, object, str);
  JSStringRelease(str);
  return ret;
}

void JSUtils::SetProperty(JSContextRef ctx, JSObjectRef ob,
                          JSClassRef clz, const std::string& name,
                          RuntimeValues* value) {
  if (!ctx || !ob)
    return;

  JSContextRef context = static_cast<JSContextRef>(ctx);
  JSObjectRef object = static_cast<JSObjectRef>(ob);
  JSClassRef clasz = static_cast<JSClassRef>(clz);
  JSStringRef str = JSStringCreateWithUTF8CString(name.c_str());
  JSValueRef ret = Conversion::RuntimeValueToJSValue(context, clasz, value);
  JSObjectSetProperty(context, object, str, ret, 0, nullptr);
  JSStringRelease(str);
}

bool JSUtils::DeleteProperty(JSContextRef ctx, JSObjectRef ob,
                             const std::string& name) {
  if (!ctx || !ob)
    return false;

  JSContextRef context = static_cast<JSContextRef>(ctx);
  JSObjectRef object = static_cast<JSObjectRef>(ob);
  JSStringRef str = JSStringCreateWithUTF8CString(name.c_str());
  bool ret = JSObjectDeleteProperty(context, object, str, nullptr);
  JSStringRelease(str);
  return ret;
}

ScopeValues JSUtils::GetProperty(JSContextRef ctx, JSObjectRef ob,
                                 const std::string& name) {
  if (!ctx || !ob)
    return ScopeValues(new RuntimeValues());

  JSContextRef context = static_cast<JSContextRef>(ctx);
  JSObjectRef object = static_cast<JSObjectRef>(ob);
  JSStringRef str = JSStringCreateWithUTF8CString(name.c_str());
  JSValueRef ret = JSObjectGetProperty(context, object, str, nullptr);
  JSStringRelease(str);
  return Conversion::JSValueToRuntimeValue(context, object, ret);
}

void JSUtils::GetPropertyNameArray(JSContextRef ctx, JSObjectRef ob,
                                   std::vector<std::string>& native_array) {
  if (!ctx || !ob)
    return;

  JSContextRef context = static_cast<JSContextRef>(ctx);
  JSObjectRef object = static_cast<JSObjectRef>(ob);
  JSPropertyNameArrayRef array_ref = JSObjectCopyPropertyNames(context,
                                                               object);
  size_t count = JSPropertyNameArrayGetCount(array_ref);
  for (size_t i = 0; i < count; i++) {
    JSStringRef str_ref = JSPropertyNameArrayGetNameAtIndex(array_ref, i);
    size_t max_len = JSStringGetMaximumUTF8CStringSize(str_ref);
    std::string holder;
    holder.resize(max_len);
    char* ptr = &holder[0];
    size_t bytes_written = JSStringGetUTF8CString(str_ref, ptr, max_len);
    holder.resize(bytes_written - 1);
    native_array.push_back(holder);
  }
  JSPropertyNameArrayRelease(array_ref);
}

ScopeValues JSUtils::CallAsFunction(JSContextRef ctx, JSObjectRef thiz,
                                    JSObjectRef function,
                                    size_t argc, const JSValueRef argv[]) {
  if (!ctx || !function)
    return ScopeValues(new RuntimeValues());

  JSContextRef context = static_cast<JSContextRef>(ctx);
  JSObjectRef thiz_ob = static_cast<JSObjectRef>(thiz);
  JSObjectRef function_ob = static_cast<JSObjectRef>(function);
  JSValueRef ret = JSObjectCallAsFunction(context, function_ob, thiz_ob,
                                          argc, argv, nullptr);
  return Conversion::JSValueToRuntimeValue(context, thiz_ob, ret);
}
}  // namespace unicorn
