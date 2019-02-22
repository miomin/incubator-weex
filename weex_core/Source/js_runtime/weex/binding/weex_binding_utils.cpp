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

namespace weex {
    namespace jsengine {
        unicorn::ScopeValues
        WeexBindingUtils::atob(const std::unique_ptr<WeexGlobalObject> &nativeObject,
                               const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST(" method :atob");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues
        WeexBindingUtils::btoa(const std::unique_ptr<WeexGlobalObject> &nativeObject,
                               const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST(" method :btoa");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexBindingUtils::nativeLog(const std::unique_ptr<WeexGlobalObject> &nativeObject,
                                                         const std::vector<unicorn::ScopeValues> &vars) {


            LOG_JS("[JS] enter native log :args->size %d",vars.size());
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
                    LOG_TEST("log arg is not str, not supprot ,arg index:%d", i);
                }
            }

            LOG_JS("[JS] %s",logStr.c_str());
            if (!logStr.empty()) {
               // nativeObject->js_bridge()->core_side()->NativeLog(logStr.c_str());
            }
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexBindingUtils::setNativeTimeout(const std::unique_ptr<WeexGlobalObject> &nativeObject,
                                                                const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST(" method :setNativeTimeout");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexBindingUtils::setNativeInterval(const std::unique_ptr<WeexGlobalObject> &nativeObject,
                                                                 const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST(" method :setNativeInterval");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues WeexBindingUtils::clearNativeTimeout(const std::unique_ptr<WeexGlobalObject> &nativeObject,
                                                                  const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST(" method :clearNativeTimeout");
            return unicorn::RuntimeValues::MakeUndefined();
        }

        unicorn::ScopeValues
        WeexBindingUtils::clearNativeInterval(const std::unique_ptr<WeexGlobalObject> &nativeObject,
                                              const std::vector<unicorn::ScopeValues> &vars) {
            LOG_TEST(" method :clearNativeInterval");
            return unicorn::RuntimeValues::MakeUndefined();
        }
    }
}
