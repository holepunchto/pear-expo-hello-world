package to.holepunch.hellopear

import com.facebook.react.bridge.CatalystInstanceImpl
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.common.annotations.FrameworkAPI
import expo.modules.kotlin.modules.Module
import expo.modules.kotlin.modules.ModuleDefinition

@Suppress("unused")
class HelloBareModule : Module() {
  @OptIn(FrameworkAPI::class)
  override fun definition() = ModuleDefinition {
    Name("HelloBare")

    Function("install") {
      val reactContext = appContext.reactContext as ReactApplicationContext
      val catalyst = reactContext.catalystInstance as CatalystInstanceImpl

      HelloBare.install(
        reactContext.javaScriptContextHolder!!.get(),
        catalyst.jsCallInvokerHolder)
    }
  }
}
