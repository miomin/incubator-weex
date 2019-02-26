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
//
// Created by 陈佩翰 on 2019/1/23.
//

#include "weex_global_binding.h"
#include "js_runtime/utils/log_utils.h"
#include "weex_binding_utils.h"
#include "core/bridge/script_bridge.h"
#include "js_runtime/weex/utils/weex_conversion_utils.h"
#include <cstdlib>
#include "js_runtime/weex/object/weex_global_object_v2.h"

namespace weex {
    namespace jsengine {
        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callNative)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callNativeModule)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callNativeComponent)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, setTimeoutNative)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, nativeLog)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, notifyTrimMemory)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, markupState)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, atob)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, btoa)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callCreateBody)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callUpdateFinish)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callCreateFinish)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callRefreshFinish)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callUpdateAttrs)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callUpdateStyle)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callAddElement)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callRemoveElement)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callMoveElement)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callAddEvent)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callRemoveEvent)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callGCanvasLinkNative)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, setIntervalWeex)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, clearIntervalWeex)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, callT3DLinkNative)

        CLASS_METHOD_CALLBACK(WeexGlobalBinding, __updateComponentData)

        CLASS_REGISTER_START(WeexGlobalBinding, Global)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callNative)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callNativeModule)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callNativeComponent)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, setTimeoutNative)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, nativeLog)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, notifyTrimMemory)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, markupState)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, atob)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, btoa)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callCreateBody)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callUpdateFinish)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callCreateFinish)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callRefreshFinish)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callUpdateAttrs)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callUpdateStyle)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callAddElement)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callRemoveElement)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callMoveElement)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callAddEvent)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callRemoveEvent)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callGCanvasLinkNative)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, setIntervalWeex)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, clearIntervalWeex)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, callT3DLinkNative)
            REGISTER_METHOD_CALLBACK(WeexGlobalBinding, __updateComponentData)
        CLASS_REGISTER_END(WeexGlobalBinding)


        WeexGlobalBinding::~WeexGlobalBinding() {
            LOG_TEST("WeexGlobalBinding delete");
        }

        WeexGlobalBinding::WeexGlobalBinding(unicorn::EngineContext *context, const OpaqueJSContext *js_ctx)
                : RuntimeObject(context, js_ctx) {
            SetJSClass(WeexGlobalBinding::s_jsclass_WeexGlobalBinding);
            LOG_TEST("WeexGlobalBinding init");
        }

        unicorn::ScopeValues
        WeexGlobalBinding::callNative(const std::vector<unicorn::ScopeValues> &vars) {
            std::string id_js;
            std::string task_str;
            std::string callback_str;
            vars[0]->GetAsString(&id_js);
            vars[1]->GetAsString(&task_str);
            vars[2]->GetAsString(&callback_str);
            LOG_TEST("WeexGlobalBinding callNative id_js:%s,task_str:%s,callback_str:%s",
                     id_js.c_str(), task_str.c_str(), callback_str.c_str());

            this->nativeObject->js_bridge()->core_side()->CallNative(id_js.c_str(), task_str.c_str(),
                                                                     callback_str.c_str());
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexGlobalBinding::callNativeModule(const std::vector<unicorn::ScopeValues> &vars) {
            std::string instanceId;
            std::string module;
            std::string method;
            Args argument;
            Args options;


            vars[0]->GetAsString(&instanceId);
            vars[1]->GetAsString(&module);
            vars[2]->GetAsString(&method);

            LOGW("WeexRuntime: callNativeModule %s %s", module.c_str(), method.c_str());
            WeexConversionUtils::ConvertRunTimeVaueToWson(vars[3].get(), argument);
            WeexConversionUtils::ConvertRunTimeVaueToWson(vars[4].get(), options);


            auto result = this->nativeObject->js_bridge()->core_side()->CallNativeModule(
                    instanceId.c_str(),
                    module.c_str(),
                    method.c_str(),
                    argument.getValue(),
                    argument.getLength(),
                    options.getValue(),
                    options.getLength()
            );
            auto ret = WeexConversionUtils::WeexValueToRuntimeValue(GetEngineContext(), result.get());
//            LOG_TEST("end :callNativeModule instance:%s,module:%s,method:%s",
//                     instanceId.c_str(), module.c_str(), method.c_str());
//                    return ret;
            return ret;
        }

        unicorn::ScopeValues WeexGlobalBinding::callNativeComponent(
                const std::vector<unicorn::ScopeValues> &vars) {

            std::string instanceId;
            std::string module;
            std::string method;
            Args argument;
            Args options;


            vars[0]->GetAsString(&instanceId);
            vars[1]->GetAsString(&module);
            vars[2]->GetAsString(&method);

            LOGW("WeexRuntime: callNativeComponent %s %s", module.c_str(), method.c_str());

            WeexConversionUtils::ConvertRunTimeVaueToWson(vars[3].get(), argument);
            WeexConversionUtils::ConvertRunTimeVaueToWson(vars[4].get(), options);


            this->nativeObject->js_bridge()->core_side()->CallNativeComponent(
                    instanceId.c_str(),
                    module.c_str(),
                    method.c_str(),
                    argument.getValue(),
                    argument.getLength(),
                    options.getValue(),
                    options.getLength()
            );
            return unicorn::RuntimeValues::MakeInt(0);
        }

        unicorn::ScopeValues WeexGlobalBinding::setTimeoutNative(
                const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :setTimeoutNative");
            std::string callback_str;
            int time;
            vars[0]->GetAsString(&callback_str);
            vars[1]->GetAsInteger(&time);

            std::string time_str = std::to_string(time);

            LOG_TEST("WeexGlobalBinding method :setTimeoutNative ,callback:%s, time:%d", callback_str.c_str(), time);

            nativeObject->js_bridge()->core_side()->SetTimeout(
                    callback_str.c_str(),
                    time_str.c_str()
            );
            return unicorn::RuntimeValues::MakeInt(0);
        }

        unicorn::ScopeValues
        WeexGlobalBinding::nativeLog(const std::vector<unicorn::ScopeValues> &vars) {
            return WeexBindingUtils::nativeLog(this->nativeObject, vars);
        }

        unicorn::ScopeValues WeexGlobalBinding::notifyTrimMemory(
                const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :notifyTrimMemory");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues
        WeexGlobalBinding::markupState(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :markupState");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues
        WeexGlobalBinding::atob(const std::vector<unicorn::ScopeValues> &vars) {
            return WeexBindingUtils::atob(this->nativeObject, vars);
        }

        unicorn::ScopeValues
        WeexGlobalBinding::btoa(const std::vector<unicorn::ScopeValues> &vars) {
            return WeexBindingUtils::btoa(this->nativeObject, vars);
        }

        unicorn::ScopeValues WeexGlobalBinding::callGCanvasLinkNative(
                const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :callGCanvasLinkNative");
            std::string id_str;
            int type;
            std::string arg_str;

            vars[0]->GetAsString(&id_str);
            vars[1]->GetAsInteger(&type);
            vars[2]->GetAsString(&arg_str);

            auto result = nativeObject->js_bridge()->core_side()->CallGCanvasLinkNative(
                    id_str.c_str(), type, arg_str.c_str()
            );
            return unicorn::RuntimeValues::MakeString(result);
        }

        unicorn::ScopeValues
        WeexGlobalBinding::setIntervalWeex(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :setIntervalWeex");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexGlobalBinding::clearIntervalWeex(
                const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :clearIntervalWeex");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexGlobalBinding::callT3DLinkNative(
                const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :callT3DLinkNative");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexGlobalBinding::__updateComponentData(
                const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :__updateComponentData");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexGlobalBinding::callCreateBody(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :callCreateBody");
            ////    base::debug::TraceScope traceScope("weex", "callCreateBody");
            Args pageId;
            std::string page_id;
            Args dom_str;
            vars[0]->GetAsString(&page_id);
            WeexConversionUtils::ConvertRunTimeVaueToWson(vars[1].get(), dom_str);

            LOGE("[WeexGlobalBinding] [sendCreateBodyAction] doc:%s", page_id.c_str());

            nativeObject->js_bridge()->core_side()->CreateBody(page_id.c_str(), dom_str.getValue(),
                                                               dom_str.getLength());
            return unicorn::RuntimeValues::MakeInt(0);
        }

        unicorn::ScopeValues WeexGlobalBinding::callUpdateFinish(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :callUpdateFinish");
            ////    base::debug::TraceScope traceScope("weex", "functionCallUpdateFinish");
////
////    Args idChar;
////    Args taskChar;
////    Args callBackChar;
////    getStringArgsFromState(state, 0, idChar);
////    getWsonArgsFromState(state, 1, taskChar);
////    getWsonArgsFromState(state, 2, callBackChar);
////    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
////    auto result = globalObject->js_bridge()->core_side()->UpdateFinish(idChar.getValue(), taskChar.getValue(),
////                                                                       taskChar.getLength(), callBackChar.getValue(),
////                                                                       callBackChar.getLength());
////    return JSValue::encode(jsNumber(result));
//    return JSValue::encode(jsUndefined());

            std::string page_id;
            Args task_str;
            Args call_back_str;

            vars[0]->GetAsString(&page_id);
            WeexConversionUtils::ConvertRunTimeVaueToWson(vars[1].get(), task_str);
            WeexConversionUtils::ConvertRunTimeVaueToWson(vars[1].get(), call_back_str);

            auto res = nativeObject->js_bridge()->core_side()->UpdateFinish(page_id.c_str(), task_str.getValue(),
                                                                            task_str.getLength(), call_back_str.getValue(),
                                                                            call_back_str.getLength());
            return unicorn::RuntimeValues::MakeInt(res);
        }

        unicorn::ScopeValues WeexGlobalBinding::callCreateFinish(const std::vector<unicorn::ScopeValues> &vars) {
            std::string page_id;
            vars[0]->GetAsString(&page_id);

            LOGE("[WeexGlobalBinding] [sendCreateFinish] doc:%s, ", page_id.c_str());

            nativeObject->js_bridge()->core_side()->CreateFinish(page_id.c_str());
            return unicorn::RuntimeValues::MakeInt(0);
        }

        unicorn::ScopeValues WeexGlobalBinding::callRefreshFinish(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :callRefreshFinish");


            ////    base::debug::TraceScope traceScope("weex", "functionCallRefreshFinish");
////
////    Args idChar;
////    Args taskChar;
////    Args callBackChar;
////    getStringArgsFromState(state, 0, idChar);
////    getStringArgsFromState(state, 1, taskChar);
////    getStringArgsFromState(state, 2, callBackChar);
////    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
////    int result = globalObject->js_bridge()->core_side()->RefreshFinish(idChar.getValue(),
////                                                                       taskChar.getValue(),
////                                                                       callBackChar.getValue());
//    //   return JSValue::encode(jsNumber(result));
//    return JSValue::encode(jsUndefined());

            std::string page_id;
            std::string task;
            std::string callBack;

            vars[0]->GetAsString(&page_id);
            vars[1]->GetAsString(&task);
            vars[2]->GetAsString(&callBack);

            auto result = nativeObject->js_bridge()->core_side()->RefreshFinish(page_id.c_str(), task.c_str(),
                                                                                callBack.c_str());
            return unicorn::RuntimeValues::MakeInt(result);
        }

        unicorn::ScopeValues WeexGlobalBinding::callUpdateAttrs(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :callUpdateAttrs");
            ////    base::debug::TraceScope traceScope("weex", "functionCallUpdateAttrs");
////
////    Args instanceId;
////    Args ref;
////    Args domAttrs;
////    getStringArgsFromState(state, 0, instanceId);
////    getStringArgsFromState(state, 1, ref);
////    getWsonArgsFromState(state, 2, domAttrs);
////
////    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
////    globalObject->js_bridge()->core_side()->UpdateAttrs(instanceId.getValue(),
////                                                        ref.getValue(),
////                                                        domAttrs.getValue(), domAttrs.getLength());
//    return JSValue::encode(jsNumber(0));

            std::string page_id;
            std::string node_ref;
            Args dom_attrs;

            vars[0]->GetAsString(&page_id);
            vars[1]->GetAsString(&node_ref);
            WeexConversionUtils::ConvertRunTimeVaueToWson(vars[2].get(), dom_attrs);

            nativeObject->js_bridge()->core_side()->UpdateAttrs(
                    page_id.c_str(),
                    node_ref.c_str(),
                    dom_attrs.getValue(),
                    dom_attrs.getLength()
            );
            return unicorn::RuntimeValues::MakeInt(0);
        }

        unicorn::ScopeValues WeexGlobalBinding::callUpdateStyle(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :callUpdateStyle");

            ////    base::debug::TraceScope traceScope("weex", "functionCallUpdateStyle");
////
////    Args instanceId;
////    Args ref;
////    Args domStyles;
////    getStringArgsFromState(state, 0, instanceId);
////    getStringArgsFromState(state, 1, ref);
////    getWsonArgsFromState(state, 2, domStyles);
////
////
////    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
////    globalObject->js_bridge()->core_side()->UpdateStyle(instanceId.getValue(),
////                                                        ref.getValue(),
////                                                        domStyles.getValue(), domStyles.getLength());
//    return JSValue::encode(jsNumber(0));

            std::string page_id;
            std::string node_ref;
            Args dom_styles;

            vars[0]->GetAsString(&page_id);
            vars[1]->GetAsString(&node_ref);
            WeexConversionUtils::ConvertRunTimeVaueToWson(vars[2].get(), dom_styles);


            nativeObject->js_bridge()->core_side()->UpdateStyle(page_id.c_str(), node_ref.c_str(),
                                                                dom_styles.getValue(),
                                                                dom_styles.getLength());

            return unicorn::RuntimeValues::MakeInt(0);
        }


        unicorn::ScopeValues WeexGlobalBinding::callAddElement(const std::vector<unicorn::ScopeValues> &vars) {

            ////    base::debug::TraceScope traceScope("weex", "callAddElement");
////    Args instanceId;
////    Args parentRefChar;
////    Args domStr;
////    Args index_cstr;
////
////    getStringArgsFromState(state, 0, instanceId);
////    getStringArgsFromState(state, 1, parentRefChar);
////    getWsonArgsFromState(state, 2, domStr);
////    getStringArgsFromState(state, 3, index_cstr);
////
////
////    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
////
////    globalObject->js_bridge()->core_side()->AddElement(instanceId.getValue(),
////                                                       parentRefChar.getValue(),
////                                                       domStr.getValue(),
////                                                       domStr.getLength(),
////                                                       index_cstr.getValue());
//    return JSValue::encode(jsNumber(0));


            std::string page_id;
            std::string parent_ref;
            Args dom_str;
            int index;

            vars[0]->GetAsString(&page_id);
            vars[1]->GetAsString(&parent_ref);
            WeexConversionUtils::ConvertRunTimeVaueToWson(vars[2].get(), dom_str);
            vars[3]->GetAsInteger(&index);

            std::string index_str = std::to_string(index);

            LOGE("[WeexGlobalBinding] [AddElementAction] doc:%s,parent:%s,index:%s", page_id.c_str(),
                 parent_ref.c_str(),
                 index_str.c_str());


            nativeObject->js_bridge()->core_side()->AddElement(
                    page_id.c_str(), parent_ref.c_str(), dom_str.getValue(),
                    dom_str.getLength(), index_str.c_str());

            return unicorn::RuntimeValues::MakeInt(0);
        }

        unicorn::ScopeValues WeexGlobalBinding::callRemoveElement(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :callRemoveElement");

            //    base::debug::TraceScope traceScope("weex", "functionCallRemoveElement");
//
//    Args idChar;
//    Args dataChar;
//    getStringArgsFromState(state, 0, idChar);
//    getStringArgsFromState(state, 1, dataChar);
//
//    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
//    globalObject->js_bridge()->core_side()->RemoveElement(idChar.getValue(), dataChar.getValue());
            //   return JSValue::encode(jsNumber(0));

            std::string page_id;
            std::string node_ref;

            vars[0]->GetAsString(&page_id);
            vars[1]->GetAsString(&node_ref);
            nativeObject->js_bridge()->core_side()->RemoveElement(page_id.c_str(), node_ref.c_str());
            return unicorn::RuntimeValues::MakeInt(0);
        }

        unicorn::ScopeValues WeexGlobalBinding::callMoveElement(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :callMoveElement");

//    base::debug::TraceScope traceScope("weex", "functionCallMoveElement");
//
//    Args idChar;
//    Args refChar;
//    Args dataChar;
//    Args indexChar;
//    getStringArgsFromState(state, 0, idChar);
//    getStringArgsFromState(state, 1, refChar);
//    getStringArgsFromState(state, 2, dataChar);
//    getStringArgsFromState(state, 3, indexChar);
//
//    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
//    globalObject->js_bridge()->core_side()->MoveElement(idChar.getValue(),
//                                                        refChar.getValue(),
//                                                        dataChar.getValue(),
//                                                        atoi(indexChar.getValue()));
//
//            return JSValue::encode(jsNumber(0));

            std::string page_id;
            std::string node_ref;
            std::string parent_ref;
            std::string index_char;

            vars[0]->GetAsString(&page_id);
            vars[1]->GetAsString(&parent_ref);
            vars[2]->GetAsString(&page_id);
            vars[3]->GetAsString(&index_char);


            nativeObject->js_bridge()->core_side()->MoveElement(
                    page_id.c_str(),
                    node_ref.c_str(),
                    parent_ref.c_str(),
                    atoi(index_char.c_str())
            );
            return unicorn::RuntimeValues::MakeInt(0);
        }

        unicorn::ScopeValues WeexGlobalBinding::callAddEvent(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :callAddEvent");

//    base::debug::TraceScope traceScope("weex", "functionCallAddEvent");
//
//    Args idChar;
//    Args refChar;
//    Args eventChar;
//    getStringArgsFromState(state, 0, idChar);
//    getStringArgsFromState(state, 1, refChar);
//    getStringArgsFromState(state, 2, eventChar);
//
//    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
//    globalObject->js_bridge()->core_side()->AddEvent(idChar.getValue(),
//                                                     refChar.getValue(),
//                                                     eventChar.getValue());
//                return JSValue::encode(jsNumber(0));

            std::string page_id;
            std::string node_ref;
            std::string event;

            vars[0]->GetAsString(&page_id);
            vars[1]->GetAsString(&node_ref);
            vars[2]->GetAsString(&event);

            nativeObject->js_bridge()->core_side()->AddEvent(
                    page_id.c_str(),
                    node_ref.c_str(),
                    event.c_str()
            );
            return unicorn::RuntimeValues::MakeInt(0);
        }

        unicorn::ScopeValues WeexGlobalBinding::callRemoveEvent(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexGlobalBinding method :callRemoveEvent");
            //    base::debug::TraceScope traceScope("weex", "functionCallRemoveEvent");
//
//    Args idChar;
//    Args refChar;
//    Args eventChar;
//    getStringArgsFromState(state, 0, idChar);
//    getStringArgsFromState(state, 1, refChar);
//    getStringArgsFromState(state, 2, eventChar);
//
//    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
//    globalObject->js_bridge()->core_side()->RemoveEvent(idChar.getValue(),
//                                                        refChar.getValue(),
//                                                        eventChar.getValue());
//                return JSValue::encode(jsNumber(0));


            std::string page_id;
            std::string node_ref;
            std::string event;

            vars[0]->GetAsString(&page_id);
            vars[1]->GetAsString(&node_ref);
            vars[2]->GetAsString(&event);

            nativeObject->js_bridge()->core_side()->RemoveEvent(
                    page_id.c_str(),
                    node_ref.c_str(),
                    event.c_str()
            );
            return unicorn::RuntimeValues::MakeInt(0);
        }
    }
}


