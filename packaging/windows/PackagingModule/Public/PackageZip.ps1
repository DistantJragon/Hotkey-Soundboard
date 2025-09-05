function Package-Zip {
    param (
        [string]$BinariesDir,
        [string]$PackageZip
    )

    if (Test-Path $PackageZip) {
        Remove-Item $PackageZip
        if ($? -ne $true) {
            throw "Failed to remove existing package zip '$PackageZip'. Please check permissions."
        }
    }
    Compress-Archive -Path "$BinariesDir" -DestinationPath $PackageZip
    if ($? -ne $true) {
        throw "Failed to create package zip '$PackageZip'. Please check permissions."
    }
}
