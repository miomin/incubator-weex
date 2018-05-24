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
// Created by 陈佩翰 on 2018/5/21.
//

#ifndef WEEXV8_WX_CORE_MANAGER_H
#define WEEXV8_WX_CORE_MANAGER_H


#include <core/bridge/bridge.h>
#include <core/bridge/js_bridge.h>
//#include <core/bridge/platform_bridge.h>


namespace WeexCore {
    class WXCoreManager {
    public:

        static WXCoreManager *getInstance() {
            static WXCoreManager s_instance;
            return &s_instance;
        }

        Bridge *getPlatformBridge() {
            return platform_bridge;
        }

        JSBridge *getJSBridge() {
            return js_bridge;
        }

        WXCoreManager *setJSBridge(JSBridge *bridge) {
           js_bridge = bridge;
        }

        WXCoreManager *setPlatformBridge(Bridge *bridge) {
           platform_bridge = bridge;
        }

    protected:
        WXCoreManager() {};

        ~WXCoreManager() {};

    private:
        Bridge *platform_bridge;
        JSBridge *js_bridge;

        //todo core_bridge

    };


}

#endif //WEEXV8_WX_CORE_MANAGER_H
