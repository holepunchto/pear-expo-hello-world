import ce from 'compact-encoding';
import { requireNativeModule } from 'expo-modules-core';
import RPC from 'tiny-buffer-rpc';

requireNativeModule('HelloBare').install();

// forward bare's logs to console
HelloBare.onLog = console.log

// RPC
const rpc = new RPC(HelloBare.sendMessage)
HelloBare.onMessage = rpc.recv.bind(rpc)

const reverseStringMethod = rpc.register(0, {
  request: ce.string,
  response: ce.string
})

export const reverseString = async (message) => await reverseStringMethod.request(message)