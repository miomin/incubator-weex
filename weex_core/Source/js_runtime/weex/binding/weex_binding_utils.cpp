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
// Created by 陈佩翰 on 2019/2/12.
//

#include "weex_binding_utils.h"
#include "js_runtime/utils/log_utils.h"
#include "core/bridge/script_bridge.h"
#include "js_runtime/weex/object/weex_global_object_v2.h"
#include "js_runtime/utils/base64.hpp"

namespace weex {
    namespace jsengine {
        unicorn::ScopeValues
        WeexBindingUtils::atob(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                               const std::vector<unicorn::ScopeValues> &vars) {

            std::string origin_data;
            vars[0]->GetAsString(&origin_data);

            LOG_WEEX_BINDING("[WeexBindingUtils] method :atob, frome: %s", origin_data.c_str());

            std::string res = beast::detail::base64_decode(origin_data);

            LOG_WEEX_BINDING("[WeexBindingUtils] method :atob, res: %s", res.c_str());

            return unicorn::RuntimeValues::MakeString(res);
        }

        unicorn::ScopeValues
        WeexBindingUtils::btoa(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                               const std::vector<unicorn::ScopeValues> &vars) {
            std::string origin_data;
            vars[0]->GetAsString(&origin_data);

            LOG_WEEX_BINDING("[WeexBindingUtils] method :btoa, frome: %s", origin_data.c_str());

            std::string res = beast::detail::base64_encode(origin_data);

            LOG_WEEX_BINDING("[WeexBindingUtils] method :btoa, res: %s", res.c_str());

            return unicorn::RuntimeValues::MakeString(res);
        }

        unicorn::ScopeValues WeexBindingUtils::nativeLog(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                                                         const std::vector<unicorn::ScopeValues> &vars) {


            // LOG_JS("[JS] enter native log :args->size %d", vars.size());
            if (vars.empty()) {
                unicorn::RuntimeValues::MakeBool(true);
            }
            std::string logStr;
            for (int i = 0; i < vars.size(); i++) {
                if (vars[i]->IsString()) {
                    std::string logItem;
                    vars[i]->GetAsString(&logItem);
                    logStr.append(logItem);
                } else {
                    LOG_WEEX_BINDING("[WeexBindingUtils]log arg is not str, not supprot ,arg index:%d", i);
                }
            }

            LOG_JS("[WeexBindingUtils][JS] %s", logStr.c_str());
            if (!logStr.empty()) {
                // nativeObject->js_bridge()->core_side()->NativeLog(logStr.c_str());
            }
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexBindingUtils::setNativeTimeout(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                                                                const std::vector<unicorn::ScopeValues> &vars) {
            LOG_WEEX_BINDING("[WeexBindingUtils][WeexBindingUtils] method :setNativeTimeout");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues
        WeexBindingUtils::setNativeInterval(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                                            const std::vector<unicorn::ScopeValues> &vars) {
            LOG_WEEX_BINDING("WeexBindingUtils method :setNativeInterval");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues
        WeexBindingUtils::clearNativeTimeout(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                                             const std::vector<unicorn::ScopeValues> &vars) {
            LOG_WEEX_BINDING("[WeexBindingUtils] method :clearNativeTimeout");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues
        WeexBindingUtils::clearNativeInterval(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                                              const std::vector<unicorn::ScopeValues> &vars) {
            LOG_WEEX_BINDING("[WeexBindingUtils] method :clearNativeInterval");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues
        WeexBindingUtils::callT3DLinkNative(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                                            const std::vector<unicorn::ScopeValues> &vars) {
            int type;
            std::string arg_str;

            vars[0]->GetAsInteger(&type);
            vars[1]->GetAsString(&arg_str);

            LOG_WEEX_BINDING("WeexBindingUtils method :callT3DLinkNative type:%d, arg_str:%s", type, arg_str.c_str());

            auto result = nativeObject->js_bridge()->core_side()->CallT3DLinkNative(type, arg_str.c_str());

            LOG_WEEX_BINDING("WeexBindingUtils method :callT3DLinkNative result :%s", result);

            std::string utf_8_result = std::string(result);

            return unicorn::RuntimeValues::MakeString(utf_8_result);
        }

        unicorn::ScopeValues
        WeexBindingUtils::callGCanvasLinkNative(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                                                const std::vector<unicorn::ScopeValues> &vars) {
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
    }
}
