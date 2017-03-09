function Component()
{
    // default constructor
    var programFiles = installer.environmentVariable("ProgramFiles");
    if (programFiles !== "")
        installer.setValue("TargetDir", programFiles + "\/SigViewer");
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/sigviewer.exe", "@StartMenuDir@/SigViewer.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/sigviewer.ico");
    }
}
