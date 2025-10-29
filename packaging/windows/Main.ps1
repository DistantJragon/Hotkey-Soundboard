param(
    [bool]$SkipBuild = $false,
    [bool]$SkipDeploy = $false,
    [bool]$SkipZip = $false,
    [bool]$SkipInstaller = $false
)

Remove-Module "PackagingModule" -ErrorAction SilentlyContinue
Import-Module "$PSScriptRoot/PackagingModule/PackagingModule.psm1"

function Reset-Path-Unmodified {
    if ($unmodified_path) {
        $env:PATH = $unmodified_path
    }
}

# Add tools if AddTools.ps1 exists
if (Test-Path "$PSScriptRoot/AddTools.ps1") {
    try {
        . "$PSScriptRoot/AddTools.ps1"
    } catch {
        Write-Error $_
        exit 1
    }
    Write-Host "Tools added to PATH."
}

try {
  Check-RequiredTools
} catch {
  Write-Error $_
  Reset-Path-Unmodified
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
try {
    $arch_choice = [int]$arch_choice
    if ($arch_choice -lt 1 -or $arch_choice -gt $architectures.Length) {
        throw "Invalid choice. Please run the script again and select a valid option."
    }
} catch {
    Write-Error $_
    Reset-Path-Unmodified
    exit 1
}

$hksb_arch = $architectures[$arch_choice - 1]
Write-Host "Selected architecture: $hksb_arch"

Write-Host "========================="
Write-Host "What version of HotkeySoundboard are you packaging?"

$hksb_version = Read-Host "Enter version (e.g., X.X.X-ab.1)"
if (-Not $hksb_version) {
    Write-Error "Version cannot be empty. Please run the script again and enter a valid version."
    Reset-Path-Unmodified
    exit 1
}
Write-Host "Version set to: $hksb_version"

Write-Host "========================="
Write-Host "What version of the installer package are you creating?"

$installer_package_version = Read-Host "Enter installer package version (e.g., X.X.X-X.X)"
if (-Not $installer_package_version) {
    Write-Error "Installer package version cannot be empty. Please run the script again and enter a valid version."
    Reset-Path-Unmodified
    exit 1
}
Write-Host "Installer package version set to: $installer_package_version"

try {
    . "$PSScriptRoot/SharedVariables.ps1"
} catch {
    Write-Error $_
    Reset-Path-Unmodified
    exit 1
}

if (-Not $SkipBuild) {
  Write-Host "========================="
  Write-Host "Building HotkeySoundboard version $hksb_version for Windows ($hksb_arch)"
  try {
    Build-HKSB -Architecture $hksb_arch `
      -AppVersion $hksb_version `
      -BuildDirectory $hksb_build_dir `
      -BuiltExecutable $hksb_built_executable `
      -ProjectFile $hksb_project_file
  } catch {
    Write-Error $_
    cd $hksb_project_root
    Reset-Path-Unmodified
    exit 1
  }
}

if (-Not $SkipDeploy) {
  Write-Host "========================="
  Write-Host "Deploying HotkeySoundboard version $hksb_version for Windows ($hksb_arch)"
  Write-Host "Deploying to $hksb_binaries_dir"
  try {
    Deploy-HKSB -BinariesDir $hksb_binaries_dir `
      -BuiltExecutable $hksb_built_executable
  } catch {
    Write-Error $_
    cd $hksb_project_root
    Reset-Path-Unmodified
    exit 1
  }
}

if (-Not $SkipZip) {
  Write-Host "========================="
  Write-Host "Packaging HotkeySoundboard version $hksb_version for Windows ($hksb_arch)"
  Write-Host "Packaging zip to $hksb_package_zip"
  try {
    Package-Zip -BinariesDir $hksb_binaries_dir `
      -PackageZip $hksb_package_zip
  } catch {
    Write-Error $_
    cd $hksb_project_root
    Reset-Path-Unmodified
    exit 1
  }
  Write-Host "========================="
  Write-Host "Zip file created at $hksb_package_zip"
  Write-Host "========================="
}

if (-Not $SkipInstaller) {
  try {
    Prepare-PackageDir -ProjectRoot $hksb_project_root `
      -DeployDir $hksb_dist_os `
      -BinariesDir $hksb_binaries_dir `
      -PackageDir $hksb_package_dir `
      -InstallerPackageVersion $installer_package_version
  } catch {
    Write-Error $_
    cd $hksb_project_root
    Reset-Path-Unmodified
    exit 1
  }
  Write-Host "Please configure the package repository at $hksb_package_dir manually."
  Write-Host "Ensure the last release's repository is placed inside."
  Read-Host "Press Enter to continue after configuring the package repository"
  Write-Host "Packaging installer..."
  try {
    Package-Repo -PackageDir $hksb_package_dir `
      -InstallerName $hksb_installer_name
  } catch {
    Write-Error $_
    cd $hksb_project_root
    Reset-Path-Unmodified
    exit 1
  }
  Write-Host "========================="
  Write-Host "Repository and installer packaging complete."
  Write-Host "Repository located at $hksb_package_dir/repository"
  Write-Host "Installer created at $hksb_package_dir/$hksb_installer_name"
}
Reset-Path-Unmodified
