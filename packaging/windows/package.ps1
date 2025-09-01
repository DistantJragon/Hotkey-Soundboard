#
# This script builds and packages the application for distribution.
#

$qt_project_file = "apps/HotkeySoundboard/HotkeySoundboard.pro"
$qt_build_dir = "apps/HotkeySoundboard/build/Desktop_Qt_6_9_2_MinGW_64_bit-Release"
$qt_built_executable = "$qt_build_dir/release/HotkeySoundboard.exe"
$deploy_dir = "dist/win64/HotkeySoundboard"
$package_zip = "dist/HotkeySoundboard-${env:HKSB_VERSION}-windows-${env:HKSB_ARCH}.zip"

# Check version variable
if (-Not $env:HKSB_VERSION) {
    Write-Error "Environment variable HKSB_VERSION is not set. Please set it before running this script."
    exit 1
}
# Check architecture variable
if (-Not $env:HKSB_ARCH) {
    Write-Error "Environment variable HKSB_ARCH is not set. Please set it before running this script."
    exit 1
}

Write-Host "Packaging HotkeySoundboard to $package_zip"

# Check if project file exists
if (-Not (Test-Path $qt_project_file)) {
    Write-Error "Qt project file '$qt_project_file' not found. Please ensure you are in the correct directory."
    exit 1
}

# We are in the correct directory. Save current location.
$project_root = Get-Location

# Check if required tools are available
if (-Not (Get-Command "qmake" -ErrorAction SilentlyContinue)) {
    Write-Error "qmake not found in PATH. Add Qt's bin directory to your PATH."
    exit 1
}
if (-Not (Get-Command "mingw32-make" -ErrorAction SilentlyContinue)) {
    Write-Error "mingw32-make not found in PATH. This script must be used with the MinGW version of Qt."
    exit 1
}
if (-Not (Get-Command "windeployqt" -ErrorAction SilentlyContinue)) {
    Write-Error "windeployqt not found in PATH. It must be missing from your Qt installation."
    exit 1
}

# Check if build directory exists
if (Test-Path $qt_build_dir) {
    # Clean previous build inside the build directory
    cd $qt_build_dir
    mingw32-make clean
    cd $project_root
}

# Run qmake (commands plucked from Qt Creator)
qmake HotkeySoundboard.pro -spec win32-g++ "CONFIG+=qtquickcompiler"
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed during qmake. Please check the output for errors."
    exit 1
}

# "E:\Qt\Tools\mingw1310_64\bin\mingw32-make.exe"
#   -f D:/MyFiles/QTProjects/HotkeySoundboard/build/Desktop_Qt_6_9_2_MinGW_64_bit-Release/Makefile
#   qmake_all
cd $qt_build_dir
mingw32-make -f "Makefile" qmake_all
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed during make qmake_all. Please check the output for errors."
    exit 1
}

# "E:\Qt\Tools\mingw1310_64\bin\mingw32-make.exe" -j12
mingw32-make
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed during make. Please check the output for errors."
    exit 1
}

cd $project_root

if (-Not (Test-Path $qt_built_executable)) {
    Write-Error "Built executable '$qt_built_executable' not found. Build may have failed."
    exit 1
}

# Prepare the deploy directory
if (Test-Path $deploy_dir) {
    Remove-Item $deploy_dir -Recurse -Force
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to remove existing deploy directory '$deploy_dir'. Please check permissions."
        exit 1
    }
}

windeployqt --release --no-system-d3d-compiler --dir $deploy_dir $qt_built_executable
if ($LASTEXITCODE -ne 0) {
    Write-Error "windeployqt failed. Please check the output for errors."
    exit 1
}

# Add the main executable and other necessary files to the deploy directory
Copy-Item $qt_built_executable $deploy_dir -Force
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to copy built executable to deploy directory."
    exit 1
}
Copy-Item "README.md" $deploy_dir -Force
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to copy README.md to deploy directory. Please check permissions."
    exit 1
}
Copy-Item "LICENSE" $deploy_dir -Force
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to copy LICENSE to deploy directory. Please check permissions."
    exit 1
}
Copy-Item "LICENSES" $deploy_dir -Recurse -Force
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to copy LICENSES to deploy directory. Please check permissions."
    exit 1
}

# Package the deploy directory into a zip file using Compress-Archive
if (Test-Path $package_zip) {
    Remove-Item $package_zip
}
Compress-Archive -Path "$deploy_dir" -DestinationPath $package_zip
