# pear-expo-hello-world

A Pear Runtime `hello world` using [Bare](https://github.com/holepunchto/bare) with [Expo](https://docs.expo.dev/).

Requirements
------------

Make sure you have installed on your system:

- `CMake` >= 3.25.
- For Android, installing Android Studio is recommended (you can follow [React Native docs](https://reactnative.dev/docs/0.72/environment-setup?platform=android)), also make sure Android NDK version `25.1.8937393` is installed and set the `ANDROID_HOME` environment variable (see `Configure the ANDROID_HOME environment variable` section on React Native docs).

Building
--------

Clone this repo and run:

```sh
git submodule update --init --recursive
```

> [!NOTE]
> From now on, you should run `npx bare-dev vendor sync` after updating `bare` git submodule.

Now install all the dependencies running `npm install` or your package manager of preference.

For ease we added a helper script you can simply install running `npm link` in the project's root -or `bin/hello-pear.js` or npx `hello-pear`-.

> [!IMPORTANT]
> The first time you run it you need to append the `--configure` flag:
> ```sh
> hello-pear --configure
> ```

By default the helpers builds bare for every architecture for both iOS and Android, but a more fine grained build could be achieved:

```sh
# iOS simulator only
hello-pear --ios-simulator

# iOS and iOS simulator
hello-pear --ios --ios-simulator

# Android only arm archs
hello-pear --android arm64 arm

# Android only arm64
hello-pear --android arm64

# Android only x86 archs
hello-pear --android x64 ia32

# help
hello-pear --help
```

Running
--------

> [!IMPORTANT]
> After any change in the source code that runs on bare you need to re-run `hello-pear` in order to apply the changes.

You can check [expo-cli docs](https://docs.expo.dev/more/expo-cli/) but as a cheat sheet:

```sh
# build and run on iOS connected devices
npx expo run:ios
# clearing the build cache
npx expo run:ios --no-build-cache

# build and run on Android connected devices
npx expo run:android
# clearing the build cache
npx expo run:android --no-build-cache

# just start the local server
npx expo start --dev-client
```
