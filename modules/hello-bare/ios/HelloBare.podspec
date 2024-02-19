hb_root = File.join(__dir__, "..", "..", "..", "bare-libs", "ios")

Pod::Spec.new do |s|
  s.name           = 'HelloBare'
  s.version        = '1.0.0'
  s.summary        = 'A sample project summary'
  s.description    = 'A sample project description'
  s.author         = ''
  s.homepage       = 'https://docs.expo.dev/modules/'
  s.platform       = :ios, '14.0'
  s.source         = { git: '' }
  s.static_framework = true

  s.dependency 'ExpoModulesCore'

  # Swift/Objective-C compatibility
  s.pod_target_xcconfig = {
    'DEFINES_MODULE' => 'YES',
    'SWIFT_COMPILATION_MODE' => 'wholemodule'
  }

  s.source_files = "**/*.{h,m,mm,swift,hpp,cpp}"

  s.pod_target_xcconfig = {
    'HEADER_SEARCH_PATHS' => "$(inherited) #{hb_root}/../../bare",
  }

  s.user_target_xcconfig = {
    'HB_ROOT[sdk=iphoneos*]' =>  "#{hb_root}/iphoneos",
    'HB_ROOT[sdk=iphonesimulator*]' =>  "#{hb_root}/iphonesimulator",
    'LIBRARY_SEARCH_PATHS' => "$(inherited) $(HB_ROOT)",
    'OTHER_LDFLAGS' => "$(inherited) -ObjC -lv8 -ljs $(HB_ROOT)/misc/libc++.a -force_load $(HB_ROOT)/libhello_bare.a -force_load $(HB_ROOT)/libbare.a",
  }
end
