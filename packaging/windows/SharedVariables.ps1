$hksb_app_root = "apps/HotkeySoundboard"
$hksb_project_file = "$hksb_app_root/HotkeySoundboard.pro"
$hksb_build_dir = "$hksb_app_root/build/Desktop_Qt_6_9_2_MinGW_64_bit-Release"
$hksb_built_executable = "$hksb_build_dir/release/HotkeySoundboard.exe"
$hksb_deploy_dir = "dist/windows-${hksb_arch}"
$hksb_binaries_dir = "$hksb_deploy_dir/HotkeySoundboard"
$hksb_package_repo = "$hksb_deploy_dir/package_repo"
$hksb_package_zip = "$hksb_deploy_dir/HotkeySoundboard-v${hksb_version}-windows-${hksb_arch}.zip"

# Check if project file exists
if (-Not (Test-Path $hksb_project_file)) {
    Write-Error "Qt project file '$hksb_project_file' not found. Please ensure you are in the correct directory."
    exit 1
}

# We are in the correct directory. Save current location.
$hksb_project_root = Get-Location
