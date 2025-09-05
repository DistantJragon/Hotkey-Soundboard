$hksb_app_root = "apps/HotkeySoundboard"
$hksb_project_file = "$hksb_app_root/HotkeySoundboard.pro"

# Check if project file exists
if (-Not (Test-Path $hksb_project_file)) {
    throw "Qt project file '$hksb_project_file' not found. Please ensure you are in the correct directory."
}

# We are in the correct directory. Save current location.
$hksb_project_root = Get-Location

# Make previous paths absolute
$hksb_app_root = "$hksb_project_root/$hksb_app_root"
$hksb_project_file = "$hksb_project_root/$hksb_project_file"

$hksb_build_dir = "$hksb_app_root/build/Desktop_Qt_6_9_2_MinGW_64_bit-Release"

# An alternative build dir may be specified via environment variable
if ($env:HKSB_BUILD_DIR) {
    $hksb_build_dir = $env:HKSB_BUILD_DIR
    if (-Not (Test-Path $hksb_build_dir)) {
      Write-Error "The specified build directory '$hksb_build_dir' does not exist. Please create it or unset the HKSB_BUILD_DIR environment variable."
      exit 1
    }
    Write-Host "Using alternative build directory from environment variable: $hksb_build_dir"
    Read-Host "Press Enter to continue"
} else {
    Write-Host "Using default build directory: $hksb_build_dir"
    Read-Host "Press Enter to continue"
}

$hksb_dist_dir = "$hksb_project_root/dist"

# An alternative dist dir may be specified via environment variable
if ($env:HKSB_DIST_DIR) {
    $hksb_dist_dir = $env:HKSB_DIST_DIR
    if (-Not (Test-Path $hksb_dist_dir)) {
      throw "The specified deploy root '$hksb_dist_dir' does not exist. Please create it or unset the HKSB_DIST_DIR environment variable."
    }
    Write-Host "Using alternative deploy root directory from environment variable: $hksb_dist_dir"
    Read-Host "Press Enter to continue"
} else {
    Write-Host "Using default deploy root directory: $hksb_dist_dir"
    Read-Host "Press Enter to continue"
}

$hksb_dist_os = "$hksb_dist_dir/windows-${hksb_arch}"

# Ensure dist_os exists
if (-Not (Test-Path $hksb_dist_os)) {
    New-Item -ItemType Directory -Path $hksb_dist_os | Out-Null
    if ($? -ne $true) {
        throw "Failed to create deploy directory '$hksb_dist_os'. Please check permissions."
    }
}

$hksb_built_executable = "$hksb_build_dir/release/HotkeySoundboard.exe"
$hksb_binaries_dir = "$hksb_dist_os/HotkeySoundboard"
$hksb_package_dir = "$hksb_dist_os/package_dir"
$hksb_zip_name = "HotkeySoundboard-v${hksb_version}-windows-${hksb_arch}.zip"
$hksb_installer_name = "HotkeySoundboard-v${hksb_version}-windows-${hksb_arch}-installer.exe"
$hksb_package_zip = "$hksb_dist_os/$hksb_zip_name"


