import * as React from 'react';

import { HelloBareViewProps } from './HelloBare.types';

export default function HelloBareView(props: HelloBareViewProps) {
  return (
    <div>
      <span>{props.name}</span>
    </div>
  );
}
