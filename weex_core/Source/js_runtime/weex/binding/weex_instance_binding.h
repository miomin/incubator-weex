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

#ifndef PROJECT_WEEX_INSTANCE_OBJECT_H
#define PROJECT_WEEX_INSTANCE_OBJECT_H

#include "js_runtime/runtime/runtime_object.h"
#include "js_runtime/runtime/binding_macro.h"

class WeexGlobalObject;

namespace weex {
    namespace jsengine {


        class WeexInstanceBinding : public unicorn::RuntimeObject {
        public:
            DECLARE_CLASS_REGISTER_OP(WeexInstanceBinding)

            WeexInstanceBinding(unicorn::EngineContext *context, const OpaqueJSContext *js_ctx);

            ~WeexInstanceBinding() override;

            unicorn::ScopeValues nativeLog( const std::vector<unicorn::ScopeValues> &vars);

            unicorn::ScopeValues atob( const std::vector<unicorn::ScopeValues> &vars);

            unicorn::ScopeValues btoa( const std::vector<unicorn::ScopeValues> &vars);

            unicorn::ScopeValues
            callGCanvasLinkNative( const std::vector<unicorn::ScopeValues> &vars);

            unicorn::ScopeValues
            callT3DLinkNative( const std::vector<unicorn::ScopeValues> &vars);

            unicorn::ScopeValues
            setNativeTimeout( const std::vector<unicorn::ScopeValues> &vars);

            unicorn::ScopeValues
            setNativeInterval( const std::vector<unicorn::ScopeValues> &vars);

            unicorn::ScopeValues
            clearNativeTimeout( const std::vector<unicorn::ScopeValues> &vars);

            unicorn::ScopeValues
            clearNativeInterval( const std::vector<unicorn::ScopeValues> &vars);

            unicorn::ScopeValues console( const std::vector<unicorn::ScopeValues> &vars);

            unicorn::ScopeValues
            __updateComponentData( const std::vector<unicorn::ScopeValues> &vars);

        public:
            std::unique_ptr<WeexGlobalObject> nativeObject;
        };
    }
}
#endif //PROJECT_WEEX_INSTANCE_OBJECT_H
