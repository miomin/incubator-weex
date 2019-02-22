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
#include "flutter/unicorn/js_runtime/runtime/test.h"
#include <utility>

#include "flutter/unicorn/js_runtime/runtime/binding_macro.h"

namespace unicorn {

CLASS_METHOD_CALLBACK(Address, Path)

CLASS_REGISTER_START(Address)
  REGISTER_METHOD_CALLBACK(Address, Path)
CLASS_REGISTER_END(Address)

Address::Address() : RuntimeObject(nullptr) {
  FML_LOG(ERROR) << "Address::Address create";
  SetJSClass(Address::s_jsclass_);
}

Address::Address(const Address& other) : RuntimeObject(nullptr) {
  FML_LOG(ERROR) << "Address::Address copy construct";
  SetJSClass(Address::s_jsclass_);
}

void Address::Path() {
  FML_LOG(ERROR) << " path is alibaba ";
}

CLASS_METHOD_CALLBACK(Base, Change)
CLASS_REGISTER_START(Base)
    REGISTER_METHOD_CALLBACK(Base, Change)
CLASS_REGISTER_END(Base)

ScopeValues Base::Change(std::vector<unicorn::ScopeValues>& args) {
  FML_LOG(ERROR) << "Base::Change";
  return RuntimeValues::MakeUndefined();
}

  // constructor or finalize callback define
  CLASS_CONSTRUCTOR_CALLBACK(RuntimeTest)
  CLASS_FINALIZE_CALLBACK(RuntimeTest)

  // member fields set or get callback define
  CLASS_MEMBER_SET_CALLBACK(RuntimeTest, Number)
  CLASS_MEMBER_GET_CALLBACK(RuntimeTest, Number)
  CLASS_MEMBER_SET_CALLBACK(RuntimeTest, Location)
  CLASS_MEMBER_GET_CALLBACK(RuntimeTest, Location)
  // method fields callback define
  CLASS_METHOD_CALLBACK(RuntimeTest, AddNum)
  CLASS_METHOD_CALLBACK(RuntimeTest, Location)
  // static method fields callback define
  CLASS_STATIC_METHOD_CALLBACK(RuntimeTest, Printf)

  CLASS_REGISTER_START(RuntimeTest)
    REGISTER_PARENT_CLASS(Base)
    CLASS_CONSTRUCTOR_ENABLE(RuntimeTest)
    REGISTER_GET_CALLBACK(RuntimeTest, Number)
    REGISTER_SET_CALLBACK(RuntimeTest, Number)
    REGISTER_GET_CALLBACK(RuntimeTest, Location)
    REGISTER_SET_CALLBACK(RuntimeTest, Location)
    REGISTER_METHOD_CALLBACK(RuntimeTest, AddNum)
    REGISTER_METHOD_CALLBACK(RuntimeTest, Location)
    REGISTER_STATIC_METHOD_CALLBACK(RuntimeTest, Printf)
    REGISTER_STATIC_MEMBER(RuntimeTest, staticNumber, int)
  CLASS_REGISTER_END(RuntimeTest)

int RuntimeTest::staticNumber = 179;

RuntimeTest::RuntimeTest(EngineContext* context)
      : Base(),
        RuntimeObject(context),
        num_(3) {
  FML_LOG(ERROR) << "create native Test";
  SetJSClass(RuntimeTest::s_jsclass_);
  location_.SetEngineContext(context);
}

void RuntimeTest::MakeObject(const char* name) {
//  std::string member("static_number");
//  RuntimeValues value(RuntimeTest::static_number);
//  SetProperty(member, nullptr, value);
//  SetProperty(member, static_cast<RuntimeObject*>(&location_));
}
void RuntimeTest::Register(EngineContext* context) {
  Base::CreateClassRef(context);
  RuntimeTest::CreateClassRef(context);
  Address::CreateClassRef(context);
}

// static method
ScopeValues RuntimeTest::Printf(std::vector<unicorn::ScopeValues>& args) {
  FML_LOG(ERROR) << "RuntimeTest::Printf";
  for (size_t i = 0; i < args.size(); i++) {
    if (args[i]->IsMap()) {
      Map* map = args[i]->GetAsMap();
      auto& local = map->GetMap();
      for (auto& iter : local) {
         FML_LOG(ERROR) << " RuntimeTest::Printf name: " << iter.first <<
                           " value: " << iter.second;
      }
    } else if (args[i]->IsFunction()) {
      Function* function = args[i]->GetAsFunction();
      std::vector<ScopeValues> argv;
      auto value = RuntimeValues::MakeInt(133);
      argv.push_back(std::move(value));
      function->Call(1, argv);
    }
  }

  return RuntimeValues::MakeUndefined();
}

// method
ScopeValues
RuntimeTest::AddNum(const std::vector<unicorn::ScopeValues>& vars) {
  AddNum();
  return RuntimeValues::MakeUndefined();
}

void RuntimeTest::AddNum() {
  num_++;
  FML_LOG(ERROR) << "add num num_: " << num_;
}

ScopeValues
RuntimeTest::Location(const std::vector<unicorn::ScopeValues>& args) {
  FML_LOG(ERROR) << "RuntimeTest::Location";
  return RuntimeValues::MakeUndefined();
}

// member get
ScopeValues RuntimeTest::GetNumber() {
  FML_LOG(ERROR) << "Test::GetNum num_: " << num_;
  return RuntimeValues::MakeInt(num_);
}

ScopeValues RuntimeTest::GetLocation() {
  FML_LOG(ERROR) << "RuntimeTest::GetLocation : " << &location_ <<
                    "Address class: " << Address::s_jsclass_;
  return RuntimeValues::MakeCommonObject(static_cast<void*>(&location_),
                                 new RuntimeClass(Address::s_jsclass_));
}

// member set
void RuntimeTest::SetNumber(ScopeValues value) {
  int i = 0;
    value->GetAsInteger(&i);
  num_ = i;
  FML_LOG(ERROR) << "Test::SetNum num_: " << num_;
}

void RuntimeTest::SetLocation(ScopeValues value) {
  FML_LOG(ERROR) << "set address";
//  location_ = addr;
}

}  // namespace unicorn
