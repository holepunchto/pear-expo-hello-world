#import <Foundation/Foundation.h>

#import <React/RCTBridge.h>

NS_ASSUME_NONNULL_BEGIN

@interface HelloBare : NSObject

+(instancetype)sharedInstance;
-(instancetype)init UNAVAILABLE_ATTRIBUTE;
+(instancetype)new UNAVAILABLE_ATTRIBUTE;

-(void)install:(nullable RCTBridge*)bridge;

@end

NS_ASSUME_NONNULL_END
