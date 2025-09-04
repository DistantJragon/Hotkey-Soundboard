# This script packages the app into a zip file.
# (This script should only be run by Main.ps1 after setting up variables and deploying binaries.)

# Package the deploy directory into a zip file using Compress-Archive
if (Test-Path $hksb_package_zip) {
    Remove-Item $hksb_package_zip
}
Compress-Archive -Path "$hksb_binaries_dir" -DestinationPath $hksb_package_zip
