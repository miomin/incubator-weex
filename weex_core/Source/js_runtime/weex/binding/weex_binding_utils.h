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

#ifndef PROJECT_WEEX_BINDING_UTILS_H
#define PROJECT_WEEX_BINDING_UTILS_H

#include <js_runtime/runtime/runtime_values.h>

class WeexGlobalObjectV2;

namespace weex {
    namespace jsengine {
        class WeexBindingUtils {
        public:
            static unicorn::ScopeValues
            atob(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                 const std::vector<unicorn::ScopeValues> &vars);

            static unicorn::ScopeValues
            btoa(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                 const std::vector<unicorn::ScopeValues> &vars);

            static unicorn::ScopeValues
            nativeLog(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                      const std::vector<unicorn::ScopeValues> &vars, bool toCoreSide);

            static unicorn::ScopeValues
            setNativeTimeout(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                             std::vector<unicorn::ScopeValues> &vars, bool interval);

            static unicorn::ScopeValues
            clearNativeTimeout(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                               std::vector<unicorn::ScopeValues> &vars);

            static unicorn::ScopeValues callT3DLinkNative(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                                                          const std::vector<unicorn::ScopeValues> &vars);


            static unicorn::ScopeValues callGCanvasLinkNative(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                                                              const std::vector<unicorn::ScopeValues> &vars);


            static unicorn::ScopeValues __updateComponentData(const std::unique_ptr<WeexGlobalObjectV2> &nativeObject,
                                                              const std::vector<unicorn::ScopeValues> &vars);

        };
    }
}


#endif //PROJECT_WEEX_BINDING_UTILS_H
