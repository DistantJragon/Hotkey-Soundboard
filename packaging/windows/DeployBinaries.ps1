# This script deploys the built Qt application and its dependencies into a specified directory.
# (This script should only be run by Main.ps1 after setting up variables.)

# Prepare the deploy directory
if (Test-Path $hksb_binaries_dir) {
    Remove-Item $hksb_binaries_dir -Recurse -Force
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to remove existing deploy directory '$hksb_binaries_dir'. Please check permissions."
        exit 1
    }
}

windeployqt --release --no-system-d3d-compiler --dir $hksb_binaries_dir $hksb_built_executable
if ($LASTEXITCODE -ne 0) {
    Write-Error "windeployqt failed. Please check the output for errors."
    exit 1
}

# Add the main executable and other necessary files to the deploy directory
Copy-Item $hksb_built_executable $hksb_binaries_dir -Force
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to copy built executable to deploy directory."
    exit 1
}
Copy-Item "README.md" $hksb_binaries_dir -Force
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to copy README.md to deploy directory. Please check permissions."
    exit 1
}
Copy-Item "LICENSE" $hksb_binaries_dir -Force
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to copy LICENSE to deploy directory. Please check permissions."
    exit 1
}
Copy-Item "LICENSES" $hksb_binaries_dir -Recurse -Force
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to copy LICENSES to deploy directory. Please check permissions."
    exit 1
}
