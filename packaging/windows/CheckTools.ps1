# Description: Checks for the presence of required tools in the system PATH.

$missing_tools = @()

if (-Not (Get-Command "qmake" -ErrorAction SilentlyContinue)) {
    Write-Error "qmake not found in PATH. Add Qt's bin directory to your PATH."
    $missing_tools += "qmake"
} else {
  Write-Host "qmake found: $(Get-Command qmake | Select-Object -ExpandProperty Source)"
}

if (-Not (Get-Command "mingw32-make" -ErrorAction SilentlyContinue)) {
    Write-Error "mingw32-make not found in PATH. This script must be used with the MinGW version of Qt."
    $missing_tools += "mingw32-make"
} else {
  Write-Host "mingw32-make found: $(Get-Command mingw32-make | Select-Object -ExpandProperty Source)"
}

if (-Not (Get-Command "windeployqt" -ErrorAction SilentlyContinue)) {
    Write-Error "windeployqt not found in PATH. It must be missing from your Qt installation."
    $missing_tools += "windeployqt"
} else {
  Write-Host "windeployqt found: $(Get-Command windeployqt | Select-Object -ExpandProperty Source)"
}

if (-Not (Get-Command "repogen" -ErrorAction SilentlyContinue)) {
    Write-Error "repogen not found in PATH. It must be missing from your Qt installation."
    $missing_tools += "repogen"
} else {
  Write-Host "repogen found: $(Get-Command repogen | Select-Object -ExpandProperty Source)"
}

if (-Not (Get-Command "binarycreator" -ErrorAction SilentlyContinue)) {
    Write-Error "binarycreator not found in PATH. It must be missing from your Qt installation."
    $missing_tools += "binarycreator"
} else {
  Write-Host "binarycreator found: $(Get-Command binarycreator | Select-Object -ExpandProperty Source)"
}

if ($missing_tools.Count -gt 0) {
    Write-Error "One or more required tools are missing. Please ensure they are in your PATH."
    exit 1
} else {
    Write-Host "All required tools are present."
}
