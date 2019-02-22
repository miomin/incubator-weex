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

#include "weex_instance_binding.h"
#include "js_runtime/utils/log_utils.h"
#include "js_runtime/weex/object/weex_global_object_v2.h"
#include "core/bridge/script_bridge.h"
#include "weex_binding_utils.h"

namespace weex {
    namespace jsengine {

        CLASS_METHOD_CALLBACK(WeexInstanceBinding, nativeLog)

        CLASS_METHOD_CALLBACK(WeexInstanceBinding, atob)

        CLASS_METHOD_CALLBACK(WeexInstanceBinding, btoa)

        CLASS_METHOD_CALLBACK(WeexInstanceBinding, callGCanvasLinkNative)

        CLASS_METHOD_CALLBACK(WeexInstanceBinding, callT3DLinkNative)

        CLASS_METHOD_CALLBACK(WeexInstanceBinding, setNativeTimeout)

        CLASS_METHOD_CALLBACK(WeexInstanceBinding, setNativeInterval)

        CLASS_METHOD_CALLBACK(WeexInstanceBinding, clearNativeTimeout)

        CLASS_METHOD_CALLBACK(WeexInstanceBinding, clearNativeInterval)

        CLASS_METHOD_CALLBACK(WeexInstanceBinding, console)

        CLASS_METHOD_CALLBACK(WeexInstanceBinding, __updateComponentData)

        CLASS_REGISTER_START(WeexInstanceBinding, Instance)
            REGISTER_METHOD_CALLBACK(WeexInstanceBinding, nativeLog)
            REGISTER_METHOD_CALLBACK(WeexInstanceBinding, atob)
            REGISTER_METHOD_CALLBACK(WeexInstanceBinding, btoa)
            REGISTER_METHOD_CALLBACK(WeexInstanceBinding, callGCanvasLinkNative)
            REGISTER_METHOD_CALLBACK(WeexInstanceBinding, callT3DLinkNative)
            REGISTER_METHOD_CALLBACK(WeexInstanceBinding, setNativeTimeout)
            REGISTER_METHOD_CALLBACK(WeexInstanceBinding, setNativeInterval)
            REGISTER_METHOD_CALLBACK(WeexInstanceBinding, clearNativeTimeout)
            REGISTER_METHOD_CALLBACK(WeexInstanceBinding, clearNativeInterval)
            REGISTER_METHOD_CALLBACK(WeexInstanceBinding, console)
            REGISTER_METHOD_CALLBACK(WeexInstanceBinding, __updateComponentData)
        CLASS_REGISTER_END(WeexInstanceBinding)


        WeexInstanceBinding::WeexInstanceBinding(unicorn::EngineContext *context, const OpaqueJSContext *js_ctx)
                : RuntimeObject(context, js_ctx) {
            SetJSClass(WeexInstanceBinding::s_jsclass_WeexInstanceBinding);
            LOG_TEST("WeexInstanceBinding init");
        }

        WeexInstanceBinding::~WeexInstanceBinding() {
            LOG_TEST("WeexInstanceBinding delete");

        }

        unicorn::ScopeValues
        WeexInstanceBinding::nativeLog(const std::vector<unicorn::ScopeValues> &vars) {
            return WeexBindingUtils::nativeLog(this->nativeObject, vars);
        }

        unicorn::ScopeValues
        WeexInstanceBinding::atob(const std::vector<unicorn::ScopeValues> &vars) {
            return WeexBindingUtils::atob(this->nativeObject, vars);
        }

        unicorn::ScopeValues
        WeexInstanceBinding::btoa(const std::vector<unicorn::ScopeValues> &vars) {
            return WeexBindingUtils::btoa(this->nativeObject, vars);
        }

        unicorn::ScopeValues WeexInstanceBinding::callGCanvasLinkNative(
                const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexInstanceBinding method :callGCanvasLinkNative");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexInstanceBinding::callT3DLinkNative(
                const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexInstanceBinding method :callT3DLinkNative");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexInstanceBinding::setNativeTimeout(
                const std::vector<unicorn::ScopeValues> &vars) {
            return WeexBindingUtils::setNativeTimeout(this->nativeObject, vars);
        }

        unicorn::ScopeValues WeexInstanceBinding::setNativeInterval(
                const std::vector<unicorn::ScopeValues> &vars) {
            return WeexBindingUtils::setNativeInterval(this->nativeObject, vars);
        }

        unicorn::ScopeValues WeexInstanceBinding::clearNativeTimeout(
                const std::vector<unicorn::ScopeValues> &vars) {
            return WeexBindingUtils::clearNativeTimeout(this->nativeObject, vars);
        }

        unicorn::ScopeValues WeexInstanceBinding::clearNativeInterval(
                const std::vector<unicorn::ScopeValues> &vars) {
            return WeexBindingUtils::clearNativeInterval(this->nativeObject, vars);
        }

        unicorn::ScopeValues
        WeexInstanceBinding::console(const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexInstanceBinding method :console");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexInstanceBinding::__updateComponentData(
                const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST("WeexInstanceBinding method :__updateComponentData");
            return unicorn::RuntimeValues::MakeUndefined();
        }
    }
}


