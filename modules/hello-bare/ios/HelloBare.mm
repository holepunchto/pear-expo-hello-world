#import "HelloBare.h"

#import <React/RCTBridge.h>
#import <ReactCommon/RCTTurboModule.h>

#import <hello-bare-jsi.hpp>

@interface RCTBridge (KeetCore)

-(void*)runtime;

@end

@implementation HelloBare

+(instancetype)sharedInstance
{
  static HelloBare* sharedInstance;
  static dispatch_once_t once;
  dispatch_once(&once, ^{
    sharedInstance = [[HelloBare alloc] init];
  });
  return sharedInstance;
}

-(void)install:(nullable RCTBridge*)bridge
{
  if (!bridge) {
    return;
  }
  
  auto rt = [bridge respondsToSelector:@selector(runtime)] 
    ? reinterpret_cast<jsi::Runtime*>(bridge.runtime)
    : nullptr;
  if (!rt) {
    return;
  }

  static bool installed = false;
  bool already_installed = installed;
  
  rn_data_t* rn_data = (rn_data_t*)malloc(sizeof(rn_data_t));
  rn_data->rt = rt;
  rn_data->call_invoker = bridge.jsCallInvoker.get();

  static dispatch_once_t once;
  dispatch_once(&once, ^{
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

  rt->global()
    .setProperty(*rt,
                 HB_GLOBAL,
                 jsi::Object::createFromHostObject(*rt, std::make_shared<JsiHelloBareHostObject>(*rt)));
}

@end
