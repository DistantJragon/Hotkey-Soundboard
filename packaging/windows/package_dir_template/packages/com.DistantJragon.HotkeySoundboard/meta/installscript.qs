function Component() {}

Component.prototype.createOperations = function() {
  component.createOperations();

  if (systemInfo.productType === "windows") {
    component.addOperation("CreateShortcut", "@TargetDir@/HotkeySoundboard.exe",
                           "@StartMenuDir@/Hotkey Soundboard.lnk",
                           "workingDirectory=@TargetDir@",
                           "iconPath=%SystemRoot%/system32/SHELL32.dll",
                           "iconId=2", "description=Open Hotkey Soundboard");
  }
}
