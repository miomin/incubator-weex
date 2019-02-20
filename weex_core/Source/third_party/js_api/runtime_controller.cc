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

#include "flutter/unicorn/runtime/runtime_controller.h"

#include <utility>
#include "flutter/fml/message_loop.h"
#include "flutter/fml/trace_event.h"
#include "flutter/lib/ui/compositing/scene.h"
#include "flutter/lib/ui/ui_state.h"
#include "flutter/lib/ui/window/window.h"
#include "flutter/runtime/runtime_delegate.h"
#include "flutter/unicorn/ui/window.h"

#ifdef ERROR
#undef ERROR
#endif

namespace unicorn {

RuntimeController::RuntimeController(
    blink::RuntimeDelegate& p_client,
    blink::TaskRunners p_task_runners,
    fml::WeakPtr<blink::SnapshotDelegate> p_snapshot_delegate,
    fml::WeakPtr<GrContext> p_resource_context,
    fml::RefPtr<flow::SkiaUnrefQueue> p_unref_queue)
    : client_(p_client),
      task_runners_(p_task_runners),
      snapshot_delegate_(p_snapshot_delegate),
      resource_context_(p_resource_context),
      unref_queue_(p_unref_queue),
      vm_(RuntimeVM::ForProcess()) {
  CreateJavaScriptContext();
  ui_state_ = blink::UIState::CreateInstance(
      p_task_runners, nullptr, nullptr, p_snapshot_delegate, resource_context_,
      std::make_unique<blink::Window>(
          this, (blink::FrameworkClient*)(unicorn::Window::GetInstance())),
      p_unref_queue, "", "", "");
}

RuntimeController::~RuntimeController() {}

bool RuntimeController::SetViewportMetrics(
    const blink::ViewportMetrics& metrics) {
  if (auto window = GetWindowIfAvailable()) {
    window->UpdateWindowMetrics(metrics);
    return true;
  }
  return false;
}

bool RuntimeController::SetLocales(
    const std::vector<std::string>& locale_data) {
  if (auto window = GetWindowIfAvailable()) {
    window->UpdateLocales(locale_data);
    return true;
  }
  return true;
}

bool RuntimeController::SetUserSettingsData(const std::string& data) {
  if (auto window = GetWindowIfAvailable()) {
    return true;
  }

  return false;
}

bool RuntimeController::SetSemanticsEnabled(bool enabled) {
  if (auto window = GetWindowIfAvailable()) {
    return true;
  }

  return false;
}

bool RuntimeController::SetAccessibilityFeatures(int32_t flags) {
  if (auto window = GetWindowIfAvailable()) {
    return true;
  }

  return false;
}

bool RuntimeController::BeginFrame(fml::TimePoint frame_time) {
  if (auto window = GetWindowIfAvailable()) {
    window->BeginFrame(frame_time);
    return true;
  }
  return false;
}

bool RuntimeController::NotifyIdle(int64_t deadline) {
  // TODO(unicorn): to see if it is needed anymore.
  return false;
}

bool RuntimeController::DispatchPlatformMessage(
    fml::RefPtr<blink::PlatformMessage> message) {
  if (Available())
    return true;
  if (auto window = GetWindowIfAvailable()) {
    TRACE_EVENT1("flutter", "RuntimeController::DispatchPlatformMessage",
                 "mode", "basic");
    window->DispatchPlatformMessage(std::move(message));
    return true;
  }
  return false;
}

bool RuntimeController::DispatchPointerDataPacket(
    const blink::PointerDataPacket& packet) {
  if (auto window = GetWindowIfAvailable()) {
    TRACE_EVENT1("flutter", "RuntimeController::DispatchPointerDataPacket",
                 "mode", "basic");
    window->DispatchPointerDataPacket(packet);
    return true;
  }
  return false;
}

bool RuntimeController::DispatchSemanticsAction(int32_t id,
                                                blink::SemanticsAction action,
                                                std::vector<uint8_t> args) {
  TRACE_EVENT1("flutter", "RuntimeController::DispatchSemanticsAction", "mode",
               "basic");
  if (auto window = GetWindowIfAvailable()) {
    window->DispatchSemanticsAction(id, action, std::move(args));
    return true;
  }
  return false;
}

void RuntimeController::CreateJavaScriptContext() {
  TRACE_EVENT1("flutter", "RuntimeController::CreateJavaScriptContext", "mode",
               "basic");
  if (!main_context_)
    main_context_ = RuntimeContext::Create(vm_.get());
}

void RuntimeController::RunJavaScript(const std::string& content) {
  if (main_context_ && main_context_->Available())
    main_context_->ExecuteJavaScript(content);
}

void RuntimeController::Shutdown() {
  vm_->Shutdown();
}

void RuntimeController::Reload() {}

bool RuntimeController::Available() {
  return main_context_ ? main_context_->Available() : false;
}

blink::Window* RuntimeController::GetWindowIfAvailable() {
  return blink::UIState::Current()->window();
}

std::string RuntimeController::DefaultRouteName() {
  return client_.DefaultRouteName();
}

void RuntimeController::ScheduleFrame() {
  client_.ScheduleFrame();
}

void RuntimeController::Render(blink::Scene* scene) {
  client_.Render(scene->takeLayerTree());
}

// void RuntimeController::UpdateSemantics(blink::SemanticsUpdate* update) {}

void RuntimeController::HandlePlatformMessage(
    fml::RefPtr<blink::PlatformMessage> message) {
  client_.HandlePlatformMessage(std::move(message));
}

void RuntimeController::SetIsolateDebugName(const std::string name) {}

blink::FontCollection& RuntimeController::GetFontCollection() {
  return client_.GetFontCollection();
}

}  // namespace unicorn
