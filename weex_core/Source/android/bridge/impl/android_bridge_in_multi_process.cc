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

#include "android/bridge/impl/android_bridge_in_multi_process.h"
#include <android/bridge/impl/android_side.h>
#include "core/layout/layout.h"
#include "core/manager/weex_core_manager.h"
#include "core/render/manager/render_manager.h"
#include "wson/wson_parser.h"

#include "IPC/Android/IPCStringResult.h"
#include "IPC/Buffering/IPCBuffer.h"
#include "IPC/IPCArguments.h"
#include "IPC/IPCByteArray.h"
#include "IPC/IPCHandler.h"
#include "IPC/IPCMessageJS.h"
#include "IPC/IPCResult.h"
#include "IPC/IPCSender.h"
#include "IPC/Serializing/IPCSerializer.h"

#include "android/base/string/string_utils.h"
#include "core_side_in_multi_process.h"
#include "include/WeexApiHeader.h"
#include "multi_process_and_so_initializer.h"

namespace WeexCore {

AndroidBridgeInMultiProcess::AndroidBridgeInMultiProcess() {
  set_platform_side(new AndroidSide);
  set_core_side(new CoreSideInMultiProcess);

  std::unique_ptr<MultiProcessAndSoInitializer> initializer(
      new MultiProcessAndSoInitializer);
  LOGE("AndroidBridgeInMultiProcess");
  bool passable = initializer->Init(
      [this](IPCHandler* handler) {
//        RegisterIPCCallback(handler);
        },
      [this](std::unique_ptr<WeexJSConnection> connection,
             std::unique_ptr<IPCHandler> handler) {
        static_cast<CoreSideInMultiProcess*>(core_side())
            ->set_sender(connection->sender());
        connection_ = std::move(connection);
        handler_ = std::move(handler);
        return true;
      },
      [this](const char* page_id, const char* func,
             const char* exception_string) {
        platform_side()->ReportException(page_id, func, exception_string);
      });
  set_is_passable(passable);
}

AndroidBridgeInMultiProcess::~AndroidBridgeInMultiProcess() {}

void AndroidBridgeInMultiProcess::RegisterIPCCallback(IPCHandler* handler) {
  LOGE("RegisterIPCCallback AND handler is %x",handler);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::INVOKE_MEASURE_FUNCTION),
      InvokeMeasureFunction);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::INVOKE_LAYOUT_BEFORE),
      InvokeLayoutBefore);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::INVOKE_LAYOUT_AFTER),
      InvokeLayoutAfter);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::SET_JS_VERSION),
      SetJSVersion);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::REPORT_EXCEPTION),
      ReportException);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::CALL_NATIVE), CallNative);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::CALL_NATIVE_MODULE),
      CallNativeModule);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::CALL_NATIVE_COMPONENT),
      CallNativeComponent);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::SET_TIMEOUT), SetTimeout);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::NATIVE_LOG), NativeLog);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::UPDATE_FINISH),
      UpdateFinish);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::REFRESH_FINISH),
      RefreshFinish);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::ADD_EVENT), AddEvent);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::REMOVE_EVENT),
      RemoveEvent);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::CREATE_BODY), CreateBody);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::ADD_ELEMENT), AddElement);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::LAYOUT), Layout);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::UPDATE_STYLE),
      UpdateStyle);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::UPDATE_ATTR), UpdateAttr);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::CREATE_FINISH),
      CreateFinish);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::REMOVE_ELEMENT),
      RemoveElement);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::MOVE_ELEMENT),
      MoveElement);
  handler->registerHandler(
      static_cast<uint32_t>(
          IPCMsgFromCoreToPlatform::APPEND_TREE_CREATE_FINISH),
      AppendTreeCreateFinish);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::HAS_TRANSITION_PROS),
      HasTransitionPros);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::POST_MESSAGE),
      PostMessage);
  handler->registerHandler(
      static_cast<uint32_t>(IPCMsgFromCoreToPlatform::DISPATCH_MESSAGE),
      DispatchMessage);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::InvokeMeasureFunction(
    IPCArguments* arguments) {
  auto page_id = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 0));
  int64_t render_ptr = getArgumentAsInt64(arguments, 1);
  float width = getArgumentAsFloat(arguments, 2);
  int32_t width_measure_mode = getArgumentAsInt32(arguments, 3);
  float height = getArgumentAsFloat(arguments, 4);
  int32_t height_measure_mode = getArgumentAsInt32(arguments, 5);

  WXCoreSize size = WeexCoreManager::getInstance()
                        ->getPlatformBridge()
                        ->platform_side()
                        ->InvokeMeasureFunction(page_id.get(), render_ptr,
                                                width, width_measure_mode,
                                                height, height_measure_mode);

  float result[]{size.width, size.height};

  return createByteArrayResult(reinterpret_cast<const char*>(result),
                               2 * sizeof(float));
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::InvokeLayoutBefore(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  int64_t render_ptr = getArgumentAsInt64(arguments, 1);

  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->platform_side()
      ->InvokeLayoutBefore(page_id, render_ptr);

  delete[] page_id;
  return createVoidResult();
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::InvokeLayoutAfter(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  int64_t render_ptr = getArgumentAsInt64(arguments, 1);
  float width = getArgumentAsFloat(arguments, 2);
  float height = getArgumentAsFloat(arguments, 3);

  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->platform_side()
      ->InvokeLayoutAfter(page_id, render_ptr, width, height);

  delete[] page_id;
  return createVoidResult();
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::SetJSVersion(
    IPCArguments* arguments) {
  const char* version = getArumentAsCStr(arguments, 0);

  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->platform_side()
      ->SetJSVersion(version);

  delete[] version;
  return createVoidResult();
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::ReportException(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* func = getArumentAsCStr(arguments, 1);
  const char* exception = getArumentAsCStr(arguments, 2);

  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->platform_side()
      ->ReportException(page_id, func, exception);

  delete[] page_id;
  delete[] func;
  delete[] exception;
  return createVoidResult();
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::CallNative(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* task = getArumentAsCStr(arguments, 1);
  const char* callback = getArumentAsCStr(arguments, 2);

  int result = WeexCoreManager::getInstance()
                   ->getPlatformBridge()
                   ->platform_side()
                   ->CallNative(page_id, task, callback);

  delete[] page_id;
  delete[] task;
  delete[] callback;
  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::CallNativeModule(
    IPCArguments* arguments) {
  auto page_id = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 0));
  auto module = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 1));
  auto method = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 2));
  auto arguments_data = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 3));
  int arguments_data_length = getArumentAsCStrLen(arguments, 3);
  auto options = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 4));
  int options_length = getArumentAsCStrLen(arguments, 4);

  auto result =
      WeexCoreManager::getInstance()
          ->getPlatformBridge()
          ->platform_side()
          ->CallNativeModule(page_id.get(), module.get(), method.get(),
                             arguments_data.get(), arguments_data_length,
                             options.get(), options_length);

  return result;
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::CallNativeComponent(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* ref = getArumentAsCStr(arguments, 1);
  const char* method = getArumentAsCStr(arguments, 2);
  const char* arguments_data = getArumentAsCStr(arguments, 3);
  int arguments_data_length = getArgumentAsInt32(arguments, 4);
  const char* options = getArumentAsCStr(arguments, 5);
  int options_length = getArgumentAsInt32(arguments, 6);

  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->platform_side()
      ->CallNativeComponent(page_id, ref, method, arguments_data,
                            arguments_data_length, options, options_length);

  delete[] page_id;
  delete[] ref;
  delete[] method;
  delete[] arguments_data;
  delete[] options;
  return createVoidResult();
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::SetTimeout(
    IPCArguments* arguments) {
  const char* callback_id = getArumentAsCStr(arguments, 0);
  const char* time = getArumentAsCStr(arguments, 1);

  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->platform_side()
      ->SetTimeout(callback_id, time);

  delete[] callback_id;
  delete[] time;
  return createVoidResult();
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::NativeLog(
    IPCArguments* arguments) {
  const char* str_array = getArumentAsCStr(arguments, 0);

  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->platform_side()
      ->NativeLog(str_array);

  delete[] str_array;
  return createVoidResult();
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::UpdateFinish(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* task = getArumentAsCStr(arguments, 1);
  const char* callback = getArumentAsCStr(arguments, 2);

  int result = WeexCoreManager::getInstance()
                   ->getPlatformBridge()
                   ->platform_side()
                   ->UpdateFinish(page_id, task, callback);

  delete[] page_id;
  delete[] task;
  delete[] callback;
  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::RefreshFinish(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* task = getArumentAsCStr(arguments, 1);
  const char* callback = getArumentAsCStr(arguments, 2);

  int result = WeexCoreManager::getInstance()
                   ->getPlatformBridge()
                   ->platform_side()
                   ->RefreshFinish(page_id, task, callback);

  delete[] page_id;
  delete[] task;
  delete[] callback;
  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::AddEvent(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* ref = getArumentAsCStr(arguments, 1);
  const char* event = getArumentAsCStr(arguments, 2);

  int result = WeexCoreManager::getInstance()
                   ->getPlatformBridge()
                   ->platform_side()
                   ->AddEvent(page_id, ref, event);

  delete[] page_id;
  delete[] ref;
  delete[] event;
  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::RemoveEvent(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* ref = getArumentAsCStr(arguments, 1);
  const char* event = getArumentAsCStr(arguments, 2);

  int result = WeexCoreManager::getInstance()
                   ->getPlatformBridge()
                   ->platform_side()
                   ->RemoveEvent(page_id, ref, event);

  delete[] page_id;
  delete[] ref;
  delete[] event;
  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::CreateBody(
    IPCArguments* arguments) {
  auto page_id = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 0));
  auto component_type = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 1));
  auto ref = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 2));
  // styles
  std::map<std::string, std::string> styles;
  if (arguments->getType(3) != IPCType::VOID) {
    auto styles_data = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 3));
    wson_parser styles_parser = wson_parser(styles_data.get());
    styles_parser.nextType();
    int styles_length = styles_parser.nextMapSize();
    for (int i = 0; i < styles_length; ++i) {
      std::string key = styles_parser.nextStringUTF8(styles_parser.nextType());
      std::string value =
          styles_parser.nextStringUTF8(styles_parser.nextType());
      styles.insert(std::make_pair<std::string, std::string>(std::move(key),
                                                             std::move(value)));
    }
  }
  // attributes
  std::map<std::string, std::string> attributes;
  if (arguments->getType(4) != IPCType::VOID) {
    auto attributes_data =
        std::unique_ptr<char[]>(getArumentAsCStr(arguments, 4));
    wson_parser attributes_parser = wson_parser(attributes_data.get());
    attributes_parser.nextType();
    int attributes_length = attributes_parser.nextMapSize();
    for (int i = 0; i < attributes_length; ++i) {
      std::string key =
          attributes_parser.nextStringUTF8(attributes_parser.nextType());
      std::string value =
          attributes_parser.nextStringUTF8(attributes_parser.nextType());
      attributes.insert(std::make_pair<std::string, std::string>(
          std::move(key), std::move(value)));
    }
  }
  // events
  std::set<std::string> events;
  if (arguments->getType(5) != IPCType::VOID) {
    auto events_data = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 5));
    wson_parser events_parser = wson_parser(events_data.get());
    events_parser.nextType();
    int events_length = events_parser.nextArraySize();
    for (int i = 0; i < events_length; ++i) {
      events.insert(events_parser.nextStringUTF8(events_parser.nextType()));
    }
  }
  // margin
  auto margins_buffer = const_cast<uint16_t*>(arguments->getString(6)->content);
  WXCoreMargin margin;
  margin.setMargin(kMarginTop,
                   reinterpret_cast<const float*>(margins_buffer)[0]);
  margin.setMargin(kMarginBottom,
                   reinterpret_cast<const float*>(margins_buffer)[1]);
  margin.setMargin(kMarginLeft,
                   reinterpret_cast<const float*>(margins_buffer)[2]);
  margin.setMargin(kMarginRight,
                   reinterpret_cast<const float*>(margins_buffer)[3]);
  // padding
  auto paddings_buffer =
      const_cast<uint16_t*>(arguments->getString(7)->content);
  WXCorePadding padding;
  padding.setPadding(kPaddingTop,
                     reinterpret_cast<const float*>(paddings_buffer)[0]);
  padding.setPadding(kPaddingBottom,
                     reinterpret_cast<const float*>(paddings_buffer)[1]);
  padding.setPadding(kPaddingLeft,
                     reinterpret_cast<const float*>(paddings_buffer)[2]);
  padding.setPadding(kPaddingRight,
                     reinterpret_cast<const float*>(paddings_buffer)[3]);
  // border
  auto borders_buffer = const_cast<uint16_t*>(arguments->getString(8)->content);
  WXCoreBorderWidth border;
  border.setBorderWidth(kBorderWidthTop,
                        reinterpret_cast<const float*>(borders_buffer)[0]);
  border.setBorderWidth(kBorderWidthBottom,
                        reinterpret_cast<const float*>(borders_buffer)[1]);
  border.setBorderWidth(kBorderWidthLeft,
                        reinterpret_cast<const float*>(borders_buffer)[2]);
  border.setBorderWidth(kBorderWidthRight,
                        reinterpret_cast<const float*>(borders_buffer)[3]);

  int result =
      WeexCoreManager::getInstance()
          ->getPlatformBridge()
          ->platform_side()
          ->CreateBody(page_id.get(), component_type.get(), ref.get(), &styles,
                       &attributes, &events, margin, padding, border);

  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::AddElement(
    IPCArguments* arguments) {
  auto page_id = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 0));
  auto component_type = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 1));
  auto ref = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 2));
  int index = getArgumentAsInt32(arguments, 3);
  auto parent_ref = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 4));
  // styles
  std::map<std::string, std::string> styles;
  if (arguments->getType(5) != IPCType::VOID) {
    auto styles_data = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 5));
    wson_parser styles_parser = wson_parser(styles_data.get());
    styles_parser.nextType();
    int styles_length = styles_parser.nextMapSize();
    for (int i = 0; i < styles_length; ++i) {
      std::string key = styles_parser.nextStringUTF8(styles_parser.nextType());
      std::string value =
          styles_parser.nextStringUTF8(styles_parser.nextType());
      styles.insert(std::make_pair<std::string, std::string>(std::move(key),
                                                             std::move(value)));
    }
  }
  // attributes
  std::map<std::string, std::string> attributes;
  if (arguments->getType(6) != IPCType::VOID) {
    auto attributes_data =
        std::unique_ptr<char[]>(getArumentAsCStr(arguments, 6));
    wson_parser attributes_parser = wson_parser(attributes_data.get());
    attributes_parser.nextType();
    int attributes_length = attributes_parser.nextMapSize();
    for (int i = 0; i < attributes_length; ++i) {
      std::string key =
          attributes_parser.nextStringUTF8(attributes_parser.nextType());
      std::string value =
          attributes_parser.nextStringUTF8(attributes_parser.nextType());
      attributes.insert(std::make_pair<std::string, std::string>(
          std::move(key), std::move(value)));
    }
  }
  // events
  std::set<std::string> events;
  if (arguments->getType(7) != IPCType::VOID) {
    auto events_data = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 7));
    wson_parser events_parser = wson_parser(events_data.get());
    events_parser.nextType();
    int events_length = events_parser.nextArraySize();
    for (int i = 0; i < events_length; ++i) {
      events.insert(events_parser.nextStringUTF8(events_parser.nextType()));
    }
  }
  // margin
  auto margins_buffer = const_cast<uint16_t*>(arguments->getString(8)->content);
  WXCoreMargin margin;
  margin.setMargin(kMarginTop,
                   reinterpret_cast<const float*>(margins_buffer)[0]);
  margin.setMargin(kMarginBottom,
                   reinterpret_cast<const float*>(margins_buffer)[1]);
  margin.setMargin(kMarginLeft,
                   reinterpret_cast<const float*>(margins_buffer)[2]);
  margin.setMargin(kMarginRight,
                   reinterpret_cast<const float*>(margins_buffer)[3]);
  // padding
  auto paddings_buffer =
      const_cast<uint16_t*>(arguments->getString(9)->content);
  WXCorePadding padding;
  padding.setPadding(kPaddingTop,
                     reinterpret_cast<const float*>(paddings_buffer)[0]);
  padding.setPadding(kPaddingBottom,
                     reinterpret_cast<const float*>(paddings_buffer)[1]);
  padding.setPadding(kPaddingLeft,
                     reinterpret_cast<const float*>(paddings_buffer)[2]);
  padding.setPadding(kPaddingRight,
                     reinterpret_cast<const float*>(paddings_buffer)[3]);
  // border
  auto borders_buffer =
      const_cast<uint16_t*>(arguments->getString(10)->content);
  WXCoreBorderWidth border;
  border.setBorderWidth(kBorderWidthTop,
                        reinterpret_cast<const float*>(borders_buffer)[0]);
  border.setBorderWidth(kBorderWidthBottom,
                        reinterpret_cast<const float*>(borders_buffer)[1]);
  border.setBorderWidth(kBorderWidthLeft,
                        reinterpret_cast<const float*>(borders_buffer)[2]);
  border.setBorderWidth(kBorderWidthRight,
                        reinterpret_cast<const float*>(borders_buffer)[3]);
  // will_layout
  bool will_layout = static_cast<bool>(getArgumentAsInt32(arguments, 11));
  int result = WeexCoreManager::getInstance()
                   ->getPlatformBridge()
                   ->platform_side()
                   ->AddElement(page_id.get(), component_type.get(), ref.get(),
                                index, parent_ref.get(), &styles, &attributes,
                                &events, margin, padding, border, will_layout);

  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::Layout(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* ref = getArumentAsCStr(arguments, 1);
  int32_t top = getArgumentAsInt32(arguments, 2);
  int32_t bottom = getArgumentAsInt32(arguments, 3);
  int32_t left = getArgumentAsInt32(arguments, 4);
  int32_t right = getArgumentAsInt32(arguments, 5);
  int32_t height = getArgumentAsInt32(arguments, 6);
  int32_t width = getArgumentAsInt32(arguments, 7);
  int32_t index = getArgumentAsInt32(arguments, 8);

  auto result = WeexCoreManager::getInstance()
                    ->getPlatformBridge()
                    ->platform_side()
                    ->Layout(page_id, ref, top, bottom, left, right, height,
                             width, index);
  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::UpdateStyle(
    IPCArguments* arguments) {
  auto page_id = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 0));
  auto ref = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 1));
  // styles
  std::vector<std::pair<std::string, std::string>> styles;
  if (arguments->getType(2) != IPCType::VOID) {
    auto styles_data = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 2));
    wson_parser styles_parser = wson_parser(styles_data.get());
    styles_parser.nextType();
    int styles_length = styles_parser.nextMapSize();
    for (int i = 0; i < styles_length; ++i) {
      std::string key = styles_parser.nextStringUTF8(styles_parser.nextType());
      std::string value =
          styles_parser.nextStringUTF8(styles_parser.nextType());
      styles.push_back(std::make_pair<std::string, std::string>(
          std::move(key), std::move(value)));
    }
  }

  // margin
  std::vector<std::pair<std::string, std::string>> margins;
  if (arguments->getType(3) != IPCType::VOID) {
    auto margins_data = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 3));
    wson_parser margins_parser = wson_parser(margins_data.get());
    margins_parser.nextType();
    int margins_length = margins_parser.nextMapSize();
    for (int i = 0; i < margins_length; ++i) {
      std::string key =
          margins_parser.nextStringUTF8(margins_parser.nextType());
      std::string value =
          margins_parser.nextStringUTF8(margins_parser.nextType());
      margins.push_back(std::make_pair<std::string, std::string>(
          std::move(key), std::move(value)));
    }
  }

  // padding
  std::vector<std::pair<std::string, std::string>> paddings;
  if (arguments->getType(4) != IPCType::VOID) {
    auto paddings_data =
        std::unique_ptr<char[]>(getArumentAsCStr(arguments, 4));
    wson_parser paddings_parser = wson_parser(paddings_data.get());
    paddings_parser.nextType();
    int paddings_length = paddings_parser.nextMapSize();
    for (int i = 0; i < paddings_length; ++i) {
      std::string key =
          paddings_parser.nextStringUTF8(paddings_parser.nextType());
      std::string value =
          paddings_parser.nextStringUTF8(paddings_parser.nextType());
      paddings.push_back(std::make_pair<std::string, std::string>(
          std::move(key), std::move(value)));
    }
  }

  // border
  std::vector<std::pair<std::string, std::string>> borders;
  if (arguments->getType(5) != IPCType::VOID) {
    auto borders_data = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 5));
    wson_parser borders_parser = wson_parser(borders_data.get());
    borders_parser.nextType();
    int borders_length = borders_parser.nextMapSize();
    for (int i = 0; i < borders_length; ++i) {
      std::string key =
          borders_parser.nextStringUTF8(borders_parser.nextType());
      std::string value =
          borders_parser.nextStringUTF8(borders_parser.nextType());
      borders.push_back(std::make_pair<std::string, std::string>(
          std::move(key), std::move(value)));
    }
  }

  int result = WeexCoreManager::getInstance()
                   ->getPlatformBridge()
                   ->platform_side()
                   ->UpdateStyle(page_id.get(), ref.get(), &styles, &margins,
                                 &paddings, &borders);

  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::UpdateAttr(
    IPCArguments* arguments) {
  auto page_id = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 0));
  auto ref = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 1));
  // attributes
  std::vector<std::pair<std::string, std::string>> attributes;
  if (arguments->getType(2) != IPCType::VOID) {
    auto attributes_data =
        std::unique_ptr<char[]>(getArumentAsCStr(arguments, 2));
    wson_parser attributes_parser = wson_parser(attributes_data.get());
    attributes_parser.nextType();
    int attributes_length = attributes_parser.nextMapSize();
    for (int i = 0; i < attributes_length; ++i) {
      std::string key =
          attributes_parser.nextStringUTF8(attributes_parser.nextType());
      std::string value =
          attributes_parser.nextStringUTF8(attributes_parser.nextType());
      attributes.push_back(std::make_pair<std::string, std::string>(
          std::move(key), std::move(value)));
    }
  }

  int result = WeexCoreManager::getInstance()
                   ->getPlatformBridge()
                   ->platform_side()
                   ->UpdateAttr(page_id.get(), ref.get(), &attributes);

  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::CreateFinish(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);

  auto result = WeexCoreManager::getInstance()
                    ->getPlatformBridge()
                    ->platform_side()
                    ->CreateFinish(page_id);
  delete[] page_id;
  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::RemoveElement(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* ref = getArumentAsCStr(arguments, 1);

  auto result = WeexCoreManager::getInstance()
                    ->getPlatformBridge()
                    ->platform_side()
                    ->RemoveElement(page_id, ref);
  delete[] page_id;
  delete[] ref;
  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::MoveElement(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* ref = getArumentAsCStr(arguments, 1);
  const char* parent_ref = getArumentAsCStr(arguments, 2);
  int32_t index = getArgumentAsInt32(arguments, 3);

  auto result = WeexCoreManager::getInstance()
                    ->getPlatformBridge()
                    ->platform_side()
                    ->MoveElement(page_id, ref, parent_ref, index);
  delete[] page_id;
  delete[] ref;
  delete[] parent_ref;
  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::AppendTreeCreateFinish(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* ref = getArumentAsCStr(arguments, 1);

  auto result = WeexCoreManager::getInstance()
                    ->getPlatformBridge()
                    ->platform_side()
                    ->AppendTreeCreateFinish(page_id, ref);
  delete[] page_id;
  delete[] ref;
  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::HasTransitionPros(
    IPCArguments* arguments) {
  const char* page_id = getArumentAsCStr(arguments, 0);
  const char* ref = getArumentAsCStr(arguments, 1);

  std::vector<std::pair<std::string, std::string>> styles;
  if (arguments->getType(2) != IPCType::VOID) {
    auto styles_data = std::unique_ptr<char[]>(getArumentAsCStr(arguments, 2));
    wson_parser styles_parser = wson_parser(styles_data.get());
    styles_parser.nextType();
    int styles_length = styles_parser.nextMapSize();
    for (int i = 0; i < styles_length; ++i) {
      std::string key = styles_parser.nextStringUTF8(styles_parser.nextType());
      std::string value =
          styles_parser.nextStringUTF8(styles_parser.nextType());
      styles.push_back(std::make_pair<std::string, std::string>(
          std::move(key), std::move(value)));
    }
  }

  auto result = WeexCoreManager::getInstance()
                    ->getPlatformBridge()
                    ->platform_side()
                    ->HasTransitionPros(page_id, ref, &styles);
  delete[] page_id;
  delete[] ref;
  return createInt32Result(result);
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::PostMessage(
    IPCArguments* arguments) {
  const char* vm_id = getArumentAsCStr(arguments, 0);
  const char* data = getArumentAsCStr(arguments, 1);

  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->platform_side()
      ->PostMessage(vm_id, data);
  delete[] vm_id;
  delete[] data;
  return createVoidResult();
}

std::unique_ptr<IPCResult> AndroidBridgeInMultiProcess::DispatchMessage(
    IPCArguments* arguments) {
  const char* client_id = getArumentAsCStr(arguments, 0);
  const char* vm_id = getArumentAsCStr(arguments, 1);
  const char* data = getArumentAsCStr(arguments, 2);
  const char* callback = getArumentAsCStr(arguments, 3);

  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->platform_side()
      ->DispatchMessage(client_id, vm_id, data, callback);
  delete[] client_id;
  delete[] vm_id;
  delete[] data;
  delete[] callback;
  return createVoidResult();
}

///////////////////////////
//
// void IPCBridge::SetPlatform(const std::string& platform) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::SET_PLATFORM));
//  serializer->add(platform.c_str(), platform.size());
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
// void IPCBridge::SetDeviceWidthAndHeight(float width, float height) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(static_cast<uint32_t>(
//      IPCMsgFromPlatformToCore::SET_DEVICE_WIDTH_AND_HEIGHT));
//  serializer->add(width);
//  serializer->add(height);
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
// void IPCBridge::AddOption(const std::string& key, const std::string& value) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::ADD_OPTION));
//  serializer->add(key.c_str(), key.size());
//  serializer->add(value.c_str(), value.size());
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
///////////////////
//
// void IPCBridge::SetDefaultHeightAndWidthIntoRootDom(
//    const std::string& instance_id, float default_width, float default_height,
//    bool is_width_wrap_content, bool is_height_wrap_content) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(static_cast<uint32_t>(
//      IPCMsgFromPlatformToCore::SET_DEFAULT_HEIGHT_AND_WIDTH_INTO_ROOT_DOM));
//  serializer->add(instance_id.c_str(), instance_id.size());
//  serializer->add(default_width);
//  serializer->add(default_height);
//  serializer->add(is_width_wrap_content);
//  serializer->add(is_height_wrap_content);
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
// void IPCBridge::OnInstanceClose(const std::string& instance_id) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::ON_INSTANCE_CLOSE));
//  serializer->add(instance_id.c_str(), instance_id.size());
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
// void IPCBridge::SetStyleWidth(const std::string& instance_id,
//                              const std::string& render_ref, float width) {
//    // TODO 无法 call on main thread
////  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
////  serializer->setMsg(
////      static_cast<uint32_t>(IPCMsgFromPlatformToCore::SET_STYLE_WIDTH));
////  serializer->add(instance_id.c_str(), instance_id.size());
////  serializer->add(render_ref.c_str(), render_ref.size());
////  serializer->add(width);
////  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
////  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//void    IPCBridge::SetStyleHeight(const std::string& instance_id,
//                               const std::string& render_ref, float height) {
//    // TODO 无法 call on main thread
////  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
////  serializer->setMsg(
////      static_cast<uint32_t>(IPCMsgFromPlatformToCore::SET_STYLE_HEIGHT));
////  serializer->add(instance_id.c_str(), instance_id.size());
////  serializer->add(render_ref.c_str(), render_ref.size());
////  serializer->add(height);
////  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
////  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//void IPC   Bridge::SetMargin(const std::string& instance_id,
//                          const std::string& render_ref, int edge,
//                          float value) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::SET_MARGIN));
//  serializer->add(instance_id.c_str(), instance_id.size());
//  serializer->add(render_ref.c_str(), render_ref.size());
//  serializer->add(edge);
//  serializer->add(value);
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//void IPC   Bridge::SetPadding(const std::string& instance_id,
//                           const std::string& render_ref, int edge,
//                           float value) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::SET_PADDING));
//  serializer->add(instance_id.c_str(), instance_id.size());
//  serializer->add(render_ref.c_str(), render_ref.size());
//  serializer->add(edge);
//  serializer->add(value);
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//void IPC   Bridge::SetPosition(const std::string& instance_id,
//                            const std::string& render_ref, int edge,
//                            float value) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::SET_POSITION));
//  serializer->add(instance_id.c_str(), instance_id.size());
//  serializer->add(render_ref.c_str(), render_ref.size());
//  serializer->add(edge);
//  serializer->add(value);
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//void IPC   Bridge::MarkDirty(const std::string& instance_id,
//                          const std::string& render_ref) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::MARK_DIRTY));
//  serializer->add(instance_id.c_str(), instance_id.size());
//  serializer->add(render_ref.c_str(), render_ref.size());
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//void IPC   Bridge::SetViewPortWidth(const std::string& instance_id, float width)

// /
//  s
// d::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::SET_VIEWPORT_WIDTH));
//  serializer->add(instance_id.c_str(), instance_id.size());
//  serializer->add(width);
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//void IPC   Bridge::ForceLayout(const std::string& instance_id) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::FORCE_LAYOUT));
//  serializer->add(instance_id.c_str(), instance_id.size());
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//bool IPC   Bridge::NotifyLayout(const std::string& instance_id) {
////  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
////  serializer->setMsg(
////      static_cast<uint32_t>(IPCMsgFromPlatformToCore::NOTIFY_LAYOUT));
////  serializer->add(instance_id.c_str(), instance_id.size());
////  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
////  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
////  return result->get<bool>();
//    // TODO 为什么需要 notfiylayout，直接一次 layout 就可以，layout 可以给 async 标签，不需要等待写入
//    return true;
//}
///
//    /
//
//
//std::vector<int64_t> IPCBr nRenderTime(
//     con st st d::string& instance_id) {
//    // TODO 无法 call on main thread
////  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
////  serializer->setMsg(static_cast<uint32_t>(
////      IPCMsgFromPlatformToCore::GET_FIRST_SCREEN_RENDER_TIME));
////  serializer->add(instance_id.c_str(), instance_id.size());
////  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
////  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
////  if (result->getType() == IPCType::VOID) {
//    return std::vector<int64_t>();
////  } else {
////    const int64_t* temp =
////        reinterpret_cast<const int64_t*>(result->getByteArrayContent());
////    return std::vector<int64_t>(temp, temp + 3);
////  }
//}
//
//std::vector<int64_t> IPCBridge::GetRenderFinishTime(
//    const s   td::string& instance_id) {
//    // TODO 无法 call on main thread
////  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
////  serializer->setMsg(
////      static_cast<uint32_t>(IPCMsgFromPlatformToCore::GET_RENDER_FINIS ));
////  serializer->add(instance_id.c_str(), instance_id.size());
////  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
////  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
////  if (result->getType() == IPCType::VOID) {
//    return std::vector<int64_t>();
////  } else {
////    const int64_t* temp =
////        reinterpret_cast<const int64_t*>(result->getByteArrayContent());
////    return std::vector<int64_t>(temp, temp + 3);
////  }
//}
//
//void IPCBridge::SetRenderContainerWrapContent(const std::string& insta   nce_id,
//                                              bool wrap) {
// // /  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(static_cast<uint32_t>(
//      IPCMsgFromPlatformToCore::SET_RENDER_CONTAINER_WRAP_CONTENT));
//  serializer->add(instance_id.c_str(), instance_id.size());
//  serializer->add(wrap);
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//void IPCBridge::BindMeasurementToRenderObject(long ptr) {
//  std::uni   que_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(static_cast<uint32_t>(
//      IPCMsgFromPlatformToCore::BIND_MEASUREMENT_TO_RENDER_OBJECT));
//  serializer->add(static_cast<int64_t>(ptr));
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//void IPCBridge::RegisterCoreEnv(const std::string& key,
//                                   const std::string& value) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::REGISTER_CORE_ENV));
//  serializer->add(key.c_str(), key.size());
//  serializer->add(value.c_str(), value.size());
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//long IPCBridge::GetRenderObject(const std::string& instance_id,
//                                   const std::string& render_ref) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::GET_RENDER_OBJECT));
//  serializer->add(instance_id.c_str(), instance_id.size());
//  serializer->add(render_ref.c_str(), render_ref.size());
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//  return result->get<long>();
//}
//
//void IPCBridge::UpdateRenderObjectStyle(long render_ptr, const std::st   ring& key,
//                                        const std::string& val
// // // e) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(static_cast<uint32_t>(
//      IPCMsgFromPlatformToCore::UPDATE_RENDER_OBJECT_STYLE));
//  serializer->add(static_cast<int64_t>(render_ptr));
//  serializer->add(key.c_str(), key.size());
//  serializer->add(value.c_str(), value.size());
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//void IPCBridge::UpdateRenderObjectAttr(long render_ptr, const std::str   ing& key,
//                                       const std::string& valu
// // // ) {
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(static_cast<uint32_t>(
//      IPCMsgFromPlatformToCore::UPDATE_RENDER_OBJECT_ATTR));
//  serializer->add(static_cast<int64_t>(render_ptr));
//  serializer->add(key.c_str(), key.size());
//  serializer->add(value.c_str(), value.size());
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}
//
//long IPCBridge::CopyRenderObject(long render_ptr) {
//  std::unique_pt   r<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(
//      static_cast<uint32_t>(IPCMsgFromPlatformToCore::COPY_RENDER_OBJECT));
//  serializer->add(static_cast<int64_t>(render_ptr));
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//  return result->get<long>();
//}
//
//void IPCBridge::SetMeasureFunctionAdapter(
//    MeasureFunctionAdapte   r* measureFunctionAdapter) {
//  Bridge_Impl_Android::SetMeasureFunctionAdapter(measureFunctionAdapter);
//  std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
//  serializer->setMsg(static_cast<uint32_t>(
//      IPCMsgFromPlatformToCore::SET_MEASURE_FUNCTION_ADAPTER));
//  std::unique_ptr<IPCBuffer> buffer = serializer->finish();
//  std::unique_ptr<IPCResult> result = sender_->send(buffer.get());
//}

//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg

//,
//                           HandleSetJSVersion);
//  handle r-> registerHandler(static_cast<uint32_t>(IPCProxyMsg::REPORTEXCEPTION
//  ,
//                           HandleReportException);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLNATIVE),
//                           HandleCallNative);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLNATIVEMODULE),
//                           HandleCallNativeModule);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLNATIVECOMPONENT),
//                           HandleCallNativeComponent);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLADDELEMENT),
//                           HandleCallAddElement);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::SETTIMEOUT),
//                           HandleSetTimeout);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::NATIVELOG),
//                           HandleCallNativeLog);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLCREATEBODY),
//                           FunctionCallCreateBody);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLUPDATEFINISH),
//                           FunctionCallUpdateFinish);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLCREATEFINISH),
//                           FunctionCallCreateFinish);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLREFRESHFINISH),
//                           FunctionCallRefreshFinish);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLUPDATEATTRS),
//                           FunctionCallUpdateAttrs);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLUPDATESTYLE),
//                           FunctionCallUpdateStyle);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLREMOVEELEMENT),
//                           FunctionCallRemoveElement);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLMOVEELEMENT),
//                           FunctionCallMoveElement);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLADDEVENT),
//                           FunctionCallAddEvent);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLREMOVEEVENT),
//                           FunctionCallRemoveEvent);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::SETINTERVAL),
//                           HandleSetInterval);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CLEARINTERVAL),
//                           HandleClearInterval);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLGCANVASLINK),
//                           HandleCallGCanvasLinkNative);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLT3DLINK),
//                           HandleT3DLinkNative);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::POSTMESSAGE),
//                           HandlePostMessage);
//  handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::DISPATCHMESSAGE),
//                           HandleDispatchMessage);
//}

// std::unique_ptr<IPCResult> IPCBridge::IPCBridge::HandleSetJSVersion(
//    IPCArguments* arguments) {
//  const char* version = getArumentAsCStr(arguments, 0);
//  WeexCoreManager::getInstance()->getPlatformBridge()->setJSVersion(version);
//  return createVoidResult();
//}
//
//    std::unique_ptr<IPCResult>
//    IPCBridge::IPCBridge::HandleReportException(IPCArguments *arguments) {
//      const char *pageId = nullptr;
//      const char *func = nullptr;
//      const char *exceptionInfo = nullptr;
//
//      if (arguments->getType(0) == IPCType::BYTEARRAY) {
//        const IPCByteArray *instanceIDBA = arguments->getByteArray(0);
//        pageId = instanceIDBA->content;
//      }
//
//      if (arguments->getType(1) == IPCType::BYTEARRAY) {
//        const IPCByteArray *funcBA = arguments->getByteArray(1);
//        func = funcBA->content;
//      }
//
//      if (arguments->getType(2) == IPCType::BYTEARRAY) {
//        const IPCByteArray *exceptionInfoBA = arguments->getByteArray(2);
//        exceptionInfo = exceptionInfoBA->content;
//      }
//
//      LOGE(" ReportException : %s", exceptionInfo);
//      WeexCoreManager::getInstance()->getPlatformBridge()->reportException(pageId,
//      func, exceptionInfo); return createVoidResult();
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::HandleCallNativeLog(IPCArguments
//    *arguments) {
//      const char* str_array = getArumentAsCStr(arguments, 0);
//      WeexCoreManager::getInstance()->getPlatformBridge()->callNativeLog(str_array);
//      return createInt32Result(static_cast<int32_t>(true));
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::HandleSetTimeout(IPCArguments
//    *arguments) {
//      char* callbackID = getArumentAsCStr(arguments, 0);
//      char* time = getArumentAsCStr(arguments, 1);
//
//      if (callbackID == nullptr || time == nullptr)
//        return createInt32Result(static_cast<int32_t>(false));
//
//      WeexCoreManager::getInstance()->getPlatformBridge()->setTimeout(callbackID,
//      time);
//
//      if (callbackID != nullptr) {
//        delete[]callbackID;
//        callbackID = nullptr;
//      }
//      if (time != nullptr) {
//        delete[]time;
//        time = nullptr;
//      }
//      return createInt32Result(static_cast<int32_t>(true));
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::HandleSetInterval(IPCArguments
//    *arguments) {
//      const char *pageId = getArumentAsCStr(arguments, 0);
//      const char *callbackID = getArumentAsCStr(arguments, 1);
//      const char *_time = getArumentAsCStr(arguments, 2);
//      if (pageId == nullptr || callbackID == nullptr || _time == nullptr)
//        return createInt32Result(-1);
//
//      long time_ = atoi(_time);
//      int _timerId =
//      WeexCoreManager::getInstance()->getJSBridge()->onSetInterval(pageId,
//                                                                                  callbackID,
//                                                                                  _time);
//
//      if (pageId != nullptr) {
//        delete[]pageId;
//        pageId = nullptr;
//      }
//      if (callbackID != nullptr) {
//        delete[]callbackID;
//        callbackID = nullptr;
//      }
//      if (_time != nullptr) {
//        delete[]_time;
//        _time = nullptr;
//      }
//      return createInt32Result(_timerId);
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::HandleClearInterval(IPCArguments
//    *arguments) {
//      const char *pageId = getArumentAsCStr(arguments, 0);
//      const char *callbackID = getArumentAsCStr(arguments, 1);
//      long id = atoi(callbackID);
//
//      if (pageId != nullptr) {
//        delete[]pageId;
//        pageId = nullptr;
//      }
//      if (callbackID != nullptr) {
//        delete[]callbackID;
//        callbackID = nullptr;
//      }
//      return createVoidResult();
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::HandleCallNative(IPCArguments
//    *arguments) {
//      char* pageId = getArumentAsCStr(arguments, 0);
//      char* task = getArumentAsCStr(arguments, 1);
//      char* callback = getArumentAsCStr(arguments, 2);
//
//      if (pageId != nullptr && task != nullptr) {
//#if JSAPI_LOG
//        LOGD("[ExtendJSApi] handleCallNative >>>> pageId: %s, task: %s",
//        pageId, task);
//#endif
//        if (strcmp(task,
//        "[{\"module\":\"dom\",\"method\":\"createFinish\",\"args\":[]}]") ==
//        0) {
//          RenderManager::GetInstance()->CreateFinish(pageId) ? 0 : -1;
//        } else {
//          WeexCoreManager::getInstance()->getPlatformBridge()->callNative(pageId,
//          task, callback);
//        }
//      }
//
//      if (pageId != nullptr) {
//        delete[]pageId;
//        pageId = nullptr;
//      }
//      if (task != nullptr) {
//        delete[]task;
//        task = nullptr;
//      }
//      if (callback != nullptr) {
//        delete[]callback;
//        callback = nullptr;
//      }
//      return createInt32Result(0);
//    }
//
//    static std::unique_ptr<IPCResult>
//    IPCBridge::HandleCallGCanvasLinkNative(IPCArguments *arguments) {
//      JNIEnv *env = getJNIEnv();
//      jstring jPageId = getArgumentAsJString(env, arguments, 0);
//      const char *pageId = env->GetStringUTFChars(jPageId, NULL);
//      int type = getArgumentAsInt32(env, arguments, 1);
//      jstring val = getArgumentAsJString(env, arguments, 2);
//      const char *args = env->GetStringUTFChars(val, NULL);
//
//#if JSAPI_LOG
//      LOGD("[ExtendJSApi] HandleCallGCanvasLinkNative >>>> pageId: %s, type:
//      %d, args: %s", pageId, type, args);
//#endif
//      const char *retVal = NULL;
//      // TODO
////      if (gCanvasFunc) {
////        retVal = callGCanvasFun(gCanvasFunc, pageId, type, args);
////      }
//
////      const char *retVal = _callGCanvasLinkNative(pageId,type,args);
//
//      std::unique_ptr<IPCResult> ret = createVoidResult();
//      if (retVal) {
//        jstring jDataStr = env->NewStringUTF(retVal);
//        ret = std::unique_ptr<IPCResult>(new IPCStringResult(env, jDataStr));
//        env->DeleteLocalRef(jDataStr);
//        retVal = NULL;
//      }
//      env->DeleteLocalRef(jPageId);
//      env->DeleteLocalRef(val);
//      return ret;
//    }
//
//    static std::unique_ptr<IPCResult>
//    IPCBridge::HandleT3DLinkNative(IPCArguments* arguments)
//    {
//      JNIEnv* env = getJNIEnv();
//      int type = getArgumentAsInt32(env, arguments, 0);
//      jstring val = getArgumentAsJString(env, arguments, 1);
//      const char* args = env->GetStringUTFChars(val, NULL);
//
//#if JSAPI_LOG
//      LOGD("[ExtendJSApi] handleT3DLinkNative >>>> type: %d, args: %s", type,
//      args);
//#endif
//      const char *retVal = NULL;
//      // TODO
////      if (t3dFunc) {
////        retVal = WeexCore::weexCallT3dFunc(t3dFunc, type, args);
////      }
////
////      const char*retVal = _t3dLinkNative(type, args);
//
//      std::unique_ptr<IPCResult> ret = createVoidResult();
//      if (retVal) {
//        jstring jDataStr = env->NewStringUTF(retVal);
//        ret = std::unique_ptr<IPCResult>(new IPCStringResult(env, jDataStr));
//        env->DeleteLocalRef(jDataStr);
//        retVal = NULL;
//      }
//      env->DeleteLocalRef(val);
//      return ret;
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::HandleCallNativeModule(IPCArguments
//    *arguments) {
//      char* pageId = getArumentAsCStr(arguments, 0);
//      char* module = getArumentAsCStr(arguments, 1);
//      char* method = getArumentAsCStr(arguments, 2);
//      char* argumentsData = getArumentAsCStr(arguments, 3);
//      int   argumentsDataLength = getArumentAsCStrLen(arguments, 3);
//      char* optionsData = getArumentAsCStr(arguments, 4);
//      int   optionsDataLength = getArumentAsCStrLen(arguments, 4);
//
//      std::unique_ptr<IPCResult> ret =
//      WeexCoreManager::getInstance()->getPlatformBridge()->callNativeModule(
//              pageId, module, method, argumentsData,
//              argumentsDataLength, optionsData,
//              optionsDataLength
//      );
//
//      if (pageId != nullptr) {
//        delete[]pageId;
//        pageId = nullptr;
//      }
//      if (module != nullptr) {
//        delete[]module;
//        module = nullptr;
//      }
//      if (method != nullptr) {
//        delete[]method;
//        method = nullptr;
//      }
//      if (argumentsData != nullptr) {
//        delete[]argumentsData;
//        argumentsData = nullptr;
//      }
//      if (optionsData != nullptr) {
//        delete[]optionsData;
//        optionsData = nullptr;
//      }
//      return ret;
//    }
//
//    std::unique_ptr<IPCResult>
//    IPCBridge::HandleCallNativeComponent(IPCArguments *arguments) {
//      char* pageId = getArumentAsCStr(arguments, 0);
//      char* ref = getArumentAsCStr(arguments, 1);
//      char* method = getArumentAsCStr(arguments, 2);
//      char* argumentsData = getArumentAsCStr(arguments, 3);
//      int   argumentsDataLength = getArumentAsCStrLen(arguments, 3);
//      char* optionsData = getArumentAsCStr(arguments, 4);
//      int   optionsDataLength = getArumentAsCStrLen(arguments, 4);
//
//      if (pageId != nullptr && ref != nullptr && method != nullptr) {
//
//#if JSAPI_LOG
//        LOGD("[ExtendJSApi] handleCallNativeComponent >>>> pageId: %s, ref:
//        %s, method: %s, arg: %s, opt: %s",
//         pageId, ref, method, argString, optString);
//#endif
//        WeexCoreManager::getInstance()->getPlatformBridge()->callNativeComponent(
//                pageId, ref, method, argumentsData, argumentsDataLength,
//                optionsData, optionsDataLength
//        );
//      }
//
//      if (pageId != nullptr) {
//        delete[]pageId;
//        pageId = nullptr;
//      }
//      if (ref != nullptr) {
//        delete[]ref;
//        ref = nullptr;
//      }
//      if (method != nullptr) {
//        delete[]method;
//        method = nullptr;
//      }
//      if (argumentsData != nullptr) {
//        delete[]argumentsData;
//        argumentsData = nullptr;
//      }
//      if (optionsData != nullptr) {
//        delete[]optionsData;
//        optionsData = nullptr;
//      }
//      return createInt32Result(static_cast<int32_t>(true));
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::FunctionCallCreateBody(IPCArguments
//    *arguments) {
//
//      char *pageId = getArumentAsCStr(arguments, 0);
//      char *domStr = getArumentAsCStr(arguments, 1);
//
//      if (pageId == nullptr || domStr == nullptr)
//        return createInt32Result(0);
//
//      RenderManager::GetInstance()->CreatePage(pageId, domStr) ? 0 : -1;
//
//      delete[]pageId;
//      delete[]domStr;
//      return createInt32Result(0);
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::HandleCallAddElement(IPCArguments
//    *arguments) {
//
//      char *pageId = getArumentAsCStr(arguments, 0);
//      char *parentRef = getArumentAsCStr(arguments, 1);
//      char *domStr = getArumentAsCStr(arguments, 2);
//      char *index_cstr = getArumentAsCStr(arguments, 3);
//
//      const char *indexChar = index_cstr == nullptr ? "\0" : index_cstr;
//
//      int index = atoi(indexChar);
//      if (pageId != nullptr && parentRef != nullptr && domStr != nullptr &&
//      index >= -1) {
//#if JSAPI_LOG
//
//        std::string log = "";
//  log.append("pageId: ").append(pageId).append(", parentRef:
//  ").append(parentRef).append(", domStr: ").append(domStr); int log_index = 0;
//  int maxLength = 800;
//  std::string sub;
//  while (log_index < log.length()) {
//    if (log.length() <= log_index + maxLength) {
//      sub = log.substr(log_index);
//    } else {
//      sub = log.substr(log_index, maxLength);
//    }
//
//
//    if (log_index == 0)
//      LOGD("[ExtendJSApi] functionCallAddElement >>>> %s", sub.c_str());
//    else
//      LOGD("      [ExtendJSApi] functionCallAddElement >>>> %s", sub.c_str());
//
//    log_index += maxLength;
//  }
//#endif
//
//        RenderManager::GetInstance()->AddRenderObject(pageId, parentRef,
//        index, domStr);
//      }
//
//      delete[]pageId;
//      delete[]parentRef;
//      delete[]domStr;
//      delete[]index_cstr;
//      return createInt32Result(0);
//    }
//
//    std::unique_ptr<IPCResult>
//    IPCBridge::FunctionCallRemoveElement(IPCArguments *arguments) {
//
//      char *pageId = getArumentAsCStr(arguments, 0);
//      char *ref = getArumentAsCStr(arguments, 1);
//
//      if (pageId == nullptr || ref == nullptr)
//        return createInt32Result(0);
//
//#if JSAPI_LOG
//      LOGD("[ExtendJSApi] functionCallRemoveElement >>>> pageId: %s, ref: %s",
//      pageId,
//       ref);
//#endif
//      RenderManager::GetInstance()->RemoveRenderObject(pageId, ref);
//
//      delete[]pageId;
//      delete[]ref;
//      return createInt32Result(0);
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::FunctionCallMoveElement(IPCArguments
//    *arguments) {
//
//      char *pageId = getArumentAsCStr(arguments, 0);
//      char *ref = getArumentAsCStr(arguments, 1);
//      char *parentRef = getArumentAsCStr(arguments, 2);
//      char *index_str = getArumentAsCStr(arguments, 3);
//      int index = atoi(index_str);
//
//      if (pageId == nullptr || ref == nullptr || parentRef == nullptr ||
//      index_str == nullptr ||
//          index < -1)
//        return createInt32Result(0);
//
//      _callMoveElement(pageId, ref, parentRef, index);
//
//      delete[]pageId;
//      delete[]ref;
//      delete[]parentRef;
//      delete[]index_str;
//      return createInt32Result(0);
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::FunctionCallAddEvent(IPCArguments
//    *arguments) {
//
//      char *pageId = getArumentAsCStr(arguments, 0);
//      char *ref = getArumentAsCStr(arguments, 1);
//      char *event = getArumentAsCStr(arguments, 2);
//
//      if (pageId == nullptr || ref == nullptr || event == nullptr)
//        return createInt32Result(0);
//
//      _callAddEvent(pageId,ref,event);
//
//      delete[]pageId;
//      delete[]ref;
//      delete[]event;
//      return createInt32Result(0);
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::FunctionCallRemoveEvent(IPCArguments
//    *arguments) {
//
//      char *pageId = getArumentAsCStr(arguments, 0);
//      char *ref = getArumentAsCStr(arguments, 1);
//      char *event = getArumentAsCStr(arguments, 2);
//
//      if (pageId == nullptr || ref == nullptr || event == nullptr)
//        return createInt32Result(0);
//
//      _callRemoveEvent(pageId,ref,event);
//
//      delete[]pageId;
//      delete[]ref;
//      delete[]event;
//      return createInt32Result(0);
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::FunctionCallUpdateStyle(IPCArguments
//    *arguments) {
//
//      char *pageId = getArumentAsCStr(arguments, 0);
//      char *ref = getArumentAsCStr(arguments, 1);
//      char *data = getArumentAsCStr(arguments, 2);
//
//      if (pageId == nullptr || ref == nullptr || data == nullptr)
//        return createInt32Result(0);
//
//      _callUpdateStyle(pageId,ref,data);
//
//      delete[] pageId;
//      delete[] ref;
//      delete[] data;
//      return createInt32Result(0);
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::FunctionCallUpdateAttrs(IPCArguments
//    *arguments) {
//
//      char *pageId = getArumentAsCStr(arguments, 0);
//      char *ref = getArumentAsCStr(arguments, 1);
//      char *data = getArumentAsCStr(arguments, 2);
//
//      if (pageId == nullptr || ref == nullptr || data == nullptr)
//        return createInt32Result(0);
//
//      _callUpdateAttrs(pageId, ref,data);
//
//      delete[] pageId;
//      delete[] ref;
//      delete[] data;
//      return createInt32Result(0);
//    }
//
//    std::unique_ptr<IPCResult>
//    IPCBridge::FunctionCallCreateFinish(IPCArguments *arguments) {
//
//      char *pageId = getArumentAsCStr(arguments, 0);
//
//      if (pageId == nullptr)
//        return createInt32Result(0);
//
//      _callCreateFinish(pageId);
//
//      delete[]pageId;
//      return createInt32Result(0);
//    }
//
//    std::unique_ptr<IPCResult>
//    IPCBridge::FunctionCallUpdateFinish(IPCArguments *arguments) {
//      char *pageId = getArumentAsCStr(arguments, 0);
//      char *task = getArumentAsCStr(arguments, 1);
//      char *callback = getArumentAsCStr(arguments, 2);
//
//      int flag = 0;
//
//      if (pageId == nullptr || task == nullptr)
//        return createInt32Result(flag);
//
//      flag = _callUpdateFinish(pageId,task,callback);
//
//      if (pageId != nullptr) {
//        delete[]pageId;
//        pageId = nullptr;
//      }
//      if (task != nullptr) {
//        delete[]task;
//        task = nullptr;
//      }
//      if (callback != nullptr) {
//        delete[]callback;
//        callback = nullptr;
//      }
//      return createInt32Result(flag);
//    }
//
//    std::unique_ptr<IPCResult>
//    IPCBridge::FunctionCallRefreshFinish(IPCArguments *arguments) {
//      char *pageId = getArumentAsCStr(arguments, 0);
//      char *task = getArumentAsCStr(arguments, 1);
//      char *callback = getArumentAsCStr(arguments, 2);
//
//      int flag = 0;
//
//      if (pageId == nullptr)
//        return createInt32Result(flag);
//
//      flag = _callRefreshFinish(pageId, task, callback);
//
//      if (pageId != nullptr) {
//        delete[]pageId;
//        pageId = nullptr;
//      }
//      if (task != nullptr) {
//        delete[]task;
//        task = nullptr;
//      }
//      if (callback != nullptr) {
//        delete[]callback;
//        callback = nullptr;
//      }
//      return createInt32Result(flag);
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::HandlePostMessage(IPCArguments
//    *arguments) {
//      char* jData = getArumentAsCStr(arguments, 0);
//      char* jVmId = getArumentAsCStr(arguments, 1);
//      WeexCoreManager::getInstance()->getPlatformBridge()->handlePostMessage(jVmId, jData);
//      // LOGE("handlePostMessage");
////  JNIEnv* env = get
//      NIEnv();
////  jbyteArray jData = getArgumentAsJByteArray(env, arguments, 0);
////  jstring jVmId = getArgumentAsJString(env, arguments, 1);
////  if (jPostMessage == NULL) {
////    jPostMessage = env->GetMethodID(jWMBridgeClazz,
////                                    "postMessage",
////                                    "(Ljava/lang/String;[B)V");
////  }
////  env->CallVoidMethod(jWMThis, jPostMessage, jVmId, jData);
////  WeexCoreManager::getInstance()->getPlatformBridge()->handlePostMessa
///(jVmId, jData);
////  env->DeleteLocalRef(jData);
////  env->DeleteLocalRef(j
///mId);
///       return createInt32Result tic_cast<int32_t>(true));
//
//    }
//
//    std::unique_ptr<IPCResult> IPCBridge::HandleDispatchMessage(IPCArguments
//    *arguments) {
//      char* jClientId = getArumentAsCStr(arguments, 0);
//      char* jData = getArumentAsCStr(arguments, 1);
//      char* jCallback = getArumentAsCStr(arguments, 2);
//      char* jVmId = getArumentAsCStr(arguments, 3);
//      WeexCoreManager::getInstance()->getPlatformBridge()->handleDispatchMessage(jClientId,
//      jVmId, jData, jCallback);
//
////  JNIEnv* env = getJNIEnv();
////  jstring jClientId = getArgumentAsJString(env, arguments, 0);
////  jbyteArray jData = getArgumentAsJByteArray(env, arguments, 1);
////  jstring jCallback = getArgumentAsJString(env, arguments, 2);
////  jstring jVmId = getArgumentAsJString(env, arguments, 3);
////  if (jDispatchMeaasge == NULL) {
////    jDispatchMeaasge = env->GetMethodID(jWMBridgeClazz,
////                                        "dispatchMessage",
//// "(Ljava/lang/String;Ljava/lang/String;[BLjava/lang/String;)V"); /  } /
/// env->CallVoidMethod(jWMThis, jDispatchMeaasge, jClientId, jVmId, jData,
/// jCallback); /
/// WeexCoreManager::getInstance()->getPlatformBridge()->handleDispatchMessage(jClientId, jVmId,
/// jData, jCallback); /  env->DeleteLocalRef(jClientId); /
//
///  env-> eLocalRef(jData); /  env->DeleteLocalRef(jCallback);
//      return createInt32Result(static_cast<int32_t>(true));
//    }

}  // namespace WeexCore