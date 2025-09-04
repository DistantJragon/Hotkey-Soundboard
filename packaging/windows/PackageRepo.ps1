# This script packages the HotkeySoundboard application into an installer
# (This script should only be run by Main.ps1)

cd $hksb_project_root

if (-Not Test-Path $hksb_package_repo) {
    Write-Error "Package repository directory '$hksb_package_repo' does not exist. This must be set up manually."
    exit 1
}

cd $hksb_package_repo

if (-Not Test-Path "repository") {
    Write-Host "There is no repository directory. There should be one here unless this is the first time you are packaging. Are you sure you want to continue?"
    $continue = Read-Host "Continue? (y/N)"
    if ($continue -ne "y" -and $continue -ne "Y") {
        Write-Host "Exiting."
        exit 1
    }
    repogen -p "packages" "repository"
} else {
    repogen --update-new-components -p "packages" "repository"
}
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to generate package repository."
    exit 1
}

binarycreator --hybrid -c "config/config.xml" -p "packages" "HotkeySoundboard-Installer.exe"

