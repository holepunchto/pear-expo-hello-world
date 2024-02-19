#include <stdlib.h>
#include <assert.h>
#include <vector>

#include <uv.h>
#include <js.h>
#include <bare.h>

#if defined(__APPLE__)
#include <os/log.h>
#elif defined(__ANDROID__)
#include <android/log.h>
#endif

#include <concurrentqueue.h>

#include "hello-bare.h"
#include "bundle.js.h"

#define RUNTIME_GLOBAL_NAME "HelloBare"
#define PEAR "🍐 "

typedef std::function<void(void)> hb_looper_queue_item_t;
typedef moodycamel::ConcurrentQueue<hb_looper_queue_item_t> hb_looper_queue_t;

static struct {
  uv_once_t preinit_once = UV_ONCE_INIT;
  uv_once_t init_once = UV_ONCE_INIT;
  uv_thread_t worker;
  uv_async_t looper_async;
  uv_sem_t init_sem;
  hb_looper_queue_t looper_queue;

  js_env_t* env = nullptr;

  uv_rwlock_t data_lock;
  hb_on_message on_message = nullptr;
  hb_on_log on_log = nullptr;
  void* data;

#if defined(__APPLE__)
  os_log_t logger;
#endif
} hb;

static void _queue_post(hb_looper_queue_item_t&& job) {
  hb.looper_queue.enqueue(std::move(job));
  int err = uv_async_send(&hb.looper_async);
  assert(err == 0);
}

static js_value_t* _runtime_send_log(js_env_t* env, js_callback_info_t* info) {
  int err;

  size_t argc = 1;
  js_value_t* argv[1];

  err = js_get_callback_info(env, info, &argc, argv, nullptr, nullptr);
  assert(err == 0);

  size_t len = 0;
  err = js_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
  assert(err == 0);

  char* str = (char*)malloc(len + 1);
  err = js_get_value_string_utf8(env, argv[0], (utf8_t*)str, len + 1, nullptr);
  assert(err == 0);

  uv_rwlock_rdlock(&hb.data_lock);
  if (hb.on_log != nullptr) {
    std::string prefix = PEAR;
    hb.on_log(prefix.append(str).c_str(), hb.data);
  }
  else {
    hb_platform_log(str);
  }
  uv_rwlock_rdunlock(&hb.data_lock);

  free(str);

  return nullptr;
}

static js_value_t* _runtime_send_message(js_env_t* env, js_callback_info_t* info) {
  int err;

  size_t argc = 1;
  js_value_t* argv[1];
  err = js_get_callback_info(env, info, &argc, argv, nullptr, nullptr);
  assert(err == 0);

  size_t len;
  js_typedarray_type_t type;
  js_value_t* arraybuffer;
  size_t offset;
  void* buf;
  err = js_get_typedarray_info(env, argv[0], &type, &buf, &len, &arraybuffer, &offset);
  assert(err == 0);

  uv_rwlock_rdlock(&hb.data_lock);
  if (hb.on_log != nullptr) {
    hb.on_message(buf, len, hb.data);
  }
  uv_rwlock_rdunlock(&hb.data_lock);

  return nullptr;
}

static js_value_t* _runtime_on_ready(js_env_t*, js_callback_info_t*) {
  uv_sem_post(&hb.init_sem);

  return nullptr;
}

// public

void hb_init(hb_on_message on_message, hb_on_log on_log, void* data) {
  hb_platform_log("init bare...");

  uv_once(&hb.preinit_once, [](void) {
    uv_rwlock_init(&hb.data_lock);
  });

  uv_rwlock_wrlock(&hb.data_lock);
  hb.on_message = on_message;
  hb.on_log = on_log;
  hb.data = data;
  uv_rwlock_wrunlock(&hb.data_lock);

  uv_once(&hb.init_once, [](void) {
    int err;

    err = uv_sem_init(&hb.init_sem, 0);
    assert(err == 0);

    uv_thread_create(&hb.worker, [](void*) {
      hb_platform_log("starting bare worker...");

      int err;

      uv_loop_t loop;
      err = uv_loop_init(&loop);
      assert(err == 0);

      err = uv_async_init(&loop, &hb.looper_async, [](uv_async_t*) {
        hb_looper_queue_item_t job;
        while (hb.looper_queue.try_dequeue(job)) {
          job();
        }
      });
      assert(err == 0);
      uv_unref((uv_handle_t*)&hb.looper_async);

      js_platform_t* platform;
      bare_t* bare;

      err = js_create_platform(&loop, nullptr, &platform);
      assert(err == 0);

      err = bare_setup(&loop, platform, &hb.env, 0, nullptr, nullptr, &bare);
      assert(err == 0);

      js_value_t* global;
      js_get_global(hb.env, &global);
      js_value_t* js_hb;
      js_create_object(hb.env, &js_hb);
      js_set_named_property(hb.env, global, RUNTIME_GLOBAL_NAME, js_hb);
      {
        const char* name = "sendLog";
        js_value_t* fn;
        err = js_create_function(hb.env, name, -1, _runtime_send_log, nullptr, &fn);
        assert(err == 0);
        err = js_set_named_property(hb.env, js_hb, name, fn);
        assert(err == 0);
      }
      {
        const char* name = "sendMessage";
        js_value_t* fn;
        err = js_create_function(hb.env, name, -1, _runtime_send_message, nullptr, &fn);
        assert(err == 0);
        err = js_set_named_property(hb.env, js_hb, name, fn);
        assert(err == 0);
      }
      {
        const char* name = "onReady";
        js_value_t* fn;
        err = js_create_function(hb.env, name, -1, _runtime_on_ready, nullptr, &fn);
        assert(err == 0);
        err = js_set_named_property(hb.env, js_hb, name, fn);
        assert(err == 0);
      }

      uv_buf_t source = uv_buf_init((char*)bundle, bundle_len);
      err = bare_run(bare, "/hello-bare.bundle", &source);
      assert(err == 0);

      int exit_code;
      err = bare_teardown(bare, &exit_code);
      assert(err == 0);

      err = js_destroy_platform(platform);
      assert(err == 0);

      uv_rwlock_destroy(&hb.data_lock);
      uv_close((uv_handle_t*)&hb.looper_async, nullptr);

      err = uv_run(&loop, UV_RUN_DEFAULT);
      assert(err == 0);

      err = uv_loop_close(&loop);
      assert(err == 0);
    }, nullptr);

    uv_sem_wait(&hb.init_sem);
    uv_sem_destroy(&hb.init_sem);
  });
}

void hb_send_message(void* buf, size_t len) {
  auto buffer = std::make_shared<std::vector<uint8_t>>();
  buffer->insert(buffer->end(), (uint8_t*)buf, (uint8_t*)buf + len);
  _queue_post([buffer]() {
    js_env_t* env = hb.env;

    js_value_t* global;
    js_get_global(env, &global);

    js_value_t* js_hb;
    js_get_named_property(env, global, RUNTIME_GLOBAL_NAME, &js_hb);

    const char* name = "onMessage";
    bool has_property;
    js_has_named_property(env, js_hb, name, &has_property);
    if (!has_property) {
      return;
    }

    js_value_t* js_on_message;
    js_get_named_property(env, js_hb, name, &js_on_message);

    js_value_t* js_buffer;
    js_create_external_arraybuffer(env, (void*)&buffer->front(), buffer->size(), nullptr, nullptr, &js_buffer);

    js_value_t* js_array;
    js_create_typedarray(env, js_uint8_array, buffer->size(), js_buffer, 0, &js_array);

    js_handle_scope_t* scope;
    int hs = js_open_handle_scope(env, &scope);

    js_value_t* args[1] = { js_array };
    js_value_t* result;
    js_call_function(env, js_hb, js_on_message, 1, args, &result);

    js_close_handle_scope(env, scope);
  });
}

void hb_get_data(void** data) {
  *data = hb.data;
}

void hb_set_data(void* data) {
  uv_rwlock_wrlock(&hb.data_lock);
  hb.data = data;
  uv_rwlock_wrunlock(&hb.data_lock);
}

void hb_platform_log(const char* str) {
  std::string prefix = PEAR;
  const char* msg = prefix.append(str).c_str();
#if defined(__APPLE__)
  os_log(hb.logger, "%{public}s", msg);
#elif defined(__ANDROID__)
  __android_log_print(ANDROID_LOG_DEBUG, "hello-bare", "%s", msg);
#endif
}
