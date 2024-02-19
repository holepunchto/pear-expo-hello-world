#pragma once

#include <jsi/jsi.h>
#include <ReactCommon/CallInvoker.h>

#include <hello-bare.h>

using namespace facebook;

#define JSI_HOSTOBJECT_FN(arg_argc, code)\
  return jsi::Function::createFromHostFunction(rt, name, arg_argc, [this](jsi::Runtime &rt, jsi::Value const&, jsi::Value const *arguments, size_t argc) \
  code \
  );\

#define JSI_TYPEDARRAY(name, val) \
  auto name##_obj = val.asObject(rt);\
  auto name##_ab = name##_obj.getProperty(rt, "buffer").asObject(rt).getArrayBuffer(rt);\
  auto name##_ptr = name##_ab.data(rt);\
  auto name##_byteOffset = static_cast<size_t>(name##_obj.getProperty(rt, "byteOffset").asNumber());\
  auto name##_data = name##_ptr + name##_byteOffset;

#define JSI_TYPEDARRAY_BYTELENGTH(name)\
  auto name##_byteLength = static_cast<size_t>(name##_obj.getProperty(rt, "byteLength").asNumber());

#define HB_GLOBAL "HelloBare"
#define HB_GLOBAL_SEND_MESSAGE "sendMessage"
#define HB_GLOBAL_ON_MESSAGE "onMessage"
#define HB_GLOBAL_ON_LOG "onLog"

#define HB_GLOBAL_FN(name)\
  if (!rt.global().hasProperty(rt, HB_GLOBAL)) {\
    return;\
  }\
  auto global = rt.global().getPropertyAsObject(rt, HB_GLOBAL);\
  if (!global.hasProperty(rt, name) ||\
      !global.getPropertyAsObject(rt, name).isFunction(rt)) {\
    return;\
  }\
  auto fn = global.getPropertyAsFunction(rt, name);

typedef struct {
  jsi::Runtime* rt;
  react::CallInvoker* call_invoker;
} rn_data_t;

struct JsiMutableVectorBuffer: jsi::MutableBuffer {
  JsiMutableVectorBuffer(uint8_t* data, size_t len) {
    v_.insert(v_.end(), data, data + len);
  }
  size_t size() const override {
    return v_.size();
  }
  uint8_t* data() override {
    return &v_.front();
  }

private:
  std::vector<uint8_t> v_;
};

class JsiHelloBareHostObject: public jsi::HostObject {
  std::shared_ptr<jsi::Object> obj_;
  
public:
  JsiHelloBareHostObject(jsi::Runtime& rt):
    obj_(std::make_shared<jsi::Object>(rt.global().getPropertyAsFunction(rt, "Object").callAsConstructor(rt).getObject(rt))) {}
  
  std::vector<jsi::PropNameID> getPropertyNames(jsi::Runtime& rt) override {
    std::vector<jsi::PropNameID> props;
    props.push_back(jsi::PropNameID::forUtf8(rt, std::string(HB_GLOBAL_SEND_MESSAGE)));
    props.push_back(jsi::PropNameID::forUtf8(rt, std::string(HB_GLOBAL_ON_MESSAGE)));
    props.push_back(jsi::PropNameID::forUtf8(rt, std::string(HB_GLOBAL_ON_LOG)));
    return props;
  }
  
  jsi::Value get(jsi::Runtime &rt, jsi::PropNameID const &name) override {
    auto prop_name = name.utf8(rt);
    
    if (prop_name == HB_GLOBAL_SEND_MESSAGE) {
      JSI_HOSTOBJECT_FN(1, {
        JSI_TYPEDARRAY(msg, arguments[0]);
        JSI_TYPEDARRAY_BYTELENGTH(msg);
        
        hb_send_message(msg_data, msg_byteLength);
        
        return jsi::Value::undefined();
      });
    }
    else if (prop_name == HB_GLOBAL_ON_MESSAGE || prop_name == HB_GLOBAL_ON_LOG) {
      return obj_->getProperty(rt, name);
    }

    return jsi::Value::undefined();
  }
  
  void set(jsi::Runtime &rt, jsi::PropNameID const &name, const jsi::Value &value) override {
    auto prop_name = name.utf8(rt);
    
    if (prop_name == HB_GLOBAL_ON_MESSAGE || 
        prop_name == HB_GLOBAL_ON_LOG) {
      obj_->setProperty(rt, name, value);
      return;
    }
    
    throw jsi::JSError(rt, "Trying to set a wrong HelloBare property.");
  }

  ~JsiHelloBareHostObject() override {
    void* data;
    hb_get_data(&data);
    if (data) {
      free(data);
    }
    hb_set_data(nullptr);
  }
};

static void on_message(void* buf, size_t len, void* data) {
  if (!data) {
    return;
  }
  
  rn_data_t* rn_data = (rn_data_t*)data;
  auto buffer = std::make_shared<JsiMutableVectorBuffer>((uint8_t*)buf, len);
  rn_data->call_invoker->invokeAsync([rn_data, buffer]() {
    jsi::Runtime& rt = *rn_data->rt;
    HB_GLOBAL_FN(HB_GLOBAL_ON_MESSAGE);

    auto array_buffer = jsi::ArrayBuffer(rt, buffer);
    auto ctor = rt.global().getPropertyAsFunction(rt, "Uint8Array");
    auto arg = ctor.callAsConstructor(rt, array_buffer).getObject(rt);

    fn.call(rt, arg);
  });
}

static void on_log(const char* msg, void* data) {
  if (!data) {
    hb_platform_log(msg);
    return;
  }

  rn_data_t* rn_data = (rn_data_t*)data;
  std::string msg_string = msg;
  rn_data->call_invoker->invokeAsync([rn_data, msg_string]() {
    jsi::Runtime& rt = *rn_data->rt;
    HB_GLOBAL_FN(HB_GLOBAL_ON_LOG);
    fn.call(rt, jsi::String::createFromUtf8(rt, msg_string));
  });
}
