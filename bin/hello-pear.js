#!/usr/bin/env node

const { program } = require('commander')
const { spawnSync } = require('child_process')
const path = require('path')

const which = require('which')

const pkg = require('../package.json')

const bareConfigure = require('bare-dev/configure')
const bareBuild = require('bare-dev/build')
const cmake = which.sync('cmake')

const optionsCommon = {
  verbose: true,
  quiet: false
}

// helpers

const cmakeInstall = (buildOptions) => {
  const proc = spawnSync(cmake, ['--install', buildOptions.build, '--prefix', 'bare-libs'], {
    stdio: 'inherit',
  })
  if (proc.status) throw new Error('spawnSync() failed')
  return proc
}

// options

const BUILD = path.join(__dirname, '..', 'build')
const buildOptionsIos = { build: `${BUILD}/ios-arm64` }
const buildOptionsIosArm64Simulator = { build: `${BUILD}/ios-arm64-simulator` }
const buildOptionsIosX64Simulator = { build: `${BUILD}/ios-x64-simulator` }
const buildOptionsAndroidArm64 = { build: `${BUILD}/android-arm64` }
const buildOptionsAndroidArm = { build: `${BUILD}/android-arm` }
const buildOptionsAndroidX64 = { build: `${BUILD}/android-x64` }
const buildOptionsAndroidIa32 = { build: `${BUILD}/android-ia32` }

const optionsIosCommon = {
  ...optionsCommon,
  platform: 'ios',
  arch: 'arm64',
  iosDeploymentTarget: 14
}
const optionsIos = { ...optionsIosCommon, ...buildOptionsIos }
const optionsIosArm64Simulator = {
  ...optionsIosCommon,
  ...buildOptionsIosArm64Simulator,
  simulator: true
}

const optionsIosX64Simulator = {
  ...optionsIosCommon,
  ...buildOptionsIosX64Simulator,
  simulator: true,
  arch: 'x64'
}

const iosSimArchs = [
  optionsIosArm64Simulator.arch,
  optionsIosX64Simulator.arch
]

const optionsAndroidCommon = {
  ...optionsCommon,
  platform: 'android',
  androidNdk: '25.1.8937393',
  androidApi: 24
}
const optionsAndroidArm64 = {
  ...optionsAndroidCommon,
  ...buildOptionsAndroidArm64,
  arch: 'arm64'
}
const optionsAndroidArm = {
  ...optionsAndroidCommon,
  ...buildOptionsAndroidArm,
  arch: 'arm'
}
const optionsAndroidX64 = {
  ...optionsAndroidCommon,
  ...buildOptionsAndroidX64,
  arch: 'x64'
}
const optionsAndroidIa32 = {
  ...optionsAndroidCommon,
  ...buildOptionsAndroidIa32,
  arch: 'ia32'
}
const androidArchs = [
  optionsAndroidArm64.arch,
  optionsAndroidArm.arch,
  optionsAndroidX64.arch,
  optionsAndroidIa32.arch
]

program
  .version(pkg.version)
  .option('-c, --configure', 'configure before', false)
  .option('-i, --ios', 'build + package for iOS')
  .option(
    '-s, --ios-sim <arch...>',
    'build + package for iOS simulator (arm64, x64)',
    (value, previous) => {
      if (iosSimArchs.findIndex((arch) => arch === value) < 0) {
        throw new Error(`${value} is not a valid iOS simulator architecture`)
      }
      return { ...previous, [value]: true }
    },
    // TODO Set this to default to match the current machines architecture
    {}
  )
  .option(
    '-a, --android <arch...>',
    'build + package for Android architectures (arm64, arm, x64, ia32)',
    (value, previous) => {
      if (androidArchs.findIndex((arch) => arch === value) < 0) {
        throw new Error(`${value} is not a valid Android architecture`)
      }
      return { ...previous, [value]: true }
    },
    {}
  )
  .parseAsync()
  .then(action)
  .catch((err) => {
    console.error(`error: ${err.message}`)
    process.exitCode = 1
  })

async function action () {
  const options = program.opts()

  const hasDarwin = options.ios === true || Object.keys(options.iosSim).length > 0
  const hasAndroid = Object.keys(options.android).length > 0

  let ios = !hasDarwin && !hasAndroid ? true : options.ios === true
  let iosSimArm64 = !hasDarwin && !hasAndroid ? true : options.iosSim.arm64 === true
  let iosSimX64 = !hasDarwin && !hasAndroid ? true : options.iosSim.x64 === true

  let androidArm64 = !hasAndroid && !hasDarwin ? true : options.android.arm64 === true
  let androidArm = !hasAndroid && !hasDarwin ? true : options.android.arm === true
  let androidX64 = !hasAndroid && !hasDarwin ? true : options.android.x64 === true
  let androidIA32 = !hasAndroid && !hasDarwin ? true : options.android.ia32 === true

  const commands = []

  // configure

  if (options.configure) {
    if (ios) {
      commands.push(() => bareConfigure(optionsIos))
    }
    if (iosSimArm64) {
      commands.push(() => bareConfigure(optionsIosArm64Simulator))
    }
    if (iosSimX64) {
      commands.push(() => bareConfigure(optionsIosX64Simulator))
    }
    if (androidArm64) {
      commands.push(() => bareConfigure(optionsAndroidArm64))
    }
    if (androidArm) {
      commands.push(() => bareConfigure(optionsAndroidArm))
    }
    if (androidX64) {
      commands.push(() => bareConfigure(optionsAndroidX64))
    }
    if (androidIA32) {
      commands.push(() => bareConfigure(optionsAndroidIa32))
    }
  }

  // build

  if (ios) {
    commands.push(() => bareBuild(optionsIos))
  }
  if (iosSimArm64) {
    commands.push(() => bareBuild(optionsIosArm64Simulator))
  }
  if (iosSimX64) {
    commands.push(() => bareBuild(optionsIosX64Simulator))
  }
  if (androidArm64) {
    commands.push(() => bareBuild(optionsAndroidArm64))
  }
  if (androidArm) {
    commands.push(() => bareBuild(optionsAndroidArm))
  }
  if (androidX64) {
    commands.push(() => bareBuild(optionsAndroidX64))
  }
  if (androidIA32) {
    commands.push(() => bareBuild(optionsAndroidIa32))
  }

  // package

  if (ios) {
    commands.push(() => cmakeInstall(buildOptionsIos))
  }
  if (iosSimArm64) {
    commands.push(() => cmakeInstall(buildOptionsIosArm64Simulator))
  }
  if (iosSimX64) {
    commands.push(() => cmakeInstall(buildOptionsIosX64Simulator))
  }
  if (androidArm64) {
    commands.push(() => cmakeInstall(buildOptionsAndroidArm64))
  }
  if (androidArm) {
    commands.push(() => cmakeInstall(buildOptionsAndroidArm))
  }
  if (androidX64) {
    commands.push(() => cmakeInstall(buildOptionsAndroidX64))
  }
  if (androidIA32) {
    commands.push(() => cmakeInstall(buildOptionsAndroidIa32))
  }

  // run

  for (let i = 0, l = commands.length; i < l; i += 1) {
    commands[i]()
  }
}
