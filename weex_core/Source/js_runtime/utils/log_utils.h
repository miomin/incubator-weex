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
// Created by 陈佩翰 on 2019/1/17.
//

#ifndef WEEXCORE_LOG_UTILS_H
#define WEEXCORE_LOG_UTILS_H

#ifndef CLOSE_RUN_TIME_LOG
#include "base/utils/log_utils.h"

#define LOG_TEST(...) LOGW(__VA_ARGS__)
#define LOG_JS(...)
#define LOG_JS_ERROR(...) LOGE(__VA_ARGS__)

#else
#define LOG_TEST(...)
#define LOG_JS(...)
#endif




#endif //WEEXCORE_LOG_UTILS_H
