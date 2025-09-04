# Description: Main script to package HotkeySoundboard for Windows

# Add tools if AddTools.ps1 exists
if (Test-Path "$PSScriptRoot/AddTools.ps1") {
    . "$PSScriptRoot/AddTools.ps1"
    if ($LASTEXITCODE -ne 0) {
        exit 1
    }
}

. "$PSScriptRoot/CheckTools.ps1"
if ($LASTEXITCODE -ne 0) {
    exit 1
}

Write-Host "Packaging HotkeySoundboard"
Write-Host "========================="
$architectures = @("x86_64", "x86")
Write-Host "Pick architecture:"
for ($i = 0; $i -lt $architectures.Length; $i++) {
    Write-Host "$($i + 1). $($architectures[$i])"
}
$arch_choice = Read-Host "Enter choice (1 or 2)"
if ($arch_choice -lt 1 -or $arch_choice -gt $architectures.Length) {
    Write-Error "Invalid choice. Please run the script again and select a valid option."
    exit 1
}

$hksb_arch = $architectures[$arch_choice - 1]
Write-Host "Selected architecture: $hksb_arch"

Write-Host "========================="
Write-Host "What version of HotkeySoundboard are you packaging?"

$hksb_version = Read-Host "Enter version (e.g., X.X.X-ab.1)"
if (-Not $hksb_version) {
    Write-Error "Version cannot be empty. Please run the script again and enter a valid version."
    exit 1
}
Write-Host "Version set to: $hksb_version"

. "$PSScriptRoot/SharedVariables.ps1"
if ($LASTEXITCODE -ne 0) {
    exit 1
}

Write-Host "========================="
Write-Host "Building HotkeySoundboard version $hksb_version for Windows ($hksb_arch)"
. "$PSScriptRoot/BuildClean.ps1"
if ($LASTEXITCODE -ne 0) {
    exit 1
}

Write-Host "========================="
Write-Host "Deploying HotkeySoundboard version $hksb_version for Windows ($hksb_arch)"
Write-Host "Deploying to $hksb_deploy_dir"
. "$PSScriptRoot/DeployBinaries.ps1"
if ($LASTEXITCODE -ne 0) {
  exit 1
}

Write-Host "========================="
Write-Host "Packaging HotkeySoundboard version $hksb_version for Windows ($hksb_arch)"
Write-Host "Packaging zip to $hksb_package_zip"
. "$PSScriptRoot/PackageZip.ps1"
if ($LASTEXITCODE -ne 0) {
  exit 1
}

Write-Host "Please set up the package repository at $hksb_package_repo manually if you haven't already. Ensure the last release's repository is placed in $hksb_package_repo."
Read-Host "Press Enter to continue after setting up the package repository..."
Write-Host "Packaging installer..."
. "$PSScriptRoot/PackageRepo.ps1"
