#include <mutex>

#include <fbjni/fbjni.h>
#include <ReactCommon/CallInvokerHolder.h>

#include <hello-bare-jsi.hpp>

struct HelloBare: jni::JavaClass<HelloBare> {
    static constexpr auto kJavaDescriptor = "Lto/holepunch/hellopear/HelloBare;";

    static void install(jni::alias_ref<jni::JClass>,
            jlong jsi_ptr,
            jni::alias_ref<react::CallInvokerHolder::javaobject> java_call_invoker) {

      auto rt = reinterpret_cast<jsi::Runtime*>(jsi_ptr);
      if (!rt) {
        return;
      }

      static bool installed = false;
      bool already_installed = installed;

      rn_data_t* rn_data = (rn_data_t*)malloc(sizeof(rn_data_t));
      rn_data->rt = rt;
      rn_data->call_invoker = java_call_invoker->cthis()->getCallInvoker().get();

      static std::once_flag once;
      std::call_once(once, [rn_data]() {
          installed = true;
          hb_init(on_message, on_log, rn_data);
      });

      if (already_installed) {
        void* data;
        hb_get_data(&data);
        if (data) {
          free(data);
        }
        hb_set_data(rn_data);
      }
      rt->global().setProperty(
              *rt,
              HB_GLOBAL,
              jsi::Object::createFromHostObject(
                      *rt,
                      std::make_shared<JsiHelloBareHostObject>(*rt)));
    }

    static void registerNatives() {
      javaClassStatic()->registerNatives({
        makeNativeMethod("install", HelloBare::install),
      });
    }
};

jint JNI_OnLoad(JavaVM* vm, void*) {
  return jni::initialize(vm, [] {
      HelloBare::registerNatives();
  });
}
