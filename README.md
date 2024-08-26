# pear-expo-hello-world

A Pear Runtime `hello world` using [Bare](https://github.com/holepunchto/bare) with [Expo](https://docs.expo.dev/).

## Requirements

Make sure below packages are installed on your system:

- `CMake` >= 3.25 - can check via `cmake --version`
- `Android Studio` - For Android, installing Android Studio is recommended (you can follow [React Native docs](https://reactnative.dev/docs/set-up-your-environment?platform=android))
- `Android NDK` - also make sure Android NDK version `25.1.8937393` is installed and set the `ANDROID_HOME` environment variable (see `Configure the ANDROID_HOME environment variable` section in the above React Native docs).

## Build

1. Clone this repo:

    ```
    git clone https://github.com/holepunchto/pear-expo-hello-world.git
    ```

2. run following command to sync git submodules:

    ```sh
    git submodule update --init --recursive
    ```

    > [!NOTE]
    > From now on, you should run `npx bare-dev vendor sync` after updating `bare` git submodule.

3. run `npm install` to install all the dependencies, or use other package manager by preference.

4. run `npm link` in the project root, then run

    ```sh
    hello-pear --configure
    ```

    > could also run as `bin/hello-pear.js --configure` or `npx hello-pear --configure`, need to append the `--configure` flag when first run

### hello-pear commands

By default the `hello-pear` helpers will builds bare for every architecture for both iOS and Android, but a more fine grained build could be achieved:

```sh
# list all commands
hello-pear --help

# iOS simulator only
hello-pear --ios-sim

# iOS simulator only with x64 architecture
hello-pear --ios-sim x64

# iOS and iOS simulator
hello-pear --ios --ios-sim arm64

# Android only arm archs
hello-pear --android arm64 arm

# Android only arm64
hello-pear --android arm64

# Android only x86 archs
hello-pear --android x64 ia32
```

## Run on Device

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
