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

#include "weex_runtime_v2.h"

WeexRuntimeV2::WeexRuntimeV2(TimerQueue *timeQueue, bool isMultiProgress)
        : WeexRuntime() {

}

WeexRuntimeV2::WeexRuntimeV2(TimerQueue *timeQueue, WeexCore::ScriptBridge *script_bridge, bool isMultiProgress)
        : WeexRuntime() {

}

bool WeexRuntimeV2::hasInstanceId(String &id) {
    return WeexRuntime::hasInstanceId(id);
}

int WeexRuntimeV2::initFramework(const String &script, std::vector<INIT_FRAMEWORK_PARAMS *> &params) {
    return WeexRuntime::initFramework(script, params);
}

int WeexRuntimeV2::initFramework(IPCArguments *arguments) {
    return WeexRuntime::initFramework(arguments);
}

int WeexRuntimeV2::initAppFrameworkMultiProcess(const String &instanceId, const String &appFramework,
                                                IPCArguments *arguments) {
    return WeexRuntime::initAppFrameworkMultiProcess(instanceId, appFramework, arguments);
}

int WeexRuntimeV2::initAppFramework(const String &instanceId, const String &appFramework,
                                    std::vector<INIT_FRAMEWORK_PARAMS *> &params) {
    return WeexRuntime::initAppFramework(instanceId, appFramework, params);
}

int WeexRuntimeV2::createAppContext(const String &instanceId, const String &jsBundle) {
    return WeexRuntime::createAppContext(instanceId, jsBundle);
}

std::unique_ptr<WeexJSResult> WeexRuntimeV2::exeJSOnAppWithResult(const String &instanceId, const String &jsBundle) {
    return WeexRuntime::exeJSOnAppWithResult(instanceId, jsBundle);
}

int WeexRuntimeV2::callJSOnAppContext(IPCArguments *arguments) {
    return WeexRuntime::callJSOnAppContext(arguments);
}

int WeexRuntimeV2::callJSOnAppContext(const String &instanceId, const String &func,
                                      std::vector<VALUE_WITH_TYPE *> &params) {
    return WeexRuntime::callJSOnAppContext(instanceId, func, params);
}

int WeexRuntimeV2::destroyAppContext(const String &instanceId) {
    return WeexRuntime::destroyAppContext(instanceId);
}

int WeexRuntimeV2::exeJsService(const String &source) {
    return WeexRuntime::exeJsService(source);
}

int WeexRuntimeV2::exeCTimeCallback(const String &source) {
    return WeexRuntime::exeCTimeCallback(source);
}

int WeexRuntimeV2::exeJS(const String &instanceId, const String &nameSpace, const String &func,
                         std::vector<VALUE_WITH_TYPE *> &params) {
    return WeexRuntime::exeJS(instanceId, nameSpace, func, params);
}

std::unique_ptr<WeexJSResult>
WeexRuntimeV2::exeJSWithResult(const String &instanceId, const String &nameSpace, const String &func,
                               std::vector<VALUE_WITH_TYPE *> &params) {
    return WeexRuntime::exeJSWithResult(instanceId, nameSpace, func, params);
}

void WeexRuntimeV2::exeJSWithCallback(const String &instanceId, const String &nameSpace, const String &func,
                                      std::vector<VALUE_WITH_TYPE *> &params, long callback_id) {
    WeexRuntime::exeJSWithCallback(instanceId, nameSpace, func, params, callback_id);
}

int
WeexRuntimeV2::createInstance(const String &instanceId, const String &func, const String &script, const String &opts,
                              const String &initData, const String &extendsApi,
                              std::vector<INIT_FRAMEWORK_PARAMS *> &params) {
    return WeexRuntime::createInstance(instanceId, func, script, opts, initData, extendsApi, params);
}

std::unique_ptr<WeexJSResult> WeexRuntimeV2::exeJSOnInstance(const String &instanceId, const String &script) {
    return WeexRuntime::exeJSOnInstance(instanceId, script);
}

int WeexRuntimeV2::destroyInstance(const String &instanceId) {
    return WeexRuntime::destroyInstance(instanceId);
}

int WeexRuntimeV2::updateGlobalConfig(const String &config) {
    return WeexRuntime::updateGlobalConfig(config);
}

int WeexRuntimeV2::exeTimerFunction(const String &instanceId, uint32_t timerFunction, JSGlobalObject *globalObject) {
    return WeexRuntime::exeTimerFunction(instanceId, timerFunction, globalObject);
}

WeexObjectHolder *WeexRuntimeV2::getLightAppObjectHolder(const String &instanceId) {
    return WeexRuntime::getLightAppObjectHolder(instanceId);
}

void WeexRuntimeV2::removeTimerFunction(const uint32_t timerFunction, JSGlobalObject *globalObject) {
    WeexRuntime::removeTimerFunction(timerFunction, globalObject);
}
