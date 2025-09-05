function Prepare-PackageDir {
    param (
        [string]$ProjectRoot,
        [string]$DeployDir,
        [string]$BinariesDir,
        [string]$PackageDir
    )

    if (-Not (Test-Path $DeployDir)) {
        throw "Deploy directory '$DeployDir' does not exist. Something went wrong in the build or deploy steps."
    }

    $package_config_path = "$PackageDir/config"
    $main_package_path = "$PackageDir/packages/com.DistantJragon.HotkeySoundboard"
    $package_meta_path = "$main_package_path/meta"
    $package_data_path = "$main_package_path/data"

    if (Test-Path $PackageDir) {
        Remove-Item -Path "$PackageDir/*" -Recurse -Exclude "repository"
        if ($? -ne $true) {
            throw "Failed to clear package directory '$PackageDir'. Please check permissions."
        }
    } else {
        New-Item -ItemType Directory -Path "$PackageDir" -Force
        if ($? -ne $true) {
            throw "Failed to create package directory '$PackageDir'. Please check permissions."
        }
    }
    Copy-Item -Path "$ProjectRoot/packaging/windows/package_dir_template/*" `
        -Destination "$PackageDir" -Recurse -Force
    if ($? -ne $true) {
        throw "Failed to copy package directory template to '$PackageDir'. Please check permissions."
    }

    Copy-Item -Path "$BinariesDir/*" -Destination "$package_data_path" -Recurse -Force
    if ($? -ne $true) {
        throw "Failed to copy built binaries from '$hksb_binaries_dir' to package data directory '$package_data_path'. Please check permissions."

    }
    Copy-Item -Path "$ProjectRoot/LICENSE" -Destination "$package_meta_path/LICENSE" -Force
    if ($? -ne $true) {
        throw "Failed to copy LICENSE file to '$package_meta_path/LICENSE'. Please check permissions."
    }

}
