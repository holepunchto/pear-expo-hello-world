import ce from 'compact-encoding';
import { requireNativeModule } from 'expo-modules-core';
import RPC from 'tiny-buffer-rpc';

requireNativeModule('HelloBare').install();

// forward bare's logs to console
HelloBare.onLog = console.log

let methodCount = 0

// RPC
const rpc = new RPC(HelloBare.sendMessage)
HelloBare.onMessage = rpc.recv.bind(rpc)

const reverseStringMethod = rpc.register(methodCount++, {
  request: ce.string,
  response: ce.string
})

// const upperStringMethod = rpc.register(methodCount++, {
//   request: ce.string,
//   response: ce.string
// })

export const reverseString = async (message) => await reverseStringMethod.request(message)
// export const upperString = async (message) => await upperStringMethod.request(message)
