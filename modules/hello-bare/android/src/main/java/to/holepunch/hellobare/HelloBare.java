package to.holepunch.hellopear;

import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;

public final class HelloBare {
  static {
    try {
      System.loadLibrary("hello_bare_jsi");
    } catch (Exception ignored) {
    }
  }

  /** @noinspection JavaJniMissingFunction*/
  static native void install(
    long jsi,
    CallInvokerHolderImpl callInvoker);
}
