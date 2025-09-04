# Builds the HotkeySoundboard project using qmake and mingw32-make.
# (This script should only be run by Main.ps1 after setting up variables.)

# Check if build directory exists
if (Test-Path $hksb_build_dir) {
    # Clean previous build inside the build directory
    cd $hksb_build_dir
    mingw32-make clean
    cd $hksb_project_root
}

# Run qmake (commands plucked from Qt Creator)
cd $hksb_app_root
qmake HotkeySoundboard.pro -spec win32-g++ "CONFIG+=qtquickcompiler" "DEFINES += HKSB_VERSION=\\\`"${hksb_version}\\\`""
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed during qmake. Please check the output for errors."
    exit 1
}
cd $hksb_project_root

# "E:\Qt\Tools\mingw1310_64\bin\mingw32-make.exe"
#   -f D:/MyFiles/QTProjects/HotkeySoundboard/build/Desktop_Qt_6_9_2_MinGW_64_bit-Release/Makefile
#   qmake_all
cd $hksb_build_dir
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

cd $hksb_project_root

if (-Not (Test-Path $hksb_built_executable)) {
    Write-Error "Built executable '$hksb_built_executable' not found. Build may have failed."
    exit 1
}

