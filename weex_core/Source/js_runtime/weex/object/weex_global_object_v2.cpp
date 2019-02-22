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

#include "weex_global_object_v2.h"

void WeexGlobalObjectV2::makeWeexGlobalObject() {

}

void WeexGlobalObjectV2::makeWeexInstanceObject(const std::string &id, const std::string &name) {

}

void WeexGlobalObjectV2::makeAppWorkerObject() {

}

void WeexGlobalObjectV2::addExtraOptions(std::vector<INIT_FRAMEWORK_PARAMS *> &params) {

}

void
WeexGlobalObjectV2::initWxEnvironment(std::vector<INIT_FRAMEWORK_PARAMS *> &params, bool forAppContext, bool isSave) {

}

void WeexGlobalObjectV2::initFunctionForContext() {

}

void WeexGlobalObjectV2::initFunctionForAppContext() {

}

void WeexGlobalObjectV2::initFunction() {

}

void WeexGlobalObjectV2::setScriptBridge(WeexCore::ScriptBridge *script_bridge) {

}


void WeexGlobalObjectV2::removeTimer(uint32_t function_id) {

}

uint32_t WeexGlobalObjectV2::genFunctionID() {
    return 0;
}

unicorn::Function *WeexGlobalObjectV2::getTimerFunction(uint32_t function_id) {
    return NULL;
}

void WeexGlobalObjectV2::addTimer(uint32_t function_id, unicorn::ScopeValues func) {

}

