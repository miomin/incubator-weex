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
// Created by 陈佩翰 on 2019/3/8.
//

#include "action_args_check.h"
#include "third_party/json11/json11.hpp"
#include "base/utils/log_utils.h"

namespace WeexCore {
    bool isCallNativeToFinish(const char *task) {
        if (nullptr == task) {
            return false;
        }
        std::string task_str(task);
        LOGE("CoreSideInScript : CallNative taskLength:%d. task:%s", task_str.length(), task);
        if (task_str.length() == 57) {
            LOGE("CoreSideInScript :111");
            std::string errr;
            json11::Json task_json = json11::Json::parse(task_str, errr);
            LOGE("CoreSideInScript :222");
            if (task_json.is_array()) {
                LOGE("CoreSideInScript :333");
                auto array = task_json.array_items();
                if (array.size() >= 0) {
                    LOGE("CoreSideInScript :444");
                    auto item = array[0];
                    if (item.is_object()) {
                        LOGE("CoreSideInScript :555");
                        auto module_value = item["module"];
                        auto method_value = item["method"];
                        auto args_value = item["args"];
                        if (module_value.is_string() && method_value.is_string() && args_value.is_array()) {
                            std::string module_str = module_value.dump();
                            std::string method_str = method_value.dump();
                            int size = args_value.array_items().size();
                            LOGE("CoreSideInScript :666 module:%s,method:%s, argsize:%d", module_str.c_str(),
                                 method_str.c_str(), size);
                            if (module_str == "\"dom\"") {
                                LOGE("CoreSideInScript :777");
                                if (method_str == std::string("\"createFinish\"")) {
                                    LOGE("CoreSideInScript :888");
                                    if (args_value.array_items().size() <= 0) {
                                        LOGE("CoreSideInScript :999");
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
