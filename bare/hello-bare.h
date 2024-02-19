#ifndef HELLO_BARE_H
#define HELLO_BARE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*hb_on_message)(void* buf, size_t len, void* data);
typedef void (*hb_on_log)(const char* msg, void* data);

void hb_init(hb_on_message on_message, hb_on_log on_log, void* data);
void hb_send_message(void* buf, size_t len);
void hb_get_data(void** data);
void hb_set_data(void* data);

// utility

void hb_platform_log(const char* msg);

#ifdef __cplusplus
}
#endif

#endif // HELLO_BARE_H
