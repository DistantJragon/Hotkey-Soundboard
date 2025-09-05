function Build-HKSB {
    param (
        [string]$Architecture,
        [string]$AppVersion,
        [string]$BuildDirectory,
        [string]$BuiltExecutable,
        [string]$ProjectFile
    )

    if (-Not (Test-Path $BuildDirectory)) {
        Write-Error "Build directory '$BuildDirectory' does not exist."
        throw "Build directory '$BuildDirectory' does not exist."
    }

    $temp_location = Get-Location
    cd $BuildDirectory

    if (Test-Path "Makefile") {
        mingw32-make clean
    }

    # Run qmake (commands plucked from Qt Creator)
    Write-Host "Running qmake...2"
    qmake $ProjectFile -spec win32-g++ `
      "CONFIG+=qtquickcompiler" "DEFINES+=HKSB_VERSION=\\\\\\\`"${AppVersion}\\\\\\\`""
    if ($LASTEXITCODE -ne 0) {
        cd $temp_location
        throw "Build failed during qmake. Please check the output for errors."
    }

    mingw32-make -f "Makefile" qmake_all
    if ($LASTEXITCODE -ne 0) {
        cd $temp_location
        throw "Build failed during make qmake_all. Please check the output for errors."
    }

    mingw32-make
    if ($LASTEXITCODE -ne 0) {
        cd $temp_location
        Write-Error "BuildDirectory: $BuildDirectory"
        throw "Build failed during make. Please check the output for errors."
    }

    if (-Not (Test-Path $BuiltExecutable)) {
        cd $temp_location
        throw "Built executable '$BuiltExecutable' not found. Build may have failed."
    }

    cd $temp_location
}
