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
// Created by 陈佩翰 on 2019/2/25.
//

#include <include/wtf/icu/unicode/utf.h>
#include <include/wtf/text/WTFString.h>
#include "wson/wson.h"
#include "wson/wson_parser.h"

#include "wson_for_runtime.h"


//#ifdef log_test

#include "js_runtime/utils/log_utils.h"

//#else
//#define LOGW
//#define LOGE
//#endif


namespace wson {

    unicorn::RuntimeValues *
    convertWsonToRuntimeValue(unicorn::EngineContext *context, wson_buffer *buffer) {
        uint8_t type = wson_next_type(buffer);
        switch (type) {
            case WSON_UINT8_STRING_TYPE: {
                LOGW("[toRunTimeValueFromWson][string][start]");
                int size = wson_next_uint(buffer);
                uint8_t *utf8 = wson_next_bts(buffer, size);
                std::string string_utf_8 = std::string(reinterpret_cast<char *>(utf8));
                LOGW("[toRunTimeValueFromWson][string][end] :%s", string_utf_8.c_str());
                return new unicorn::RuntimeValues(string_utf_8);
            }
            case WSON_STRING_TYPE:
            case WSON_NUMBER_BIG_INT_TYPE:
            case WSON_NUMBER_BIG_DECIMAL_TYPE: {
                LOGW("[toRunTimeValueFromWson][string_utf_16][start]");
                uint32_t length = wson_next_uint(buffer);
                UChar *destination;
                WTF::String s = WTF::String::createUninitialized(length / sizeof(UChar), destination);
                void *src = wson_next_bts(buffer, length);
                memcpy(destination, src, length);
                std::string string_utf_8 = std::string(s.utf8().data());
                LOGW("[toRunTimeValueFromWson][string_utf_16][end] :%s", s.utf8().data());
                return new unicorn::RuntimeValues(string_utf_8);
            }
                break;
            case WSON_ARRAY_TYPE: {
                LOGW("[toRunTimeValueFromWson][array][start]");
                uint32_t length = wson_next_uint(buffer);
                auto runtime_array = unicorn::Array::CreateFromNative(context, unicorn::RuntimeValues::MakeNull());
                for (uint32_t i = 0; i < length; i++) {
                    if (wson_has_next(buffer)) {
                        runtime_array->PushBack(convertWsonToRuntimeValue(context, buffer));
                    } else {
                        break;
                    }
                }
                LOGW("[toRunTimeValueFromWson][array][end]");
                return new unicorn::RuntimeValues(std::move(runtime_array));
            }
                break;
            case WSON_MAP_TYPE: {
                LOGW("[toRunTimeValueFromWson][map][start]");
                uint32_t length = wson_next_uint(buffer);
                std::unique_ptr<unicorn::Map> runtime_map = unicorn::Map::CreateFromNative(context,
                                                                                           unicorn::RuntimeValues::MakeNull());
                for (uint32_t i = 0; i < length; i++) {
                    if (wson_has_next(buffer)) {
                        int propertyLength = wson_next_uint(buffer);
                        UChar *destination;
                        WTF::String name_utf_16 = WTF::String::createUninitialized(propertyLength / sizeof(UChar),
                                                                                   destination);
                        void *name = wson_next_bts(buffer, propertyLength);
                        memcpy(destination, name, propertyLength);
                        std::string name_utf_8 = std::string(name_utf_16.utf8().data());
                        LOGW("[toRunTimeValueFromWson][map][itemkey] :%s", name_utf_8.c_str());
                        runtime_map->Insert(name_utf_8, convertWsonToRuntimeValue(context, buffer));

                    } else {
                        break;
                    }
                }
                LOGW("[toRunTimeValueFromWson][map][end]");
                return new unicorn::RuntimeValues(std::move(runtime_map));
            }
                break;
            case WSON_NUMBER_INT_TYPE: {
                int32_t num = wson_next_int(buffer);
                return new unicorn::RuntimeValues(num);
            }
                break;
            case WSON_BOOLEAN_TYPE_TRUE: {
                return new unicorn::RuntimeValues(true);
            }
                break;
            case WSON_BOOLEAN_TYPE_FALSE: {
                return new unicorn::RuntimeValues(false);
            }
                break;
            case WSON_NUMBER_DOUBLE_TYPE: {
                double num = wson_next_double(buffer);
                return new unicorn::RuntimeValues(num);
            }
                break;
            case WSON_NUMBER_FLOAT_TYPE: {
                float num = wson_next_float(buffer);
                return new unicorn::RuntimeValues(num);
            }
                break;
            case WSON_NUMBER_LONG_TYPE: {
                int64_t num = wson_next_long(buffer);
                return new unicorn::RuntimeValues((double) num);
            }
                break;
            case WSON_NULL_TYPE: {
                return new unicorn::RuntimeValues(nullptr);
            }
            default: {
                LOGE("[WsonToRuntimeValue] unsupport data type in default case!!");
                return new unicorn::RuntimeValues(nullptr);
            }
        }
    }

    unicorn::ScopeValues toRunTimeValueFromWson(unicorn::EngineContext *context, void *data, int length) {
        wson_buffer *buffer = wson_buffer_from(data, length);
        auto ret = convertWsonToRuntimeValue(context, buffer);

        wson_parser parser((char *) buffer->data);
        LOGE("[WeexValueToRuntimeValue][wson] :%s", parser.toStringUTF8().c_str());



        buffer->data = nullptr;
        wson_buffer_free(buffer);
        return unicorn::ScopeValues(ret);
    }

    void pushStringToWsonBuffer(wson_buffer *buffer, std::string str_utf_8) {
        WTF::String wtf_str = WTF::String::fromUTF8(str_utf_8.c_str());

        size_t length = wtf_str.length();
        if (wtf_str.is8Bit()) {
            // Convert latin1 chars to unicode.
            wson_push_type(buffer, WSON_STRING_TYPE);
            wson_push_uint(buffer, length * sizeof(UChar));
            wson_buffer_require(buffer, length * sizeof(UChar));
            UChar *jchars = (UChar *) ((uint8_t *) buffer->data + buffer->position);
            for (unsigned i = 0; i < length; i++) {
//#ifdef __ANDROID__
//                    jchars[i] = s.at(i);
//#else
                jchars[i] = wtf_str.characterAt(i);
//#endif
            }
            buffer->position += length * sizeof(UChar);
        } else {
            wson_push_type(buffer, WSON_STRING_TYPE);
            wson_push_uint(buffer, length * sizeof(UChar));
            wson_push_bytes(buffer, wtf_str.characters16(), wtf_str.length() * sizeof(UChar));
        }
    }

    void pushMapKeyToBuffer(wson_buffer *buffer, std::string str_utf_8) {
        WTF::String wtf_str = WTF::String::fromUTF8(str_utf_8.c_str());

        size_t length = wtf_str.length();
        if (wtf_str.is8Bit()) {
            // Convert latin1 chars to unicode.
            wson_push_uint(buffer, length * sizeof(UChar));
            wson_buffer_require(buffer, length * sizeof(UChar));
            UChar *jchars = (UChar *) ((uint8_t *) buffer->data + buffer->position);
            for (unsigned i = 0; i < length; i++) {
//#ifdef __ANDROID__
//                    jchars[i] = s.at(i);
//#else
                jchars[i] = wtf_str.characterAt(i);
//#endif
            }
            buffer->position += length * sizeof(UChar);
        } else {
            wson_push_uint(buffer, length * sizeof(UChar));
            wson_push_bytes(buffer, wtf_str.characters16(), wtf_str.length() * sizeof(UChar));
        }
    }


    void putValuesToWson(unicorn::RuntimeValues *value, wson_buffer *buffer) {
        if (value->IsUndefined() || value->IsNull()) {
            wson_push_type_null(buffer);
        } else if (value->IsInt()) {
            int num = -1;
            value->GetAsInteger(&num);
            wson_push_type_int(buffer, num);
        } else if (value->IsDouble()) {
            double num = -1;
            value->GetAsDouble(&num);
            wson_push_type_double(buffer, num);
        } else if (value->IsBool()) {
            bool flag = false;
            value->GetAsBoolean(&flag);
            wson_push_type_boolean(buffer, flag ? 1 : 0);
        } else if (value->IsString()) {
            std::string str_utf_8;
            value->GetAsString(&str_utf_8);
            pushStringToWsonBuffer(buffer, str_utf_8);
        } else if (value->IsArray()) {
            auto array = value->GetAsArray();
            uint32_t length = array->Size();
            wson_push_type_array(buffer, length);
            for (uint32_t i = 0; i < length; i++) {
                auto item = array->atIndex(i);
                putValuesToWson(item, buffer);
            }
        } else if (value->IsMap()) {
            auto map = value->GetAsMap()->GetMap();
            uint32_t map_size = map.size();
            uint32_t undefinedOrFunctionSize = 0;
            for (auto item:map) {
                if (item.second->IsUndefined() || item.second->IsNull() || item.second->IsFunction() ||
                    item.second->IsObject()) {
                    undefinedOrFunctionSize++;
                    LOGE("[wson]putValuesToWson data type not match ,type :%d ", item.second->GetType());
                }
            }
            wson_push_type_map(buffer, map_size - undefinedOrFunctionSize);
            for (auto item:map) {
                if (item.second->IsUndefined() || item.second->IsNull() || item.second->IsFunction() ||
                    item.second->IsObject()) {
                    continue;
                }
                pushMapKeyToBuffer(buffer, item.first);
                putValuesToWson(item.second, buffer);
            }
        } else {
            LOGE("[wson][else] putValuesToWson data type not match ,type :%d ", value->GetType());
        }
    }


    wson_buffer *runTimeValueToWson(unicorn::RuntimeValues *value) {
        wson_buffer *buffer = wson_buffer_new();
        putValuesToWson(value, buffer);
        return buffer;
    }
}