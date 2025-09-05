function Deploy-HKSB {
    param (
        [string]$BinariesDir,
        [string]$BuiltExecutable
    )

    # Prepare the deploy directory
    if (Test-Path $BinariesDir) {
        Remove-Item $BinariesDir -Recurse
        if ($? -ne $true) {
            throw "Failed to remove existing deploy directory '$BinariesDir'. Please check permissions."
        }
    }

    windeployqt --release --no-system-d3d-compiler --dir $BinariesDir $BuiltExecutable
    if ($LASTEXITCODE -ne 0) {
        throw "windeployqt failed. Please check the output for errors."
    }

    # Add the main executable and other necessary files to the deploy directory
    Copy-Item $BuiltExecutable $BinariesDir -Force
    if ($? -ne $true) {
        throw "Failed to copy built executable to deploy directory."
    }
    Copy-Item "README.md" $BinariesDir -Force
    if ($? -ne $true) {
        throw "Failed to copy README.md to deploy directory. Please check permissions."
    }
    Copy-Item "LICENSE" $BinariesDir -Force
    if ($? -ne $true) {
        throw "Failed to copy LICENSE to deploy directory. Please check permissions."
    }
    Copy-Item "LICENSES" $BinariesDir -Recurse -Force
    if ($? -ne $true) {
        throw "Failed to copy LICENSES to deploy directory. Please check permissions."
    }

}
