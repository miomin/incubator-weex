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
/**
# Copyright 2018 Taobao (China) Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
**/

#ifndef FLUTTER_UNICORN_RUNTIME_RUNTIME_CONTROLLER_H_
#define FLUTTER_UNICORN_RUNTIME_RUNTIME_CONTROLLER_H_

#include <memory>
#include <string>
#include <vector>

#include "flutter/common/task_runners.h"
#include "flutter/flow/layers/layer_tree.h"
#include "flutter/flow/skia_gpu_object.h"
#include "flutter/fml/logging.h"
#include "flutter/fml/macros.h"
#include "flutter/lib/ui/semantics/semantics_update.h"
#include "flutter/lib/ui/snapshot_delegate.h"
#include "flutter/lib/ui/text/font_collection.h"
#include "flutter/lib/ui/ui_state.h"
#include "flutter/lib/ui/window/pointer_data_packet.h"
#include "flutter/lib/ui/window/window.h"
#include "flutter/unicorn/js_runtime/runtime/runtime_context.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

namespace blink {
class Scene;
class RuntimeDelegate;
class View;
class Window;
}  // namespace blink

namespace unicorn {
class RuntimeController final : public blink::WindowClient {
 public:
  RuntimeController(blink::RuntimeDelegate& client,
                    blink::TaskRunners task_runners,
                    fml::WeakPtr<blink::SnapshotDelegate> snapshot_delegate,
                    fml::WeakPtr<GrContext> resource_context,
                    fml::RefPtr<flow::SkiaUnrefQueue> unref_queue);

  ~RuntimeController();

  bool SetViewportMetrics(const blink::ViewportMetrics& metrics);

  bool SetLocales(const std::vector<std::string>& locale_data);

  bool SetUserSettingsData(const std::string& data);

  bool SetSemanticsEnabled(bool enabled);

  bool SetAccessibilityFeatures(int32_t flags);

  bool BeginFrame(fml::TimePoint frame_time);

  bool NotifyIdle(int64_t deadline);

  bool DispatchPlatformMessage(fml::RefPtr<blink::PlatformMessage> message);

  bool DispatchPointerDataPacket(const blink::PointerDataPacket& packet);

  bool DispatchSemanticsAction(int32_t id,
                               blink::SemanticsAction action,
                               std::vector<uint8_t> args);

  RuntimeContext* GetContext() { return main_context_.get(); }
  void CreateJavaScriptContext();

  void RunJavaScript(const std::string& content);

  void Shutdown();

  void Reload();

  bool Available();

 private:
  struct Locale {
    Locale(std::string language_code_,
           std::string country_code_,
           std::string script_code_,
           std::string variant_code_)
        : language_code(language_code_),
          country_code(country_code_),
          script_code(script_code_),
          variant_code(variant_code_) {}

    std::string language_code;
    std::string country_code;
    std::string script_code;
    std::string variant_code;
  };

  struct WindowData {
    blink::ViewportMetrics viewport_metrics;
    std::string language_code;
    std::string country_code;
    std::string script_code;
    std::string variant_code;
    std::vector<std::string> locale_data;
    std::string user_settings_data = "{}";
    bool semantics_enabled = false;
    bool assistive_technology_enabled = false;
    int32_t accessibility_feature_flags_ = 0;
  };

  blink::RuntimeDelegate& client_;
  blink::TaskRunners task_runners_;
  fml::WeakPtr<blink::SnapshotDelegate> snapshot_delegate_;
  fml::WeakPtr<GrContext> resource_context_;
  fml::RefPtr<flow::SkiaUnrefQueue> unref_queue_;
  std::unique_ptr<RuntimeVM> vm_;
  std::unique_ptr<RuntimeContext> main_context_;
  blink::UIState* ui_state_;

  blink::Window* GetWindowIfAvailable();

  // |blink::WindowClient|
  std::string DefaultRouteName() override;

  // |blink::WindowClient|
  void ScheduleFrame() override;

  // |blink::WindowClient|
  void Render(blink::Scene* scene) override;

  // |blink::WindowClient|
  // void UpdateSemantics(blink::SemanticsUpdate* update) override;

  // |blink::WindowClient|
  void HandlePlatformMessage(
      fml::RefPtr<blink::PlatformMessage> message) override;

  // |blink::WindowClient|
  void SetIsolateDebugName(const std::string name) override;

  // |blink::WindowClient|
  blink::FontCollection& GetFontCollection() override;

  void UpdateSemantics(blink::SemanticsUpdate* update) override {}


  FML_DISALLOW_COPY_AND_ASSIGN(RuntimeController);
};

}  // namespace unicorn

#endif  // FLUTTER_UNICORN_RUNTIME_RUNTIME_CONTROLLER_H_
