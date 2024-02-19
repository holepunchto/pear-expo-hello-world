import { NativeModulesProxy, EventEmitter, Subscription } from 'expo-modules-core';

// Import the native module. On web, it will be resolved to HelloBare.web.ts
// and on native platforms to HelloBare.ts
import HelloBareModule from './src/HelloBareModule';
import HelloBareView from './src/HelloBareView';
import { ChangeEventPayload, HelloBareViewProps } from './src/HelloBare.types';

// Get the native constant value.
export const PI = HelloBareModule.PI;

export function hello(): string {
  return HelloBareModule.hello();
}

export async function setValueAsync(value: string) {
  return await HelloBareModule.setValueAsync(value);
}

const emitter = new EventEmitter(HelloBareModule ?? NativeModulesProxy.HelloBare);

export function addChangeListener(listener: (event: ChangeEventPayload) => void): Subscription {
  return emitter.addListener<ChangeEventPayload>('onChange', listener);
}

export { HelloBareView, HelloBareViewProps, ChangeEventPayload };
