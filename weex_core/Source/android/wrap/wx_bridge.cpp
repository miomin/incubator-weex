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

#include "android/wrap/wx_bridge.h"
#include <fstream>
#include <android/bridge/impl/android_bridge.h>
#include "android/base/jni_type.h"
#include "android/base/string/string_utils.h"
#include "android/bridge/impl/android_bridge_in_multi_process.h"
#include "android/bridge/impl/android_bridge_in_multi_so.h"
#include "android/jniprebuild/jniheader/WXBridge_jni.h"
#include "android/utils/cache_utils.h"
#include "android/utils/params_utils.h"
#include "android/utils/so_utils.h"
#include "android/wrap/hash_set.h"
#include "android/wrap/wx_js_object.h"
#include "android/wrap/wx_map.h"
#include "core/config/core_environment.h"
#include "core/layout/layout.h"
#include "core/layout/measure_func_adapter_impl_android.h"
#include "core/manager/weex_core_manager.h"

using namespace WeexCore;
jlongArray jFirstScreenRenderTime = nullptr;
jlongArray jRenderFinishTime = nullptr;
static jint InitFrameworkEnv(JNIEnv* env, jobject jcaller, jstring framework,
                             jobject params, jstring cacheDir,
                             jboolean pieSupport) {
  const char* cache = env->GetStringUTFChars(cacheDir, nullptr);
  if (strlen(cache) > 0) {
    SoUtils::set_cache_dir(const_cast<char*>(cache));
  }
  SoUtils::set_pie_support(pieSupport);
  return InitFramework(env, jcaller, framework, params);
}

static void BindMeasurementToRenderObject(JNIEnv* env, jobject jcaller,
                                          jlong ptr) {
  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->BindMeasurementToRenderObject(ptr);
}

static void OnInstanceClose(JNIEnv* env, jobject jcaller, jstring instanceId) {
  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->OnInstanceClose(jString2StrFast(env, instanceId));
}

static void SetDefaultHeightAndWidthIntoRootDom(JNIEnv* env, jobject jcaller,
                                                jstring instanceId,
                                                jfloat defaultWidth,
                                                jfloat defaultHeight,
                                                jboolean isWidthWrapContent,
                                                jboolean isHeightWrapContent) {
  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->SetDefaultHeightAndWidthIntoRootDom(
          jString2StrFast(env, instanceId), defaultWidth, defaultHeight,
          isWidthWrapContent, isHeightWrapContent);
}

static void SetRenderContainerWrapContent(JNIEnv* env, jobject jcaller,
                                          jboolean wrap, jstring instanceId) {
  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->SetRenderContainerWrapContent(jString2StrFast(env, instanceId), wrap);
}

// Call on main thread
static jlongArray GetFirstScreenRenderTime(JNIEnv* env, jobject jcaller,
                                           jstring instanceId) {
  jlongArray jTemp = env->NewLongArray(3);

  std::vector<int64_t> temp =
      WeexCoreManager::getInstance()
          ->getPlatformBridge()
          ->core_side()
          ->GetFirstScreenRenderTime(jString2StrFast(env, instanceId));

  if (temp.empty()) {
    if (jFirstScreenRenderTime != nullptr) {
      env->DeleteGlobalRef(jFirstScreenRenderTime);
      jFirstScreenRenderTime = nullptr;
    }
    jFirstScreenRenderTime = static_cast<jlongArray>(env->NewGlobalRef(jTemp));
    return jFirstScreenRenderTime;
  }

  jlong ret[3];

  ret[0] = temp[0];
  ret[1] = temp[1];
  ret[2] = temp[2];
  env->SetLongArrayRegion(jTemp, 0, 3, ret);

  if (jFirstScreenRenderTime != nullptr) {
    env->DeleteGlobalRef(jFirstScreenRenderTime);
    jFirstScreenRenderTime = nullptr;
  }
  jFirstScreenRenderTime = static_cast<jlongArray>(env->NewGlobalRef(jTemp));

  env->DeleteLocalRef(jTemp);
  return jFirstScreenRenderTime;
}

// Call on main thread
static jlongArray GetRenderFinishTime(JNIEnv* env, jobject jcaller,
                                      jstring instanceId) {
  jlongArray jTemp = env->NewLongArray(3);

  std::vector<int64_t> temp =
      WeexCoreManager::getInstance()
          ->getPlatformBridge()
          ->core_side()
          ->GetRenderFinishTime(jString2StrFast(env, instanceId));
  if (temp.empty()) {
    if (jRenderFinishTime != nullptr) {
      env->DeleteGlobalRef(jRenderFinishTime);
      jRenderFinishTime = nullptr;
    }
    jRenderFinishTime = static_cast<jlongArray>(env->NewGlobalRef(jTemp));
    return jRenderFinishTime;
  }

  jlong ret[3];

  ret[0] = temp[0];
  ret[1] = temp[1];
  ret[2] = temp[2];
  env->SetLongArrayRegion(jTemp, 0, 3, ret);

  if (jRenderFinishTime != nullptr) {
    env->DeleteGlobalRef(jRenderFinishTime);
    jRenderFinishTime = nullptr;
  }
  jRenderFinishTime = static_cast<jlongArray>(env->NewGlobalRef(jTemp));

  env->DeleteLocalRef(jTemp);
  return jRenderFinishTime;
}

// Notice that this method is invoked from main thread.
static jboolean NotifyLayout(JNIEnv* env, jobject jcaller, jstring instanceId) {
  return WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->NotifyLayout(jString2StrFast(env, instanceId));
}

// Notice that this method is invoked from JS thread.
static void ForceLayout(JNIEnv* env, jobject jcaller, jstring instanceId) {
  WeexCoreManager::getInstance()->getPlatformBridge()->core_side()->ForceLayout(
      jString2StrFast(env, instanceId));
}

// call on main thread
static void SetStyleWidth(JNIEnv* env, jobject jcaller, jstring instanceId,
                          jstring ref, jfloat value) {
  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->SetStyleWidth(jString2StrFast(env, instanceId),
                      jString2StrFast(env, ref), value);
}

// call on main thread
static void SetStyleHeight(JNIEnv* env, jobject jcaller, jstring instanceId,
                           jstring ref, jfloat value) {
  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->SetStyleHeight(jString2StrFast(env, instanceId),
                       jString2StrFast(env, ref), value);
}

static void SetMargin(JNIEnv* env, jobject jcaller, jstring instanceId,
                      jstring ref, jint edge, jfloat value) {
  WeexCoreManager::getInstance()->getPlatformBridge()->core_side()->SetMargin(
      jString2StrFast(env, instanceId), jString2StrFast(env, ref), edge, value);
}

static void SetPadding(JNIEnv* env, jobject jcaller, jstring instanceId,
                       jstring ref, jint edge, jfloat value) {
  WeexCoreManager::getInstance()->getPlatformBridge()->core_side()->SetPadding(
      jString2StrFast(env, instanceId), jString2StrFast(env, ref), edge, value);
}

static void SetPosition(JNIEnv* env, jobject jcaller, jstring instanceId,
                        jstring ref, jint edge, jfloat value) {
  WeexCoreManager::getInstance()->getPlatformBridge()->core_side()->SetPosition(
      jString2StrFast(env, instanceId), jString2StrFast(env, ref), edge, value);
}

static void MarkDirty(JNIEnv* env, jobject jcaller, jstring instanceId,
                      jstring ref, jboolean dirty) {
  WeexCoreManager::getInstance()->getPlatformBridge()->core_side()->MarkDirty(
      jString2StrFast(env, instanceId), jString2StrFast(env, ref));
}

static void RegisterCoreEnv(JNIEnv* env, jobject jcaller, jstring key,
                            jstring value) {
  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->RegisterCoreEnv(jString2StrFast(env, key), jString2StrFast(env, value));
}

static void SetViewPortWidth(JNIEnv* env, jobject jcaller, jstring instanceId,
                             jfloat value) {
  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->SetViewPortWidth(jString2StrFast(env, instanceId), value);
}

static jint InitFramework(JNIEnv* env, jobject object, jstring script,
                          jobject params) {
  WXBridge::Instance()->Reset(env, object);
  // Init platform thread --- ScriptThread
  WeexCoreManager::getInstance()->InitScriptThread();
  // Exception handler for so
  SoUtils::RegisterExceptionHanler(
      [](const char* status_code, const char* error_msg) {
        WeexCoreManager::getInstance()
            ->getPlatformBridge()
            ->platform_side()
            ->ReportNativeInitStatus(status_code, error_msg);
      });
  std::vector<INIT_FRAMEWORK_PARAMS*> params_vector =
      initFromParam(env, params);
  // Init platform bridge
  Bridge_Impl_Android* bridge = new AndroidBridgeInSimple;
  Bridge_Impl_Android::InitInstance(bridge);
  WeexCoreManager::getInstance()->setPlatformBridge(bridge);
  // Set project mode
  if (isSingleProcess()) {
    WeexCoreManager::getInstance()->set_project_mode(WeexCoreManager::ProjectMode::MULTI_SO);
  } else {
    WeexCoreManager::getInstance()->set_project_mode(WeexCoreManager::ProjectMode::MULTI_PROCESS);
  }
  // for environment
  bridge->core_side()->SetPlatform(
      WXCoreEnvironment::getInstance()->platform());
  bridge->core_side()->SetDeviceWidthAndHeight(
      WXCoreEnvironment::getInstance()->DeviceWidth(),
      WXCoreEnvironment::getInstance()->DeviceHeight());
  auto options = WXCoreEnvironment::getInstance()->options();
  auto it = options.begin();
  for (; it != options.end(); it++) {
    bridge->core_side()->AddOption(it->first, it->second);
  }
  // Set measure function
  WeexCoreManager::getInstance()->SetMeasureFunctionAdapter(
      new MeasureFunctionAdapterImplAndroid());
  bridge->core_side()->SetMeasureFunctionAdapter();
  char* c_script = const_cast<char*>(env->GetStringUTFChars(script, JNI_FALSE));
  // Call InitFramework
  auto result = bridge->core_side()->InitFramework(c_script, params_vector);
  env->ReleaseStringUTFChars(script, c_script);
  return result;
}

static jint ExecJSService(JNIEnv* env, jobject object, jstring script) {
  if (script == nullptr) return false;
  ScopedJStringUTF8 source(env, script);
  return WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->ExecJsService(source.getChars());
}

static void TakeHeapSnapshot(JNIEnv* env, jobject object, jstring name) {}

/**
 * Called to execute JavaScript such as . createInstance(),destroyInstance ext.
 *
 */
static jint ExecJS(JNIEnv* env, jobject jthis, jstring jinstanceid,
                   jstring jnamespace, jstring jfunction, jobjectArray jargs) {
  if (jfunction == NULL || jinstanceid == NULL) {
    LOGE("native_execJS function is NULL");
    return false;
  }

  ScopedJStringUTF8 instance_id(env, jinstanceid);
  ScopedJStringUTF8 name_space(env, jnamespace);
  ScopedJStringUTF8 function(env, jfunction);
  int length = 0;
  if (jargs != nullptr) {
    length = env->GetArrayLength(jargs);
  }
  std::vector<VALUE_WITH_TYPE*> params;

  for (int i = 0; i < length; i++) {
    VALUE_WITH_TYPE* param = nullptr;

    param = WeexCore::getValueWithTypePtr();
    auto jni_object = base::android::ScopedLocalJavaRef<jobject>(
        env, env->GetObjectArrayElement(jargs, i));
    auto wx_js_object =
        std::unique_ptr<WXJSObject>(new WXJSObject(env, jni_object.Get()));
    addParamsFromJArgs(params, param, env, wx_js_object);
  }
  auto result =
      WeexCoreManager::getInstance()->getPlatformBridge()->core_side()->ExecJS(
          instance_id.getChars(), name_space.getChars(), function.getChars(),
          params);

  freeParams(params);
  return result;
}

static jbyteArray ExecJSWithResult(JNIEnv* env, jobject jcaller,
                                   jstring instanceId, jstring _namespace,
                                   jstring _function, jobjectArray args) {
  if (_function == NULL || instanceId == NULL) {
    LOGE("native_execJS function is NULL");
    return NULL;
  }
  ScopedJStringUTF8 instance_id(env, instanceId);
  ScopedJStringUTF8 name_space(env, _namespace);
  ScopedJStringUTF8 function(env, _function);
  int length = 0;
  if (args != NULL) {
    length = env->GetArrayLength(args);
  }
  std::vector<VALUE_WITH_TYPE*> params;

  for (int i = 0; i < length; i++) {
    VALUE_WITH_TYPE* param = nullptr;

    param = getValueWithTypePtr();

    auto wx_js_object = std::unique_ptr<WXJSObject>(
        new WXJSObject(env, base::android::ScopedLocalJavaRef<jobject>(
                                env, env->GetObjectArrayElement(args, i))
                                .Get()));
    addParamsFromJArgs(params, param, env, wx_js_object);
  }

  auto result =
      WeexCoreManager::getInstance()
          ->getPlatformBridge()
          ->core_side()
          ->ExecJSWithResult(instance_id.getChars(), name_space.getChars(),
                             function.getChars(), params);
  jbyteArray array = env->NewByteArray(result.length);
  env->SetByteArrayRegion(array, 0, result.length,
                          reinterpret_cast<const jbyte*>(result.data));
  freeParams(params);
  return array;
}

static void UpdateGlobalConfig(JNIEnv* env, jobject jcaller, jstring config) {
  if (config == NULL) {
    LOGE("native_execJS function is NULL");
    return;
  }
  ScopedJStringUTF8 scoped_config(env, config);
  WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->UpdateGlobalConfig(scoped_config.getChars());
}

static jint CreateInstanceContext(JNIEnv* env, jobject jcaller,
                                  jstring instanceId, jstring name,
                                  jstring function, jobjectArray args) {
  if (function == NULL || instanceId == NULL) {
    LOGE("native_createInstanceContext function is NULL");
    return false;
  }
  int length = 0;
  if (args != NULL) {
    length = env->GetArrayLength(args);
  }
  if (length < 4) {
    LOGE("native_createInstanceContext jargs format error");
    return false;
  }
  auto arg1 = std::unique_ptr<WXJSObject>(
      new WXJSObject(env, base::android::ScopedLocalJavaRef<jobject>(
                              env, env->GetObjectArrayElement(args, 1))
                              .Get()));
  auto jscript = arg1->GetData(env);
  auto opts = base::android::ScopedLocalJavaRef<jstring>(
      env, getJsonData(env, args, 2));
  // init jsonData
  auto initData = base::android::ScopedLocalJavaRef<jstring>(
      env, getJsonData(env, args, 3));
  auto arg4 = std::unique_ptr<WXJSObject>(
      new WXJSObject(env, base::android::ScopedLocalJavaRef<jobject>(
                              env, env->GetObjectArrayElement(args, 4))
                              .Get()));
  auto japi = arg4->GetData(env);
  ScopedJStringUTF8 scoped_id(env, instanceId);
  ScopedJStringUTF8 scoped_func(env, function);
  ScopedJStringUTF8 scoped_script(env, static_cast<jstring>(jscript.Get()));
  ScopedJStringUTF8 scoped_opts(env, opts.Get());
  ScopedJStringUTF8 scoped_init_data(env, initData.Get());
  ScopedJStringUTF8 scoped_api(env, static_cast<jstring>(japi.Get()));

  return WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->CreateInstance(scoped_id.getChars(), scoped_func.getChars(),
                       scoped_script.getChars(), scoped_opts.getChars(),
                       scoped_init_data.getChars(), scoped_api.getChars());
}

static jint DestoryInstance(JNIEnv* env, jobject jcaller, jstring instanceId,
                            jstring name, jstring function, jobjectArray args) {
  int ret = ExecJS(env, nullptr, instanceId, name, function, args);
  if (function == NULL || instanceId == NULL) {
    LOGE("native_destoryInstance function is NULL");
    return false;
  }

  ScopedJStringUTF8 idChar(env, instanceId);
  return WeexCoreManager::getInstance()
      ->getPlatformBridge()
      ->core_side()
      ->DestroyInstance(idChar.getChars());
}

static jstring ExecJSOnInstance(JNIEnv* env, jobject jcaller,
                                jstring instanceId, jstring script, jint type) {
  if (instanceId == NULL || script == NULL) {
    return env->NewStringUTF("");
  }

  ScopedJStringUTF8 idChar(env, instanceId);
  ScopedJStringUTF8 scriptChar(env, script);
  auto result =
      WeexCoreManager::getInstance()
          ->getPlatformBridge()
          ->core_side()
          ->ExecJSOnInstance(idChar.getChars(), scriptChar.getChars());
  return env->NewStringUTF(result);
}

namespace WeexCore {

WXBridge* WXBridge::g_instance = nullptr;

bool WXBridge::RegisterJNIUtils(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

base::android::ScopedLocalJavaRef<jobject> WXBridge::GetMeasureFunc(
    JNIEnv* env, const char* page_id, jlong render_object_ptr) {
  jstring jni_page_id = getKeyFromCache(env, page_id);
  return Java_WXBridge_getMeasurementFunc(env, jni_object(), jni_page_id,
                                          render_object_ptr);
}

int WXBridge::HasTransitionPros(
    JNIEnv* env, const char* page_id, const char* ref,
    const std::vector<std::pair<std::string, std::string>>& styles) {
  jstring jni_page_id = getKeyFromCache(env, page_id);
  jstring jni_ref = getKeyFromCache(env, ref);

  auto styles_map = std::unique_ptr<WXMap>(new WXMap);
  if (!styles.empty()) {
    styles_map->Put(env, styles);
  }

  return Java_WXBridge_callHasTransitionPros(env, jni_object(), jni_page_id,
                                             jni_ref, styles_map->jni_object());
}

int WXBridge::AppendTreeCreateFinish(JNIEnv* env, const char* page_id,
                                     const char* ref) {
  jstring jni_page_id = getKeyFromCache(env, page_id);
  jstring jni_ref = getKeyFromCache(env, ref);
  return Java_WXBridge_callAppendTreeCreateFinish(env, jni_object(),
                                                  jni_page_id, jni_ref);
}

int WXBridge::MoveElement(JNIEnv* env, const char* page_id, const char* ref,
                          const char* parent_ref, int index) {
  jstring jni_page_id = getKeyFromCache(env, page_id);
  jstring jni_ref = getKeyFromCache(env, ref);
  jstring jni_parent_ref = getKeyFromCache(env, parent_ref);

  return Java_WXBridge_callMoveElement(env, jni_object(), jni_page_id, jni_ref,
                                       jni_parent_ref, index);
}

int WXBridge::RemoveElement(JNIEnv* env, const char* page_id, const char* ref) {
  jstring jni_page_id = getKeyFromCache(env, page_id);
  jstring jni_ref = getKeyFromCache(env, ref);

  return Java_WXBridge_callRemoveElement(env, jni_object(), jni_page_id,
                                         jni_ref);
}

int WXBridge::CreateFinish(JNIEnv* env, const char* page_id) {
  jstring jni_page_id = getKeyFromCache(env, page_id);
  return Java_WXBridge_callCreateFinish(env, jni_object(), jni_page_id);
}

int WXBridge::UpdateAttr(
    JNIEnv* env, const char* page_id, const char* ref,
    std::vector<std::pair<std::string, std::string>>* attrs) {
  jstring jni_page_id = getKeyFromCache(env, page_id);
  jstring jni_ref = getKeyFromCache(env, ref);

  auto attributes_map = std::unique_ptr<WXMap>();
  if (attrs != nullptr) {
    attributes_map.reset(new WXMap);
    attributes_map->Put(env, *attrs);
  }

  jobject jni_attributes =
      attributes_map.get() != nullptr ? attributes_map->jni_object() : nullptr;
  return Java_WXBridge_callUpdateAttrs(env, jni_object(), jni_page_id, jni_ref,
                                       jni_attributes);
}

int WXBridge::UpdateStyle(
    JNIEnv* env, const char* page_id, const char* ref,
    std::vector<std::pair<std::string, std::string>>* style,
    std::vector<std::pair<std::string, std::string>>* margin,
    std::vector<std::pair<std::string, std::string>>* padding,
    std::vector<std::pair<std::string, std::string>>* border) {
  jstring jni_page_id = getKeyFromCache(env, page_id);
  jstring jni_ref = getKeyFromCache(env, ref);

  auto styles_map = std::unique_ptr<WXMap>();
  if (style != nullptr) {
    styles_map.reset(new WXMap);
    styles_map->Put(env, *style);
  }
  auto margins_map = std::unique_ptr<WXMap>();
  if (margin != nullptr) {
    margins_map.reset(new WXMap);
    margins_map->Put(env, *margin);
  }
  auto paddings_map = std::unique_ptr<WXMap>();
  if (padding != nullptr) {
    paddings_map.reset(new WXMap);
    paddings_map->Put(env, *padding);
  }
  auto borders_map = std::unique_ptr<WXMap>();
  if (border != nullptr) {
    borders_map.reset(new WXMap);
    borders_map->Put(env, *border);
  }

  jobject jni_styles =
      styles_map.get() != nullptr ? styles_map->jni_object() : nullptr;
  jobject jni_margins =
      margins_map.get() != nullptr ? margins_map->jni_object() : nullptr;
  jobject jni_paddings =
      paddings_map.get() != nullptr ? paddings_map->jni_object() : nullptr;
  jobject jni_borders =
      borders_map.get() != nullptr ? borders_map->jni_object() : nullptr;

  return Java_WXBridge_callUpdateStyle(env, jni_object(), jni_page_id, jni_ref,
                                       jni_styles, jni_margins, jni_paddings,
                                       jni_borders);
}

int WXBridge::Layout(JNIEnv* env, const char* page_id, const char* ref, int top,
                     int bottom, int left, int right, int height, int width,
                     int index) {
  jstring jPageId = getKeyFromCache(env, page_id);
  jstring jRef = getKeyFromCache(env, ref);

  return Java_WXBridge_callLayout(env, jni_object(), jPageId, jRef, top, bottom,
                                  left, right, height, width, index);
}

int WXBridge::AddElement(JNIEnv* env, const char* page_id,
                         const char* component_type, const char* ref,
                         int& index, const char* parentRef,
                         std::map<std::string, std::string>* styles,
                         std::map<std::string, std::string>* attributes,
                         std::set<std::string>* events,
                         const WXCoreMargin& margins,
                         const WXCorePadding& paddings,
                         const WXCoreBorderWidth& borders, bool willLayout) {
  jstring jni_page_id = getKeyFromCache(env, page_id);
  jstring jni_ref = getKeyFromCache(env, ref);
  jstring jni_parent_ref = getKeyFromCache(env, parentRef);

  auto styles_map = std::unique_ptr<WXMap>(new WXMap);
  styles_map->Put(env, *styles);
  auto attributes_map = std::unique_ptr<WXMap>(new WXMap);
  attributes_map->Put(env, *attributes);
  auto events_set = std::unique_ptr<HashSet>(new HashSet);
  events_set->Add(env, *events);

  float c_margins[4];
  float c_paddings[4];
  float c_borders[4];

  c_margins[0] = margins.getMargin(kMarginTop);
  c_margins[1] = margins.getMargin(kMarginBottom);
  c_margins[2] = margins.getMargin(kMarginLeft);
  c_margins[3] = margins.getMargin(kMarginRight);

  c_paddings[0] = paddings.getPadding(kPaddingTop);
  c_paddings[1] = paddings.getPadding(kPaddingBottom);
  c_paddings[2] = paddings.getPadding(kPaddingLeft);
  c_paddings[3] = paddings.getPadding(kPaddingRight);

  c_borders[0] = borders.getBorderWidth(kBorderWidthTop);
  c_borders[1] = borders.getBorderWidth(kBorderWidthBottom);
  c_borders[2] = borders.getBorderWidth(kBorderWidthLeft);
  c_borders[3] = borders.getBorderWidth(kBorderWidthRight);

  auto jni_margins =
      0 == c_margins[0] && 0 == c_margins[1] && 0 == c_margins[2] &&
              0 == c_margins[3]
          ? base::android::ScopedLocalJavaRef<jfloatArray>()
          : base::android::JNIType::NewFloatArray(env, 4, c_margins);
  auto jni_paddings =
      0 == c_paddings[0] && 0 == c_paddings[1] && 0 == c_paddings[2] &&
              0 == c_paddings[3]
          ? base::android::ScopedLocalJavaRef<jfloatArray>()
          : base::android::JNIType::NewFloatArray(env, 4, c_paddings);
  auto jni_borders =
      0 == c_borders[0] && 0 == c_borders[1] && 0 == c_borders[2] &&
              0 == c_borders[3]
          ? base::android::ScopedLocalJavaRef<jfloatArray>()
          : base::android::JNIType::NewFloatArray(env, 4, c_borders);

  jstring jni_component_type = getComponentTypeFromCache(component_type);
  if (jni_component_type == nullptr) {
    jni_component_type = putComponentTypeToCache(component_type);
  }

  return Java_WXBridge_callAddElement(
      env, jni_object(), jni_page_id, jni_component_type, jni_ref, index,
      jni_parent_ref, styles_map->jni_object(), attributes_map->jni_object(),
      events_set->jni_object(), jni_margins.Get(), jni_paddings.Get(),
      jni_borders.Get(), willLayout);
}

int WXBridge::CreateBody(JNIEnv* env, const char* page_id,
                         const char* component_type, const char* ref,
                         std::map<std::string, std::string>* styles,
                         std::map<std::string, std::string>* attributes,
                         std::set<std::string>* events,
                         const WXCoreMargin& margins,
                         const WXCorePadding& paddings,
                         const WXCoreBorderWidth& borders) {
  jstring jni_pageId = getKeyFromCache(env, page_id);
  jstring jni_ref = getKeyFromCache(env, ref);

  auto styles_map = std::unique_ptr<WXMap>(new WXMap);
  styles_map->Put(env, *styles);
  auto attributes_map = std::unique_ptr<WXMap>(new WXMap);
  attributes_map->Put(env, *attributes);
  auto events_set = std::unique_ptr<HashSet>(new HashSet);
  events_set->Add(env, *events);

  float c_margins[4];
  float c_paddings[4];
  float c_borders[4];

  c_margins[0] = margins.getMargin(kMarginTop);
  c_margins[1] = margins.getMargin(kMarginBottom);
  c_margins[2] = margins.getMargin(kMarginLeft);
  c_margins[3] = margins.getMargin(kMarginRight);

  c_paddings[0] = paddings.getPadding(kPaddingTop);
  c_paddings[1] = paddings.getPadding(kPaddingBottom);
  c_paddings[2] = paddings.getPadding(kPaddingLeft);
  c_paddings[3] = paddings.getPadding(kPaddingRight);

  c_borders[0] = borders.getBorderWidth(kBorderWidthTop);
  c_borders[1] = borders.getBorderWidth(kBorderWidthBottom);
  c_borders[2] = borders.getBorderWidth(kBorderWidthLeft);
  c_borders[3] = borders.getBorderWidth(kBorderWidthRight);

  auto jni_margins =
      0 == c_margins[0] && 0 == c_margins[1] && 0 == c_margins[2] &&
              0 == c_margins[3]
          ? base::android::ScopedLocalJavaRef<jfloatArray>()
          : base::android::JNIType::NewFloatArray(env, 4, c_margins);
  auto jni_paddings =
      0 == c_paddings[0] && 0 == c_paddings[1] && 0 == c_paddings[2] &&
              0 == c_paddings[3]
          ? base::android::ScopedLocalJavaRef<jfloatArray>()
          : base::android::JNIType::NewFloatArray(env, 4, c_paddings);
  auto jni_borders =
      0 == c_borders[0] && 0 == c_borders[1] && 0 == c_borders[2] &&
              0 == c_borders[3]
          ? base::android::ScopedLocalJavaRef<jfloatArray>()
          : base::android::JNIType::NewFloatArray(env, 4, c_borders);

  jstring jni_component_type = getComponentTypeFromCache(component_type);
  if (jni_component_type == nullptr) {
    jni_component_type = putComponentTypeToCache(component_type);
  }

  int flag = Java_WXBridge_callCreateBody(
      env, jni_object(), jni_pageId, jni_component_type, jni_ref,
      styles_map->jni_object(), attributes_map->jni_object(),
      events_set->jni_object(), jni_margins.Get(), jni_paddings.Get(),
      jni_borders.Get());
  return flag;
}

int WXBridge::RemoveEvent(JNIEnv* env, const char* page_id, const char* ref,
                          const char* event) {
  jstring jPageId = getKeyFromCache(env, page_id);
  jstring jRef = getKeyFromCache(env, ref);

  auto jEventId =
      base::android::ScopedLocalJavaRef<jstring>(env, env->NewStringUTF(event));

  return Java_WXBridge_callRemoveEvent(env, jni_object(), jPageId, jRef,
                                       jEventId.Get());
}

int WXBridge::AddEvent(JNIEnv* env, const char* page_id, const char* ref,
                       const char* event) {
  jstring jPageId = getKeyFromCache(env, page_id);
  jstring jRef = getKeyFromCache(env, ref);

  auto jEventId =
      base::android::ScopedLocalJavaRef<jstring>(env, env->NewStringUTF(event));
  return Java_WXBridge_callAddEvent(env, jni_object(), jPageId, jRef,
                                    jEventId.Get());
}

int WXBridge::RefreshFinish(JNIEnv* env, const char* page_id, const char* task,
                            const char* callback) {
  auto jTask = base::android::ScopedLocalJavaRef<jbyteArray>(
      env, newJByteArray(env, task));
  auto jCallback = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(callback));
  jstring jPageId = getKeyFromCache(env, page_id);
  return Java_WXBridge_callRefreshFinish(env, jni_object(), jPageId,
                                         jTask.Get(), jCallback.Get());
}

int WXBridge::UpdateFinish(JNIEnv* env, const char* page_id, const char* task,
                           const char* callback) {
  auto jTask = base::android::ScopedLocalJavaRef<jbyteArray>(
      env, newJByteArray(env, task));
  auto jCallback = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(callback));
  jstring jPageId = getKeyFromCache(env, page_id);

  return Java_WXBridge_callUpdateFinish(env, jni_object(), jPageId, jTask.Get(),
                                        jCallback.Get());
}

void WXBridge::SetTimeout(JNIEnv* env, const char* callback_id,
                          const char* time) {
  auto jCallbackID = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(callback_id));
  auto jTime =
      base::android::ScopedLocalJavaRef<jstring>(env, env->NewStringUTF(time));

  Java_WXBridge_setTimeoutNative(env, jni_object(), jCallbackID.Get(),
                                 jTime.Get());
}

void WXBridge::CallNativeComponent(JNIEnv* env, const char* page_id,
                                   const char* ref, const char* method,
                                   const char* arguments, int arguments_length,
                                   const char* options, int options_length) {
  auto jMethod = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(method));
  auto jArgString = base::android::ScopedLocalJavaRef<jbyteArray>(
      env, newJByteArray(env, arguments, arguments_length));
  auto jOptString = base::android::ScopedLocalJavaRef<jbyteArray>(
      env, newJByteArray(env, options, options_length));
  jstring jPageId = getKeyFromCache(env, page_id);
  jstring jRef = getKeyFromCache(env, ref);

  if (jMethod.IsNull()) {
    Java_WXBridge_callNativeComponent(env, jni_object(), jPageId, jRef,
                                      jMethod.Get(), jArgString.Get(),
                                      jOptString.Get());
  }
}

base::android::ScopedLocalJavaRef<jobject> WXBridge::CallNativeModule(
    JNIEnv* env, const char* page_id, const char* module, const char* method,
    const char* arguments, int arguments_length, const char* options,
    int options_length) {
  auto jModule = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(module));
  auto jMethod = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(method));
  auto jArgString = base::android::ScopedLocalJavaRef<jbyteArray>(
      env, newJByteArray(env, arguments, arguments_length));
  auto jOptString = base::android::ScopedLocalJavaRef<jbyteArray>(
      env, newJByteArray(env, options, options_length));
  jstring jPageId = getKeyFromCache(env, page_id);

  if (!jModule.IsNull() && !jMethod.IsNull()) {
    return Java_WXBridge_callNativeModule(env, jni_object(), jPageId,
                                          jModule.Get(), jMethod.Get(),
                                          jArgString.Get(), jOptString.Get());
  }
  return base::android::ScopedLocalJavaRef<jobject>();
}

int WXBridge::CallNative(JNIEnv* env, const char* page_id, const char* task,
                         const char* callback) {
  auto jTask = base::android::ScopedLocalJavaRef<jbyteArray>(
      env, newJByteArray(env, task));
  auto jCallback = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(callback));
  jstring jPageId = getKeyFromCache(env, page_id);

  if (!jTask.IsNull()) {
    return Java_WXBridge_callNative(env, jni_object(), jPageId, jTask.Get(),
                                    jCallback.Get());
  }
  return -1;
}

void WXBridge::ReportException(JNIEnv* env, const char* page_id,
                               const char* func, const char* exception_string) {
  auto jFunc =
      base::android::ScopedLocalJavaRef<jstring>(env, env->NewStringUTF(func));
  auto jExceptionString = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(exception_string));
  jstring jPageId = getKeyFromCache(env, page_id);

  Java_WXBridge_reportJSException(env, jni_object(), jPageId, jFunc.Get(),
                                  jExceptionString.Get());
}

void WXBridge::SetJSFrmVersion(JNIEnv* env, const char* version) {
  auto jVersion = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(version));
  Java_WXBridge_setJSFrmVersion(env, jni_object(), jVersion.Get());
}

void WXBridge::ReportServerCrash(JNIEnv* env, const char* instance_id,
                                 const char* crash_file) {
  auto jni_instance_id = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(instance_id));
  auto jni_crash_file = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(crash_file));
  Java_WXBridge_reportServerCrash(env, jni_object(), jni_instance_id.Get(),
                                  jni_crash_file.Get());
}

void WXBridge::ReportNativeInitStatus(JNIEnv* env, const char* statusCode,
                                      const char* errorMsg) {
  auto jni_status_code = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(statusCode));
  auto jni_error_msg = base::android::ScopedLocalJavaRef<jstring>(
      env, env->NewStringUTF(errorMsg));
  Java_WXBridge_reportNativeInitStatus(env, jni_object(), jni_status_code.Get(),
                                       jni_error_msg.Get());
}

}  // namespace WeexCore