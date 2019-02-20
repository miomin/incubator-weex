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

#include <string>
#include <vector>

#include "flutter/fml/logging.h"
#include "flutter/unicorn/runtime/binding_macro.h"
#include "flutter/unicorn/runtime/engine_context.h"
#include "flutter/unicorn/runtime/runtime_context.h"
#include "flutter/unicorn/runtime/runtime_controller.h"
#include "flutter/unicorn/runtime/runtime_object.h"
#include "flutter/unicorn/runtime/runtime_values.h"
#include "gtest/gtest.h"

namespace unicorn {

class RuntimeTest : public RuntimeObject{
 public:
  explicit RuntimeTest(EngineContext* context);
  ~RuntimeTest() = default;

  static void Register(EngineContext*);
  void AddNum(const std::vector<RuntimeValues>&);
  int GetNumber();
  void SetNumber(int value);
  void MakeObject();

 private:
  void AddNum();
  int num_;
};

namespace {
  GLOBAL_CLASS_REF(RuntimeTest)
  CLASS_OBJECT_REGISTER(RuntimeTest)
  CLASS_CONSTRUCTOR_CALLBACK_FUNCTION(RuntimeTest)
  MEMBER_GET_CALLBACK_FUNCTION(RuntimeTest, Number)
  MEMBER_SET_CALLBACK_FUNCTION(RuntimeTest, Number)
  MEMBER_VOID_METHOD_CALLBACK_FUNCTION(RuntimeTest, AddNum)

  CLASS_REGISTER_START(RuntimeTest)
    CLASS_CONSTRUCTOR_ENABLE(true)
    REGISTER_SET_AND_GET_CALLBACK(RuntimeTest, Number, true, true)
    REGISTER_METHOD_CALLBACK(RuntimeTest, AddNum)
  CLASS_REGISTER_END(RuntimeTest)
}  // namespace

RuntimeTest::RuntimeTest(EngineContext* context)
      : RuntimeObject(context),
        num_(0) {
  FML_LOG(ERROR) << "create native Test";
}

void RuntimeTest::MakeObject() {
  RegisterNativeObjectToJSObject(GetEngineContext(),
                                 "g_test", static_cast<void*>(this));
}
void RuntimeTest::Register(EngineContext* context) {
  CreateJSCClassRef(context);
}

void RuntimeTest::AddNum(const std::vector<RuntimeValues>& vars) {
  AddNum();
}

void RuntimeTest::AddNum() {
  num_++;
  FML_LOG(ERROR) << "add num num_: " << num_;
}

int RuntimeTest::GetNumber() {
  FML_LOG(ERROR) << "Test::GetNum num_: " << num_;
  return num_;
}

void RuntimeTest::SetNumber(int value) {
  num_ = value;
  FML_LOG(ERROR) << "Test::SetNum num_: " << num_;
}

//////////////////////////////////////////////////////////////////////////////

TEST(RuntimeTest, RegisterClassAndTestFunction) {
  auto vm = RuntimeVM::ForProcess();
  auto context = RuntimeContext::Create(vm);
  RuntimeTest::Register(context->GetEngineContext());
  RuntimeTest* t = new RuntimeTest(context->GetEngineContext());
  t->MakeObject();
  std::string source1("function testTest() { if (g_test) \
          { var n = g_test.number; g_test.addNum(); g_test.number = 6;}}");
  source1.append("testTest()");
  context->ExecuteJavaScript(source1);
}

}  // namespace unicorn
