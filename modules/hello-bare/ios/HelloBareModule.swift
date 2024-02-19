import ExpoModulesCore

public class HelloBareModule: Module {
  public func definition() -> ModuleDefinition {
    Name("HelloBare")

    Function("install") {
      HelloBare.sharedInstance().install(appContext?.reactBridge)
    }
  }
}
