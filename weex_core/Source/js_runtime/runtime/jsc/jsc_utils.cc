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

#include "js_runtime/runtime/jsc/jsc_utils.h"

#include <string>
#include <vector>
#include <include/wtf/text/WTFString.h>
#include <include/JavaScriptCore/API/APICast.h>
#include <include/JavaScriptCore/runtime/LiteralParser.h>
#include <include/JavaScriptCore/runtime/JSONObject.h>

#include "js_runtime/runtime/jsc/runtime_values_jsc.h"
#include "js_runtime/runtime/runtime_object.h"
#include "js_runtime/runtime/runtime_values.h"
#include "js_runtime/utils/log_utils.h"
#include "js_runtime/runtime/js_runtime_conversion.h"

namespace unicorn {

    static JSClassRef kDefaultClass = nullptr;

    JSValueRef JSRuntimeConversion::RuntimeValueToJSRuntimeValue(EngineContext *ctx,
                                                                        JSClassRef class_ref,
                                                                        const RuntimeValues *value) {
        return Conversion::RuntimeValueToJSValue(static_cast<JSGlobalContextRef>(ctx->GetContext()), class_ref, value);
    }

    ScopeValues JSRuntimeConversion::JSRunTimeValueToRuntimeValue(EngineContext *ctx,
                                                                         JSObjectRef thiz,
                                                                         JSValueRef value) {
        return Conversion::JSValueToRuntimeValue(static_cast<JSGlobalContextRef>(ctx->GetContext()), thiz, value);
    }


    JSValueRef Conversion::RuntimeValueToJSValue(JSContextRef ctx,
                                                 JSClassRef class_ref,
                                                 const RuntimeValues *value) {
        if (value->IsUndefined()) {
           // LOG_TEST("RuntimeValueToJSValue -> undefined");
            return JSValueMakeUndefined(ctx);
        } else if (value->IsNull()) {
         //   LOG_TEST("RuntimeValueToJSValue -> null");
            return JSValueMakeNull(ctx);
        } else if (value->IsBool()) {
            bool b = false;
            value->GetAsBoolean(&b);
            LOG_TEST("[Context] RuntimeValueToJSValue -> value %p on ctx:%p, bool :%d",value,ctx,b);
            JSValueMakeBoolean(ctx, b);
        } else if (value->IsInt()) {
            int num = 0;
            value->GetAsInteger(&num);
          //  LOG_TEST("RuntimeValueToJSValue -> int :%d",num);
            return JSValueMakeNumber(ctx, static_cast<double>(num));
        } else if (value->IsDouble()) {
            double num = 0.0;
            value->GetAsDouble(&num);
           // LOG_TEST("RuntimeValueToJSValue -> double:%d",num);
            return JSValueMakeNumber(ctx, num);
        } else if (value->IsString()) {
            std::string tmp;
            value->GetAsString(&tmp);
          //  LOG_TEST("RuntimeValueToJSValue -> string:%s",tmp.c_str());
            JSStringRef str = JSStringCreateWithUTF8CString(tmp.c_str());
            return JSValueMakeString(ctx, str);
        } else if (value->IsJsonObject()) {
            const char *json_str = nullptr;
            value->GetAsUtf8JsonStr(&json_str);
          //  LOG_TEST("RuntimeValueToJSValue -> json:%s",json_str);
            return Conversion::ParserUtf8CharJsonToJValueJSContextRef(ctx, json_str);
        } else if (value->IsObject()) {
          //  LOG_TEST("RuntimeValueToJSValue -> obj");
            BaseObject *native_ob = value->GetAsObject();
            if (class_ref == nullptr) {
                auto clz = native_ob->GetRuntimeClass();
                if (clz)
                    class_ref = clz->GetJSClass();
            }
            void *data = native_ob->GetDataPtr();
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
          //  LOG_TEST("RuntimeValueToJSValue -> map");
            const JSCMap *local = static_cast<const JSCMap *>(value->GetAsMap());
            auto &map = local->GetMap();
            JSObjectRef thiz = local->GetThisObject();
            if (thiz == nullptr) {
                thiz = JSObjectMake(ctx, nullptr, nullptr);
            }
            for (auto &iter : map) {
                JSStringRef str = JSStringCreateWithUTF8CString(iter.first.c_str());
                JSValueRef value_ref = RuntimeValueToJSValue(ctx, nullptr, iter.second);
                JSObjectSetProperty(ctx, thiz, str, value_ref, 0, nullptr);
                JSStringRelease(str);
            }
            return thiz;
        } else if (value->IsFunction()) {
           // LOG_TEST("RuntimeValueToJSValue -> func");
            const JSCFunction *local =
                    static_cast<const JSCFunction *>(value->GetAsFunction());
            return local->GetFunction();
        }
// JSValueIsArray is only used in ios 9.0 or above, so we just enable
// array in android now
#if defined(OS_ANDROID)
        else if (value->IsArray()) {
            const JSCArray *local = static_cast<const JSCArray *>(value->GetAsArray());
            JSObjectRef thiz = local->GetThisObject();
            if (thiz == nullptr) {
                thiz = JSObjectMake(ctx, nullptr, nullptr);
            }
            auto &array = local->GetArray();
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
          //  LOG_TEST("[Conversion] JSValueToRuntimeValue JSValueIsNumber ");
            double origin = JSValueToNumber(ctx, value, nullptr);
            if (origin == static_cast<int>(origin)) {
                return ScopeValues(new RuntimeValues(static_cast<int>(origin)));
            }
            return ScopeValues(new RuntimeValues(origin));
        } else if (JSValueIsBoolean(ctx, value)) {
            //LOG_TEST("[Conversion] JSValueToRuntimeValue JSValueIsBoolean ");
            return ScopeValues(new RuntimeValues(JSValueToBoolean(ctx, value)));
        } else if (JSValueIsNull(ctx, value)) {
           // LOG_TEST("[Conversion] JSValueToRuntimeValue JSValueIsNull ");
            return ScopeValues(new RuntimeValues(nullptr));
        } else if (JSValueIsUndefined(ctx, value)) {
           // LOG_TEST("[Conversion] JSValueToRuntimeValue JSValueIsUndefined ");
            return ScopeValues(new RuntimeValues());
        } else if (JSValueIsString(ctx, value)) {
          //  LOG_TEST("[Conversion] JSValueToRuntimeValue JSValueIsString ");
            std::string result;
            if (Conversion::JSValueToStdString(ctx, value, &result)) {
                return RuntimeValues::MakeString(result);
            } else {
                return RuntimeValues::MakeString("");
            }
        }// JSValueIsArray is only used in ios 9.0 or above, so we just enable
// array in android now
//#if defined(OS_ANDROID)
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
//#endif
        else if (JSValueIsObject(ctx, value)) {
            JSObjectRef ob = JSValueToObject(ctx, value, nullptr);
            auto native_ob = JSObjectGetPrivate(ob);
            if (native_ob) {
                return RuntimeValues::MakeCommonObject(native_ob, nullptr);
            } else if (JSObjectIsFunction(ctx, ob)) {
                LOG_TEST("[Context][Conversion] JSValueToRuntimeValue JSObjectIsFunction context:%p ,func:%p",ctx,ob);
                auto func = JSCFunction::Create(ctx, "", thiz, ob);
                auto func_holder = RuntimeValues::MakeFunction(std::move(func));
                return func_holder;
            } else {
                std::vector<std::string> properties;
                JSUtils::GetPropertyNameArray(ctx, ob, properties);
                //LOG_TEST("[Conversion]  JSCMap::Create");
                auto jsc_map = JSCMap::Create(ctx, ob);
               // LOG_TEST("[Conversion] JSValueToRuntimeValue else properties.size():%lu", properties.size());
                for (size_t i = 0; i < properties.size(); i++) {
                  //  LOG_TEST("[Conversion] JSValueToRuntimeValue JSStringCreateWithUTF8CString name :%s",properties[i].c_str());
                    JSStringRef str_ref = JSStringCreateWithUTF8CString(
                            properties[i].c_str());
                    JSValueRef val = JSObjectGetProperty(ctx, ob, str_ref, nullptr);
                  //  LOG_TEST("[Conversion]  Conversion::JSValueToRuntimeValue(ctx, ob, val)");
                    auto native = Conversion::JSValueToRuntimeValue(ctx, ob, val);
                    jsc_map->Insert(properties[i], native.release());
                }
               // LOG_TEST("[Conversion] map_holder = RuntimeValues::MakeMap(std::move(jsc_map)");
                auto map_holder = RuntimeValues::MakeMap(std::move(jsc_map));
              //  LOG_TEST("[Conversion] return map_holder");
                return map_holder;
            }
        }
        return ScopeValues(new RuntimeValues());
    }

    void Conversion::JSValuesArrayToRuntimeValues(JSContextRef ctx,
                                                  JSObjectRef thiz,
                                                  size_t argc, const JSValueRef *argv,
                                                  std::vector<ScopeValues> &output) {
        if (argc == 0 || argv == nullptr)
            return;
        for (size_t i = 0; i < argc; i++) {
            output.push_back(JSValueToRuntimeValue(ctx, thiz, argv[i]));
        }
    }

    bool Conversion::JSValueToStdString(JSContextRef ctx, JSValueRef value, std::string *result) {
        if (!JSValueIsString(ctx, value)) {
            return false;
        }
        JSStringRef str = JSValueToStringCopy(ctx, value, nullptr);
        size_t max_bytes = JSStringGetMaximumUTF8CStringSize(str);
        result->resize(max_bytes);
        size_t bytes_written = JSStringGetUTF8CString(str, &(*result)[0], max_bytes);
        if (max_bytes == 0) {
            return false;
        }
        result->resize(bytes_written - 1);
//        if (nullptr != result){
//            LOG_TEST("Conversion::JSValueToStdString result: %s",result->c_str());
//        } else{
//            LOG_TEST("Conversion::JSValueToStdString result is null ptr");
//        }

        JSStringRelease(str);
        return true;
    }

    JSValueRef Conversion::ParserUtf8CharJsonToJValueJSContextRef(JSContextRef ctx, const char *utf_8_str) {


        //LOG_TEST("Conversion::ParserUtf8CharJson : length:%d, str: %s", std::strlen(utf_8_str), utf_8_str);
        JSStringRef jsonStrRef = JSStringCreateWithUTF8CString(utf_8_str);
        JSValueRef jValue = JSValueMakeFromJSONString(ctx, jsonStrRef);
        if (jsonStrRef) {
            //   LOG_TEST("Conversion::ParserUtf8CharJson : release str");
             JSStringRelease(jsonStrRef);
        }
        return jValue;
    }

    void Conversion::printJSValueRefException(JSContextRef context, JSValueRef exc) {
        if (nullptr == exc || JSValueIsNull(context, exc)) {
            return;
        }
        std::string result;
        JSStringRef str = JSValueToStringCopy(context, exc, nullptr);
        size_t max_bytes = JSStringGetMaximumUTF8CStringSize(str);
        result.resize(max_bytes);
        size_t bytes_written = JSStringGetUTF8CString(str, &result[0], max_bytes);
        if (max_bytes == 0) {
            return;
        }
        result.resize(bytes_written - 1);
        if (!result.empty()) {
            LOG_JS_ERROR("[JS_ERROR] : %s", result.c_str());
            LOG_JS_ERROR("[JS_ERROR][WeexRuntime] : %s", result.c_str());
        }
        JSStringRelease(str);
    }


/////////////////////////////////////////////////////////////////////////////
/*JSUtils implemention*/
    bool JSUtils::HasProperty(JSContextRef ctx, JSObjectRef ob,
                              const std::string &name) {
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
                              JSClassRef clz, const std::string &name,
                              RuntimeValues *value) {
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
                                 const std::string &name) {
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
                                     const std::string &name) {
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
                                       std::vector<std::string> &native_array) {
        if (!ctx || !ob)
            return;
        //  LOG_TEST("[jsc_utils] GetPropertyNameArray ");

        JSContextRef context = static_cast<JSContextRef>(ctx);
        JSObjectRef object = static_cast<JSObjectRef>(ob);
        JSPropertyNameArrayRef array_ref = JSObjectCopyPropertyNames(context,
                                                                     object);
        size_t count = JSPropertyNameArrayGetCount(array_ref);

        //  LOG_TEST("[jsc_utils] JSPropertyNameArrayGetCount :%lu", count);
        for (size_t i = 0; i < count; i++) {
            JSStringRef str_ref = JSPropertyNameArrayGetNameAtIndex(array_ref, i);
            size_t max_len = JSStringGetMaximumUTF8CStringSize(str_ref);
            std::string holder;
            holder.resize(max_len);
            char *ptr = &holder[0];
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
