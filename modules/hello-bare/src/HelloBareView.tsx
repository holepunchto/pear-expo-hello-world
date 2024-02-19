import { requireNativeViewManager } from 'expo-modules-core';
import * as React from 'react';

import { HelloBareViewProps } from './HelloBare.types';

const NativeView: React.ComponentType<HelloBareViewProps> =
  requireNativeViewManager('HelloBare');

export default function HelloBareView(props: HelloBareViewProps) {
  return <NativeView {...props} />;
}
