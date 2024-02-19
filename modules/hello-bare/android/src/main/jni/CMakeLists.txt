cmake_minimum_required(VERSION 3.22.1)

set(
  PACKAGE_NAME
  hello_bare_jsi
)
project(
  ${PACKAGE_NAME}
  LANGUAGES CXX
)

add_library(
  ${PACKAGE_NAME}
  SHARED
  hello-bare.cc
)

target_include_directories(
  ${PACKAGE_NAME}
  PRIVATE
  ${HB_SRC}
  # react-native
  ${RN_SRC}/ReactAndroid/src/main/jni/react/turbomodule
  ${RN_SRC}/ReactAndroid/src/main/jni/react/jni
  ${RN_SRC}/ReactCommon
  ${RN_SRC}/ReactCommon/jsi
  ${RN_SRC}/ReactCommon/callinvoker
  ${RN_SRC}/ReactCommon/runtimeexecutor
  ${RN_SRC}/ReactCommon/react/nativemodule/core
)

# libs

set(HB_LIBS_ARCH ${HB_LIBS}/${CMAKE_ANDROID_ARCH_ABI})

add_library(v8 STATIC IMPORTED)
set_target_properties(
  v8
  PROPERTIES IMPORTED_LOCATION
  ${HB_LIBS_ARCH}/libv8.a
)

add_library(c++ STATIC IMPORTED)
set_target_properties(
  c++
  PROPERTIES IMPORTED_LOCATION
  ${HB_LIBS_ARCH}/misc/libc++.a
)

add_library(js STATIC IMPORTED)
set_target_properties(
  js
  PROPERTIES IMPORTED_LOCATION
  ${HB_LIBS_ARCH}/libjs.a
)

add_library(bare STATIC IMPORTED)
set_target_properties(
  bare
  PROPERTIES IMPORTED_LOCATION
  ${HB_LIBS_ARCH}/libbare.a
)

add_library(hello-bare STATIC IMPORTED)
set_target_properties(
  hello-bare
  PROPERTIES IMPORTED_LOCATION
  ${HB_LIBS_ARCH}/libhello_bare.a
)

find_package(ReactAndroid REQUIRED CONFIG)
find_package(fbjni REQUIRED CONFIG)
find_library(android-log log)

# linking

target_link_libraries(
  ${PACKAGE_NAME}
  # react-native
  fbjni::fbjni
  ReactAndroid::jsi
  ReactAndroid::turbomodulejsijni
  ReactAndroid::react_nativemodule_core
  # bare
  v8
  c++
  js
  -Wl,--whole-archive bare -Wl,--no-whole-archive
  -Wl,--whole-archive hello-bare -Wl,--no-whole-archive
  # android
  android
  ${android-log}
)
