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
// Created by furture on 2018/5/15.
//

#include "core/render/node/render_object.h"
#include "core/render/page/render_page.h"
#include "core/render/node/factory/render_creator.h"
#include "dom_wson.h"
#include "wson/wson.h"
#include "wson/wson_parser.h"
#include "base/utils/log_utils.h"

namespace WeexCore {



    inline bool  keys_order_as_expect(RenderObject *render, bool keyOrderRight){
        return (render != nullptr && keyOrderRight);
    }


    /**
     * parser wson to render object
     * */
    RenderObject *parserWson2RenderObject(wson_parser& parser, RenderObject *parent, int index, const std::string &pageId){
        int objectType = parser.nextType();
        if(!parser.isMap(objectType)){
            LOGE("[dom_wson],parser is not Map !!!!!!!!!");
            parser.skipValue(objectType);
            return nullptr;
        }
        /**
         * because strem key order specified, so will cann't dependecy it's keys order,
         * if key orders right parse one time, if not first parse ref type create render object
         * then parse others attrs
         * */
        int size = parser.nextMapSize();
        LOGW("[dom_wson],parser map size %d",size);
        std::string ref;
        std::string renderType;
        RenderObject *render = nullptr;
        int state = parser.getState();
        bool keyOrderRight = true;
        for(int i=0; i < size; i++){
            std::string objectKey = parser.nextMapKeyUTF8();
            LOGW("[dom_wson][for-item],objectKey :%s",objectKey.c_str());
            if(0 == strcmp(objectKey.c_str(), "ref")){
                 ref = parser.nextStringUTF8(parser.nextType());
                LOGW("[dom_wson][ref],parser value:%s",ref.c_str());
                if (render != nullptr) {
                    // ref may be after type, so need set to render
                    render->set_ref(ref);
                }
            }else if (0 == strcmp(objectKey.c_str(), "type")) {
                renderType = parser.nextStringUTF8(parser.nextType());
                LOGW("[dom_wson][type], value:%s",renderType.c_str());
                render = (RenderObject *) RenderCreator::GetInstance()->CreateRender(renderType, ref);
                render->set_page_id(pageId);
                if (parent != nullptr){
                    parent->AddRenderObject(index, render);
                }
            }else if (0 == strcmp(objectKey.c_str(), "attr")){ //attr is map object
                uint8_t attrType = parser.nextType();
                LOGW("[dom_wson][attr][pre],attrType :%d",attrType);
                if(parser.isMap(attrType) && keys_order_as_expect(render, keyOrderRight)){
                    int attrMapSize = parser.nextMapSize();
                    LOGW("[dom_wson][attr][start],parser size:%d",attrMapSize);
                    for(int attrIndex=0; attrIndex<attrMapSize; attrIndex++){
                        std::string attrKeyString = parser.nextMapKeyUTF8();
                        std::string attrValueString = parser.nextStringUTF8(parser.nextType());
                        LOGW("[dom_wson][attr][item],key:%s,value:%s",attrKeyString.c_str(),attrValueString.c_str());
                        render->AddAttr(attrKeyString, attrValueString);
                    }
                    LOGW("[dom_wson][attr][end] parser item ,size:%d",attrMapSize);
                }else{
                    LOGW("[dom_wson][attr][skip] skipValue");
                    keyOrderRight = keys_order_as_expect(render, keyOrderRight);
                    parser.skipValue(attrType);
                }
            }else if (0 == strcmp(objectKey.c_str(), "style")){ //style is map object
                uint8_t styleType = parser.nextType();
                LOGW("[dom_wson][style][pre],styleType :%d",styleType);
                if(parser.isMap(styleType) && keys_order_as_expect(render, keyOrderRight)){
                    int styleMapSize = parser.nextMapSize();
                    LOGW("[dom_wson][style][start],parser size:%d",styleMapSize);
                    for(int styleIndex=0; styleIndex<styleMapSize; styleIndex++){
                        std::string styleKeyString = parser.nextMapKeyUTF8();
                        std::string styleValueString = parser.nextStringUTF8(parser.nextType());
                        LOGW("[dom_wson][style][item],key:%s,value:%s",styleKeyString.c_str(),styleValueString.c_str());
                        render->AddStyle(styleKeyString, styleValueString);
                    }
                    LOGW("[dom_wson][style][end] size:%d",styleMapSize);
                }else{
                    LOGW("[dom_wson][style][skip] skipValue");
                    keyOrderRight = keys_order_as_expect(render, keyOrderRight);
                    parser.skipValue(styleType);
                }
            }else if (0 == strcmp(objectKey.c_str(), "event")) {//event is array
                uint8_t  eventType = parser.nextType();
                LOGW("[dom_wson][style][pre],eventType :%d",eventType);
                if(parser.isArray(eventType) && keys_order_as_expect(render, keyOrderRight)){
                    int eventSize = parser.nextArraySize();
                    LOGW("[dom_wson][event][start],parser size:%d",eventSize);
                    for(int eventIndex=0; eventIndex < eventSize; eventIndex++){
                        std::string eventValue = parser.nextStringUTF8(parser.nextType());
                        LOGW("[dom_wson][event][item], value:%s",eventValue.c_str());
                        if(eventValue.size() > 0){
                            render->AddEvent(eventValue);
                        }
                    }
                    LOGW("[dom_wson][event][end],parser size:%d",eventSize);
                }else{
                    LOGW("[dom_wson][event][skip]");
                    keyOrderRight = keys_order_as_expect(render, keyOrderRight);
                    parser.skipValue(eventType);
                }
            }else if (0 == strcmp(objectKey.c_str(), "children")) {
                uint8_t  childType = parser.nextType();
                LOGW("[dom_wson][children][pre],childType :%d",childType);
                if(parser.isArray(childType) && keys_order_as_expect(render, keyOrderRight)){
                    int childSize = parser.nextArraySize();
                    LOGW("[dom_wson][children][start],size :%d",childSize);
                    for(int childIndex=0; childIndex < childSize; childIndex++){
                        parserWson2RenderObject(parser, render, childIndex, pageId);
                    }
                    LOGW("[dom_wson][children][end],size :%d",childSize);
                }else{
                    LOGW("[dom_wson][children][skip]");
                    keyOrderRight = keys_order_as_expect(render, keyOrderRight);
                    parser.skipValue(childType);
                }
            }else{
                LOGW("[dom_wson][else][skip]");
                parser.skipValue(parser.nextType());
            }
        }

        /**
         * if key order not right, parse attr&style  events children again
         * */
        if(!keyOrderRight && render != nullptr){
            parser.restoreToState(state);
            for(int i=0; i < size; i++){
                std::string objectKey = parser.nextMapKeyUTF8();
                if (0 == strcmp(objectKey.c_str(), "attr")){ //attr is map object
                    uint8_t attrType = parser.nextType();
                    if(parser.isMap(attrType)){
                        int attrMapSize = parser.nextMapSize();
                        for(int attrIndex=0; attrIndex<attrMapSize; attrIndex++){
                            std::string attrKeyString = parser.nextMapKeyUTF8();
                            std::string attrValueString = parser.nextStringUTF8(parser.nextType());
                            render->AddAttr(attrKeyString, attrValueString);
                        }
                    }else{
                        parser.skipValue(attrType);
                    }
                }else if (0 == strcmp(objectKey.c_str(), "style")){ //style is map object
                    uint8_t styleType = parser.nextType();
                    if(parser.isMap(styleType)){
                        int styleMapSize = parser.nextMapSize();
                        for(int styleIndex=0; styleIndex<styleMapSize; styleIndex++){
                            std::string styleKeyString = parser.nextMapKeyUTF8();
                            std::string styleValueString = parser.nextStringUTF8(parser.nextType());
                            render->AddStyle(styleKeyString, styleValueString);
                        }
                    }else{
                        parser.skipValue(styleType);
                    }
                }else if (0 == strcmp(objectKey.c_str(), "event")) {//event is array
                    uint8_t  eventType = parser.nextType();
                    if(parser.isArray(eventType)){
                        int eventSize = parser.nextArraySize();
                        for(int eventIndex=0; eventIndex < eventSize; eventIndex++){
                            std::string eventValue = parser.nextStringUTF8(parser.nextType());
                            if(eventValue.size() > 0){
                                render->AddEvent(eventValue);
                            }
                        }
                    }else{;
                        parser.skipValue(eventType);
                    }
                }else if (0 == strcmp(objectKey.c_str(), "children")) {
                    uint8_t  childType = parser.nextType();
                    if(parser.isArray(childType)){
                        int childSize = parser.nextArraySize();
                        for(int childIndex=0; childIndex < childSize; childIndex++){
                            parserWson2RenderObject(parser, render, childIndex, pageId);
                        }
                    }else{
                        parser.skipValue(childType);
                    }
                }else{
                    parser.skipValue(parser.nextType());
                }
            }
        }


        if (render != nullptr) {
            render->ApplyDefaultStyle();
            render->ApplyDefaultAttr();
        }
        return render;
    }


    RenderObject *Wson2RenderObject(const char *data, const std::string &pageId){
        if(!data){
            return nullptr;
        }
        wson_parser parser(data);
        return parserWson2RenderObject(parser, nullptr, 0, pageId);
    }

    std::vector<std::pair<std::string, std::string>> *Wson2Pairs(const char *data){
        if(!data){
            return nullptr;
        }
        wson_parser parser(data);
        std::vector<std::pair<std::string, std::string>> *pairs = nullptr;
        uint8_t  type = parser.nextType();
        if(parser.isMap(type)){
            pairs = new std::vector<std::pair<std::string, std::string>>();
            int mapSize = parser.nextMapSize();
            for(int index=0; index < mapSize; index++){
                std::string mapKeyString = parser.nextMapKeyUTF8();
                std::string mapValueString = parser.nextStringUTF8(parser.nextType());
                std::pair<std::string, std::string> mapPair(mapKeyString, mapValueString);
                pairs->insert(pairs->end(), mapPair);
            }
        }
        return pairs;
    };

}
