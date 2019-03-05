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
// Created by 陈佩翰 on 2019/2/22.
//

#include <js_runtime/weex/binding/weex_global_binding.h>
#include <js_runtime/weex/binding/weex_instance_binding.h>
#include <js_runtime/weex/binding/app_context_binding.h>
#include "weex_global_object_v2.h"
#include "js_runtime/utils/log_utils.h"

void WeexGlobalObjectV2::makeWeexGlobalObject(unicorn::RuntimeVM *vm) {
    this->object_type_ = WeexGlobal;
    weex::jsengine::WeexGlobalBinding::CreateClassRef(nullptr);
    context = unicorn::RuntimeContext::Create(vm, weex::jsengine::WeexGlobalBinding::s_jsclass_WeexGlobalBinding);
    LOG_RUNTIME("[Context] makeWeexGlobalObject context ptr:%p", context->GetEngineContext()->GetContext());
    auto globalObjectBinding = new weex::jsengine::WeexGlobalBinding(context->GetEngineContext(), nullptr);
    auto globalJSObject = context->GetEngineContext()->GetGlobalObjectInContext();
    context->GetEngineContext()->BindDataToObject(globalJSObject, globalObjectBinding);
    globalObjectBinding->SetJSObject(globalJSObject);
    globalObjectBinding->nativeObject.reset(this);
    LOG_RUNTIME("WeexGlobalObject make binding:%p,native this:%p", globalObjectBinding, this);
    //binding dom api
    //weex::jsengine::DomManager::BindingDomApi(context->GetEngineContext(), std::string("global"),std::string("weex_global_context"));
}

void
WeexGlobalObjectV2::makeWeexInstanceObject(unicorn::RuntimeVM *vm, const std::string &id, const std::string &name) {
    this->object_type_ = WeexInstance;
    //global object not support bind static method for globalObject now cause jsruntime
    weex::jsengine::WeexInstanceBinding::CreateClassRef(nullptr);
    context = unicorn::RuntimeContext::Create(vm, weex::jsengine::WeexInstanceBinding::s_jsclass_WeexInstanceBinding);
    LOG_RUNTIME("[Context] makeWeexInstanceObject context ptr:%p", context->GetEngineContext()->GetContext());
    context->GetEngineContext()->SetName(id);
    auto globalObjectBinding = new weex::jsengine::WeexInstanceBinding(context->GetEngineContext(), nullptr);
    globalObjectBinding->nativeObject.reset(this);
    auto globalJSObject = context->GetEngineContext()->GetGlobalObjectInContext();
    context->GetEngineContext()->BindDataToObject(globalJSObject, globalObjectBinding);
    globalObjectBinding->SetJSObject(globalJSObject);
}

void WeexGlobalObjectV2::makeAppWorkerObject(unicorn::RuntimeVM *vm) {
    this->object_type_ = AppWorker;
    //global object not support bind static method for globalObject now cause jsruntime
    weex::jsengine::AppWorkerBinding::CreateClassRef(nullptr);
    context = unicorn::RuntimeContext::Create(vm, weex::jsengine::AppWorkerBinding::s_jsclass_AppWorkerBinding);
    auto globalObjectBinding = new weex::jsengine::AppWorkerBinding(context->GetEngineContext(), nullptr);
    auto globalJSObject = context->GetEngineContext()->GetGlobalObjectInContext();
    context->GetEngineContext()->BindDataToObject(globalJSObject, globalObjectBinding);
    globalObjectBinding->nativeObject.reset(this);
    globalObjectBinding->SetJSObject(globalJSObject);
}

void WeexGlobalObjectV2::addExtraOptions(std::vector<INIT_FRAMEWORK_PARAMS *> &params) {
    if (params.size() <= 0) {
        return;
    }
    std::unique_ptr<unicorn::Map> wxExtraOption = unicorn::Map::CreateFromNative(this->context->GetEngineContext(),
                                                                                 unicorn::RuntimeValues::MakeNull());
//    VM &vm = this->vm();
//    JSNonFinalObject *WXExtraOption = SimpleObject::create(vm, this);
    for (int i = 0; i < params.size(); i++) {
        INIT_FRAMEWORK_PARAMS *param = params[i];
        wxExtraOption->Insert(
                std::string(param->type->content),
                new unicorn::RuntimeValues(param->value->content, param->value->length)
        );

//        String &&type = String::fromUTF8(param->type->content);
//        String &&value = String::fromUTF8(param->value->content);
//        addString(vm, WXExtraOption, param->type->content, WTFMove(value));
    }
    this->context->GetEngineContext()->SetGlobalPropertyValue(
            std::string("WXExtraOption"),
            unicorn::RuntimeValues::MakeMap(std::move(wxExtraOption))
    );
}

void
WeexGlobalObjectV2::initWxEnvironment(std::vector<INIT_FRAMEWORK_PARAMS *> &params, bool isSave) {
    std::unique_ptr<unicorn::Map> wxEnvironment = unicorn::Map::CreateFromNative(this->context->GetEngineContext(),
                                                                                 unicorn::RuntimeValues::MakeNull());
    for (int i = 0; i < params.size(); i++) {
        INIT_FRAMEWORK_PARAMS *param = params[i];

        String &&type = String::fromUTF8(param->type->content);
        String &&value = String::fromUTF8(param->value->content);
        if (isSave) {
            auto init_framework_params = (INIT_FRAMEWORK_PARAMS *) malloc(sizeof(INIT_FRAMEWORK_PARAMS));

            if (init_framework_params == nullptr) {
                return;
            }

            memset(init_framework_params, 0, sizeof(INIT_FRAMEWORK_PARAMS));
            init_framework_params->type = genWeexByteArraySS(param->type->content, param->type->length);
            init_framework_params->value = genWeexByteArraySS(param->value->content, param->value->length);

            m_initFrameworkParams.push_back(init_framework_params);
        }

//        if (!isGlobalConfigStartUpSet) {
//            if (strncmp(type.utf8().data(), WX_GLOBAL_CONFIG_KEY, strlen(WX_GLOBAL_CONFIG_KEY)) == 0) {
//                const char *config = value.utf8().data();
//                doUpdateGlobalSwitchConfig(config);
//            }
//            isGlobalConfigStartUpSet = true;
//        }

        // --------------------------------------------------------
        // add for debug mode
        if (String("debugMode") == type && String("true") == value) {
            Weex::LogUtil::setDebugMode(true);
        }
        // --------------------------------------------------------

        //LOGE("initWxEnvironment and value is %s", value.utf8().data());
        // addString(vm, WXEnvironment, param->type->content, WTFMove(value));
        //LOGD("initWxEnvironment initWxEnvironment  key:%s ,vaule: %s", type.utf8().data(), value.utf8().data());
        wxEnvironment->Insert(std::string(type.utf8().data()),
                              new unicorn::RuntimeValues(std::string(value.utf8().data())));
        //free(params);
    }
    const char *key = object_type_ == AppWorker ? "__windmill_environment__" : "WXEnvironment";

    this->context->GetEngineContext()->SetGlobalPropertyValue(
            std::string(key),
            unicorn::RuntimeValues::MakeMap(std::move(wxEnvironment))
    );
}

void WeexGlobalObjectV2::setScriptBridge(WeexCore::ScriptBridge *script_bridge) {
    this->script_bridge_ = script_bridge;
}


unicorn::RuntimeValues* WeexGlobalObjectV2::removeTimer(uint32_t function_id) {
    auto iter = function_maps_.find(function_id);
    if (iter == function_maps_.end()) {
        LOGE("timer do not exist!");
        return nullptr;
    }
    auto funcValue = function_maps_[function_id];
    function_maps_.erase(function_id);
    return funcValue;
}

uint32_t WeexGlobalObjectV2::genFunctionID() {
    if (function_id_ > (INT_MAX - 1)) {
        LOGE(" WeexGlobalObject::genFunctionID timer fucntion id to large, something wrong now, crash!");
        abort();
    }
    return function_id_++;
}

unicorn::RuntimeValues* WeexGlobalObjectV2::getTimerFunction(uint32_t function_id) {
    auto iter = function_maps_.find(function_id);
    if (iter == function_maps_.end())
        return nullptr;
    return function_maps_[function_id];
}

void WeexGlobalObjectV2::addTimer(uint32_t function_id,  unicorn::RuntimeValues* func) {

    auto iter = function_maps_.find(function_id);
    if (iter != function_maps_.end()) {
        LOGE("timer already exist in map, return now");
        return;
    }
    function_maps_[function_id] = func;

}

