function Package-Repo {
    param (
        [string]$PackageDir,
        [string]$InstallerName
    )

    if (-Not (Test-Path $PackageDir)) {
        throw "Package repository directory '$PackageDir' does not exist. This must be set up manually."
    }

    $temp_location = Get-Location
    cd $PackageDir

    if (-Not (Test-Path "repository")) {
        Write-Host "There is no repository directory. There should be one here unless this is the first release of this repo. Are you sure you want to continue?"
        $continue = Read-Host "Continue? (y/N)"
        if ($continue -ne "y" -and $continue -ne "Y") {
            cd $temp_location
            throw "Aborting as per user request."
        }
        repogen -p "packages" "repository"
    } else {
        repogen --update-new-components -p "packages" "repository"
    }
    if ($LASTEXITCODE -ne 0) {
        cd $temp_location
        throw "Failed to generate package repository."
    }

    binarycreator --hybrid -c "config/config.xml" -p "packages" "$InstallerName"
    if ($LASTEXITCODE -ne 0) {
        cd $temp_location
        throw "Failed to create installer."
    }

    cd $temp_location

}
